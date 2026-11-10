# Chiikawa development plan

Theme work belongs on `feature/chiikawa-theme`; `main` receives finished runtime assets, required shared hooks/tests, release documentation and native screenshots. Keep prompts, alternatives, original clips, the browser gallery and detailed journals on this feature branch.

## Branch separation

This completed split used the former branch name `5_1-new`. The default branch is now `main`; use that name for ongoing development and promotion.

1. Preserve the complete theme history and all development sources on the feature branch, with a separate worktree.
2. Prepare a release commit from the published `5_1-new` tip, carrying the latest verified theme and removing development-only files from its current tree. Preserve all already-published commits.
3. Keep final screenshots, user documentation and an asset checksum manifest on the main branch. Update links and describe how to promote future completed work selectively.
4. Verify runtime files are identical to the tested movie revision, run focused Lua checks, validate links and provenance, and commit the release snapshot.
5. Move local `5_1-new` to that snapshot, then merge its baseline into the feature branch while preserving the development package. Keep subsequent theme plans/progress in these feature-only files. Do not push or package a ZIP in this task.

The original history remains available below and in Git. Engine changes unrelated to the theme continue on `main`.

Completed: preserved the development package, created and verified the main release snapshot, documented selective promotion and reconciled the shared baseline. Continue theme work here and update these dedicated journals at checkpoints.

## Earlier theme plan

### Chiikawa artwork preview


Create a reviewable artwork plan and four AI-generated interface decoration previews using recognizable Chiikawa, Hachiware, and Usagi with warm cream, pastel pink, mint, and sky blue. The user confirmed characters and palette after requesting explicit questions in chat. Store the plan, exact prompts, preview gallery, images, and inspection metadata in `Docs/theme-preview/chiikawa/`. Preserve the current playable theme during this preview stage. Follow the detailed plan in that folder and commit at the plan and completed-preview checkpoints.


Completed the artwork-preview stage: four generated illustrations, exact prompts and provenance, a local gallery with five screen concepts, and checks of image integrity/transparency, layout, links, and text contrast. The current theme remains unchanged. See the preview folder's plan and `PROGRESS.md` for inspection notes and the subsequent playable-theme steps.


## Follow-up: original gameplay visuals and varied Chiikawa artwork

Preserve the original theme's font atlases, noteskin arrow/receptor sprites, and judgment artwork, with optional color tint only. Correct the preview's browser-font/schematic-arrow representations by reusing actual assets. Add six varied illustration candidates (solo listening, guitar, leap, group picnic, vertical peekers, resting pair), retain the first four, and extend the gallery for comparison. Record exact prompts, provenance, inspection, and local-browser validation. Commit at plan and completed-preview checkpoints.


Completed: added six distinct illustration candidates (ten total), replaced schematic gameplay assets and browser typography with the original font/sprite artwork, added comparison controls, and updated exact prompts, provenance, and inspection notes. Verified source hashes, transparency, local references, script syntax, and five layouts at both aspect ratios. Details are in `PROGRESS.md`; the preview is ready for review before later playable-theme integration.


## Follow-up: paired Chiikawa illustrations and animated backgrounds

Remove the rejected vertical peekers and replace the three solo candidates with distinct two-character compositions. Every active illustration must show at least two recognizable characters. Retain original fonts, arrows, and judgment artwork. Inspect the default theme's Lua background animation, add a gentle animated title/menu demonstration and motion toggle to the preview, and document a native `FancyUIBG`-controlled implementation without recompiling. Validate artwork and layouts, update provenance/progress, and commit at plan/completion checkpoints.


Completed: replaced three solo illustrations with distinct pairs, removed the rejected vertical peekers, and kept nine active multi-character candidates. Added a verified animated menu-background preview using original texture shapes, motion controls, and reduced-motion handling, plus source-linked native Lua integration notes. Verified images, character counts, original asset hashes, local links, syntax, both layout ratios, and moving/paused/resumed states.


## Follow-up: choosing and randomizing character decorations

Recommend artwork by screen purpose and add Recommended, Random, and Manual modes to the preview. Random mode chooses from a curated set once when entering each screen and avoids immediately repeating that screen's previous choice. Keep gameplay art fixed for the whole song; resizing, animation toggles, and repeated clicks on the active screen must not reroll it. Provide an explicit Show another control for preview comparison. Preserve full painted title scenes as independent choices. Document the native Lua route and commit after focused browser verification.


Completed per-screen recommendations and random/manual gallery controls, including no immediate repeats, stable artwork during a screen visit, explicit comparison, and fixed painted scenes. Documented the eligible groups and native Lua approach; focused browser and link/syntax checks pass.


## Follow-up: playable Chiikawa theme and five instrument illustrations

Implement the agreed random-per-screen policy in a separate `Themes/chiikawa/` theme inheriting `default`. Preserve the original fonts, noteskin, judgments, gameplay scoring/speed/replay behavior, and graph placement. Choose from curated pools on visible screen entry, avoid immediate repeats, and hold the same illustration throughout gameplay, pauses, rate changes, and replay viewing. Account for cached title screens and hide decorations where two-player/centered/double layouts leave no clear space.

Generate five additional transparent multi-character scenes featuring piano, drums, violin, flute, and saxophone. Keep at least two recognizable Chiikawa characters in every illustration, the warm pastel palette, and varied compositions. Save exact prompts and full-resolution outputs, extend provenance and the preview gallery, and share consistent pools between documentation, preview, and native theme.

Add cream/pastel menu backgrounds with the original-style optional animation, and place randomized illustrations on title, song selection, gameplay, evaluation, and replay screens. Inherit functional actors; add minimal replay theming hooks only if the existing C++ widgets prevent integration, preserving default-theme behavior.

Checkpoints: commit this plan; commit inspected artwork and updated pools; implement the native theme, verify Lua and actual rendered screens in the isolated fixture, document installation and limits, and commit the completed feature. Test random selection stability and no repeats, valid assets, cached screen returns, 16:9/4:3, P1/P2 and crowded layouts, motion off/on, gameplay speed/pause, and replay return. No Windows ZIP or remote push is requested.

Completed: added the selectable inherited Chiikawa theme, five new instrument scenes, stable random selection in screen underlays, optional menu motion, and minimal replay-browser styling hooks. Native Linux rendering and focused selection/graph/replay checks passed; screenshots, installation guidance and Windows/audio/video limits are recorded in the theme guide and `PROGRESS.md`.


## Follow-up: inactive selection panel

The native Chiikawa song-selection screenshot shows a black/gray unused P2 difficulty panel. Direct `TestInitialScreen=ScreenSelectMusic` startup temporarily sets a versus style before loading decorations, so their initialization-time `IsHumanPlayer` check treats both sides as active. `BeginScreen` then selects a single-player style, leaving the unused backdrop visible.

Make each inherited difficulty pane use actual joined-side state, refresh visibility on screen entry, and hide when its player leaves. Keep the existing joined-player chart data and animations. Verify direct entry, ordinary title-to-selection navigation, P1-only, P2-only and a second player joining at 16:9/4:3. Refresh the native selection screenshot, record checks and commit this focused Lua correction. No executable rebuild is needed.

Completed: both panes now follow joined-player state on initialization, screen entry, join and leave. Native checks passed for direct entry at 16:9/4:3, normal P1-only and P2-only navigation, late joins with populated chart details, and the original default theme. Replaced the selection preview with an unedited capture from normal P1 navigation.

## Follow-up: dancing-trio gameplay fallback movie

Replace the Chiikawa theme's no-song-background fallback with a silent looping movie of Chiikawa, Hachiware and Usagi playing DDR. Use a vibrant cream/pink/mint/sky-blue arcade scene, expressive footwork and smooth camera orbits, low-angle pushes and pullbacks. Preserve the original notes, fonts and judgments; keep the note field readable through the engine's background brightness/filter controls. Song-supplied images and scripted movies retain precedence.

1. Confirm the existing fallback actor, movie timing and theme-only integration path. Inspect the native preview chart separately because it may supply its own background.
2. Create a reference keyframe and exact animation brief in `Docs/theme-preview/chiikawa/dance-movie/`, with provenance. The built-in generator produces images; a full generated movie requires a connected video provider. Discover a provider and ask the user to choose that route or a local illustrated animation before producing the clip.
3. Produce and inspect a short seamless silent loop. Export a Windows-compatible movie plus a still poster. Integrate it through the existing song-background pipeline so rate changes, pause, replay and brightness retain their engine behavior. Respect static-background settings and avoid redundant character decorations over the movie.
4. Verify songs with no art, existing art and scripted video, movie looping, rate changes/pause, 16:9/4:3 readability, and poster behavior. Refresh the genuine gameplay capture, document settings and limitations, and commit at plan/artwork/implementation checkpoints. No ZIP is requested.

Investigation and reference-art checkpoint complete: the theme-only fallback route is viable; the current preview uses a song-supplied dark image. Saved an inspected DDR trio keyframe, exact motion prompt and provenance in `Docs/theme-preview/chiikawa/dance-movie/`. Actual movie production and runtime integration are pending the user's choice of connected video generation or local illustrated animation.

## Follow-up: center-tile dance reference and varied choreography

The user generated trial movies and reports repeated hopping on the rear/up arrow, matching the first keyframe's stance. Edit the existing reference so all three characters stand with both feet on their own neutral center tile. Preserve the arcade, palette, character identities and pad geometry; save a versioned replacement and retain the original for comparison. Expand the movie prompt with travel across all four directional panels, running steps, jumps and a spin, returning to center for the loop. Inspect the actual edited feet/tile contact, update provenance and preview instructions, and commit the completed artwork/prompt checkpoint. Movie generation now happens externally; engine integration awaits a chosen clip.

Completed: saved and inspected the center-tile v2 reference, added an explicit eight-second run/jump/spin sequence with travel across all directional panels, and updated use instructions and provenance while retaining v1. Image integrity, historical preservation and local links pass verification. The user can regenerate externally with v2; movie playback integration still awaits a chosen clip.

## Follow-up: assemble and install the supplied dance movies

The user supplied three 1280×720 H.264 clips, each containing 240 video frames at 24 fps (10 seconds), with AAC audio. Concatenate their video streams in filename order into a 30-second silent loop, preserving every frame and the original uploads. Use explicit 10-second segment durations so the slightly longer audio tracks cannot introduce timing gaps. Inspect each join and the wraparound, record source/output hashes and a reproducible assembly recipe, and save the runtime MP4 under `Themes/chiikawa/Movies/`.

Extract a matching still poster for the common fallback. Add an optional theme fallback-movie metric to the existing StretchNormal background effect, respecting Static Background and disabled song backgrounds. This keeps the movie inside the engine's existing music-clock, brightness, rate, pause and scripted-background pipeline, while thumbnails use a bitmap. Suppress the random gameplay cutout on songs using the trio fallback so it cannot cover the dancers.

Validate the silent output, exact frame preservation, poster/asset links, theme fallback selection and original-theme behavior. Enable movie decoding in an isolated Linux verification build if feasible and render true background-free gameplay, own-art/scripted movies, static/disabled settings, rate changes, pause/replay and both aspect ratios. Refresh the native gameplay preview, document portable-update files and limitations, and commit at plan/asset/implementation checkpoints. No Windows ZIP is requested.


Completed: concatenated and verified all three uploads into the silent 30-second MP4, installed the movie and matching poster through the optional theme metric, and suppressed duplicate no-art gameplay cutouts. The decoder-enabled Linux build, focused Lua checks and native playback/layout checks pass. Refreshed the authentic gameplay preview and documented settings, provenance, rebuild steps and portable-update files. See `PROGRESS.md` for the evidence and Windows/audio limitations. No Windows ZIP was built.


## Publish the movie-background gameplay capture

Use the existing authentic native movie-background screenshot, preserving its pixels and provenance. Add a movie link beside it, promote the finished preview documentation to main, publish the development branch before the release branch, and verify the image/document/movie blobs on GitHub. Keep the experiment package on the feature branch. No Windows ZIP is requested.


Completed: published the movie-background capture and adjacent MP4 link on `main`, verified all three remote blobs against the local release, and synchronized the feature baseline while preserving its source package.


## Transparent dancing trio on the right of the title menu

Replace the title menu's right-side random still with Chiikawa, Hachiware and Usagi dancing on three matching cartoon pads. Work and commit only on `feature/chiikawa-theme`; leave main and remote publication for a later instruction.

Create transparent AI-drawn character key poses and a separate fixed pad layer. Use a native PNG sprite animation with deterministic stepping/hopping and a periodic clock, preserving transparency and avoiding the engine's first-frame-only GIF loader. Export a looping transparent GIF for preview, alongside source artwork, exact prompts and provenance. Keep the actual title menu fonts and menu items inherited.

Checkpoints: commit this plan; generate/inspect the artwork; implement the title actor and preview assembly; verify native 16:9/4:3 layout, seamless wraparound, motion-off behavior, cached title return and normal menu navigation; update the feature documentation, preview, progress and asset manifest; commit the completed feature. No main commits, push or Windows ZIP.

Completed: added the transparent three-character title loop, fixed cartoon pad layer, four poses per character and periodic Lua movement. Exported a transparent 96-frame / 3.2-second GIF and interactive preview from the actual Lua motion. Native Linux checks passed at 16:9 and 4:3, including motion off, Options/return and Game Start. Updated native captures, installation/customization instructions, source provenance, runtime media hashes and verification evidence. All changes are committed only on `feature/chiikawa-theme`; main remains at `eca4da9033`. No executable rebuild, ZIP or remote push.


## Make Chiikawa the StepMania 6 default

Ship `Data/Defaults.ini` with `Theme=chiikawa` and `DefaultTheme=chiikawa` in `[Options]`. The first setting supplies the selected-theme default and invalid-theme recovery; the second supplies the engine's secondary fallback. Existing saved per-game theme choices remain authoritative. Label the original theme **Classic** in `ThemeInfo.ini`, retaining its internal `default` directory to preserve Chiikawa/Home inheritance, direct asset paths and saved preferences.

Update portable-package required paths, installation and theme-selection guides, historical screenshot captions, and the feature release manifest. Verify actual native startup with an empty Save folder, a missing theme choice and explicit Classic/other saved choices; verify the appearance menu labels and theme switching. Retain the isolated evidence under `out/default-theme/`. Commit the small completed revision on `feature/chiikawa-theme` only. No engine rebuild, main promotion, push or Windows ZIP is requested.

Completed: shipped configurable Chiikawa defaults and renamed the original display label to Classic, preserving its internal directory. Native first-run and missing-theme recovery select Chiikawa; saved Classic and Legacy selections remain intact. The appearance menu displays both names, and switching in both directions updates the saved theme correctly. Package requirements, upgrade instructions, preview captions and configuration hashes are updated. No engine rebuild, ZIP, main changes or push.


## Promote the default theme and refresh repository captures

Promote the completed title animation and Chiikawa-default/Classic-label changes to main through a focused integration commit. Retain source artwork, generation prompts, browser comparisons and detailed journals on the feature branch. Main receives runtime PNG/Lua/configuration, required checks, release guidance and final native captures only.

Capture current Chiikawa title, gameplay with the movie fallback, evaluation and replay using the existing Linux executable and an isolated synthetic chart. Refresh the README's gameplay GIF and linked silent MP4 so all displayed captures use the new appearance. Preserve original fonts, notes and judgments; use actual key events for live speed changes and replay viewing. Record source/runtime revisions, media hashes, capture settings and limits. Update README and guide links for main, avoiding local links to feature-only sources.

Commit the reviewed capture/documentation checkpoint on the feature branch, copy the selected release paths onto an integration branch from main, run focused Lua checks, verify media/configuration hashes and local links, and fast-forward main. Merge the release baseline back into the feature branch while retaining its development materials. No Windows ZIP or remote publication is requested.

Completed: refreshed actual title/gameplay/evaluation/replay captures and gameplay media in `56789e3115`, selectively promoted the final theme/default/Classic files and docs to main as `d961b87fb9`, passed release-tree checks and native default startup, and reconciled the feature baseline while retaining development sources.


## Simplify the theme display name

Use `Chiikawa` as the display name in `ThemeInfo.ini`, the current installation/selection guides and the release manifest. Retain the internal `chiikawa` ID and existing StepMania 6 game branding. Validate the configuration, relevant Lua check and current links, then selectively promote the finished files to main and reconcile the feature branch. This text-only update needs no executable or ZIP rebuild.

Completed: feature checkpoint `8dbfd4a8e1` was selectively promoted to main as `c274d020c3`; configuration, focused Lua checks, links and release-path equivalence pass. Reconciled the feature baseline with the published runtime name and guides.


## Keep gameplay media clear of character decorations

Remove the additional gameplay cutout from the Chiikawa underlay and its obsolete layout/message handling. Continue loading Classic's screen filter and life graph. Gameplay and replay playback should display their song background or fallback movie/poster without a separate character illustration. Keep random illustrations on selection, evaluation and the replay browser; retain older gallery concepts as labeled development comparisons.

Reproduce the overlap with a song-provided image, update the existing selection/layout checks, and verify native image/movie/fallback backgrounds plus replay playback and menu decorations. Update current guides and the release manifest, commit the feature checkpoint, selectively promote the completed result to main, and reconcile/publish the branches. No executable rebuild or new Windows ZIP is required.

Implementation and verification complete: gameplay/replay omit the extra cutout; inherited functional actors and menu decorations remain. All focused and native checks pass. The finished runtime, checks, guides and manifest are ready for selective main promotion.
