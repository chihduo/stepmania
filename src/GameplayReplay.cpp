#include "global.h"
#include "GameplayReplay.h"
#include "GameState.h"
#include "GameManager.h"
#include "InputMapper.h"
#include "LifeMeter.h"
#include "LuaBinding.h"
#include "NetworkSyncManager.h"
#include "Player.h"
#include "PrefsManager.h"
#include "RageFileManager.h"
#include "RageUtil.h"
#include "ScreenGameplay.h"
#include "ScreenManager.h"
#include "Song.h"
#include "StatsManager.h"
#include "Steps.h"
#include "Style.h"
#include "ver.h"
#include <ctime>
#include <cmath>
#include <cstdlib>
#include <memory>

namespace
{
struct TrackedNote
{
	int track, row;
	float start, end;
	Json::Value previous;
};
struct Recording
{
	ReplayData data;
	PlayerInfo* player = nullptr;
	vector<TrackedNote> notes;
	vector<unsigned> active;
	unsigned next = 0, events = 0, bytes = 0;
	std::map<const TapNote*, std::pair<int,int>> positions;
	Json::Value judgments{Json::arrayValue}, inputs{Json::arrayValue}, lives{Json::arrayValue};
	Json::Value lastScore;
	float lastTime = -3600, lastLife = -1, lastRate = -1;
	int lastPressed = -1;
};
std::unique_ptr<Recording> recording;
std::unique_ptr<ReplayData> playback;
RString status, lastFile;
bool playing = false;

// The underlying evaluation or song-selection screen stays on the screen stack.
// Preserve its stage and settings rather than committing a synthetic new play.
struct SavedSession
{
	StageStats stats;
	PlayerState players[NUM_PLAYERS];
	ModsGroup<SongOptions> songOptions;
	SongPosition position;
	int seed, stageIndex;
	float offset;
	bool leadIn, adjustTokens;
};
std::unique_ptr<SavedSession> saved;

RString CurrentHash()
{
	const auto pn = GAMESTATE->GetMasterPlayerNumber();
	Steps* steps = GAMESTATE->m_pCurSteps[pn];
	NoteData notes; steps->GetNoteData(notes);
	return ReplayData::Fingerprint(notes, *steps->GetTimingData());
}
void SetStatus(const RString& text)
{
	status = text;
	LOG->Info("Replay: %s", text.c_str());
	MESSAGEMAN->Broadcast("ReplayStatusChanged");
}
bool ValidModifiers(const RString& text, bool song, RString& error)
{
	vector<RString> words, mods;
	split(text, " ", words);
	for(const auto& word: words)
	{
		// Reject non-finite/overflowing numeric modifiers before the legacy
		// options parser can pass them into rendering or integer conversions.
		for(unsigned i = 0; i < word.size(); ++i)
		{
			if(i && word[i-1] != ',' && word[i-1] != '*' && word[i-1] != 'C' && word[i-1] != 'M') continue;
			const char* start = word.c_str() + i; char* end;
			double value = std::strtod(start, &end);
			if(end != start && (!std::isfinite(value) || std::fabs(value) > 1000000))
			{ error = "Replay contains an out-of-range numeric modifier."; return false; }
		}
	}
	PlayerOptions po; po.Init();
	SongOptions so; so.Init();
	split(text, ",", mods, true);
	for(const auto& mod: mods)
	{
		RString detail;
		if(!(song ? so.FromOneModString(mod, detail) : po.FromOneModString(mod, detail)))
		{ error = "Replay modifier or noteskin is unavailable: " + mod; return false; }
	}
	if(!song && (po.m_LifeType != LifeType_Bar || po.m_fRandAttack != 0))
	{ error = "Replay uses unsupported life or attack modifiers."; return false; }
	return true;
}
bool SafeFilename(const RString& name)
{
	if(name.size() != 93 || name.substr(84) != ".smr.json" || name[64] != '-' || name[73] != '-' || name[80] != '-') return false;
	for(unsigned i = 0; i < 84; ++i)
	{
		if(i == 64 || i == 73 || i == 80) continue;
		if(name[i] >= '0' && name[i] <= '9') continue;
		if(i < 64 && name[i] >= 'a' && name[i] <= 'f') continue;
		return false;
	}
	return true;
}
}

bool GameplayReplay::IsPlaying() { return playing; }
bool GameplayReplay::IsRecording(const Player* player)
{
	return recording && (!player || recording->player->m_pPlayer == player);
}
const ReplayData* GameplayReplay::Playback() { return playback.get(); }
PlayerNumber GameplayReplay::PlaybackPlayer() { return GAMESTATE->GetMasterPlayerNumber(); }
RString GameplayReplay::Status() { return status; }
RString GameplayReplay::LastFile() { return lastFile; }

bool GameplayReplay::CheckContext(RString& error)
{
	if(GAMESTATE->m_PlayMode != PLAY_MODE_REGULAR || GAMESTATE->IsCourseMode() ||
		GAMESTATE->m_bMultiplayer || GAMESTATE->GetNumPlayersEnabled() != 1 ||
		GAMESTATE->m_bDemonstrationOrJukebox || GAMESTATE->IsEditing() || (NSMAN && NSMAN->useSMserver))
	{ error = "Replays require a local single-player song in regular play mode."; return false; }
	auto pn = GAMESTATE->GetMasterPlayerNumber();
	if(pn == PLAYER_INVALID || !GAMESTATE->m_pCurSong || !GAMESTATE->m_pCurSteps[pn])
	{ error = "Select a song and chart first."; return false; }
	if(!GAMESTATE->IsHumanPlayer(pn))
	{ error = "Replays require one joined player."; return false; }
	error.clear(); return true;
}

void GameplayReplay::StartRecording(PlayerInfo& pi)
{
	if(playing) return;
	recording.reset(); lastFile.clear();
	if(!GAMESTATE->m_SongOptions.GetCurrent().m_bSaveReplay)
	{ SetStatus("Recording off (enable Save Replays in song options)."); return; }
	RString error;
	if(!CheckContext(error)) { SetStatus("Not recording: " + error); return; }
	const auto& options = pi.GetPlayerState()->m_PlayerOptions.GetStage();
	Song* song = GAMESTATE->m_pCurSong;
	Steps* steps = GAMESTATE->m_pCurSteps[pi.m_pn];
	const auto& songOptions = GAMESTATE->m_SongOptions.GetCurrent();
	if(options.m_LifeType != LifeType_Bar || options.m_fRandAttack != 0 ||
		!song->m_Attacks.empty() || !steps->m_Attacks.empty() || !song->m_vsKeysoundFile.empty() ||
		songOptions.m_SoundEffectType != SoundEffectType_Off || songOptions.m_fHaste != 0 || songOptions.m_AutosyncType != AutosyncType_Off)
	{ SetStatus("Not recording: use a life bar, no attacks/keysounds, Haste off, and Autosync off."); return; }
	const auto& notes = pi.m_pPlayer->GetNoteData();
	for(int col = 0; col < notes.GetNumTracks(); ++col)
		for(auto it = notes.begin(col); it != notes.end(col); ++it)
			if(it->second.type == TapNoteType_Attack || it->second.type == TapNoteType_AutoKeysound || it->second.iKeysoundIndex >= 0)
			{ SetStatus("Not recording: charts with attacks or keysounds are not supported yet."); return; }
	recording.reset(new Recording);
	auto& rec = *recording;
	rec.player = &pi;
	auto& r = rec.data.root;
	r["version"] = ReplayData::VERSION;
	r["song"] = song->GetSongDir(); r["title"] = song->GetDisplayFullTitle(); r["artist"] = song->GetDisplayArtist();
	r["steps_type"] = GAMEMAN->GetStepsTypeInfo(steps->m_StepsType).szName;
	r["chart_hash"] = CurrentHash(); r["style"] = GAMESTATE->GetCurrentStyle(pi.m_pn)->m_szName;
	r["player"] = int(pi.m_pn); r["tracks"] = notes.GetNumTracks();
	r["seed"] = GAMESTATE->m_iStageSeed & INT_MAX;
	r["stage_index"] = GAMESTATE->m_iCurrentStageIndex;
	r["player_options"] = options.GetString(true); r["song_options"] = songOptions.GetString();
	r["initial_rate"] = songOptions.m_fMusicRate; r["global_offset"] = float(PREFSMAN->m_fGlobalOffsetSeconds);
	r["build"] = product_version; r["outcome"] = "unfinished";
	std::time_t now = std::time(nullptr); char date[32];
	std::strftime(date, sizeof(date), "%Y%m%d-%H%M%S", std::gmtime(&now));
	r["recorded_at"] = date;
	r["chart"] = ReplayData::Chart(notes); r["frames"] = Json::Value(Json::arrayValue);
	rec.bytes = Json::FastWriter().write(r).size();
	if(r["chart"].size() > ReplayData::MAX_NOTES)
	{ recording.reset(); SetStatus("Not recording: chart exceeds the replay note limit."); return; }
	auto* timing = steps->GetTimingData();
	for(int col = 0; col < notes.GetNumTracks(); ++col)
		for(auto it = notes.begin(col); it != notes.end(col); ++it)
		{
			TrackedNote n;
			n.track = col; n.row = it->first;
			n.start = timing->GetElapsedTimeFromBeat(NoteRowToBeat(n.row));
			n.end = timing->GetElapsedTimeFromBeat(NoteRowToBeat(n.row + it->second.iDuration));
			n.previous = ReplayData::Note(col, n.row, it->second);
			rec.notes.push_back(n);
			rec.positions[&it->second] = {col, n.row};
		}
	std::sort(rec.notes.begin(), rec.notes.end(), [](const TrackedNote& a, const TrackedNote& b) { return a.start < b.start; });
	for(const auto& life: pi.GetPlayerStageStats()->m_fLifeRecord)
		RecordLife(pi.GetPlayerStageStats(), life.second, life.first);
	SetStatus("Recording single-player replay.");
}

void GameplayReplay::Capture(bool force)
{
	if(!recording) return;
	auto& rec = *recording;
	if(GAMESTATE->m_SongOptions.GetCurrent().m_AutosyncType != AutosyncType_Off ||
		!rec.player->GetPlayerState()->m_ActiveAttacks.empty())
	{ recording.reset(); SetStatus("Recording stopped: calibration or attacks were enabled during play."); return; }
	const auto pn = rec.player->m_pn;
	float now = GAMESTATE->m_Position.m_fMusicSeconds;
	// StartPlayingSong initializes the music clock after LoadNextSong.
	if(now < -3600) return;
	if(now < rec.lastTime - .25f)
	{ recording.reset(); SetStatus("Recording stopped: the song clock was rewound."); return; }
	now = std::max(now, rec.lastTime);
	const auto& notes = rec.player->m_pPlayer->GetNoteData();
	float horizon = std::max(5.0f, Player::GetMaxStepDistanceSeconds() * GAMESTATE->m_SongOptions.GetCurrent().m_fMusicRate + 1);
	while(rec.next < rec.notes.size() && rec.notes[rec.next].start <= now + horizon) rec.active.push_back(rec.next++);
	Json::Value changes(Json::arrayValue);
	for(auto it = rec.active.begin(); it != rec.active.end(); )
	{
		auto& n = rec.notes[*it];
		auto note = notes.FindTapNote(n.track, n.row);
		if(note != notes.end(n.track))
		{
			auto value = ReplayData::Note(n.track, n.row, note->second);
			if(n.previous != value) { changes.append(value); n.previous = value; }
		}
		if(n.end + horizon < now) it = rec.active.erase(it); else ++it;
	}
	auto& stage = STATSMAN->m_CurStageStats;
	auto& stats = *rec.player->GetPlayerStageStats();
	Json::Value score = ReplayData::Score(stats);
	float rate = GAMESTATE->m_SongOptions.GetCurrent().m_fMusicRate;
	float life = rec.player->m_pLifeMeter ? rec.player->m_pLifeMeter->GetLife() : 0;
	int pressed = 0;
	for(int col = 0; col < notes.GetNumTracks(); ++col)
	{
		vector<GameInput> inputs;
		GAMESTATE->GetCurrentStyle(pn)->StyleInputToGameInput(col, pn, inputs);
		if(INPUTMAPPER->IsBeingPressed(inputs)) pressed |= 1 << col;
	}
	if(!force && changes.empty() && rec.judgments.empty() && rec.inputs.empty() && rec.lives.empty() &&
		score == rec.lastScore && rate == rec.lastRate && life == rec.lastLife && pressed == rec.lastPressed && now < rec.lastTime + .25f) return;
	Json::Value f(Json::arrayValue);
	f.append(now); f.append(stage.m_fStepsSeconds); f.append(stats.m_fAliveSeconds); f.append(stage.m_fGameplaySeconds);
	f.append(rate); f.append(life); f.append(pressed);
	f.append(score == rec.lastScore ? Json::Value() : score);
	f.append(changes); f.append(rec.judgments); f.append(rec.inputs); f.append(rec.lives);
	rec.bytes += Json::FastWriter().write(f).size();
	rec.events += changes.size() + rec.judgments.size() + rec.inputs.size() + rec.lives.size();
	if(rec.data.root["frames"].size() >= ReplayData::MAX_FRAMES || rec.events > 1000000 || rec.bytes > ReplayData::MAX_BYTES - 4096 || now > 86400)
	{ recording.reset(); SetStatus("Recording stopped: replay size/duration/event limit reached."); return; }
	rec.data.root["frames"].append(f);
	rec.lastScore = score; rec.lastTime = now; rec.lastRate = rate; rec.lastLife = life; rec.lastPressed = pressed;
	rec.judgments.clear(); rec.inputs.clear(); rec.lives.clear();
}

void GameplayReplay::RecordInput(Player* player, int column, bool released, float seconds)
{
	if(!IsRecording(player) || column < 0) return;
	Json::Value input(Json::arrayValue); input.append(column); input.append(released); input.append(seconds);
	recording->inputs.append(input);
}
void GameplayReplay::RecordJudgment(Player* player, int kind, int row, int column, int score, float offset, const TapNote* note)
{
	if(!IsRecording(player)) return;
	if(kind == 1)
	{
		auto found = recording->positions.find(note);
		if(found == recording->positions.end()) return;
		row = found->second.second;
	}
	Json::Value judgment(Json::arrayValue);
	judgment.append(kind); judgment.append(std::max(0, row)); judgment.append(column); judgment.append(score); judgment.append(offset);
	judgment.append(kind == 0 && note ? note->result.bHeld : false);
	recording->judgments.append(judgment);
}
void GameplayReplay::RecordLife(const PlayerStageStats* stats, float life, float seconds)
{
	if(!recording || recording->player->GetPlayerStageStats() != stats || seconds < 0) return;
	Json::Value value(Json::arrayValue); value.append(seconds); value.append(life);
	recording->lives.append(value);
}
void GameplayReplay::FinishRecording(bool aborted)
{
	if(!recording) return;
	Capture(true);
	if(!recording) return;
	auto& r = recording->data.root;
	if(CurrentHash() != r["chart_hash"].asString() || float(PREFSMAN->m_fGlobalOffsetSeconds) != r["global_offset"].asFloat())
	{ recording.reset(); SetStatus("Replay not saved: chart timing or calibration changed during play."); return; }
	r["outcome"] = aborted ? "aborted" : STATSMAN->m_CurStageStats.m_bGaveUp ? "gave up" :
		recording->player->GetPlayerStageStats()->m_bFailed ? "failed" : "completed";
	r["autoplay"] = STATSMAN->m_CurStageStats.m_bUsedAutoplay;
	RString basename = r["chart_hash"].asString() + "-" + r["recorded_at"].asString();
	RString path;
	for(int index = 0; index < 1000; ++index)
	{
		path = "Save/Replays/" + basename + ssprintf("-%03d.smr.json", index);
		if(!FILEMAN->IsAFile(path)) break;
		path.clear();
	}
	RString error;
	if(path.empty() || !recording->data.Save(path, error)) SetStatus("Replay not saved: " + (path.empty() ? RString("filename limit reached") : error));
	else { lastFile = Basename(path); SetStatus("Replay saved. Press F10 on evaluation to watch it."); }
	recording.reset();
}
void GameplayReplay::DiscardRecording() { recording.reset(); }

vector<RString> GameplayReplay::FilesForCurrentChart(RString& error)
{
	vector<RString> files;
	if(!CheckContext(error)) return files;
	FILEMAN->GetDirListing("Save/Replays/" + CurrentHash() + "-*.smr.json", files, false, false);
	files.erase(std::remove_if(files.begin(), files.end(), [](const RString& name) { return !SafeFilename(name); }), files.end());
	std::sort(files.rbegin(), files.rend());
	return files;
}

bool GameplayReplay::PreparePlayback(const RString& filename, RString& error)
{
	if(playing || !CheckContext(error)) return false;
	if(!SafeFilename(filename))
	{ error = "Invalid replay filename."; return false; }
	std::unique_ptr<ReplayData> loaded(new ReplayData);
	if(!loaded->Load("Save/Replays/" + filename, error)) return false;
	const auto& r = loaded->root;
	if(!ValidModifiers(r["player_options"].asString(), false, error) ||
		!ValidModifiers(r["song_options"].asString(), true, error)) return false;
	if(r["song"].asString() != GAMESTATE->m_pCurSong->GetSongDir() || r["chart_hash"].asString() != CurrentHash())
	{ error = "This recording belongs to a different or changed song/chart/timing."; return false; }
	auto pn = GAMESTATE->GetMasterPlayerNumber();
	if((r.isMember("steps_type") && r["steps_type"].asString() != GAMEMAN->GetStepsTypeInfo(GAMESTATE->m_pCurSteps[pn]->m_StepsType).szName) ||
		r["style"].asString() != GAMESTATE->GetCurrentStyle(pn)->m_szName ||
		r["tracks"].asInt() != GAMESTATE->GetCurrentStyle(pn)->m_iColsPerPlayer)
	{ error = "Select the same play style used for this recording."; return false; }
	if(!GAMESTATE->m_pCurSong->HasMusic()) { error = "The song's music file is missing."; return false; }
	saved.reset(new SavedSession);
	saved->stats = STATSMAN->m_CurStageStats;
	FOREACH_PlayerNumber(p) saved->players[p] = *GAMESTATE->m_pPlayerState[p];
	saved->songOptions = GAMESTATE->m_SongOptions; saved->position = GAMESTATE->m_Position;
	saved->stageIndex = GAMESTATE->m_iCurrentStageIndex;
	saved->seed = GAMESTATE->m_iStageSeed; saved->offset = PREFSMAN->m_fGlobalOffsetSeconds;
	saved->leadIn = GAMESTATE->m_bGameplayLeadIn; saved->adjustTokens = GAMESTATE->m_AdjustTokensBySongCostForFinalStageCheck;
	playback = std::move(loaded); playing = true;
	return true;
}

bool GameplayReplay::Watch(const RString& filename, RString& error)
{
	if(!PreparePlayback(filename, error)) return false;
	SCREENMAN->AddNewScreenToTop("ScreenGameplayReplay");
	return true;
}

void GameplayReplay::BeginPlayback()
{
	ASSERT(playback && saved);
	GAMESTATE->ResetGameplayRate();
	STATSMAN->m_CurStageStats = StageStats();
	STATSMAN->m_CurStageStats.m_playMode = PLAY_MODE_REGULAR;
	STATSMAN->m_CurStageStats.m_fMusicRate = playback->root["initial_rate"].asFloat();
	auto pn = PlaybackPlayer();
	auto& ps = *GAMESTATE->m_pPlayerState[pn];
	ps.m_ActiveAttacks.clear(); ps.m_ModsToApply.clear();
	PlayerOptions po; po.Init(); po.m_pn = pn; po.FromString(playback->root["player_options"].asString());
	// Recorded outcomes determine failure. Do not let current preferences stop the viewer early.
	po.m_FailType = FailType_Off; po.m_fRandAttack = 0;
	ps.m_PlayerOptions.Assign(ModsLevel_Preferred, po);
	ps.m_HealthState = HealthState_Alive;
	SongOptions so; so.Init(); so.FromString(playback->root["song_options"].asString());
	so.m_fMusicRate = playback->root["initial_rate"].asFloat(); so.m_fHaste = 0;
	so.m_AutosyncType = AutosyncType_Off; so.m_SoundEffectType = SoundEffectType_Off; so.m_bSaveReplay = false; so.m_bSaveScore = false;
	GAMESTATE->m_SongOptions.Assign(ModsLevel_Preferred, so);
	GAMESTATE->m_iStageSeed = playback->root["seed"].asInt();
	GAMESTATE->m_iCurrentStageIndex = playback->root.get("stage_index", 0).asInt();
	PREFSMAN->m_fGlobalOffsetSeconds.Set(playback->root["global_offset"].asFloat());
}
void GameplayReplay::EndPlayback()
{
	if(!saved) return;
	GAMESTATE->ResetGameplayRate();
	STATSMAN->m_CurStageStats = saved->stats;
	FOREACH_PlayerNumber(p) *GAMESTATE->m_pPlayerState[p] = saved->players[p];
	GAMESTATE->m_SongOptions = saved->songOptions; GAMESTATE->m_Position = saved->position;
	GAMESTATE->m_iCurrentStageIndex = saved->stageIndex;
	GAMESTATE->m_iStageSeed = saved->seed; PREFSMAN->m_fGlobalOffsetSeconds.Set(saved->offset);
	GAMESTATE->m_bGameplayLeadIn.Set(saved->leadIn); GAMESTATE->SetPaused(false);
	GAMESTATE->m_AdjustTokensBySongCostForFinalStageCheck = saved->adjustTokens;
	saved.reset(); playback.reset(); playing = false;
}

LuaFunction(GetReplayStatus, GameplayReplay::Status());
LuaFunction(GetLastReplayFile, GameplayReplay::LastFile());
LuaFunction(IsReplay, GameplayReplay::IsPlaying());
