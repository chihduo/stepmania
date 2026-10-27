// Normal score recording after live rate changes, using real engine objects.
#include "global.h"
#include "GameState.h"
#include "GameManager.h"
#include "PlayerState.h"
#include "Profile.h"
#include "Preference.h"
// Configure an isolated persistent profile without loading a real player card.
#define private public
#include "ProfileManager.h"
#undef private
#include "CommonMetrics.h"
#include "CryptManager.h"
#include "LuaManager.h"
#include "PrefsManager.h"
#include "RageFile.h"
#include "RageFileManager.h"
#include "RageLog.h"
#include "RageThreads.h"
#include "Song.h"
#include "StageStats.h"
#include "Steps.h"
#include "ThemeManager.h"
#include "XmlFile.h"
#include "XmlFileUtil.h"
#include "arch/ArchHooks/ArchHooks.h"
#include <cassert>
#include <iostream>

class ScoreTestTheme : public ThemeManager
{
public:
	void Load() { LoadThemeMetrics("test", "en"); }
};

static StageStats Result(Song& song, Steps& steps, unsigned score)
{
	StageStats result;
	result.m_playMode = PLAY_MODE_REGULAR;
	result.m_vpPlayedSongs.push_back(&song);
	result.m_vpPossibleSongs.push_back(&song);
	auto& player = result.m_player[PLAYER_1];
	player.m_bJoined = true;
	player.m_vpPossibleSteps.push_back(&steps);
	player.m_iStepsPlayed = 1;
	player.m_iScore = score;
	player.m_iActualDancePoints = player.m_iPossibleDancePoints = 100;
	// Failed plays still save personal scores under normal StepMania rules.
	// Using one here avoids needing a graphical theme's grading metrics.
	player.m_bFailed = true;
	return result;
}

static void RoundTrip(Profile& profile, Song& song, Steps& steps, const char* path)
{
	profile.IncrementStepsPlayCount(&song, &steps);
	std::unique_ptr<XNode> scores(profile.SaveSongScoresCreateNode());
	assert(XmlFileUtil::SaveToFile(scores.get(), path));
	XNode reloaded;
	assert(XmlFileUtil::LoadFromFileShowErrors(reloaded, path));
	Profile restored;
	restored.LoadSongScoresFromNode(&reloaded);
	const auto& before = profile.GetStepsHighScoreList(&song, &steps).vHighScores;
	const auto& after = restored.GetStepsHighScoreList(&song, &steps).vHighScores;
	assert(!before.empty() && after.size() == before.size());
	assert(after.front().GetScore() == before.front().GetScore());
	assert(after.front().GetGrade() == before.front().GetGrade());
	assert(!after.front().GetDisqualified());
}

int main(int argc, char** argv)
{
	assert(argc == 2);
	RageThreadRegister thread("Live rate score test");
	SetCommandlineArguments(argc, argv);
	HOOKS = ArchHooks::Create();
	HOOKS->Init();
	LUA = new LuaManager;
	FILEMAN = new RageFileManager(argv[0]);
	FILEMAN->Mount("dir", argv[1], "/");
	LOG = new RageLog;
	LOG->SetLogToDisk(false);
	MESSAGEMAN = new MessageManager;
	PREFSMAN = new PrefsManager;
	GAMESTATE = new GameState;
	GAMEMAN = new GameManager;
	GAMESTATE->m_pCurGame.Set(GAMEMAN->StringToGame("dance"));
	GAMESTATE->m_PlayMode.Set(PLAY_MODE_REGULAR);
	GAMESTATE->m_bMultiplayer = false;
	FOREACH_MultiPlayer(mp)
		GAMESTATE->m_MultiPlayerStatus[mp] = MultiPlayerStatus_NotJoined;
	GAMESTATE->m_bTemporaryEventMode = false;
	GAMESTATE->SetMasterPlayerNumber(PLAYER_1);
	GAMESTATE->m_bSideIsJoined[PLAYER_1] = true;
	GAMESTATE->SetCurrentStyle(GAMEMAN->GameAndStringToStyle(GAMESTATE->m_pCurGame, "single"), PLAYER_1);
	PREFSMAN->m_bDisqualification.Set(true);
	PREFSMAN->m_bEventMode.Set(false);
	CRYPTMAN = new CryptManager;
	PROFILEMAN = new ProfileManager;
	PROFILEMAN->m_sProfileDir[PLAYER_1] = "/Save/TestPlayer/";
	PROFILEMAN->m_bWasLoadedFromMemoryCard[PLAYER_1] = true;

	RageFile metrics;
	assert(metrics.Open("/Themes/test/metrics.ini", RageFile::WRITE));
	metrics.Write("[Global]\nFallbackTheme=\n[Common]\nImageCache=\"\"\n[RadarValues]\nWriteSimpleValues=true\nWriteComplexValues=true\n");
	metrics.Close();
	auto theme = new ScoreTestTheme;
	theme->Load();
	CommonMetrics::IMAGES_TO_CACHE.Read();
	RadarValues::WRITE_SIMPLE_VALIES.Read();
	RadarValues::WRITE_COMPLEX_VALIES.Read();

	Song song;
	song.SetSongDir("/Songs/Test/LiveRate/");
	Steps& steps = *new Steps(&song);
	steps.m_StepsType = StepsType_dance_single;
	steps.SetDifficulty(Difficulty_Easy);
	steps.SetMeter(3);
	song.AddSteps(&steps); // Song owns the fixture chart.
	GAMESTATE->m_pCurSong.Set(&song);
	GAMESTATE->m_pCurSteps[PLAYER_1].Set(&steps);
	auto personal = PROFILEMAN->GetProfile(PLAYER_1);
	auto machine = PROFILEMAN->GetMachineProfile();

	// Active overrides, reset-to-normal, and song-end cleanup all retain normal
	// personal saving. Higher scores must replace earlier normal scores.
	unsigned score = 100;
	for(float rate : {0.2f, 0.8f, 1.1f, 3.0f})
	for(int cleanup = 0; cleanup < 3; ++cleanup)
	{
		SongOptions options;
		options.Init();
		GAMESTATE->m_SongOptions.Assign(ModsLevel_Preferred, options);
		assert(GAMESTATE->SetGameplayRate(rate));
		if(cleanup == 1)
			assert(GAMESTATE->SetGameplayRate(1.0f));
		if(cleanup == 2)
			GAMESTATE->ResetGameplayRate();
		auto stage = Result(song, steps, ++score);
		assert(!stage.m_player[PLAYER_1].IsDisqualified());
		stage.FinalizeScores(false);
		assert(stage.m_player[PLAYER_1].m_iPersonalHighScoreIndex == 0);
		assert(personal->GetStepsHighScoreList(&song, &steps).GetTopScore().GetScore() == score);
		assert(!stage.m_player[PLAYER_1].m_HighScore.GetDisqualified());
		GAMESTATE->ResetGameplayRate();
	}
	RoundTrip(*personal, song, steps, "/Save/TestPlayer/SongScores.xml");

	// Successful score insertion and recognition use the normal machine and
	// personal lists while a live override is active. Grade is fixture data.
	assert(GAMESTATE->SetGameplayRate(0.8f));
	auto passed = Result(song, steps, ++score);
	passed.m_player[PLAYER_1].m_bFailed = false;
	HighScore highScore;
	highScore.SetName(RANKING_TO_FILL_IN_MARKER[PLAYER_1]);
	highScore.SetGrade(Grade_Tier01);
	highScore.SetScore(score);
	highScore.SetPercentDP(1.0f);
	PROFILEMAN->AddStepsScore(&song, &steps, PLAYER_1, highScore,
		passed.m_player[PLAYER_1].m_iPersonalHighScoreIndex,
		passed.m_player[PLAYER_1].m_iMachineHighScoreIndex);
	assert(passed.m_player[PLAYER_1].m_iPersonalHighScoreIndex == 0);
	assert(passed.m_player[PLAYER_1].m_iMachineHighScoreIndex == 0);
	assert(passed.PlayerHasHighScore(PLAYER_1));
	RoundTrip(*machine, song, steps, "/Save/MachineProfile/SongScores.xml");

	// Unrelated eligibility rules remain intact.
	for(int restriction = 0; restriction < 3; ++restriction)
	{
		auto blocked = Result(song, steps, 9999);
		SongOptions options;
		options.Init();
		options.m_bSaveScore = restriction != 0;
		GAMESTATE->m_SongOptions.Assign(ModsLevel_Preferred, options);
		GAMESTATE->ApplyGameplayRate();
		blocked.m_bUsedAutoplay = restriction == 1;
		blocked.m_player[PLAYER_1].m_bDisqualified = restriction == 2;
		blocked.FinalizeScores(false);
		assert(personal->GetStepsHighScoreList(&song, &steps).GetTopScore().GetScore() == score);
	}
	GAMESTATE->ResetGameplayRate();

	StageStats summary;
	summary.AddStats(passed);
	summary.AddStats(Result(song, steps, 1));
	assert(!summary.m_player[PLAYER_1].IsDisqualified());
	assert(!summary.m_bUsedAutoplay);
	assert(summary.m_player[PLAYER_1].m_iScore == score + 1);

	std::cout << "Live-rate score checks passed: normal personal recording, machine score "
		"insertion/recognition, XML round trips, reset/cleanup, eligibility, and aggregation.\n";
}
