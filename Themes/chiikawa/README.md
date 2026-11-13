# Chiikawa theme for StepMania 6

The default StepMania 6 theme, with cream and pastel menus, gentle menu motion, AI-generated Chiikawa character decorations, and a silent dancing-trio gameplay background. It inherits the original **Classic** theme's fonts, notes/receptors, judgment labels and effects, live speed controls, life graph, score handling, and recording/replay features.

## Default theme and switching

Fresh installations select **Chiikawa** automatically through [Data/Defaults.ini](../../Data/Defaults.ini). Existing installations keep their saved theme choice. To switch, choose **Options → Display Options → Appearance Options → Theme → Chiikawa**, then save/exit that menu. Choose **Classic** there to use the original appearance.

Keep this folder named **`Themes/chiikawa`**, alongside `Themes/default` and `Themes/_fallback`. The original theme is labeled **Classic** but retains its internal `default` directory for inheritance and compatibility with saved `Theme=default` settings. Existing songs, profiles, logs and recordings remain in their usual folders.

For an existing portable installation, copy the updated `Themes/` files and `Data/Defaults.ini` into the same relative locations. The defaults do not overwrite a saved choice; switch themes in Options once if necessary. This default/label change needs no executable rebuild.

The theme's Lua, PNG and MP4 files control artwork, movies and random selection. Its replay-browser layout also uses the optional replay actor commands added in this source revision, so use a matching executable for the complete appearance. An older executable still shows its original replay browser. A new Windows ZIP has not been built for this revision.

To freeze the title dancers and disable moving menu backgrounds, use **Options → Theme Options → Fancy UI Background → Off**. The cream background and characters remain. This preference is stored as `FancyUIBG` under `[chiikawa]` in `Save/ThemePrefs.ini`.

## Dancing title illustration

The right side of the **Game Start / Options** title menu shows Chiikawa, Hachiware and Usagi stepping and hopping on three pastel cartoon pads. The transparent animation repeats every **3.2 seconds**, with the pads held still and the feet returning to the center tiles. It restarts when returning to the title menu. **Fancy UI Background → Off** keeps a neutral still pose.

[View the looping GIF](../../Docs/media/title-dance.gif) or [see the actual title menu](../../Docs/theme-preview/chiikawa/native/title.png). The interactive comparison, source artwork and assembly tools are retained on the [development branch](../../Docs/ChiikawaDevelopment.md) under `Docs/theme-preview/chiikawa/title-dance/`. The native theme uses four drawn PNG poses per character with smooth Lua movement; its GIF loader only reads the first frame. The PNG version preserves smooth alpha edges. Copy the updated `Themes/chiikawa/` folder into an existing installation to use this title animation; it needs no executable rebuild.

## Random per screen

A curated illustration is chosen each time song selection, single-song evaluation, or the replay browser opens. Consecutive visits to the same screen category avoid its previous illustration. The memory lasts until theme reload or game exit. The title menu always uses the dancing trio described above.

The selected decoration stays fixed while that screen is open. Moving the song wheel does not reroll it. Returning from a replay to the existing browser keeps the browser's current choice; closing and reopening the browser chooses again. These cutouts are static. The separate gameplay fallback movie animates the dancing trio.

| Screen | Eligible decorations |
| --- | --- |
| Song selection | Welcoming trio, Listening together, Acoustic duet, Piano duet, Drum circle, Violin serenade, Flute practice, Saxophone swing |
| Single-song evaluation | Celebration trio, Shared jump, Drum circle, Saxophone swing |
| Replay browser | Resting pair, Listening together, Acoustic duet, Piano duet, Violin serenade, Flute practice |

All eleven cutouts contain at least two recognizable characters. The five latest additions feature **piano, drums, violin, flute, and saxophone** with different poses and compositions. [View the included artwork](Art/README.md) or [see the native screens](../../Docs/theme-preview/chiikawa/native/README.md).

Gameplay and replay playback add no separate character cutouts, keeping song images, song movies and the fallback movie/poster unobstructed. Notes, judgments, the screen filter and the life graph retain their normal layers. Single-player song evaluation fits an illustration up to 345 × 270 theme pixels beside the judgment panel, on the right for P1 and left for P2. Its width adapts to narrower screens while leaving space above the detail rows. Course/summary and multiplayer layouts keep their inherited presentation.

To fix an older installation that shows characters over song backgrounds, close the game and replace [BGAnimations/ScreenGameplay underlay.lua](BGAnimations/ScreenGameplay%20underlay.lua) and [Scripts/06 Chiikawa.lua](Scripts/06%20Chiikawa.lua), then restart. This change needs no executable rebuild.

## Gameplay timing feedback

With Pro Timing enabled, each tap's millisecond offset appears above its receptor (below it with reverse scrolling) and is drawn in front of the score bar. The small gameplay accuracy readout is removed; whole-song accuracy remains on the evaluation screen.

For this update, close the game, replace [Themes/default/BGAnimations/ScreenGameplay overlay/default.lua](../default/BGAnimations/ScreenGameplay%20overlay/default.lua) and [Themes/default/Graphics/Player judgment/default.lua](../default/Graphics/Player%20judgment/default.lua), then restart. Chiikawa inherits these shared Classic actors. No executable rebuild is needed.

## Dancing-trio gameplay background

When a song has no background image, the theme can use a **30-second silent loop** of Chiikawa, Hachiware and Usagi dancing on DDR pads. It combines the three supplied clips in order, without re-encoding, at 1280×720 and 24 fps. [Preview the MP4](Movies/ddr-trio-loop.mp4) or see the [development branch](../../Docs/ChiikawaDevelopment.md) for source clips and the assembly recipe.

The movie uses the normal gameplay background pipeline, beneath the notes, judgments and life-history graph. It loops, follows live speed changes and pause/resume, and also appears in replays. The existing background-brightness setting controls its visibility; the preview uses 45%. Choreography is decorative and does not match every chart's steps or beat.

Song-supplied images, scripted movies and the engine's configured random-background selection keep their usual precedence. **Static Background** in song options, or `SongBackgrounds=0` in `Save/Preferences.ini`, uses the matching still poster. Turning **Fancy UI Background** off affects menu motion only. The poster also supplies missing-art thumbnails. Standard background scaling may crop the outer edges on 4:3 displays.

To add this movie to an existing portable installation, close the game and copy **`Themes/chiikawa/`** and **`BackgroundEffects/StretchNormal.lua`** from this revision into the same relative locations. Keep `Themes/default` and `Themes/_fallback`. This movie change does not require recompiling the existing Windows executable. Saved settings, songs and recordings stay in their current folders; no new ZIP has been built for this revision.

## Upload Statistics

The upload menu uses the title menu’s bold font, with a larger selected option.
A prominent status headline uses a soft shadow and blue for progress, green for
success, amber for warnings, or rose for errors. Details wrap beneath it, and
unavailable actions explain why they cannot proceed. See the
[connection setup guide](../../Docs/DashboardUpload.md).

## Customize or maintain

- [Scripts/06 Chiikawa.lua](Scripts/06%20Chiikawa.lua) defines `Pools`, placement in `Layout`, and the menu background. Active cutout pools are `select`, `result` and `replay`; the title/game entries are retained for older gallery concepts. Change these Lua values without recompiling. Keep each pool nonempty and use PNG filenames from `Art/` without the extension.
- The private selector avoids using or reseeding `math.random`. This keeps decoration choices separate from gameplay randomness and recorded outcomes.
- The random-decoration underlays select art in `OnCommand`, after preloading. Functional gameplay/evaluation actors are loaded from Classic (`Themes/default`) explicitly, so the life graph and other behavior remain inherited.
- [metrics.ini](metrics.ini) sets `Background/FallbackMovie` to the MP4 path. Set it to `""` to keep the poster for all missing-art songs; an absent movie file also falls back to the poster. [BackgroundEffects/StretchNormal.lua](../../BackgroundEffects/StretchNormal.lua) applies this optional metric while retaining the normal music clock and focus handlers.
- [metrics.ini](metrics.ini) styles the replay browser through `ReplayBackdrop`, `ReplayTitle`, `ReplayList`, and `ReplayHelp` commands. Original themes need no new metrics.
- [Graphics/LifeHistory graph.lua](Graphics/LifeHistory%20graph.lua) only darkens evaluation labels for cream backgrounds. Their 80% opacity and the graph's original line/fill opacity remain intact.
- [Scripts/07 TitleDance.lua](Scripts/07%20TitleDance.lua) controls the title loop's duration, step/hop distances, pose timing and fixed pad layout. It uses a periodic clock without gameplay randomness. The title's underlay calls `Chiikawa.TitleDance()`.
- The remaining decorated screens use random per screen. Their pools are in [artwork-pools.json](../../Docs/theme-preview/chiikawa/artwork-pools.json); the retained title pool is for the older gallery comparisons.

The [released asset manifest](../../Docs/theme-preview/chiikawa/release-manifest.json) records hashes and source attribution. Exact prompts, source outputs, alternatives and assembly scripts are retained on the [development branch](../../Docs/ChiikawaDevelopment.md). The two menu textures are unchanged copies of the original theme. These illustrations are AI-generated fan artwork, not official Chiikawa artwork.

## Verification

The local Linux build and focused Lua selection/layout checks pass. Isolated native captures cover title, selection, gameplay, evaluation and replay at 16:9 and 4:3, including P2-only and centered fields, rate/reset controls, gameplay pause, replay pause/restart/return, cached title returns, and motion off/on. Replay checks compare saved profile, recording and upload hashes before/after viewing. [Native screenshots](../../Docs/theme-preview/chiikawa/native/README.md) show the actual engine separately from the browser concepts.

The dance movie additionally passes full decoding and exact frame-preservation checks, plus native Linux playback with FFmpeg enabled. Native checks cover looping, live rate changes, gameplay/replay pause and resume, replay restart, still-poster settings, own-art/scripted-video precedence, centered/P2 layouts and the original default-theme fallback.

The title animation passes periodicity, pose coverage, RNG isolation, lifecycle and layout checks. Native Linux captures confirm moving/still modes, cached title returns and menu navigation at 16:9 and 4:3. The transparent GIF contains 96 frames totaling 3.2 seconds, and its rendered loop endpoint matches its start exactly.

The gameplay-decoration fix was checked with song-provided images and scripted movies, fallback movie/poster settings, P1/P2 at 16:9/4:3, and recorded movie playback. Selection and evaluation still display their illustrations, and the final native logs have no Lua command errors.

Windows x64 rendering, audible audio/video synchronization, controller play and unusual song scripts still need checking on the next Windows build. The Linux fixture uses software OpenGL and null audio; it cannot verify those behaviors. Existing default-theme dialogs and functional panels remain inherited.
