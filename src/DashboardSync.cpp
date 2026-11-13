#include "global.h"
#include "DashboardSync.h"
#include "CryptManager.h"
#include "DateTime.h"
#include "GameManager.h"
#include "Preference.h"
#include "PrefsManager.h"
#include "Profile.h"
#include "ProfileManager.h"
#include "RageFile.h"
#include "RageFileManager.h"
#include "RageSurface.h"
#include "RageSurface_Load.h"
#include "RageSurface_Save_PNG.h"
#include "RageSurfaceUtils.h"
#include "RageSurfaceUtils_Zoom.h"
#include "RageThreads.h"
#include "RageUtil.h"
#include "Song.h"
#include "SongManager.h"
#include "SongCacheIndex.h"
#include "Steps.h"
#include "XmlFile.h"
#include "XmlFileUtil.h"
#include <chrono>
#include <regex>
#include <set>
#include <stdexcept>
#ifdef _WIN32
#include <windows.h>
#include <winhttp.h>
#include <wincred.h>
#endif

namespace Dashboard
{
static Preference<bool> enabled("DashboardEnabled", false);
static Preference<RString> repository("DashboardRepository", "");
static Preference<RString> branch("DashboardDataBranch", "dashboard-data");
static Preference<RString> profile("DashboardProfile", "");
static Preference<RString> credential("DashboardCredentialKey", "StepManiaDashboard");
static const RString ROOT = "/Save/Dashboard/";
static void Require(bool condition, const RString& message)
{ if(!condition) throw std::runtime_error(message); }
static void CheckCancel(const std::atomic<bool>& cancel)
{ Require(!cancel, "Upload stopped. Select Upload Statistics to retry; accepted uploads remain on GitHub."); }
static Json::Value Parse(const RString& text)
{
	Json::Value value; Json::Reader reader;
	Require(reader.parse(text, value), "Invalid JSON response or export file.");
	return value;
}
static RString Json(const Json::Value& value) { return Json::FastWriter().write(value); }
static RString Read(const RString& path, int limit = 32*1024*1024)
{
	RageFile f;
	Require(f.Open(path, RageFile::READ), "Cannot read statistics/export file.");
	Require(f.GetFileSize() >= 0 && f.GetFileSize() <= limit, "Statistics/export file exceeds the supported size.");
	RString text;
	Require(f.Read(text, f.GetFileSize()) == f.GetFileSize(), "Statistics/export read failed.");
	return text;
}
static void Write(const RString& path, const RString& text)
{
	// RageFile WRITE uses a temporary file and renames on successful close.
	RageFile f;
	Require(f.Open(path, RageFile::WRITE), "Cannot create the local dashboard export.");
	Require(f.Write(text) == int(text.size()) && f.Flush() == 0, "Cannot save the local dashboard export.");
	f.Close();
}
RString Hash(const RString& text) { return BinaryToHex(CryptManager::GetSHA256ForString(text)); }
static RString GitHash(const RString& text)
{
	RString prefix = "blob " + std::to_string(text.size()); prefix.push_back('\0');
	return BinaryToHex(CryptManager::GetSHA1ForString(prefix + text));
}
RString Base64(const RString& text)
{
	static const char* alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	RString out;
	for(size_t i=0; i<text.size(); i+=3)
	{
		unsigned n = unsigned((unsigned char)text[i]) << 16;
		if(i+1<text.size()) n |= unsigned((unsigned char)text[i+1]) << 8;
		if(i+2<text.size()) n |= (unsigned char)text[i+2];
		out += alphabet[(n>>18)&63]; out += alphabet[(n>>12)&63];
		out += i+1<text.size()? alphabet[(n>>6)&63] : '=';
		out += i+2<text.size()? alphabet[n&63] : '=';
	}
	return out;
}
RString Unbase64(const RString& text)
{
	const RString alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	RString out; unsigned buffer=0; int bits=0;
	for(unsigned char c: text)
	{
		if(c=='=' || c=='\n' || c=='\r') continue;
		size_t n = alphabet.find(c); Require(n != RString::npos, "Invalid base64 data.");
		buffer=(buffer<<6)|unsigned(n); bits+=6;
		if(bits>=8) { bits-=8; out.push_back(char((buffer>>bits)&255)); }
	}
	return out;
}
Settings GetSettings()
{
	Settings s; s.enabled=enabled.Get(); s.repository=repository.Get();
	s.branch=branch.Get(); s.profile=profile.Get(); s.credential=credential.Get(); return s;
}
static void ValidateRepository(const RString& value)
{
	Require(std::regex_match(value, std::regex("[A-Za-z0-9_-]+/[A-Za-z0-9_.-]+")),
		"Set DashboardRepository=owner/repository in Save/Preferences.ini, then restart the game.\nSee Docs/DashboardUpload.md for the connection settings.");
}
RString WebURL(const Settings& s, bool buildLog)
{
	ValidateRepository(s.repository);
	if(buildLog) return "https://github.com/"+s.repository+"/actions/workflows/dashboard-sync.yml";
	size_t slash=s.repository.find('/');
	return "https://"+s.repository.substr(0,slash)+".github.io/"+s.repository.substr(slash+1)+"/";
}
static void ValidateSettings(const Settings& s)
{
	Require(s.enabled, "Uploads are disabled.\nSet DashboardEnabled=1 in Save/Preferences.ini, then restart the game.\nSaving a token does not configure the connection; see Docs/DashboardUpload.md.");
	ValidateRepository(s.repository);
	Require(std::regex_match(s.branch, std::regex("[A-Za-z0-9_-][A-Za-z0-9_-]{0,63}")) && s.branch!="main" && s.branch!="master" && s.branch!="gh-pages", "Choose a separate DashboardDataBranch, such as dashboard-data.");
	Require(!s.credential.empty() && s.credential.size()<128, "Set a valid DashboardCredentialKey.");
}
RString ProfileChoices()
{
	RString text="DashboardProfile=machine";
	vector<RString> ids; PROFILEMAN->GetLocalProfileIDs(ids);
	for(const auto& id: ids)
		text += "\nDashboardProfile=local:"+id+" ("+PROFILEMAN->GetLocalProfile(id)->m_sDisplayName+")";
	return text;
}
static std::set<RString> Words(const RString& text)
{ vector<RString> words; split(text," ",words,true); return std::set<RString>(words.begin(),words.end()); }
static const std::map<RString, std::set<RString>>& Children()
{
	static const std::map<RString,std::set<RString>> allowed = {
		{"Stats",Words("GeneralData SongScores CalorieData RecentSongScores")},
		{"GeneralData",Words("DisplayName Guid IsMachine LastPlayedDate TotalSessions TotalSessionSeconds TotalGameplaySeconds TotalCaloriesBurned TotalDancePoints NumTotalSongsPlayed TotalTapsAndHolds TotalJumps TotalHolds TotalRolls TotalMines TotalHands TotalLifts NumSongsPlayedByDifficulty NumStagesPassedByGrade NumSongsPlayedByStyle")},
		{"NumSongsPlayedByDifficulty",Words("Beginner Easy Medium Hard Challenge Edit")},
		{"NumStagesPassedByGrade",Words("Tier01 Tier02 Tier03 Tier04 Tier05 Tier06 Tier07 Tier08 Tier09 Tier10 Tier11 Tier12 Tier13 Tier14 Tier15 Tier16 Tier17 Tier18 Tier19 Tier20 Failed NoData")},
		{"NumSongsPlayedByStyle",Words("Style")}, {"SongScores",Words("Song")},
		{"Song",Words("Steps")}, {"Steps",Words("HighScoreList")},
		{"HighScoreList",Words("NumTimesPlayed LastPlayed HighScore")},
		{"HighScore",Words("Guid StageGuid PlayerGuid Grade Score PercentDP SurviveSeconds MaxCombo Modifiers DateTime TapNoteScores HoldNoteScores")},
		{"TapNoteScores",Words("W1 W2 W3 W4 W5 Miss HitMine AvoidMine CheckpointHit CheckpointMiss")},
		{"HoldNoteScores",Words("Held LetGo MissedHold")}, {"CalorieData",Words("CaloriesBurned")},
		{"RecentSongScores",Words("HighScoreForASongAndSteps")},
		{"HighScoreForASongAndSteps",Words("Song Steps HighScore")}
	}; return allowed;
}
// Match the dashboard's legacy XML reader: replace invalid UTF-8 and drop
// XML-forbidden controls without modifying the player's original files.
static RString ExportText(const RString& text)
{
    RString out;
    for(unsigned at=0;at<text.size();)
    {
        unsigned begin=at;
        wchar_t ch;
        if(!utf8_to_wchar_ec(text,at,ch)) { out += "\xef\xbf\xbd"; continue; }
        unsigned char first=static_cast<unsigned char>(text[begin]);
        if(first<32 && first!=9 && first!=10 && first!=13) continue;
        // Keep the original bytes: wchar_t is only 16 bits on Windows.
        out.append(text,begin,at-begin);
    }
    return out;
}
static XNode* Clean(const XNode& source)
{
	auto* dest = new XNode(source.GetName());
	static const std::map<RString, std::set<RString>> attrs = {
		{"Song",Words("Dir")}, {"Steps",Words("StepsType Difficulty Description Hash")},
		{"Style",Words("Game Style")}, {"CaloriesBurned",Words("Date")}
	};
	auto a=attrs.find(source.GetName());
	if(a!=attrs.end()) for(const auto& key: a->second)
	{
		RString value; if(source.GetAttrValue(key,value)) dest->AppendAttr(key,ExportText(value));
	}
	RString text; source.GetTextValue(text);
	if(!text.empty()) dest->AppendAttr(XNode::TEXT_ATTRIBUTE,ExportText(text));
	auto allowed=Children().find(source.GetName());
	if(allowed!=Children().end())
	{
		FOREACH_CONST_Child(&source, child)
			if(allowed->second.count(child->GetName())) dest->AppendChild(Clean(*child));
	}
	return dest;
}
RString Sanitize(const XNode& source)
{ std::unique_ptr<XNode> safe(Clean(source)); return XmlFileUtil::GetXML(safe.get()); }
Snapshot Capture()
{
	Snapshot out; out.settings=GetSettings(); ValidateSettings(out.settings);
	Require(!PREFSMAN->m_DisableUploadDir, "Set DisableUploadDir=0 in Save/Preferences.ini so each play is recorded for the dashboard.");
	const Profile* p=nullptr;
	if(out.settings.profile=="machine") { out.machine=true; p=PROFILEMAN->GetMachineProfile(); }
	else if(out.settings.profile.Left(6)=="local:") p=PROFILEMAN->GetLocalProfile(out.settings.profile.substr(6));
	Require(p!=nullptr, "Select an existing DashboardProfile in Save/Preferences.ini.\n"+ProfileChoices());
	out.guid=p->m_sGuid; out.name=p->m_sDisplayName;
	Require(std::regex_match(out.guid,std::regex("[A-Za-z0-9-]{8,64}")), "This profile has an invalid identifier.");
	std::unique_ptr<XNode> node(p->SaveStatsXmlCreateNode());
	out.stats=Sanitize(*node);
	std::set<RString> played;
	const XNode* songScores=node->GetChild("SongScores");
	if(songScores)
	{
		FOREACH_CONST_Child(songScores, songNode)
		{ RString dir; songNode->GetAttrValue("Dir",dir); played.insert(dir); }
	}
	out.songs=Json::Value(Json::arrayValue);
	for(const auto& dir: played)
	{
		Song* song=SONGMAN->GetSongFromDir(dir); if(!song) continue;
		Json::Value m; m["dir"]=ExportText(dir); m["title"]=ExportText(song->GetDisplayMainTitle()); m["artist"]=ExportText(song->GetDisplayArtist());
		m["source_banner"]=song->GetBannerPath(); // Local only; removed before export.
		m["charts"]=Json::Value(Json::arrayValue);
		for(Steps* steps: song->GetAllSteps())
		{
			Json::Value c; c["stepstype"]=GAMEMAN->GetStepsTypeInfo(steps->m_StepsType).szName;
			c["diff"]=DifficultyToString(steps->GetDifficulty()); c["meter"]=steps->GetMeter();
			const char* names[]={"notes","jumps","holds","mines","hands","rolls","lifts"};
			RadarCategory categories[]={RadarCategory_TapsAndHolds,RadarCategory_Jumps,RadarCategory_Holds,RadarCategory_Mines,RadarCategory_Hands,RadarCategory_Rolls,RadarCategory_Lifts};
			for(int i=0;i<7;++i) c["radar"][names[i]]=int(std::max(0.f,steps->GetRadarValues(PLAYER_1)[categories[i]]));
			const char* normalized[]={"stream","voltage","air","freeze","chaos"};
			for(int i=0;i<5;++i) c["radar"][normalized[i]]=std::max(0.f,steps->GetRadarValues(PLAYER_1)[RadarCategory(i)]);
			m["charts"].append(c);
		}
		out.songs.append(m);
	}
	GetDirListing("/Save/Upload/*.xml",out.logs,false,true);
	return out;
}
static void AddFile(Snapshot& snapshot,const RString& remote,const RString& text)
{
	RString local=ROOT+"outbox/"+remote;
	Write(local,text); snapshot.files[remote]=local;
}
void Export(Snapshot& s,const Progress& progress,const std::atomic<bool>& cancel)
{
	const RString prefix="profiles/"+s.guid+"/";
	AddFile(s,prefix+"Stats.xml",s.stats);
	// Previously exported events remain in the outbox even if Upload logs were archived.
	vector<RString> old; GetDirListing(ROOT+"outbox/"+prefix+"events/*.xml",old,false,true);
	for(const auto& path: old) s.files[prefix+"events/"+Basename(path)]=path;
	unsigned skipped=0;
	for(size_t i=0;i<s.logs.size();++i)
	{
		CheckCancel(cancel);
		if(i%100==0) progress(ssprintf("Preparing play history: %u / %u",unsigned(i),unsigned(s.logs.size())));
		XNode root; RString error; XmlFileUtil::Load(&root,Read(s.logs[i],128*1024),error);
		Require(error.empty(), "A play log could not be read. Repair or restore Save/Upload before uploading.");
		const XNode* recent=root.GetChild("RecentSongScores"); if(!recent) continue;
		FOREACH_CONST_Child(recent, record)
		{
			const XNode* hs=record->GetChild("HighScore"); if(!hs) continue;
			RString who,guid; hs->GetChildValue("PlayerGuid",who); hs->GetChildValue("Guid",guid); guid=ExportText(guid);
			if(!s.machine && who!=s.guid) { ++skipped; continue; }
			std::unique_ptr<XNode> clean(Clean(*record));
			if(guid.empty()) { guid="legacy-"+Hash(XmlFileUtil::GetXML(clean.get())); clean->GetChild("HighScore")->AppendChild("Guid",guid); }
			XNode event("Stats"); event.AppendChild("RecentSongScores")->AppendChild(clean.release());
			AddFile(s,prefix+"events/"+Hash(guid)+".xml",XmlFileUtil::GetXML(&event));
		}
	}
	for(unsigned i=0;i<s.songs.size();++i)
	{
		CheckCancel(cancel); progress(ssprintf("Preparing song banners: %u / %u",i+1,s.songs.size()));
		Json::Value m=s.songs[i]; RString source=m["source_banner"].asString(); m.removeMember("source_banner");
		m["banner"]="";
		const RString metaPath=prefix+"songs/"+Hash(m["dir"].asString())+".json";
		// Keep a previously exported banner if the source is no longer available.
		if(FILEMAN->IsAFile(ROOT+"outbox/"+metaPath)) m["banner"]=Parse(Read(ROOT+"outbox/"+metaPath)).get("banner","");
		if(!source.empty())
		{
			RString err; RageSurface* surface=nullptr;
			RString cached=SongCacheIndex::GetCacheFilePath("Banners",source);
			if(FILEMAN->IsAFile(cached)) surface=RageSurfaceUtils::LoadSurface(cached);
			if(!surface && FILEMAN->IsAFile(source))
			{
				RString ext=GetExtension(source); ext.MakeLower();
				if(ext=="png" || ext=="jpg" || ext=="jpeg" || ext=="bmp" || ext=="gif")
					surface=RageSurfaceUtils::LoadFile(source,err);
			}
			if(surface)
			{
				float scale=std::min(1.f,160.f/std::max(surface->w,surface->h));
				RageSurfaceUtils::Zoom(surface,std::max(1,int(surface->w*scale)),std::max(1,int(surface->h*scale)));
				const RString temp=ROOT+"banner.png";
				RageFile f; bool ok=f.Open(temp,RageFile::WRITE) && RageSurfaceUtils::SavePNG(surface,f,err); f.Close(); delete surface;
				if(ok)
				{
					RString png=Read(temp,256*1024), hash=Hash(png);
					Json::Value envelope; envelope["png_base64"]=Base64(png);
					AddFile(s,"banners/"+hash+".json",Json(envelope));
					m["banner"]="banners/"+hash+".png";
				}
			}
		}
		AddFile(s,metaPath,Json(m));
	}
	// Retain metadata and image copies for historical songs removed from Songs/.
	for(const RString& dir: {prefix+"songs/"})
	{
		vector<RString> files; GetDirListing(ROOT+"outbox/"+dir+"*.json",files,false,true);
		for(const auto& path:files)
		{
			s.files[dir+Basename(path)]=path;
			RString banner=Parse(Read(path)).get("banner","").asString();
			if(std::regex_match(banner,std::regex("banners/[0-9a-f]{64}\\.png")))
			{
				RString envelope=banner.substr(0,banner.size()-4)+".json";
				if(FILEMAN->IsAFile(ROOT+"outbox/"+envelope)) s.files[envelope]=ROOT+"outbox/"+envelope;
			}
		}
	}
	Json::Value manifest; manifest["version"]=1; manifest["profile_guid"]=s.guid;
	manifest["profile_kind"]=s.machine?"machine":"local";
	unsigned events=0; for(const auto& f:s.files) if(f.first.find(prefix+"events/")==0) ++events;
	manifest["event_count"]=events;
	AddFile(s,prefix+"manifest.json",Json(manifest));
	Json::Value pending; pending["repository"]=s.settings.repository; pending["branch"]=s.settings.branch;
	pending["profile_guid"]=s.guid; pending["events"]=events; pending["skipped_other_profiles"]=skipped;
	Write(ROOT+"pending.json",Json(pending));
}
static RString HttpError(int code)
{
	if(code==401) return "GitHub rejected the credential. Set a valid, unexpired token.";
	if(code==403 || code==429) return "GitHub denied the request or applied a rate limit. Check Contents: write permission, then retry later.";
	if(code==404) return "GitHub repository or branch was not found. Check the repository and token access.";
	return "GitHub request failed (HTTP "+std::to_string(code)+"). Retry from this screen.";
}
RString Publish(const Snapshot& s,const Request& request,const Progress& progress,const std::atomic<bool>& cancel)
{
	ValidateSettings(s.settings);
	const RString base="/repos/"+s.settings.repository;
	auto call=[&](const RString& method,const RString& path,const Json::Value& data=Json::Value()) {
		CheckCancel(cancel); Response r=request(method,base+path,data.isNull()?RString():Json(data));
		Require(r.code>=200 && r.code<300,HttpError(r.code));
		return r.body.empty()?Json::Value():Parse(r.body);
	};
	for(int attempt=0;attempt<3;++attempt)
	{
		progress("Checking uploaded statistics..."); CheckCancel(cancel);
		Response ref=request("GET",base+"/git/ref/heads/"+s.settings.branch,"");
		Require(ref.code==404 || ref.code==200,HttpError(ref.code));
		RString head,tree; std::map<RString,RString> remote;
		if(ref.code==200)
		{
			head=Parse(ref.body)["object"]["sha"].asString();
			Json::Value commit=call("GET","/git/commits/"+head); tree=commit["tree"]["sha"].asString();
			Json::Value listing=call("GET","/git/trees/"+tree+"?recursive=1");
			Require(!listing.get("truncated",false).asBool(),"Dashboard data tree is too large; migration to a larger store is required.");
			for(const auto& entry:listing["tree"]) if(entry["type"].asString()=="blob") remote[entry["path"].asString()]=entry["sha"].asString();
		}
		// Cumulative counters must never move backwards, even before deployment.
		const RString statsPath="profiles/"+s.guid+"/Stats.xml";
		if(remote.count(statsPath) && remote[statsPath]!=GitHash(s.stats))
		{
			Json::Value blob=call("GET","/git/blobs/"+remote[statsPath]);
			XNode old,newer; RString error;
			XmlFileUtil::Load(&old,Unbase64(blob["content"].asString()),error);
			Require(error.empty(),"Remote profile snapshot is invalid.");
			XmlFileUtil::Load(&newer,s.stats,error); Require(error.empty(),"Local profile snapshot is invalid.");
			const XNode* a=old.GetChild("GeneralData"); const XNode* b=newer.GetChild("GeneralData");
			Require(a && b,"Profile snapshot is missing totals.");
			for(const RString& key:Words("NumTotalSongsPlayed TotalTapsAndHolds TotalGameplaySeconds TotalSessions"))
			{
				int before=0,after=0; a->GetChildValue(key,before); b->GetChildValue(key,after);
				Require(after>=before,"Local history is older than the uploaded profile. Restore the complete Save folder before uploading.");
			}
		}
		Json::Value entries(Json::arrayValue); size_t bytes=0; bool changed=false;
		auto flush=[&]() {
			if(entries.empty()) return;
			Json::Value data; if(!tree.empty()) data["base_tree"]=tree; data["tree"]=entries;
			tree=call("POST","/git/trees",data)["sha"].asString();
			Require(!tree.empty(),"GitHub did not return the uploaded tree.");
			entries=Json::Value(Json::arrayValue); bytes=0; changed=true;
		};
		unsigned count=0;
		for(const auto& file:s.files)
		{
			CheckCancel(cancel); RString content=Read(file.second);
			if(remote[file.first]==GitHash(content)) continue;
			// Events are immutable; an existing ID must never acquire another score.
			Require(remote[file.first].empty() || file.first.find("/events/")==RString::npos,"A play ID conflicts with previously uploaded data.");
			Json::Value entry; entry["path"]=file.first; entry["mode"]="100644"; entry["type"]="blob";
			if(content.size()>400*1024)
			{
				Json::Value blob; blob["encoding"]="utf-8"; blob["content"]=content;
				entry["sha"]=call("POST","/git/blobs",blob)["sha"];
			}
			else entry["content"]=content;
			bytes+=content.size(); entries.append(entry); ++count;
			progress(ssprintf("Uploading new or changed files: %u",count));
			if(entries.size()>=100 || bytes>=400*1024) flush();
		}
		flush();
		if(changed)
		{
			Json::Value commit; commit["message"]="Update StepMania statistics"; commit["tree"]=tree;
			commit["parents"]=Json::Value(Json::arrayValue); if(!head.empty()) commit["parents"].append(head);
			RString next=call("POST","/git/commits",commit)["sha"].asString();
			Json::Value update; update["sha"]=next; update["force"]=false;
			RString path="/git/refs/heads/"+s.settings.branch, method="PATCH";
			if(head.empty()) { method="POST"; path="/git/refs"; update.removeMember("force"); update["ref"]="refs/heads/"+s.settings.branch; }
			CheckCancel(cancel); Response result=request(method,base+path,Json(update));
			if(result.code==409 || result.code==422) continue;
			Require(result.code>=200 && result.code<300,HttpError(result.code)); head=next;
		}
		Require(!head.empty(),"The export contains no statistics.");
        if(!changed && FILEMAN->IsAFile(ROOT+"last-upload.json"))
        {
            Json::Value last=Parse(Read(ROOT+"last-upload.json"));
            if(last.get("published",false).asBool() && last["revision"].asString()==head
                && last["repository"].asString()==s.settings.repository && last["branch"].asString()==s.settings.branch)
            {
                progress("Nothing new to upload.\nLast published: "+last.get("published_at","").asString());
                return "";
            }
        }
		Json::Value state; state["repository"]=s.settings.repository; state["branch"]=s.settings.branch;
		state["profile_guid"]=s.guid; state["revision"]=head; state["published"]=false;
		Write(ROOT+"last-upload.json",Json(state));
		progress(changed?"Upload accepted. Starting dashboard publication...":"No new files. Checking dashboard publication...");
		Json::Value dispatch; dispatch["event_type"]="stepmania_upload";
		dispatch["client_payload"]["data_branch"]=s.settings.branch;
		dispatch["client_payload"]["data_revision"]=head;
		dispatch["client_payload"]["profile"]=s.guid;
        try { call("POST","/dispatches",dispatch); }
        catch(const std::exception& e)
        { throw std::runtime_error("Upload accepted, but publication could not be started.\n"+RString(e.what())); }
		return head;
	}
	throw std::runtime_error("The data branch changed during upload. Select Retry; no plays were discarded.");
}

#ifdef _WIN32
static std::wstring Wide(const RString& s) { return std::wstring(s.begin(),s.end()); } // Settings/token/URLs validated ASCII.
struct Internet
{
	HINTERNET handle; explicit Internet(HINTERNET h):handle(h) {}
	~Internet() { if(handle) WinHttpCloseHandle(handle); }
	operator HINTERNET() const { return handle; }
};
static RString ReadCredential(const Settings& s)
{
	PCREDENTIALW cred=nullptr;
	Require(CredReadW(Wide(s.credential).c_str(),CRED_TYPE_GENERIC,0,&cred),"No GitHub credential found. Choose Set GitHub Token on this screen.");
	RString token;
	const wchar_t* blob=reinterpret_cast<const wchar_t*>(cred->CredentialBlob);
	for(DWORD i=0;i<cred->CredentialBlobSize/sizeof(wchar_t);++i) if(blob[i]) token+=char(blob[i]);
	SecureZeroMemory(cred->CredentialBlob,cred->CredentialBlobSize); CredFree(cred);
	Require(std::regex_match(token,std::regex("[A-Za-z0-9_]{20,255}")),"Stored token is invalid. Choose Set GitHub Token again.");
	return token;
}
static Response Http(const RString& method,const RString& path,const RString& body,const RString& token)
{
	Internet session(WinHttpOpen(L"StepMania-Dashboard/1",WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,WINHTTP_NO_PROXY_NAME,WINHTTP_NO_PROXY_BYPASS,0));
	Require(session.handle!=nullptr,"Windows could not initialize HTTPS.");
	WinHttpSetTimeouts(session,5000,5000,10000,10000);
	Internet connection(WinHttpConnect(session,L"api.github.com",INTERNET_DEFAULT_HTTPS_PORT,0));
	Require(connection.handle!=nullptr,"Cannot connect to GitHub.");
	Internet req(WinHttpOpenRequest(connection,Wide(method).c_str(),Wide(path).c_str(),nullptr,WINHTTP_NO_REFERER,WINHTTP_DEFAULT_ACCEPT_TYPES,WINHTTP_FLAG_SECURE));
	Require(req.handle!=nullptr,"Cannot create GitHub HTTPS request.");
	DWORD redirects=WINHTTP_OPTION_REDIRECT_POLICY_NEVER;
	WinHttpSetOption(req,WINHTTP_OPTION_REDIRECT_POLICY,&redirects,sizeof(redirects));
	RString headers="Accept: application/vnd.github+json\r\nContent-Type: application/json\r\nX-GitHub-Api-Version: 2022-11-28\r\n";
	if(!token.empty()) headers+="Authorization: Bearer "+token+"\r\n";
	std::wstring wideHeaders=Wide(headers);
	bool sent=WinHttpSendRequest(req,wideHeaders.c_str(),DWORD(wideHeaders.size()),body.empty()?WINHTTP_NO_REQUEST_DATA:const_cast<char*>(body.data()),DWORD(body.size()),DWORD(body.size()),0)!=FALSE;
	std::fill(headers.begin(),headers.end(),'\0'); std::fill(wideHeaders.begin(),wideHeaders.end(),L'\0');
	Require(sent && WinHttpReceiveResponse(req,nullptr),"Network or TLS connection failed. Check your connection, then select Retry.");
	DWORD code=0,size=sizeof(code);
	Require(WinHttpQueryHeaders(req,WINHTTP_QUERY_STATUS_CODE|WINHTTP_QUERY_FLAG_NUMBER,WINHTTP_HEADER_NAME_BY_INDEX,&code,&size,WINHTTP_NO_HEADER_INDEX)!=FALSE,"Missing GitHub response status.");
	Response response; response.code=int(code); char buffer[16384]; DWORD got;
	for(;;)
	{
		Require(WinHttpReadData(req,buffer,sizeof(buffer),&got)!=FALSE,"Connection interrupted while reading GitHub's response.");
		if(!got) break;
		response.body.append(buffer,got);
		Require(response.body.size()<=32*1024*1024,"GitHub response is too large.");
	}
	return response;
}
#endif
bool SaveCredential(const RString& token,RString& error)
{
#ifdef _WIN32
	if(!std::regex_match(token,std::regex("[A-Za-z0-9_]{20,255}"))) { error="Enter a valid fine-grained GitHub token."; return false; }
	Settings s=GetSettings(); std::wstring target=Wide(s.credential), secret=Wide(token), user=L"GitHub";
	CREDENTIALW cred={}; cred.Type=CRED_TYPE_GENERIC; cred.TargetName=const_cast<wchar_t*>(target.c_str());
	cred.UserName=const_cast<wchar_t*>(user.c_str()); cred.CredentialBlobSize=DWORD(secret.size()*sizeof(wchar_t));
	cred.CredentialBlob=reinterpret_cast<LPBYTE>(&secret[0]); cred.Persist=CRED_PERSIST_LOCAL_MACHINE;
	bool ok=CredWriteW(&cred,0)!=FALSE; SecureZeroMemory(&secret[0],secret.size()*sizeof(wchar_t));
	if(!ok) error="Windows could not save the credential."; return ok;
#else
	error="Integrated upload and credential storage currently require Windows."; return false;
#endif
}
Job::~Job() { m_Cancel=true; if(m_Thread.joinable()) m_Thread.join(); }
RString Job::Status() const { return GetStatus().text; }
StatusReport Job::GetStatus() const { std::lock_guard<std::mutex> lock(m_Mutex); return m_Status; }
void Job::SetStatus(const RString& text, StatusKind kind)
{ std::lock_guard<std::mutex> lock(m_Mutex); m_Status.text=text; m_Status.kind=kind; }
bool Job::Start()
{
	if(m_Busy) return false;
	if(m_Thread.joinable()) m_Thread.join();
	try
	{
		Snapshot snapshot=Capture(); m_Cancel=false; m_Busy=true;
		SetStatus("Preparing upload...\nProfile: "+(snapshot.name.empty()?RString(snapshot.machine?"Machine profile":"Selected local profile"):snapshot.name));
		m_Thread=std::thread(&Job::Run,this,std::move(snapshot));
		return true;
	}
	catch(const std::exception& e) { m_Busy=false; SetStatus(e.what(), StatusKind::Error); return false; }
}
void Job::Run(Snapshot s)
{
	RageThreadRegister registerThread("Dashboard upload");
	try
	{
		Progress progress=[this](const RString& t){ SetStatus(t); };
		Export(s,progress,m_Cancel);
#ifdef _WIN32
		RString token=ReadCredential(s.settings);
		// GitHub applies secondary write limits. A first historical import can
        // require hundreds of tree requests; pace them so it can finish.
        auto nextWrite=std::chrono::steady_clock::now();
        Request request=[&](const RString& method,const RString& path,const RString& body) {
            if(method!="GET")
            {
                while(std::chrono::steady_clock::now()<nextWrite)
                { CheckCancel(m_Cancel); std::this_thread::sleep_for(std::chrono::milliseconds(100)); }
                nextWrite=std::chrono::steady_clock::now()+std::chrono::milliseconds(1200);
            }
            return Http(method,path,body,token);
        };
		RString revision;
		try { revision=Publish(s,request,progress,m_Cancel); }
		catch(...) { std::fill(token.begin(),token.end(),'\0'); throw; }
		std::fill(token.begin(),token.end(),'\0');
		if(revision.empty()) { SetStatus(Status(), StatusKind::Success); m_Busy=false; return; }
		SetStatus("Upload accepted. GitHub is building the dashboard.\nYou may return to the menu; publication continues online.");
		// Public workflow status does not require Actions permission on the token.
		bool finished=false;
		for(int poll=0;poll<40;++poll)
		{
			for(int tick=0;tick<100;++tick) { if(m_Cancel) break; std::this_thread::sleep_for(std::chrono::milliseconds(100)); }
			if(m_Cancel) break;
			Response response=Http("GET","/repos/"+s.settings.repository+"/actions/workflows/dashboard-sync.yml/runs?event=repository_dispatch&per_page=20","","");
			if(response.code!=200) continue;
			Json::Value runs=Parse(response.body);
			for(const auto& run:runs["workflow_runs"])
			{
				if(run.get("display_title","").asString()!="Dashboard upload "+revision) continue;
				if(run["status"].asString()!="completed") break;
				if(run["conclusion"].asString()=="success")
				{
					SetStatus("Dashboard published successfully.", StatusKind::Success);
					Json::Value state=Parse(Read(ROOT+"last-upload.json")); state["published"]=true; state["published_at"]=run["updated_at"];
					Write(ROOT+"last-upload.json",Json(state));
				}
				else SetStatus("Dashboard publication needs attention.\nUpload accepted, but publication did not finish. Open Build Log for details, then select Retry.", StatusKind::Warning);
				finished=true; break;
			}
			if(finished) break;
		}
		if(!finished) SetStatus("Publication not yet confirmed.\nUpload accepted. Open Build Log to check progress; GitHub may still be building the dashboard.", StatusKind::Warning);
#else
		SetStatus("Statistics exported locally.\nSaved to Save/Dashboard/outbox. Integrated GitHub upload currently requires Windows.", StatusKind::Warning);
#endif
	}
	catch(const std::exception& e) { SetStatus(e.what(), m_Cancel ? StatusKind::Warning : StatusKind::Error); }
	m_Busy=false;
}
}
