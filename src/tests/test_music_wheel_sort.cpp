// Headless checks linked against the application's actual wheel implementation.
#include "global.h"
#include "RageSound.h"
#include "GameConstantsAndTypes.h"
#include "MusicWheelItem.h"
#include "ThemeMetric.h"
#include "WheelBase.h"
// Seed cached song lists without loading textures, fonts, or a song library.
// Dependencies above keep this access change confined to MusicWheel's cache.
#define private public
#include "MusicWheel.h"
#undef private
#include "ActorUtil.h"
#include "CommonMetrics.h"
#include "GameManager.h"
#include "GameState.h"
#include "LuaManager.h"
#include "PrefsManager.h"
#include "RageFile.h"
#include "RageFileManager.h"
#include "RageLog.h"
#include "RageSoundManager.h"
#include "RageThreads.h"
#include "ScreenDimensions.h"
#include "ScreenManager.h"
#include "Song.h"
#include "SongManager.h"
#include "SongUtil.h"
#include "arch/ArchHooks/ArchHooks.h"
#include <cassert>
#include <iostream>

extern ThemeMetric<bool> SHOW_SECTIONS_IN_BPM_SORT;

class TestTheme : public ThemeManager
{
public:
	void Load() { LoadThemeMetrics("test", "en"); }
};

class TestMusicWheel : public MusicWheel
{
public:
	TestMusicWheel()
	{
		NUM_WHEEL_ITEMS_TO_DRAW.Load("MusicWheel", "NumWheelItems");
		REMIND_WHEEL_POSITIONS.Load("MusicWheel", "RemindWheelPositions");
		HIDE_INACTIVE_SECTIONS.Load("MusicWheel", "OnlyShowActiveSection");
		HIDE_ACTIVE_SECTION_TITLE.Load("MusicWheel", "HideActiveSectionTitle");
		SORT_ORDERS.Load("MusicWheel", "SortOrders");
		m_exprItemTransformFunction.SetFromReference(THEME->GetMetricR("MusicWheel", "ItemTransformFunction"));
		for(int i = 0; i < 3; ++i)
			m_WheelBaseItems.push_back(new WheelItemBase("Test"));
		m_iSelection = 0;
		m_Moving = 0;
		m_TimeBeforeMovingBegins = 0;
		m_fPositionOffsetFromSelection = 0;
		m_fTimeLeftInState = 0;
		m_bEmpty = false;
		FOREACH_ENUM(SortOrder, so)
			m_WheelItemDatasStatus[so] = VALID;
		WheelBase::BeginScreen();
	}

	void Cache(SortOrder sort, const vector<Song*>& songs)
	{
		// Deliberately give the same song different indices/sections per sort.
		std::map<RString, vector<Song*>> sections;
		for(auto song : songs)
			sections[SongUtil::GetSectionNameFromSongAndSort(song, sort)].push_back(song);
		for(const auto& section : sections)
		{
			if(!section.first.empty())
				Add(sort, WheelItemDataType_Section, nullptr, section.first);
			for(auto song : section.second)
				Add(sort, WheelItemDataType_Song, song, section.first);
		}
	}

	void Start(SortOrder sort, Song* song)
	{
		GAMESTATE->m_SortOrder.Set(sort);
		m_CurWheelItemData.clear();
		m_iSelection = 0;
		m_Moving = 0;
		WheelBase::BeginScreen();
		assert(SelectSong(song));
		RememberHighlight();
	}

	void FinishSort()
	{
		assert(m_WheelState == STATE_FLYING_OFF_BEFORE_NEXT_SORT);
		UpdateSwitch();
		assert(m_WheelState == STATE_FLYING_ON_AFTER_NEXT_SORT);
		UpdateSwitch();
		assert(m_WheelState == STATE_SELECTING);
	}

	void RememberHighlight()
	{
		// ScreenSelectMusic::AfterMusicChange runs on queued SM_SongChanged.
		auto song = GetSelectedSong();
		GAMESTATE->m_pCurSong.Set(song);
		if(song)
			GAMESTATE->m_pPreferredSong = song;
	}

private:
	void Add(SortOrder sort, WheelItemDataType type, Song* song, const RString& section)
	{
		auto item = new MusicWheelItemData(type, song, section, nullptr, RageColor(1,1,1,1), 0);
		m__UnFilteredWheelItemDatas[sort].push_back(item);
		m__WheelItemDatas[sort].push_back(item);
	}
};

static const SortOrder sorts[] = {
	SORT_PREFERRED, SORT_GROUP, SORT_TITLE, SORT_BPM, SORT_ARTIST, SORT_GENRE
};

static void CheckSong(TestMusicWheel& wheel, Song* expected, const char* scenario)
{
	auto actual = wheel.GetSelectedSong();
	if(actual != expected)
	{
		std::cerr << scenario << ": expected " << expected->m_sMainTitle
			<< " / " << expected->m_sArtist << ", got "
			<< (actual ? actual->m_sMainTitle + " / " + actual->m_sArtist : "<section>") << std::endl;
		std::abort();
	}
}

static void Chord(TestMusicWheel& wheel, int firstDirection, bool splitFrames)
{
	// Match the screen's first-arrow and both-arrows handling.  In a single
	// batch both keys are already down; across frames the first starts Move.
	if(splitFrames)
	{
		wheel.Move(firstDirection);
		wheel.RememberHighlight();
	}
	else
		wheel.ChangeMusicUnlessLocked(firstDirection);
	wheel.Move(0);
	wheel.ChangeMusicUnlessLocked(-firstDirection);
	assert(wheel.NextSort());
	wheel.FinishSort(); // Child wheel updates before queued song notifications.
	wheel.RememberHighlight();
}

int main(int argc, char** argv)
{
	assert(argc == 2);
	RageThreadRegister thread("Music wheel sort test");
	SetCommandlineArguments(argc, argv);
	HOOKS = ArchHooks::Create();
	HOOKS->Init();
	ActorUtil::InitFileTypeLists();
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
	SOUNDMAN = new RageSoundManager;

	RageFile metrics;
	assert(metrics.Open("/Themes/test/metrics.ini", RageFile::WRITE));
	metrics.Write(
		"[Global]\nFallbackTheme=\n"
		"[Common]\nScreenWidth=640\nScreenHeight=480\nAutoSetStyle=false\nImageCache=\"\"\n"
		"[SongManager]\nNumSongGroupColors=0\nNumCourseGroupColors=0\nNumProfileSongGroupColors=0\n"
		"[MusicWheel]\nNumWheelItems=1\nItemTransformFunction=function(self) end\nRemindWheelPositions=false\n"
		"OnlyShowActiveSection=false\nHideActiveSectionTitle=false\nShowSectionsInBPMSort=false\n"
		"SortOrders={ 'SortOrder_Preferred', 'SortOrder_Group', 'SortOrder_Title', 'SortOrder_BPM', 'SortOrder_Artist', 'SortOrder_Genre' }\n");
	metrics.Close();
	for(const char* part : {"middle", "top", "bottom", "TickThumb", "StretchThumb"})
	{
		RageFile actor;
		assert(actor.Open(RString("/Themes/test/Graphics/ScrollBar ") + part + ".lua", RageFile::WRITE));
		actor.Write("return { Class='Actor', InitCommand=function(self) self:setsize(8,100) end }");
		actor.Close();
	}
	FILEMAN->FlushDirCache();
	assert(FILEMAN->IsAFile("/Themes/test/Graphics/ScrollBar middle.lua"));
	auto theme = new TestTheme;
	theme->Load();
	ScreenDimensions::ReloadScreenDimensions();
	CommonMetrics::AUTO_SET_STYLE.Read();
	CommonMetrics::IMAGES_TO_CACHE.Read();
	SHOW_SECTIONS_IN_BPM_SORT.Read();
	SONGMAN = new SongManager;
	SCREENMAN = new ScreenManager;

	Song before, target, after, missing;
	before.m_sMainTitle = "Before";
	before.m_sArtist = "Artist Z";
	target.m_sMainTitle = "Target";
	target.m_sArtist = "Artist M";
	after.m_sMainTitle = "After";
	after.m_sArtist = "Artist A";
	missing.m_sMainTitle = "Missing";
	for(auto song : {&before, &target, &after})
	{
		song->m_sGroupName = "Test pack";
		song->m_sGenre = "Test genre";
	}

	TestMusicWheel wheel;
	for(auto sort : sorts)
		wheel.Cache(sort, sort == SORT_ARTIST ? vector<Song*>{&after, &target, &before}
			: vector<Song*>{&before, &target, &after});

	// Regression: split-frame Left+Right leaves the neighbor in the preference,
	// but the highlighted item has already returned to the original song.
	for(bool splitFrames : {true, false})
	for(int direction : {-1, 1})
	{
		wheel.Start(SORT_BPM, &target);
		Chord(wheel, direction, splitFrames);
		assert(GAMESTATE->m_SortOrder == SORT_ARTIST);
		CheckSong(wheel, &target, "BPM -> Artist via Left+Right");

		// Repeated shortcuts must preserve identity through two complete cycles.
		for(int i = 0; i < 12; ++i)
		{
			Chord(wheel, direction, splitFrames);
			CheckSong(wheel, &target, "Repeated sort cycle");
		}
	}

	// Direct sort changes also preserve a highlight ahead of its notification.
	for(auto sort : sorts)
	{
		wheel.Start(SORT_BPM, &target);
		GAMESTATE->m_pPreferredSong = &before;
		GAMESTATE->m_pCurSong.Set(&before);
		assert(wheel.ChangeSort(sort, true));
		wheel.FinishSort();
		CheckSong(wheel, &target, "Stale preferred/current song");
	}

	// A section heading should still fall back to the preferred song.
	wheel.Start(SORT_GROUP, &target);
	assert(wheel.SelectSection("Test pack"));
	assert(wheel.GetSelectedSong() == nullptr);
	assert(wheel.ChangeSort(SORT_ARTIST));
	wheel.FinishSort();
	CheckSong(wheel, &target, "Section-heading fallback");

	// A song absent from a destination list keeps the old fallback policy.
	TestMusicWheel filtered;
	filtered.Cache(SORT_GROUP, {&before, &target, &after});
	filtered.Cache(SORT_ARTIST, {&after, &before});
	for(int fallback = 0; fallback < 3; ++fallback)
	{
		filtered.Start(SORT_GROUP, &target);
		GAMESTATE->m_pPreferredSong = fallback == 0 ? &before : &missing;
		GAMESTATE->m_pCurSong.Set(fallback == 1 ? &before : &target);
		assert(filtered.ChangeSort(SORT_ARTIST));
		filtered.FinishSort();
		CheckSong(filtered, fallback < 2 ? &before : &after, "Unavailable-song fallback");
	}

	std::cout << "Music wheel sort checks passed: both arrow orders, same/split frames, "
		"repeated cycles, stale preferences, and fallbacks." << std::endl;
}
