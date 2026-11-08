# Chiikawa theme for StepMania 6

An optional theme with cream and pastel menus, gentle background motion, and AI-generated Chiikawa character decorations. It inherits the default theme's fonts, notes/receptors, judgment labels and effects, live speed controls, life graph, score handling, and recording/replay features.

## Enable it

Keep this folder named **`Themes/chiikawa`**, alongside `Themes/default` and `Themes/_fallback`. From the title menu, choose **Options → Display Options → Appearance Options → Theme → Chiikawa - StepMania 6**, then save/exit that menu. Existing songs, profiles, logs, and recordings remain in their usual folders.

The theme's Lua/PNG files control artwork and random selection. Its replay-browser layout also uses the optional replay actor commands added in this source revision, so use a matching executable for the complete appearance. An older executable still shows its original replay browser. A new Windows ZIP has not been built for this revision.

To disable moving menu backgrounds, use **Options → Theme Options → Fancy UI Background → Off**. The cream background and characters remain. This preference is stored as `FancyUIBG` under `[chiikawa]` in `Save/ThemePrefs.ini`.

## Random per screen

A curated illustration is chosen each time the title, song selection, gameplay, single-song evaluation, or replay browser opens. Consecutive visits to the same screen category avoid its previous illustration. Cached title-screen returns also choose again. The memory lasts until theme reload or game exit.

An illustration stays fixed while that screen is open. Moving the song wheel, changing playback speed, pausing gameplay, or pausing/restarting a replay does not reroll it. Returning from a replay to the existing browser keeps the browser's current choice; closing and reopening the browser chooses again. Characters are static; only menu backgrounds animate.

| Screen | Eligible decorations |
| --- | --- |
| Title and song selection | Welcoming trio, Listening together, Acoustic duet, Piano duet, Drum circle, Violin serenade, Flute practice, Saxophone swing |
| Gameplay | Resting pair, Listening together, Piano duet, Violin serenade, Flute practice |
| Single-song evaluation | Celebration trio, Shared jump, Drum circle, Saxophone swing |
| Replay browser | Resting pair, Listening together, Acoustic duet, Piano duet, Violin serenade, Flute practice |

All eleven cutouts contain at least two recognizable characters. The five latest additions feature **piano, drums, violin, flute, and saxophone** with different poses and compositions. [Preview all artwork](../../Docs/theme-preview/chiikawa/index.html) or [open the image folder](../../Docs/theme-preview/chiikawa/images).

Gameplay art uses the empty side of a one-player, non-centered, one-side field with at most four columns. It moves left for P2, above the life graph and clear of the life bar. Two-player, centered, double/shared-side, wider-column, and demonstration fields omit characters. Single-song evaluation uses the unused grade area; course/summary layouts keep their inherited presentation.

## Customize or maintain

- [Scripts/06 Chiikawa.lua](Scripts/06%20Chiikawa.lua) defines `Pools`, placement in `Layout`, and the menu background. Change these Lua values without recompiling. Keep each pool nonempty and use PNG filenames from `Art/` without the extension.
- The private selector avoids using or reseeding `math.random`. This keeps decoration choices separate from gameplay randomness and recorded outcomes.
- Each screen's underlay selects its art in `OnCommand`, after preloading. Functional gameplay/evaluation actors are loaded from the default theme explicitly, so the life graph and other behavior remain inherited.
- [metrics.ini](metrics.ini) styles the replay browser through `ReplayBackdrop`, `ReplayTitle`, `ReplayList`, and `ReplayHelp` commands. Original themes need no new metrics.
- [Graphics/LifeHistory graph.lua](Graphics/LifeHistory%20graph.lua) only darkens evaluation labels for cream backgrounds. Their 80% opacity and the graph's original line/fill opacity remain intact.
- The browser gallery's Recommended/Manual controls are comparison tools. The playable theme always uses random per screen. Keep gallery and Lua pools in sync with [artwork-pools.json](../../Docs/theme-preview/chiikawa/artwork-pools.json).

Generated PNGs are copied unchanged from the preview package. [Exact prompts and provenance](../../Docs/theme-preview/chiikawa/PROMPTS.md) identify the source outputs, characters, and hashes. The two menu textures are unchanged copies of the original theme. These illustrations are AI-generated fan artwork, not official Chiikawa artwork.

## Verification

The local Linux build and focused Lua selection/layout checks pass. Isolated native captures cover title, selection, gameplay, evaluation and replay at 16:9 and 4:3, including P2-only and centered fields, rate/reset controls, gameplay pause, replay pause/restart/return, cached title returns, and motion off/on. Replay checks compare saved profile, recording and upload hashes before/after viewing. [Native screenshots](../../Docs/theme-preview/chiikawa/native/README.md) show the actual engine separately from the browser concepts.

Windows x64 rendering, audible audio/video synchronization, controller play and unusual song scripts still need checking on the next Windows build. The Linux fixture uses software OpenGL and null audio; it cannot verify those behaviors. Existing default-theme dialogs and functional panels remain inherited.
