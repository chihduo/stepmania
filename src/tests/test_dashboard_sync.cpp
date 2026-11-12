// Real serializer/filesystem and an in-memory GitHub API. Never contacts GitHub.
#include "global.h"
#include "DashboardSync.h"
#include "ActorUtil.h"
#include "CryptManager.h"
#include "LuaManager.h"
#include "RageFile.h"
#include "RageFileManager.h"
#include "RageLog.h"
#include "RageThreads.h"
#include "RageUtil.h"
#include "XmlFile.h"
#include "XmlFileUtil.h"
#include "arch/ArchHooks/ArchHooks.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <stdexcept>
using namespace Dashboard;
static Json::Value parse(const RString& s) { Json::Value v;Json::Reader r;assert(r.parse(s,v));return v; }
static RString json(const Json::Value& v) { return Json::FastWriter().write(v); }
static RString gitHash(const RString& s) { RString prefix="blob "+std::to_string(s.size());prefix.push_back('\0');return BinaryToHex(CryptManager::GetSHA1ForString(prefix+s)); }
static void write(const RString& path,const RString& value) { RageFile f;assert(f.Open(path,RageFile::WRITE));assert(f.Write(value)==int(value.size())); }
static RString read(const RString& path) { RageFile f;assert(f.Open(path,RageFile::READ));RString s;assert(f.Read(s,f.GetFileSize())==f.GetFileSize());return s; }
struct GitHub
{
	RString head; int serial=0,commitsMade=0,dispatches=0; bool conflict=false,failDispatch=false,deny=false;
	std::map<RString,std::map<RString,RString>> trees;
	std::map<RString,RString> commits,blobs;
	Response operator()(const RString& method,const RString& path,const RString& body)
	{
		Response out;out.code=200;Json::Value v;
		if(deny) { out.code=401;return out; }
		RString p=path.substr(RString("/repos/test/dashboard").size());
		Json::Value data=body.empty()?Json::Value():parse(body);
		if(method=="GET" && p=="/git/ref/heads/dashboard-data") { if(head.empty()) out.code=404; else v["object"]["sha"]=head; }
		else if(method=="GET" && p.find("/git/commits/")==0) v["tree"]["sha"]=commits.at(p.substr(13));
		else if(method=="GET" && p.find("/git/trees/")==0)
		{
			RString id=p.substr(11);id=id.substr(0,id.find('?'));v["truncated"]=false;
			v["tree"]=Json::Value(Json::arrayValue);
			for(auto f:trees.at(id)) { Json::Value e;e["path"]=f.first;e["sha"]=f.second;e["type"]="blob";v["tree"].append(e); }
		}
		else if(method=="GET" && p.find("/git/blobs/")==0) v["content"]=Base64(blobs.at(p.substr(11)));
		else if(method=="POST" && p=="/git/trees")
		{
			std::map<RString,RString> tree;
			if(data.isMember("base_tree")) tree=trees.at(data["base_tree"].asString());
			for(const auto& e:data["tree"])
			{
				RString hash=e["sha"].asString();
				if(e.isMember("content")) { RString content=e["content"].asString();hash=gitHash(content);blobs[hash]=content; }
				tree[e["path"].asString()]=hash;
			}
			RString id="tree"+std::to_string(++serial);trees[id]=tree;v["sha"]=id;out.code=201;
		}
		else if(method=="POST" && p=="/git/blobs") { RString content=data["content"].asString(),hash=gitHash(content);blobs[hash]=content;v["sha"]=hash;out.code=201; }
		else if(method=="POST" && p=="/git/commits") { RString id="commit"+std::to_string(++serial);commits[id]=data["tree"].asString();v["sha"]=id;++commitsMade;out.code=201; }
		else if((method=="POST" && p=="/git/refs") || (method=="PATCH" && p=="/git/refs/heads/dashboard-data"))
		{
			assert(!data.get("force",false).asBool());
			if(conflict) { conflict=false;out.code=409;return out; }
			head=data["sha"].asString();
		}
		else if(method=="POST" && p=="/dispatches") { ++dispatches;assert(data["client_payload"]["data_revision"].asString()==head);out.code=failDispatch?503:204; }
		else { std::cerr<<method<<" "<<p<<std::endl;assert(false); }
		out.body=json(v);return out;
	}
};
int main(int argc,char** argv)
{
	assert(argc==2);RageThreadRegister reg("Dashboard test");SetCommandlineArguments(argc,argv);
	HOOKS=ArchHooks::Create();HOOKS->Init();LUA=new LuaManager;
	FILEMAN=new RageFileManager(argv[0]);FILEMAN->Mount("dir",argv[1],"/");LOG=new RageLog;LOG->SetLogToDisk(false);CRYPTMAN=new CryptManager;ActorUtil::InitFileTypeLists();
	for(const RString& s:{RString(),RString("a"),RString("ab"),RString("abc"),RString("\0\xff",2)}) assert(Unbase64(Base64(s))==s);
	assert(Hash("abc")=="ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
	XNode original;RString error;
	XmlFileUtil::Load(&original,"<Stats><GeneralData><Guid>0123456789abcdef</Guid><NumTotalSongsPlayed>2</NumTotalSongsPlayed><DisplayName>A &amp; B \xf0\x9f\x8e\xb5 \xff</DisplayName><Secret>password</Secret></GeneralData><UserTable><key>private</key></UserTable><SongScores><Song Dir=\"Songs/Test/Song/\"><Steps StepsType=\"dance-single\" Difficulty=\"Hard\"><HighScoreList><NumTimesPlayed>2</NumTimesPlayed></HighScoreList></Steps></Song></SongScores></Stats>",error);
	RString sanitized=Sanitize(original);assert(utf8_is_valid(sanitized));assert(sanitized.find("\xf0\x9f\x8e\xb5")!=RString::npos);assert(sanitized.find("A &amp; B")!=RString::npos);assert(sanitized.find("password")==RString::npos && sanitized.find("UserTable")==RString::npos);
	Snapshot s;s.settings.enabled=true;s.settings.repository="test/dashboard";s.settings.branch="dashboard-data";s.settings.profile="local:00000000";s.settings.credential="test";
	s.guid="0123456789abcdef";s.name="Player";s.stats=sanitized;s.songs=Json::Value(Json::arrayValue);
	RString event="<Stats><RecentSongScores><HighScoreForASongAndSteps><Song Dir=\"Songs/Test/Song/\"/><Steps StepsType=\"dance-single\" Difficulty=\"Hard\"/><HighScore><Guid>one</Guid><PlayerGuid>0123456789abcdef</PlayerGuid><DateTime>2026-11-12 12:00:00</DateTime><Score>123</Score><Name>private</Name></HighScore></HighScoreForASongAndSteps></RecentSongScores></Stats>";
	write("/Save/Upload/one.xml",event);write("/Save/Upload/copy.xml",event);
	RString other=event;other.Replace("0123456789abcdef","ffffffffffffffff");write("/Save/Upload/other.xml",other);
    write("/Save/test-banner.png",Unbase64("iVBORw0KGgoAAAANSUhEUgAAACAAAAAICAIAAAAX52r4AAAAGUlEQVR4nGP8f+A0Ay0BE01NH7Vg1AKqAADJIQKaMYnVZAAAAABJRU5ErkJggg=="));
    Json::Value song;song["dir"]="Songs/Test/Song/";song["title"]="Example";song["artist"]="Artist";
    song["source_banner"]="/Save/test-banner.png";song["charts"]=Json::Value(Json::arrayValue);s.songs.append(song);
	s.logs={"/Save/Upload/one.xml","/Save/Upload/copy.xml","/Save/Upload/other.xml"};
	std::atomic<bool> cancel(false);auto progress=[](const RString&){};
	Export(s,progress,cancel);
	assert(s.files.size()==5); // snapshot, single unique play, manifest, metadata, banner
    assert(s.files.count("profiles/"+s.guid+"/songs/"+Hash("Songs/Test/Song/")+".json"));
	RString eventPath="profiles/"+s.guid+"/events/"+Hash("one")+".xml";
	assert(read(s.files.at(eventPath)).find("private")==RString::npos);
    if(const char* target=std::getenv("DASHBOARD_CONTRACT_EXPORT"))
    {
        Json::Value fixture;
        for(const auto& f:s.files) fixture[f.first]=read(f.second);
        std::ofstream output(target); output<<json(fixture);assert(output.good());
    }
	GitHub server;Request req=[&](const RString& m,const RString& p,const RString& b){return server(m,p,b);};
	RString first=Publish(s,req,progress,cancel);assert(!first.empty());assert(server.commitsMade==1 && server.dispatches==1);
	assert(Publish(s,req,progress,cancel)==first);assert(server.commitsMade==1 && server.dispatches==2);
	server.failDispatch=true;bool caught=false;try { Publish(s,req,progress,cancel); } catch(const std::exception&) { caught=true; }assert(caught);
	server.failDispatch=false;Publish(s,req,progress,cancel);assert(server.commitsMade==1);
	// A new record and ref conflict must preserve the original record.
	RString secondPath="profiles/"+s.guid+"/events/"+Hash("two")+".xml";
	event.Replace("<Guid>one", "<Guid>two");write("/Save/second.xml",event);s.files[secondPath]="/Save/second.xml";
	server.conflict=true;Publish(s,req,progress,cancel);auto tree=server.trees.at(server.commits.at(server.head));assert(tree.count(eventPath) && tree.count(secondPath));
	// A confirmed publication with no changed data does not trigger another build.
    Json::Value published=parse(read("/Save/Dashboard/last-upload.json"));published["published"]=true;published["published_at"]="2026-11-12T12:00:00Z";
    write("/Save/Dashboard/last-upload.json",json(published));int dispatched=server.dispatches;
    assert(Publish(s,req,progress,cancel).empty() && server.dispatches==dispatched);
    // Regression and changed event IDs must fail before updating a branch.
	RString head=server.head,stats=s.stats;s.stats.Replace("<NumTotalSongsPlayed>2", "<NumTotalSongsPlayed>1");
	caught=false;try { Publish(s,req,progress,cancel); } catch(const std::exception&) { caught=true; }assert(caught && server.head==head);s.stats=stats;
	write("/Save/second.xml",event+" ");caught=false;try { Publish(s,req,progress,cancel); } catch(const std::exception&) { caught=true; }assert(caught && server.head==head);
	server.deny=true;caught=false;try { Publish(s,req,progress,cancel); } catch(const std::exception&) { caught=true; }assert(caught && server.head==head);server.deny=false;
	cancel=true;caught=false;try { Publish(s,req,progress,cancel); } catch(const std::exception&) { caught=true; }assert(caught && server.head==head);
	// Cross-language fixture kept outside the temporary test root when requested.
	std::cout<<"PASS: export privacy/profile filtering, GUID preservation, repeat/dispatch retry, atomic conflicts, regression, auth and cancellation."<<std::endl;
	return 0;
}
