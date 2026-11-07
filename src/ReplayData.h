#ifndef REPLAY_DATA_H
#define REPLAY_DATA_H

#include "json/json.h"
#include "NoteData.h"

class PlayerStageStats;
class TimingData;

// Version 1 is a journal of the original outcomes, not instructions to rejudge
// inputs. This keeps holds, rolls and scores independent of the viewer's FPS.
class ReplayData
{
public:
	static const int VERSION = 1;
	static const unsigned MAX_BYTES = 64 * 1024 * 1024;
	static const unsigned MAX_NOTES = 250000;
	static const unsigned MAX_FRAMES = 500000;
	Json::Value root;

	bool Parse(const RString& text, RString& error);
	bool Validate(RString& error) const;
	bool Load(const RString& path, RString& error);
	bool Save(const RString& path, RString& error) const;

	static Json::Value Chart(const NoteData& notes);
	static void RestoreChart(const Json::Value& chart, int tracks, NoteData& notes);
	static RString Fingerprint(const NoteData& notes, const TimingData& timing);
	static Json::Value Note(int track, int row, const TapNote& note);
	static void RestoreNote(const Json::Value& value, TapNote& note);
	static Json::Value Score(const PlayerStageStats& stats);
	static void RestoreScore(const Json::Value& value, PlayerStageStats& stats);
};

#endif
