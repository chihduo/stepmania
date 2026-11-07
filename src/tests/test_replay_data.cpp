// Replay storage and compatibility checks against the actual engine types.
#include "global.h"
#include "ReplayData.h"
#include "CryptManager.h"
#include "LuaManager.h"
#include "MessageManager.h"
#include "PlayerStageStats.h"
#include "PrefsManager.h"
#include "RageFileManager.h"
#include "RageLog.h"
#include "RageThreads.h"
#include "TimingData.h"
#include "arch/ArchHooks/ArchHooks.h"
#include <cassert>
#include <iostream>

int main(int argc, char** argv)
{
	assert(argc == 2);
	RageThreadRegister thread("Replay storage test");
	SetCommandlineArguments(argc, argv);
	HOOKS = ArchHooks::Create(); HOOKS->Init();
	LUA = new LuaManager;
	FILEMAN = new RageFileManager(argv[0]); FILEMAN->Mount("dir", argv[1], "/");
	LOG = new RageLog; LOG->SetLogToDisk(false);
	MESSAGEMAN = new MessageManager;
	PREFSMAN = new PrefsManager;
	CRYPTMAN = new CryptManager;

	NoteData chart; chart.SetNumTracks(4);
	chart.SetTapNote(0, 48, TAP_ORIGINAL_TAP);
	TapNote hold = TAP_ORIGINAL_HOLD_HEAD; hold.iDuration = 96;
	chart.SetTapNote(1, 96, hold);
	chart.SetTapNote(2, 144, TAP_ORIGINAL_MINE);
	chart.SetTapNote(3, 192, TAP_ORIGINAL_LIFT);
	TimingData timing; timing.AddSegment(BPMSegment(0, 120));
	RString hash = ReplayData::Fingerprint(chart, timing);
	timing.AddSegment(StopSegment(96, .5f));
	assert(ReplayData::Fingerprint(chart, timing) != hash);
	assert(hash.size() == 64);

	PlayerStageStats stats;
	stats.m_iPossibleDancePoints = 12; stats.m_iActualDancePoints = 9;
	stats.m_iScore = 1234567; stats.m_iCurCombo = 3; stats.m_iMaxCombo = 5;
	stats.m_iTapNoteScores[TNS_W2] = 2;
	stats.m_iHoldNoteScores[HNS_Held] = 1;
	stats.m_bDisqualified = false;
	ReplayData replay;
	auto& r = replay.root;
	r["version"] = 1;
	for(const char* key: {"song", "title", "artist", "recorded_at", "build", "style", "player_options", "song_options", "outcome"}) r[key] = "fixture";
	r["chart_hash"] = hash; r["player"] = 0; r["tracks"] = 4; r["seed"] = 123;
	r["initial_rate"] = 1.0; r["global_offset"] = 0.0; r["chart"] = ReplayData::Chart(chart);
	Json::Value frame(Json::arrayValue);
	for(double number: {0., 0., 0., 0., 1., .5}) frame.append(number);
	frame.append(0); frame.append(ReplayData::Score(stats));
	for(int i = 0; i < 4; ++i) frame.append(Json::Value(Json::arrayValue));
	r["frames"].append(frame);
	hold.result.tns = TNS_W2; hold.result.fTapNoteOffset = -.025f;
	hold.HoldResult.hns = HNS_Held; hold.HoldResult.iLastHeldRow = 192;
	frame[0] = 2.; frame[1] = 1.5; frame[4] = .8;
	frame[8].append(ReplayData::Note(1, 96, hold));
	Json::Value judgment(Json::arrayValue);
	for(int n: {1, 96, 1, int(TNS_W2), 0}) judgment.append(n);
	frame[9].append(judgment);
	r["frames"].append(frame);
	RString error;
	assert(replay.Validate(error));
	assert(replay.Save("Save/Replays/fixture.smr.json", error));
	ReplayData loaded;
	assert(loaded.Load("Save/Replays/fixture.smr.json", error));
	NoteData restored;
	ReplayData::RestoreChart(loaded.root["chart"], 4, restored);
	assert(ReplayData::Chart(restored) == ReplayData::Chart(chart));
	auto it = restored.FindTapNote(1, 96);
	ReplayData::RestoreNote(loaded.root["frames"][1][8][0], it->second);
	assert(it->second.HoldResult.hns == HNS_Held);
	assert(it->second.result.tns == TNS_W2);
	assert(it->second.result.fTapNoteOffset == hold.result.fTapNoteOffset);
	PlayerStageStats result;
	ReplayData::RestoreScore(loaded.root["frames"][1][7], result);
	assert(result.m_iScore == stats.m_iScore && result.m_iCurCombo == 3 && result.m_iMaxCombo == 5);
	assert(result.m_iTapNoteScores[TNS_W2] == 2 && result.m_iHoldNoteScores[HNS_Held] == 1);
	assert(!result.m_bDisqualified);

	auto rejects = [&](Json::Value bad) {
		Json::FastWriter writer; ReplayData rejected;
		assert(!rejected.Parse(writer.write(bad), error)); assert(!error.empty());
		assert(rejected.root.isNull());
	};
	auto bad = r; bad["version"] = 99; rejects(bad);
	bad = r; bad["frames"][1][0] = -1; rejects(bad);
	bad = r; bad["frames"][1][8][0][0] = 4; rejects(bad);
	bad = r; bad["frames"][1][8][0][1] = 999; rejects(bad);
	bad = r; bad["frames"][1][8][0][6] = NUM_HoldNoteScore; rejects(bad);
	bad = r; bad["frames"][1][9][0][2] = 0; rejects(bad);
	bad = r; bad["frames"][1][4] = 0; rejects(bad);
	bad = r; bad["chart"].append(bad["chart"][0]); rejects(bad);
	bad = r; bad["frames"][1][7][4] = 2.5; rejects(bad);
	bad = r; bad["frames"][1][7][0] = Json::UInt(UINT_MAX); rejects(bad);
	bad = r; bad["frames"][1][8][0][6] = HNS_None; rejects(bad);
	assert(!loaded.Parse("{broken", error));
	assert(!loaded.Parse(RString(32, '[') + RString(32, ']'), error));
	assert(!loaded.Load("Save/Replays/missing.smr.json", error));
	std::cout << "Replay data: round trip, outcomes, chart/timing identity and malformed input checks passed.\n";
	return 0;
}
