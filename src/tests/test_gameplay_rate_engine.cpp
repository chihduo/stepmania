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
	Equal(GAMESTATE->m_SongOptions.GetCurrent().m_fMusicRate, 1.35f);
	Equal(GAMESTATE->m_SongOptions.GetCurrent().m_fHaste, 0.0f);
	assert(GAMESTATE->m_SongOptions.GetCurrent().m_AutosyncType == AutosyncType_Off);
	assert(GAMESTATE->m_SongOptions.GetPreferred() == options);
	assert(GAMESTATE->m_SongOptions.GetStage() == options);
	assert(GAMESTATE->m_SongOptions.GetSong() == options);
	GAMESTATE->m_SongOptions.Update(0.1f);
	GAMESTATE->ApplyGameplayRate();
	Equal(GAMESTATE->m_SongOptions.GetCurrent().m_fMusicRate, 1.35f);
	assert(GAMESTATE->SetGameplayRate(1.0f));
	Equal(GAMESTATE->m_SongOptions.GetCurrent().m_fMusicRate, 1.0f);
	Equal(GAMESTATE->m_GameplayRate.GetScrollSpeedRate(1.0f), 1.25f);
	assert(GAMESTATE->m_GameplayRate.IsActive());
	assert(GAMESTATE->m_SongOptions.GetPreferred() == options);
	assert(GAMESTATE->m_SongOptions.GetStage() == options);
	assert(GAMESTATE->m_SongOptions.GetSong() == options);
	assert(!GAMESTATE->SetGameplayRate(1.0f));
	GAMESTATE->m_SongOptions.Update(0.1f);
	GAMESTATE->ApplyGameplayRate();
	Equal(GAMESTATE->m_SongOptions.GetCurrent().m_fMusicRate, 1.0f);
	Equal(GAMESTATE->m_SongOptions.GetCurrent().m_fHaste, 0.0f);
	assert(GAMESTATE->m_SongOptions.GetCurrent().m_AutosyncType == AutosyncType_Off);
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
		assert(result.handled && result.action == (i < 2 ? GameplayRateInput::Faster : GameplayRateInput::Slower));
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

	// Exercise every pair/order with both Shift keys through the real mapper.
	using Keys = GameplayRateInput;
	for(auto shift : shifts)
	for(int first = 0; first < 4; ++first)
	for(int second = 0; second < 4; ++second)
	{
		if(first == second)
			continue;
		mapper->Clear();
		InputEventPlus a;
		a.DeviceI = DeviceInput(DEVICE_KEYBOARD, arrows[first]);
		a.DeviceI.bDown = true;
		DeviceInput modifier(DEVICE_KEYBOARD, shift);
		modifier.bDown = true;
		a.InputList = {a.DeviceI, modifier};
		std::sort(a.InputList.begin(), a.InputList.end());
		mapper->Map(a.DeviceI, game, 0);
		assert(!mapper->FilterGameplayRateInput(a, false).handled);
		assert(mapper->FilterGameplayRateInput(a, true).action ==
			(first < 2 ? Keys::Faster : Keys::Slower));
		InputEventPlus b;
		b.DeviceI = DeviceInput(DEVICE_KEYBOARD, arrows[second]);
		b.DeviceI.bDown = true;
		b.InputList = {a.DeviceI, b.DeviceI, modifier};
		std::sort(b.InputList.begin(), b.InputList.end());
		mapper->Map(b.DeviceI, game, 1);
		assert(!mapper->FilterGameplayRateInput(b, false).handled);
		const auto chord = mapper->FilterGameplayRateInput(b, true);
		const auto expected = (first + 2) % 4 == second ? Keys::ResetToNormal :
			(second < 2 ? Keys::Faster : Keys::Slower);
		assert(chord.handled && chord.action == expected);
		assert(!mapper->IsBeingPressed(game, MultiPlayer_Invalid, &b.InputList));
		DeviceInput pad(DEVICE_JOY1, JOY_BUTTON_1);
		pad.bDown = true;
		mapper->Map(pad, game, 2);
		b.InputList.push_back(pad);
		std::sort(b.InputList.begin(), b.InputList.end());
		assert(mapper->IsBeingPressed(game, MultiPlayer_Invalid, &b.InputList));
		b.type = IET_REPEAT;
		assert(mapper->FilterGameplayRateInput(b, false).action == Keys::NoChange);
		// Reverse the release order across cases, and release Shift first.
		for(auto input : first < second ? std::vector<InputEventPlus>{a, b} :
			std::vector<InputEventPlus>{b, a})
		{
			input.type = IET_RELEASE;
			input.InputList.clear();
			const auto release = mapper->FilterGameplayRateInput(input, false);
			assert(release.handled && release.action == Keys::NoChange);
			assert(!mapper->IsGameplayRateInputSuppressed(input.DeviceI));
		}
	}

	// Opposite-key state comes from the event snapshot, not stale captures.
	InputEventPlus stale;
	stale.DeviceI = DeviceInput(DEVICE_KEYBOARD, KEY_UP);
	stale.DeviceI.bDown = true;
	DeviceInput modifier(DEVICE_KEYBOARD, KEY_LSHIFT);
	modifier.bDown = true;
	stale.InputList = {stale.DeviceI, modifier};
	std::sort(stale.InputList.begin(), stale.InputList.end());
	assert(mapper->FilterGameplayRateInput(stale, true).action == Keys::Faster);
	InputEventPlus down;
	down.DeviceI = DeviceInput(DEVICE_KEYBOARD, KEY_DOWN);
	down.DeviceI.bDown = true;
	down.InputList = {down.DeviceI, modifier};
	std::sort(down.InputList.begin(), down.InputList.end());
	assert(mapper->FilterGameplayRateInput(down, true).action == Keys::Slower);
	stale.type = down.type = IET_RELEASE;
	mapper->FilterGameplayRateInput(stale, false);
	mapper->FilterGameplayRateInput(down, false);

	// A reset can use an ordinary held arrow without stealing that note hold.
	mapper->Clear();
	stale.type = down.type = IET_FIRST_PRESS;
	stale.InputList = {stale.DeviceI};
	assert(!mapper->FilterGameplayRateInput(stale, true).handled);
	down.InputList = {stale.DeviceI, down.DeviceI, modifier};
	std::sort(down.InputList.begin(), down.InputList.end());
	mapper->Map(stale.DeviceI, game, 0);
	assert(mapper->FilterGameplayRateInput(down, true).action == Keys::ResetToNormal);
	assert(!mapper->IsGameplayRateInputSuppressed(stale.DeviceI));
	assert(mapper->IsBeingPressed(game, MultiPlayer_Invalid, &down.InputList));
	stale.type = down.type = IET_RELEASE;
	assert(!mapper->FilterGameplayRateInput(stale, false).handled);
	assert(mapper->FilterGameplayRateInput(down, false).handled);

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

	// The graph uses the recorded time domain, including failure and summary tails.
	PlayerStageStats history;
	history.Init(PLAYER_1);
	Equal(history.GetLifeRecordLastSecond(), 0.0f);
	history.SetLifeRecordAt(0.5f, 0.0f);
	history.SetLifeRecordAt(1.0f, 10.0f);
	history.SetLifeRecordAt(0.0f, 15.0f);
	Equal(history.GetLifeRecordLastSecond(), 15.0f);
	Equal(history.GetCurrentLife(), 0.0f);
	PlayerStageStats nextHistory;
	nextHistory.Init(PLAYER_1);
	nextHistory.SetLifeRecordAt(0.5f, 0.0f);
	nextHistory.SetLifeRecordAt(0.75f, 4.0f);
	history.AddStats(nextHistory);
	Equal(history.GetLifeRecordLastSecond(), 20.0f); // Includes the 1s gap.

	Lua *L = LUA->Get();
	history.PushSelf(L);
	lua_setglobal(L, "LifeHistoryTestStats");
	RString luaError;
	const bool lifeLuaOK = LuaHelpers::RunScript(L,
		"local s = LifeHistoryTestStats; "
		"assert(s:GetLifeRecordLastSecond() == 20); "
		"local values = s:GetLifeRecord(s:GetLifeRecordLastSecond(), 128); "
		"assert(#values == 128 and values[1] == 0.5 and values[128] == 0.75); "
		"assert(s:GetCurrentLife() == values[128]); "
		"LifeHistoryTestStats = nil",
		"life-history-test", luaError);
	if(!lifeLuaOK) std::cerr << luaError << "\n";
	assert(lifeLuaOK);
	LUA->Release(L);

	std::cout << "Engine rate, modifier restoration, input suppression, practice-score, and life-history checks passed.\n";
	// Leave the managers alive: normal shutdown writes preferences/mappings and
	// is outside this test. All filesystem access was mounted in the fixture.
	return 0;
}
