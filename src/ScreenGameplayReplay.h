#ifndef SCREEN_GAMEPLAY_REPLAY_H
#define SCREEN_GAMEPLAY_REPLAY_H

#include "ScreenGameplayNormal.h"
#include "StageStats.h"
#include "Quad.h"

class ScreenGameplayReplay : public ScreenGameplayNormal
{
public:
	void Init() override;
	void BeginScreen() override;
	void EndScreen() override;
	void Update(float delta) override;
	bool InputShortcut(const InputEventPlus& input) override;
	bool Input(const InputEventPlus& input) override;
	void HandleScreenMessage(ScreenMessage message) override;
	void StageFinished(bool) override {}
private:
	void Restart();
	void UpdateHint();
	unsigned m_Frame = 0;
	bool m_Finished = false, m_Exiting = false;
	BitmapText m_Hint;
	StageStats m_InitialStats;
};

class ScreenReplayBrowser : public ScreenWithMenuElements
{
public:
	void Init() override;
	bool InputShortcut(const InputEventPlus& input) override;
	bool Input(const InputEventPlus& input) override;
	void HandleScreenMessage(ScreenMessage message) override;
private:
	void Refresh();
	void DrawList();
	vector<RString> m_Files;
	int m_Selection = 0;
	Quad m_Backdrop;
	BitmapText m_Title, m_List, m_Help;
};
#endif
