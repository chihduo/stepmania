#ifndef GAMEPLAY_REPLAY_H
#define GAMEPLAY_REPLAY_H

#include "ReplayData.h"
#include "PlayerNumber.h"

class Player;
class PlayerInfo;
class PlayerStageStats;

namespace GameplayReplay
{
	bool IsPlaying();
	bool IsRecording(const Player* player = nullptr);
	const ReplayData* Playback();
	PlayerNumber PlaybackPlayer();
	bool CheckContext(RString& error);
	void StartRecording(PlayerInfo& player);
	void Capture(bool force = false);
	void RecordInput(Player* player, int column, bool released, float seconds);
	void RecordJudgment(Player* player, int kind, int row, int column, int score, float offset, const TapNote* note = nullptr);
	void RecordLife(const PlayerStageStats* stats, float life, float seconds);
	void FinishRecording(bool aborted);
	void DiscardRecording();
	RString Status();
	RString LastFile();
	vector<RString> FilesForCurrentChart(RString& error);
	bool PreparePlayback(const RString& filename, RString& error);
	bool Watch(const RString& filename, RString& error);
	void BeginPlayback();
	void EndPlayback();
}

#endif
