#include "global.h"
#include "ReplayData.h"
#include "CryptManager.h"
#include "PlayerStageStats.h"
#include "RageFile.h"
#include "RageFileManager.h"
#include "RageUtil.h"
#include "TimingData.h"
#include <cmath>
#include <set>

namespace
{
bool Number(const Json::Value& v, double lo, double hi)
{
	return v.isNumeric() && std::isfinite(v.asDouble()) &&
		v.asDouble() >= lo && v.asDouble() <= hi;
}
bool Integer(const Json::Value& v, int lo, int hi)
{
	return v.isInt() && v.asInt() >= lo && v.asInt() <= hi;
}
bool Text(const Json::Value& v, unsigned limit)
{
	return v.isString() && v.asString().size() <= limit;
}
bool Array(const Json::Value& v, unsigned size)
{
	return v.isArray() && v.size() == size;
}
const unsigned SCORE_SIZE = 17 + NUM_TapNoteScore + NUM_HoldNoteScore;
}

Json::Value ReplayData::Chart(const NoteData& notes)
{
	Json::Value result(Json::arrayValue);
	for(int track = 0; track < notes.GetNumTracks(); ++track)
		for(auto it = notes.begin(track); it != notes.end(track); ++it)
		{
			const TapNote& tn = it->second;
			Json::Value n(Json::arrayValue);
			n.append(track); n.append(it->first); n.append(int(tn.type));
			n.append(int(tn.subType)); n.append(int(tn.source)); n.append(tn.iDuration);
			result.append(n);
		}
	return result;
}

void ReplayData::RestoreChart(const Json::Value& chart, int tracks, NoteData& notes)
{
	notes.ClearAll();
	notes.SetNumTracks(tracks);
	for(const auto& n: chart)
	{
		TapNote tn;
		tn.type = TapNoteType(n[2].asInt());
		tn.subType = TapNoteSubType(n[3].asInt());
		tn.source = TapNoteSource(n[4].asInt());
		tn.iDuration = n[5].asInt();
		notes.SetTapNote(n[0].asInt(), n[1].asInt(), tn);
	}
}

RString ReplayData::Fingerprint(const NoteData& notes, const TimingData& timing)
{
	Json::FastWriter writer;
	RString canonical = writer.write(Chart(notes));
	canonical += ssprintf("tracks=%d;offset=%.9g;", notes.GetNumTracks(), timing.m_fBeat0OffsetInSeconds);
	for(int type = 0; type < NUM_TimingSegmentType; ++type)
	{
		canonical += ssprintf("type=%d;", type);
		for(const auto* segment: timing.GetTimingSegments(TimingSegmentType(type)))
			canonical += segment->ToString(9) + ";";
	}
	return BinaryToHex(CryptManager::GetSHA256ForString(canonical));
}

Json::Value ReplayData::Note(int track, int row, const TapNote& tn)
{
	Json::Value n(Json::arrayValue);
	n.append(track); n.append(row); n.append(int(tn.result.tns));
	n.append(tn.result.fTapNoteOffset); n.append(tn.result.bHidden); n.append(tn.result.bHeld);
	n.append(int(tn.HoldResult.hns)); n.append(tn.HoldResult.fLife);
	n.append(tn.HoldResult.iLastHeldRow); n.append(tn.HoldResult.bHeld); n.append(tn.HoldResult.bActive);
	n.append(tn.HoldResult.iCheckpointsHit); n.append(tn.HoldResult.iCheckpointsMissed);
	n.append(tn.HoldResult.fOverlappedTime);
	return n;
}

void ReplayData::RestoreNote(const Json::Value& n, TapNote& tn)
{
	tn.result.tns = TapNoteScore(n[2].asInt());
	tn.result.fTapNoteOffset = n[3].asFloat();
	tn.result.bHidden = n[4].asBool(); tn.result.bHeld = n[5].asBool();
	tn.HoldResult.hns = HoldNoteScore(n[6].asInt()); tn.HoldResult.fLife = n[7].asFloat();
	tn.HoldResult.iLastHeldRow = n[8].asInt();
	tn.HoldResult.bHeld = n[9].asBool(); tn.HoldResult.bActive = n[10].asBool();
	tn.HoldResult.iCheckpointsHit = n[11].asInt(); tn.HoldResult.iCheckpointsMissed = n[12].asInt();
	tn.HoldResult.fOverlappedTime = n[13].asFloat();
}

Json::Value ReplayData::Score(const PlayerStageStats& s)
{
	Json::Value v(Json::arrayValue);
	v.append(s.m_iPossibleDancePoints); v.append(s.m_iCurPossibleDancePoints);
	v.append(s.m_iActualDancePoints); v.append(s.m_iPossibleGradePoints);
	v.append(s.m_iCurCombo); v.append(s.m_iMaxCombo); v.append(s.m_iCurMissCombo);
	v.append(s.m_iCurScoreMultiplier); v.append(s.m_iScore);
	v.append(s.m_iCurMaxScore); v.append(s.m_iMaxScore);
	v.append(s.m_bFailed ? 1 : 0); v.append(s.m_bDisqualified ? 1 : 0);
	v.append(s.m_bPlayerCanAchieveFullCombo ? 1 : 0);
	v.append(s.m_iSongsPassed); v.append(s.m_iSongsPlayed); v.append(s.m_iStepsPlayed);
	for(int i = 0; i < NUM_TapNoteScore; ++i) v.append(s.m_iTapNoteScores[i]);
	for(int i = 0; i < NUM_HoldNoteScore; ++i) v.append(s.m_iHoldNoteScores[i]);
	return v;
}

void ReplayData::RestoreScore(const Json::Value& v, PlayerStageStats& s)
{
	s.m_iPossibleDancePoints = v[0].asInt(); s.m_iCurPossibleDancePoints = v[1].asInt();
	s.m_iActualDancePoints = v[2].asInt(); s.m_iPossibleGradePoints = v[3].asInt();
	s.m_iCurCombo = v[4].asUInt(); s.m_iMaxCombo = v[5].asUInt(); s.m_iCurMissCombo = v[6].asUInt();
	s.m_iCurScoreMultiplier = v[7].asInt(); s.m_iScore = v[8].asUInt();
	s.m_iCurMaxScore = v[9].asUInt(); s.m_iMaxScore = v[10].asUInt();
	s.m_bFailed = v[11].asInt() != 0; s.m_bDisqualified = v[12].asInt() != 0;
	s.m_bPlayerCanAchieveFullCombo = v[13].asInt() != 0;
	s.m_iSongsPassed = v[14].asInt(); s.m_iSongsPlayed = v[15].asInt(); s.m_iStepsPlayed = v[16].asInt();
	for(int i = 0; i < NUM_TapNoteScore; ++i) s.m_iTapNoteScores[i] = v[17+i].asInt();
	for(int i = 0; i < NUM_HoldNoteScore; ++i) s.m_iHoldNoteScores[i] = v[17+NUM_TapNoteScore+i].asInt();
}

bool ReplayData::Validate(RString& error) const
{
	auto fail = [&](const char* why) { error = why; return false; };
	if(!root.isObject() || !Integer(root["version"], VERSION, VERSION))
		return fail("Unsupported replay format (this player reads version 1 .smr.json files).");
	for(const char* key: {"song", "title", "artist", "recorded_at", "build", "style", "player_options", "song_options", "chart_hash", "outcome"})
		if(!Text(root[key], 4096)) return fail("Missing or invalid replay header.");
	if(root["chart_hash"].asString().size() != 64 ||
		!Integer(root["tracks"], 1, MAX_NOTE_TRACKS) || !Integer(root["player"], 0, NUM_PLAYERS-1) ||
		!Integer(root["seed"], 0, INT_MAX) || !Number(root["initial_rate"], .01, 10) ||
		!Number(root["global_offset"], -10, 10)) return fail("Invalid replay settings.");
	if(root.isMember("steps_type") && !Text(root["steps_type"], 64)) return fail("Invalid replay chart type.");
	if(root.isMember("stage_index") && !Integer(root["stage_index"], 0, INT_MAX)) return fail("Invalid replay stage index.");
	if(root.isMember("autoplay") && !root["autoplay"].isBool()) return fail("Invalid replay controller flag.");
	const int tracks = root["tracks"].asInt();
	const auto& chart = root["chart"];
	if(!chart.isArray() || chart.size() > MAX_NOTES) return fail("Invalid or excessively large replay chart.");
	std::map<std::pair<int,int>, int> positions;
	for(const auto& n: chart)
	{
		if(!Array(n, 6) || !Integer(n[0], 0, tracks-1) || !Integer(n[1], 0, MAX_NOTE_ROW/2) ||
			!Integer(n[2], TapNoteType_Tap, TapNoteType_Fake) || n[2].asInt() == TapNoteType_Attack ||
			n[2].asInt() == TapNoteType_AutoKeysound || !Integer(n[3], 0, TapNoteSubType_Invalid) ||
			!Integer(n[4], 0, NUM_TapNoteSource-1) || !Integer(n[5], 0, MAX_NOTE_ROW/2))
			return fail("Invalid replay note.");
		if(n[2].asInt() == TapNoteType_HoldHead &&
			(n[5].asInt() == 0 || n[3].asInt() >= NUM_TapNoteSubType)) return fail("Invalid replay hold.");
		if(!positions.emplace(std::make_pair(n[0].asInt(), n[1].asInt()), n[2].asInt()).second)
			return fail("Duplicate replay note.");
	}
	const auto& frames = root["frames"];
	if(!frames.isArray() || frames.empty() || frames.size() > MAX_FRAMES)
		return fail("Empty or excessively large replay timeline.");
	std::map<std::pair<int,int>, int> holdResults;
	double previous = -3600;
	unsigned events = 0;
	for(const auto& f: frames)
	{
		// music, steps time, alive time, gameplay time, rate, life, pressed mask,
		// optional score, changed note results, judgments, inputs, life history.
		if(!Array(f, 12) || !Number(f[0], previous, 86400) || !Number(f[1], 0, 86400) ||
			!Number(f[2], 0, 86400) || !Number(f[3], 0, 86400) || !Number(f[4], .01, 10) ||
			!Number(f[5], 0, 1) || !Integer(f[6], 0, (1 << tracks)-1)) return fail("Invalid replay frame or time order.");
		previous = f[0].asDouble();
		if(!f[7].isNull())
		{
			if(!Array(f[7], SCORE_SIZE)) return fail("Invalid replay score.");
			for(unsigned i = 0; i < SCORE_SIZE; ++i)
			{
				bool unsignedField = (i >= 4 && i <= 6) || (i >= 8 && i <= 10);
				double upper = unsignedField ? double(UINT_MAX) : double(INT_MAX);
				if(i >= 11 && i <= 13) upper = 1;
				if(!Number(f[7][i], i == 2 ? -INT_MAX : 0, upper) ||
					(!f[7][i].isInt() && !f[7][i].isUInt())) return fail("Invalid replay score counter.");
			}
		}
		for(int i = 8; i < 12; ++i)
		{
			if(!f[i].isArray() || f[i].size() > MAX_NOTES) return fail("Invalid replay event list.");
			events += f[i].size();
			if(events > 2000000) return fail("Replay has too many events.");
		}
		for(const auto& n: f[8])
		{
			if(!Array(n, 14) || !Integer(n[0], 0, tracks-1) || !Integer(n[1], 0, MAX_NOTE_ROW/2) ||
				!positions.count({n[0].asInt(), n[1].asInt()}) || !Integer(n[2], 0, NUM_TapNoteScore-1) ||
				!Number(n[3], -60, 60) || !n[4].isBool() || !n[5].isBool() ||
				!Integer(n[6], 0, NUM_HoldNoteScore-1) || !Number(n[7], 0, 1) ||
				!Integer(n[8], 0, MAX_NOTE_ROW-1) || !n[9].isBool() || !n[10].isBool() ||
				!Integer(n[11], 0, INT_MAX) || !Integer(n[12], 0, INT_MAX) || !Number(n[13], 0, 86400))
				return fail("Invalid replay note result.");
			holdResults[{n[0].asInt(), n[1].asInt()}] = n[6].asInt();
		}
		for(const auto& j: f[9])
		{
			if(!Array(j, 5) || !Integer(j[0], 0, 2) || !Integer(j[1], 0, MAX_NOTE_ROW/2) ||
				!Integer(j[2], -1, tracks-1) || !Integer(j[3], 0, NUM_TapNoteScore-1) || !Number(j[4], -60, 60))
				return fail("Invalid replay judgment.");
			auto note = positions.find({j[2].asInt(), j[1].asInt()});
			if((j[0].asInt() != 0 && note == positions.end()) ||
				(j[0].asInt() == 1 && note->second != TapNoteType_HoldHead))
				return fail("Replay judgment refers to an absent or incompatible note.");
			if(j[0].asInt() == 1 && holdResults[{j[2].asInt(), j[1].asInt()}] == HNS_None)
				return fail("Replay hold judgment has no recorded result.");
		}
		for(const auto& input: f[10])
			if(!Array(input, 3) || !Integer(input[0], 0, tracks-1) || !input[1].isBool() || !Number(input[2], -3600, 86400))
				return fail("Invalid replay input.");
		for(const auto& life: f[11])
			if(!Array(life, 2) || !Number(life[0], 0, 86400) || !Number(life[1], 0, 1))
				return fail("Invalid replay life record.");
	}
	if(frames[0][7].isNull()) return fail("Replay has no initial score state.");
	error.clear();
	return true;
}

bool ReplayData::Parse(const RString& text, RString& error)
{
	root = Json::Value();
	if(text.size() > MAX_BYTES) { error = "Replay exceeds the 64 MiB limit."; return false; }
	// Bound nesting before entering the recursive JSON parser.
	int depth = 0; bool quoted = false, escaped = false;
	for(char c: text)
	{
		if(quoted) { if(escaped) escaped = false; else if(c == '\\') escaped = true; else if(c == '"') quoted = false; }
		else if(c == '"') quoted = true;
		else if(c == '[' || c == '{') { if(++depth > 16) { error = "Replay JSON is nested too deeply."; return false; } }
		else if(c == ']' || c == '}') --depth;
	}
	Json::Reader reader;
	if(!reader.parse(text, root, false)) { error = "Replay contains invalid JSON."; root = Json::Value(); return false; }
	if(!Validate(error)) { root = Json::Value(); return false; }
	return true;
}

bool ReplayData::Load(const RString& path, RString& error)
{
	RageFile file;
	if(!file.Open(path)) { error = "Cannot open replay: " + file.GetError(); return false; }
	int size = file.GetFileSize();
	if(size < 0 || size > int(MAX_BYTES)) { error = "Replay exceeds the 64 MiB limit."; return false; }
	RString text;
	if(file.Read(text, size) != size) { error = "Cannot read replay: " + file.GetError(); return false; }
	return Parse(text, error);
}

bool ReplayData::Save(const RString& path, RString& error) const
{
	if(!Validate(error)) return false;
	Json::FastWriter writer;
	RString text = writer.write(root);
	if(text.size() > MAX_BYTES) { error = "Replay exceeds the 64 MiB limit."; return false; }
	// RageFile's non-streamed writes use a temporary file and rename on close.
	RageFile file;
	if(!file.Open(path, RageFile::WRITE | RageFile::SLOW_FLUSH) ||
		file.Write(text) != int(text.size()) || file.Flush() < 0)
	{ error = "Cannot save replay: " + file.GetError(); return false; }
	file.Close();
	RageFile verify;
	if(!verify.Open(path) || verify.GetFileSize() != int(text.size()))
	{ error = "Replay write did not complete."; return false; }
	error.clear();
	return true;
}
