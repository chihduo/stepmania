// Headless checks linked against the application's actual engine objects.
#include "global.h"
#include "GameState.h"
#include "GameManager.h"
#include "RageThreads.h"
#include "RageUtil.h"
#include "InputMapper.h"
#include "InputEventPlus.h"
#include "LuaManager.h"
#include "PrefsManager.h"
#include "RageFileManager.h"
#include "RageLog.h"
#include "StageStats.h"
#include "arch/ArchHooks/ArchHooks.h"
#include <cassert>
#include <iostream>

class TestInputMapper : public InputMapper
{
public:
	void Map(const DeviceInput &device, const GameInput &game, int slot)
	{
		m_mappings.SetInputMap(device, game, slot);
	}
	void Clear() { m_mappings.Clear(); }
};

static void Equal(float actual, float expected)
{
	assert(std::fabs(actual - expected) < 0.0001f);
}

int main(int argc, char **argv)
{
	assert(argc == 2); // A fresh, temporary virtual filesystem root.
	RageThreadRegister thread("Gameplay rate test");
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
	GAMESTATE->m_bMultiplayer = false;
	INPUTFILTER = new InputFilter;
	auto mapper = new TestInputMapper;
	INPUTMAPPER = mapper;
	mapper->Clear();

	SongOptions options;
	options.Init();
	options.m_fMusicRate = 1.25f;
	options.m_fHaste = 0.5f;
	options.m_AutosyncType = AutosyncType_Song;
	GAMESTATE->m_SongOptions.Assign(ModsLevel_Preferred, options);
	assert(GAMESTATE->ChangeGameplayRate(1));
	Equal(GAMESTATE->m_SongOptions.GetCurrent().m_fMusicRate, 1.45f);
	Equal(GAMESTATE->m_SongOptions.GetCurrent().m_fHaste, 0.0f);
	assert(GAMESTATE->m_SongOptions.GetCurrent().m_AutosyncType == AutosyncType_Off);
	assert(GAMESTATE->m_SongOptions.GetPreferred() == options);
	assert(GAMESTATE->m_SongOptions.GetStage() == options);
	assert(GAMESTATE->m_SongOptions.GetSong() == options);
	GAMESTATE->m_SongOptions.Update(0.1f);
	GAMESTATE->ApplyGameplayRate();
	Equal(GAMESTATE->m_SongOptions.GetCurrent().m_fMusicRate, 1.45f);
	GAMESTATE->ResetGameplayRate();
	assert(!GAMESTATE->m_GameplayRate.IsActive());
	assert(GAMESTATE->m_SongOptions.GetCurrent() == options);
	GAMESTATE->ResetGameplayRate(); // Repeated cleanup is harmless.
	assert(GAMESTATE->m_SongOptions.GetCurrent() == options);

	const DeviceButton arrows[] = {KEY_UP, KEY_RIGHT, KEY_DOWN, KEY_LEFT};
	const DeviceButton shifts[] = {KEY_LSHIFT, KEY_RSHIFT};
	const GameInput game(GameController_1, GAME_BUTTON_MENULEFT);
	for(auto shift : shifts)
	for(int i = 0; i < 4; ++i)
	{
		mapper->Clear();
		InputEventPlus input;
		input.DeviceI = DeviceInput(DEVICE_KEYBOARD, arrows[i]);
		input.DeviceI.bDown = true;
		DeviceInput modifier(DEVICE_KEYBOARD, shift);
		modifier.bDown = true;
		input.InputList = {input.DeviceI, modifier};
		std::sort(input.InputList.begin(), input.InputList.end());
		mapper->Map(input.DeviceI, game, 0);
		assert(mapper->IsBeingPressed(game, MultiPlayer_Invalid, &input.InputList));
		assert(!mapper->FilterGameplayRateInput(input, false).handled);
		const auto result = mapper->FilterGameplayRateInput(input, true);
		assert(result.handled && result.direction == (i < 2 ? 1 : -1));
		assert(!mapper->IsBeingPressed(game, MultiPlayer_Invalid, &input.InputList));
		// A separate pad mapped to this same column must remain usable.
		DeviceInput pad(DEVICE_JOY1, JOY_BUTTON_1);
		pad.bDown = true;
		mapper->Map(pad, game, 1);
		input.InputList.push_back(pad);
		std::sort(input.InputList.begin(), input.InputList.end());
		assert(mapper->IsBeingPressed(game, MultiPlayer_Invalid, &input.InputList));
		// Release Shift before the arrow. The arrow release is still consumed.
		input.InputList.clear();
		input.type = IET_REPEAT;
		assert(mapper->FilterGameplayRateInput(input, false).handled);
		input.type = IET_RELEASE;
		assert(mapper->FilterGameplayRateInput(input, false).handled);
		assert(!mapper->IsGameplayRateInputSuppressed(input.DeviceI));
		input.type = IET_FIRST_PRESS;
		assert(!mapper->FilterGameplayRateInput(input, true).handled);
	}

	StageStats practice;
	practice.m_bUsedGameplayRate = true;
	practice.m_vpPlayedSongs.push_back(nullptr); // Aggregation copies this pointer only.
	StageStats summary;
	summary.AddStats(practice);
	assert(summary.m_bUsedGameplayRate);
	assert(!summary.PlayerHasHighScore(PLAYER_1));
	PREFSMAN->m_bDisqualification.Set(false);
	summary.FinalizeScores(true); // Must return before touching profiles/songs.
	StageStats normal;
	normal.m_vpPlayedSongs.push_back(nullptr);
	summary.AddStats(normal);
	assert(summary.m_bUsedGameplayRate);
	assert(!normal.m_bUsedGameplayRate);

	std::cout << "Engine rate, modifier restoration, input suppression, and practice-score checks passed.\n";
	// Leave the managers alive: normal shutdown writes preferences/mappings and
	// is outside this test. All filesystem access was mounted in the fixture.
	return 0;
}
