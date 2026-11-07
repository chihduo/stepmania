// Real engine session/persistence checks without a display, sound device or user profile.
#include "global.h"
#include "GameplayReplay.h"
#include "CryptManager.h"
#include "CommonMetrics.h"
#include "ThemeManager.h"
#include "GameManager.h"
#include "GameState.h"
#include "LuaManager.h"
#include "MessageManager.h"
#include "NoteSkinManager.h"
#include "PlayerState.h"
#include "PrefsManager.h"
#include "Profile.h"
#include "ProfileManager.h"
#include "RageFile.h"
#include "RageFileManager.h"
#include "RageLog.h"
#include "RageThreads.h"
#include "Song.h"
#include "StatsManager.h"
#include "Steps.h"
#include "Style.h"
#include "arch/ArchHooks/ArchHooks.h"
#include <cassert>
#include <iostream>

class ReplayTestTheme : public ThemeManager
{
public:
	void Load() { LoadThemeMetrics("test", "en"); }
};

int main(int argc, char** argv)
{
	assert(argc == 2);
	RageThreadRegister thread("Replay session test");
	SetCommandlineArguments(argc, argv);
	HOOKS = ArchHooks::Create(); HOOKS->Init();
	LUA = new LuaManager;
	FILEMAN = new RageFileManager(argv[0]); FILEMAN->Mount("dir", argv[1], "/");
	LOG = new RageLog; LOG->SetLogToDisk(false);
	MESSAGEMAN = new MessageManager; PREFSMAN = new PrefsManager;
	GAMESTATE = new GameState; GAMEMAN = new GameManager;
	NOTESKIN = new NoteSkinManager;
	CRYPTMAN = new CryptManager; PROFILEMAN = new ProfileManager; STATSMAN = new StatsManager;
	GAMESTATE->m_pCurGame.Set(GAMEMAN->StringToGame("dance"));
	GAMESTATE->m_PlayMode.Set(PLAY_MODE_REGULAR);
	GAMESTATE->m_EditMode = EditMode_Invalid;
	GAMESTATE->m_bMultiplayer = false;
	GAMESTATE->m_bTemporaryEventMode = false;
	GAMESTATE->SetMasterPlayerNumber(PLAYER_1);
	GAMESTATE->m_bSideIsJoined[PLAYER_1] = true;
	GAMESTATE->SetCurrentStyle(GAMEMAN->GameAndStringToStyle(GAMESTATE->m_pCurGame, "single"), PLAYER_1);
	FOREACH_MultiPlayer(mp) GAMESTATE->m_MultiPlayerStatus[mp] = MultiPlayerStatus_NotJoined;
	RageFile metrics;
	assert(metrics.Open("/Themes/test/metrics.ini", RageFile::WRITE));
	metrics.Write("[Global]\nFallbackTheme=\n[Common]\nImageCache=\"\"\n"); metrics.Close();
	auto* theme = new ReplayTestTheme; theme->Load(); CommonMetrics::IMAGES_TO_CACHE.Read();
	Song song; song.SetSongDir("/Songs/Test/Replay/"); song.m_sMusicFile = "fixture.wav";
	song.m_SongTiming.AddSegment(BPMSegment(0, 120));
	RageFile music; assert(music.Open(song.GetMusicPath(), RageFile::WRITE)); music.Write("fixture"); music.Close();
	Steps* steps = new Steps(&song); steps->m_StepsType = StepsType_dance_single;
	steps->SetDifficulty(Difficulty_Easy); steps->SetMeter(3); song.AddSteps(steps);
	NoteData notes; notes.SetNumTracks(4); notes.SetTapNote(0, 48, TAP_ORIGINAL_TAP); steps->SetNoteData(notes);
	GAMESTATE->m_pCurSong.Set(&song); GAMESTATE->m_pCurSteps[PLAYER_1].Set(steps);
	ReplayData replay; auto& r = replay.root;
	r["version"] = 1; r["song"] = song.GetSongDir(); r["style"] = "single";
	for(const char* key: {"title", "artist", "recorded_at", "build", "outcome"}) r[key] = "fixture";
	r["player_options"] = "1.5x,mirror"; r["song_options"] = "";
	r["chart_hash"] = ReplayData::Fingerprint(notes, *steps->GetTimingData());
	r["player"] = 0; r["tracks"] = 4; r["seed"] = 12; r["stage_index"] = 2;
	r["initial_rate"] = .8; r["global_offset"] = .025;
	r["chart"] = ReplayData::Chart(notes);
	PlayerStageStats result; result.m_iScore = 987654; result.m_iMaxCombo = 123;
	Json::Value frame(Json::arrayValue);
	for(double number: {0.,0.,0.,0.,.8,.5}) frame.append(number);
	frame.append(0); frame.append(ReplayData::Score(result));
	for(int i = 0; i < 4; ++i) frame.append(Json::Value(Json::arrayValue));
	r["frames"].append(frame);
	RString filename = r["chart_hash"].asString() + "-20261107-120000-000.smr.json", error;
	assert(replay.Save("Save/Replays/" + filename, error));
	auto files = GameplayReplay::FilesForCurrentChart(error);
	assert(files.size() == 1);
	// A malformed name must not crash the browser's timestamp formatter.
	RageFile malformed; assert(malformed.Open("Save/Replays/" + r["chart_hash"].asString() + "-.smr.json", RageFile::WRITE)); malformed.Write("{}"); malformed.Close();
	assert(GameplayReplay::FilesForCurrentChart(error).size() == 1);
	assert(!GameplayReplay::PreparePlayback("../Preferences.ini", error));
	assert(!GameplayReplay::IsPlaying());
	r["player_options"] = "1e999x";
	assert(replay.Save("Save/Replays/" + filename, error));
	assert(!GameplayReplay::PreparePlayback(filename, error));
	r["player_options"] = "1.5x,mirror";
	assert(replay.Save("Save/Replays/" + filename, error));
	steps->m_Timing.AddSegment(BPMSegment(0, 150));
	assert(!GameplayReplay::PreparePlayback(filename, error));
	steps->m_Timing.Clear();
	const RString audio = song.m_sMusicFile; song.m_sMusicFile = "missing.wav";
	assert(!GameplayReplay::PreparePlayback(filename, error));
	song.m_sMusicFile = audio;
	GAMESTATE->m_bSideIsJoined[PLAYER_2] = true;
	GAMESTATE->SetCurrentStyle(GAMEMAN->GameAndStringToStyle(GAMESTATE->m_pCurGame, "versus"), PLAYER_1);
	assert(!GameplayReplay::PreparePlayback(filename, error));
	GAMESTATE->m_bSideIsJoined[PLAYER_2] = false;
	GAMESTATE->SetCurrentStyle(GAMEMAN->GameAndStringToStyle(GAMESTATE->m_pCurGame, "single"), PLAYER_1);

	SongOptions so; so.Init(); so.m_fMusicRate = 1.4f; so.m_bSaveReplay = true;
	GAMESTATE->m_SongOptions.Assign(ModsLevel_Preferred, so);
	GAMESTATE->m_pPlayerState[PLAYER_1]->m_PlayerOptions.FromString(ModsLevel_Preferred, "2x,reverse");
	const PlayerOptions original = GAMESTATE->m_pPlayerState[PLAYER_1]->m_PlayerOptions.GetPreferred();
	GAMESTATE->m_iStageSeed = 77; GAMESTATE->m_iCurrentStageIndex = 9;
	GAMESTATE->m_iPlayerStageTokens[PLAYER_1] = 3;
	PREFSMAN->m_fGlobalOffsetSeconds.Set(-.014f);
	STATSMAN->m_CurStageStats.m_player[PLAYER_1].m_iScore = 333;
	STATSMAN->m_vPlayedStageStats.push_back(STATSMAN->m_CurStageStats);
	auto* machine = PROFILEMAN->GetMachineProfile();
	machine->m_iTotalGameplaySeconds = 45; machine->m_iTotalTapsAndHolds = 17; machine->m_iTotalDancePoints = 91;
	machine->m_iCurrentCombo = 6;
	const int playCount = machine->GetStepsHighScoreList(&song, steps).GetNumTimesPlayed();
	const size_t scoreCount = machine->GetStepsHighScoreList(&song, steps).vHighScores.size();
	assert(GameplayReplay::PreparePlayback(filename, error));
	assert(GameplayReplay::IsPlaying());
	GameplayReplay::BeginPlayback();
	assert(GAMESTATE->m_iCurrentStageIndex == 2);
	assert(GAMESTATE->m_iStageSeed == 12);
	assert(std::fabs(GAMESTATE->m_SongOptions.GetCurrent().m_fMusicRate - .8f) < .00001f);
	assert(!GAMESTATE->m_SongOptions.GetCurrent().m_bSaveScore);
	assert(!GAMESTATE->m_SongOptions.GetCurrent().m_bSaveReplay);
	ReplayData::RestoreScore(frame[7], STATSMAN->m_CurStageStats.m_player[PLAYER_1]);
	assert(STATSMAN->m_CurStageStats.m_player[PLAYER_1].m_iScore == 987654);
	// Exercise every persistence entrance that a gameplay completion path uses.
	GAMESTATE->BeginStage(); GAMESTATE->CommitStageStats(); GAMESTATE->FinishStage(); GAMESTATE->CancelStage();
	GAMESTATE->SaveCurrentSettingsToProfile(PLAYER_1);
	STATSMAN->m_CurStageStats.FinalizeScores(false);
	STATSMAN->CommitStatsToProfiles(&STATSMAN->m_CurStageStats);
	PROFILEMAN->IncrementStepsPlayCount(&song, steps, PLAYER_1);
	assert(machine->m_iTotalGameplaySeconds == 45 && machine->m_iTotalTapsAndHolds == 17);
	assert(machine->m_iTotalDancePoints == 91 && machine->m_iCurrentCombo == 6);
	assert(machine->GetStepsHighScoreList(&song, steps).GetNumTimesPlayed() == playCount);
	assert(machine->GetStepsHighScoreList(&song, steps).vHighScores.size() == scoreCount);
	assert(STATSMAN->m_vPlayedStageStats.size() == 1);
	assert(GAMESTATE->m_iPlayerStageTokens[PLAYER_1] == 3);
	GAMESTATE->SetGameplayRate(1.2f);
	GameplayReplay::BeginPlayback(); // restarting reinitializes presentation only
	GameplayReplay::EndPlayback();
	assert(!GameplayReplay::IsPlaying());
	assert(GAMESTATE->m_SongOptions.GetCurrent() == so);
	assert(GAMESTATE->m_pPlayerState[PLAYER_1]->m_PlayerOptions.GetPreferred() == original);
	assert(GAMESTATE->m_iStageSeed == 77 && GAMESTATE->m_iCurrentStageIndex == 9);
	assert(PREFSMAN->m_fGlobalOffsetSeconds == -.014f);
	assert(STATSMAN->m_CurStageStats.m_player[PLAYER_1].m_iScore == 333);
	assert(STATSMAN->m_vPlayedStageStats.size() == 1);
	GameplayReplay::EndPlayback(); // cleanup is idempotent
	assert(GameplayReplay::PreparePlayback(filename, error));
	GameplayReplay::EndPlayback(); // exit before the first update
	assert(GAMESTATE->m_iCurrentStageIndex == 9);
	assert(!GameplayReplay::IsPlaying());
	std::cout << "Replay session: compatibility, browsing, profile isolation, restart and early-exit restoration passed.\n";
	return 0;
}
