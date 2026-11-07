#include "global.h"
#include "ScreenGameplayReplay.h"
#include "ArrowEffects.h"
#include "GameSoundManager.h"
#include "GameState.h"
#include "GameplayReplay.h"
#include "Player.h"
#include "PrefsManager.h"
#include "RageUtil.h"
#include "ScreenDimensions.h"
#include "ScreenManager.h"
#include "Song.h"
#include "StatsManager.h"
#include "Steps.h"
#include "TimingData.h"
#include "ThemeManager.h"

REGISTER_SCREEN_CLASS(ScreenGameplayReplay);
REGISTER_SCREEN_CLASS(ScreenReplayBrowser);

void ScreenGameplayReplay::Init()
{
	GameplayReplay::BeginPlayback();
	m_bForceNoNetwork = true;
	ScreenGameplayNormal::Init();
	m_InitialStats = STATSMAN->m_CurStageStats;
	m_Hint.LoadFromFont(THEME->GetPathF("Common", "Normal"));
	m_Hint.SetXY(SCREEN_CENTER_X, SCREEN_TOP + 66);
	m_Hint.SetZoom(.5f);
	m_Hint.SetMaxWidth((SCREEN_WIDTH - 30) / .5f);
	m_Hint.SetShadowLength(2);
	m_Hint.SetDrawOrder(1000);
	AddChild(&m_Hint);
	SortByDrawOrder();
	UpdateHint();
}

void ScreenGameplayReplay::BeginScreen()
{
	ScreenWithMenuElements::BeginScreen();
	GAMESTATE->m_bGameplayLeadIn.Set(false);
	m_DancingState = STATE_DANCING;
	StartPlayingSong(MIN_SECONDS_TO_STEP, MIN_SECONDS_TO_MUSIC);
}

void ScreenGameplayReplay::EndScreen()
{
	FOREACH_EnabledPlayer(pn)
		if(GAMESTATE->m_pCurSteps[pn]) GAMESTATE->m_pCurSteps[pn]->GetTimingData()->ReleaseLookup();
	ScreenGameplayNormal::EndScreen();
}

void ScreenGameplayReplay::Update(float delta)
{
	if(m_Exiting) return;
	UpdateSongPosition(delta);
	const auto& frames = GameplayReplay::Playback()->root["frames"];
	if(!IsPaused())
	{
		float now = GAMESTATE->m_Position.m_fMusicSeconds;
		Player* player = GetPlayerInfo(GameplayReplay::PlaybackPlayer())->m_pPlayer;
		while(m_Frame < frames.size() && frames[m_Frame][0].asFloat() <= now)
		{
			const auto& frame = frames[m_Frame++];
			float rate = frame[4].asFloat();
			if(fabsf(rate - GAMESTATE->m_SongOptions.GetCurrent().m_fMusicRate) > .0001f)
			{
				GAMESTATE->SetGameplayRate(rate);
				RageSoundParams params = m_pSoundMusic->GetParams();
				params.m_fSpeed = rate;
				m_pSoundMusic->SetParams(params);
				m_textSongOptions.SetText(GAMESTATE->m_SongOptions.GetCurrent().GetLocalizedString());
				Message feedback("RateChanged"); feedback.SetParam("Rate", rate);
				if(m_PlaybackRateDisplay.IsLoaded()) m_PlaybackRateDisplay->HandleMessage(feedback);
			}
			player->ApplyReplayFrame(frame);
		}
		if(m_Frame == frames.size())
		{
			m_Finished = true;
			PauseGame(true);
			UpdateHint();
		}
		m_AutoKeysounds.Update(delta);
		SendCrossedMessages();
		if(!IsPaused()) PlayTicks();
		UpdateLights();
	}
	// Player::Update still animates the notefield, but bypasses all judging.
	const float rate = PREFSMAN->m_bRateModsAffectTweens ? GAMESTATE->m_SongOptions.GetCurrent().m_fMusicRate : 1;
	Screen::Update(delta * rate);
	ArrowEffects::Update();
}

void ScreenGameplayReplay::UpdateHint()
{
	RString text = "REPLAY | Space / Start: pause | R: restart | Esc / Back: return";
	if(IsPaused() && !m_Finished) text = "REPLAY PAUSED | Space / Start: resume | R: restart | Esc / Back: return";
	if(m_Finished)
	{
		const auto& stats = STATSMAN->m_CurStageStats.m_player[GameplayReplay::PlaybackPlayer()];
		text = ssprintf("REPLAY %s | Score %u | Max combo %u\nR: restart | Enter / Esc: return",
			GameplayReplay::Playback()->root["outcome"].asCString(), stats.m_iScore, stats.m_iMaxCombo);
	}
	m_Hint.SetText(text);
}

void ScreenGameplayReplay::Restart()
{
	PauseGame(false);
	m_pSoundMusic->StopPlaying();
	GameplayReplay::BeginPlayback();
	STATSMAN->m_CurStageStats = m_InitialStats;
	STATSMAN->m_CurStageStats.m_vpPlayedSongs.clear();
	auto& ps = STATSMAN->m_CurStageStats.m_player[GameplayReplay::PlaybackPlayer()];
	ps.m_iSongsPlayed = 0; ps.m_iStepsPlayed = 0;
	m_Frame = 0; m_Finished = false;
	LoadNextSong();
	GetPlayerInfo(GameplayReplay::PlaybackPlayer())->m_pPlayer->m_ReplayPressed = 0;
	m_DancingState = STATE_DANCING;
	StartPlayingSong(MIN_SECONDS_TO_STEP, MIN_SECONDS_TO_MUSIC);
	UpdateHint();
}

bool ScreenGameplayReplay::InputShortcut(const InputEventPlus& input)
{
	Input(input);
	return true; // Also receive unmapped keyboard keys, and block gameplay shortcuts.
}
bool ScreenGameplayReplay::Input(const InputEventPlus& input)
{
	if(input.type != IET_FIRST_PRESS || m_Exiting) return true;
	bool key = input.DeviceI.device == DEVICE_KEYBOARD;
	if((key && input.DeviceI.button == KEY_ESC) || input.MenuI == GAME_BUTTON_BACK ||
		(m_Finished && ((key && input.DeviceI.button == KEY_ENTER) || input.MenuI == GAME_BUTTON_START)))
	{
		m_Exiting = true;
		SCREENMAN->PopTopScreen(SM_None);
	}
	else if(key && input.DeviceI.button == KEY_Cr) Restart();
	else if(!m_Finished && ((key && input.DeviceI.button == KEY_SPACE) || input.MenuI == GAME_BUTTON_START))
	{ PauseGame(!IsPaused()); UpdateHint(); }
	return true;
}

void ScreenGameplayReplay::HandleScreenMessage(ScreenMessage message)
{
	if(message == SM_GainFocus) SOUND->HandleSongTimer(false);
	else if(message == SM_LoseFocus) SOUND->HandleSongTimer(true);
	// Never enter normal gameplay's completion/failure/profile-saving path.
	if(message == SM_GoToNextScreen || message == SM_GoToPrevScreen || message == SM_NotesEnded ||
		message == SM_BeginFailed || message == SM_LeaveGameplay || message == SM_DoneFadingIn) return;
	ScreenWithMenuElements::HandleScreenMessage(message);
}

void ScreenReplayBrowser::Init()
{
	ScreenWithMenuElements::Init();
	m_Backdrop.StretchTo(RectF(SCREEN_LEFT, SCREEN_TOP, SCREEN_RIGHT, SCREEN_BOTTOM));
	m_Backdrop.SetDiffuse(RageColor(.04f, .06f, .10f, 1));
	m_Backdrop.SetDrawOrder(999);
	AddChild(&m_Backdrop);
	for(auto* text: {&m_Title, &m_List, &m_Help})
	{
		text->LoadFromFont(THEME->GetPathF("Common", "Normal"));
		text->SetShadowLength(2);
		text->SetDrawOrder(1000);
		AddChild(text);
	}
	m_Title.SetXY(SCREEN_CENTER_X, SCREEN_TOP + 55); m_Title.SetZoom(.8f);
	m_Title.SetMaxWidth((SCREEN_WIDTH-40)/.8f);
	m_Title.SetText("REPLAYS - " + GAMESTATE->m_pCurSong->GetDisplayFullTitle());
	m_List.SetXY(SCREEN_CENTER_X, SCREEN_CENTER_Y); m_List.SetZoom(.85f);
	m_List.SetMaxWidth((SCREEN_WIDTH-50)/.85f);
	m_Help.SetXY(SCREEN_CENTER_X, SCREEN_BOTTOM-55); m_Help.SetZoom(.5f);
	m_Help.SetMaxWidth((SCREEN_WIDTH-30)/.5f);
	m_Help.SetText("Up / Down: select | Enter / Start: watch | Esc / Back: return\nRecordings are stored in Save/Replays. Dates are UTC.");
	SortByDrawOrder();
	Refresh();
}
void ScreenReplayBrowser::Refresh()
{
	RString error;
	m_Files = GameplayReplay::FilesForCurrentChart(error);
	m_Selection = clamp(m_Selection, 0, std::max(0, int(m_Files.size())-1));
	DrawList();
	if(!error.empty()) m_Help.SetText(error);
}
void ScreenReplayBrowser::DrawList()
{
	if(m_Files.empty()) { m_List.SetText("No recordings for this chart yet.\nEnable Save Replays in song options, then play a song."); return; }
	int first = (m_Selection / 8) * 8;
	RString text;
	for(int i = first; i < std::min(first+8, int(m_Files.size())); ++i)
	{
		RString stamp = m_Files[i].substr(65, 15);
		RString date = stamp.substr(0,4)+"-"+stamp.substr(4,2)+"-"+stamp.substr(6,2)+"  "+stamp.substr(9,2)+":"+stamp.substr(11,2)+":"+stamp.substr(13,2);
		text += (i == m_Selection ? ">  " : "    ") + ssprintf("%d.  ", i+1) + date + "\n";
	}
	m_List.SetText(text);
}
bool ScreenReplayBrowser::InputShortcut(const InputEventPlus& input) { Input(input); return true; }
bool ScreenReplayBrowser::Input(const InputEventPlus& input)
{
	if(input.type != IET_FIRST_PRESS && input.type != IET_REPEAT) return true;
	bool key = input.DeviceI.device == DEVICE_KEYBOARD;
	if((key && input.DeviceI.button == KEY_ESC) || input.MenuI == GAME_BUTTON_BACK) SCREENMAN->PopTopScreen(SM_None);
	else if((key && input.DeviceI.button == KEY_UP) || input.MenuI == GAME_BUTTON_UP)
	{ if(!m_Files.empty()) m_Selection = (m_Selection + m_Files.size()-1) % m_Files.size(); DrawList(); }
	else if((key && input.DeviceI.button == KEY_DOWN) || input.MenuI == GAME_BUTTON_DOWN)
	{ if(!m_Files.empty()) m_Selection = (m_Selection+1) % m_Files.size(); DrawList(); }
	else if(input.type == IET_FIRST_PRESS && ((key && input.DeviceI.button == KEY_ENTER) || input.MenuI == GAME_BUTTON_START) && !m_Files.empty())
	{
		RString error;
		if(!GameplayReplay::Watch(m_Files[m_Selection], error)) m_Help.SetText(error);
	}
	return true;
}
void ScreenReplayBrowser::HandleScreenMessage(ScreenMessage message)
{
	ScreenWithMenuElements::HandleScreenMessage(message);
	if(message == SM_GainFocus) Refresh();
}
