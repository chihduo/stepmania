# Live gameplay speed progress

## Status

The playback-control and themed-popup changes are committed locally. The single-player life-history graph is now being restored for default-theme gameplay and evaluation. Windows ZIP builds and publishing remain deferred.

## Checkpoints

- [x] Investigate playback, video clocks, modifier lifetime, input routing, and score handling.
- [x] Create and commit `EDIT_PLAN.md` and this progress file.
- [x] Implement Shift+arrow controls and suppress shortcut note inputs.
- [x] Implement temporary rate lifetime and practice score/replay handling.
- [x] Implement synchronized scrolling/animation behavior and rate feedback.
- [x] Build the application and run focused and engine-level regression checks.
- [x] Review and commit the completed revision.
- [x] Build, smoke-test, download, and verify the Windows x64 portable ZIP.
- [ ] Verify an actual song with audio/video, including every exit/retry path, in an interactive game session.

## Commit ledger

1. `7ec3f6389f` — `docs: plan live gameplay speed controls`.
2. `47acb9a9ee` — `feat: add temporary Shift-arrow gameplay rate controls`.
3. `f5e98c70a4` — `feat: synchronize live rate visuals and verify gameplay controls`.

Baseline: `b3561fc60e` on `5_1-new`. The working tree was clean before this task. No repository `AGENTS.md` files were found.

## Implemented behavior

- Either Shift + Up/Right increases playback rate by 0.1x; either Shift + Down/Left decreases it by 0.1x. Each arrow press changes the rate once. New rates are limited to 0.2x through 3.0x. Existing out-of-range options cannot make a shortcut change speed in the opposite direction.
- Holding either Shift with Up+Down or Left+Right resets the current play to 1.0x when the second arrow is pressed. Both press orders work. Physical key snapshots prevent stale captures from causing a false reset; repeats and releases make no additional changes. The original song rate and practice status remain preserved.
- Shortcuts are intercepted before input-code queues and normal dispatch. Captured keys remain excluded from press, held-duration, and analog-level queries until release, including when Shift is released first. Other mapped devices remain usable.
- Only current song options receive the temporary override. Preferred, stage, and song targets remain intact. The override survives normal modifier updates and clears on song end, reload, screen exit, or destruction. Persistent screens and lesson-page input routing are covered.
- Manual control temporarily takes precedence over Haste and suspends autosync sampling. Normal song targets resume afterward.
- CMod/MMod retain their original spacing while the song clock accelerates. Saved scrolling settings are untouched.
- Standard video effects already use the music clock. Timer-based background/foreground animations follow song time while live control is active. Videos receive no additional rate multiplier.
- A compact multiplier such as `1.1x` appears at the bottom-left after each change. It uses the default theme's bold italic font, the W1 color/outline, and the existing FLAWLESS pop/glow/fade command. Repeated adjustments replace its value immediately. The display clears on song completion, retry/course advance, and screen exit. Practice-score handling remains unchanged.
- A stage-level practice flag survives restoring the original rate and statistics aggregation. It blocks normal high scores, replay files, Padmiss score exports, and upload files. Activity totals still accumulate.
- User instructions and test commands are in `Docs/LiveGameplayRate.md`.

## Validation results

- **PASS:** standalone regression checks with GCC 13, `-Wall -Wextra -Werror -pedantic`, AddressSanitizer, and UndefinedBehaviorSanitizer. Cases include the requested sequence, bounds, 10,000 increase/decrease cycles, non-1x starts, reset, scroll continuity, arrow aliases, repeat suppression, release ordering, pre-existing holds, concurrent arrows, and recovery after a lost release.
- **PASS:** headless checks linked against the actual engine objects. Both physical Shift keys and all four arrows were exercised. Captured arrows do not satisfy note polling, but a pad mapped to the same column does. Preferred/stage/song options remain unchanged; current options, Haste, and autosync restore correctly. Practice status survives aggregation and prevents high-score handling even with normal disqualification disabled.
- **PASS:** CMake Debug application compilation and link, producing the ignored `stepmania-debug` binary. All changed translation units compile.
- **PASS:** final diff whitespace check and review of input routing, cleanup, statistics, and video clock behavior.

## Build environment and limits

The command sandbox failed to initialize, so repository commands used the managed escalation path. CMake 3.31.10 was installed under `/tmp/stepmania-live-rate-tools`; missing development headers were extracted under ignored `build/live-rate-deps`. No system packages were installed.

The build is under `build/live-rate`, using GCC 13, Unix Makefiles, Debug, ALSA, X11/Xrandr, and system JPEG/GLEW. MP3, OGG, FFmpeg, GTK3, PulseAudio, Xinerama, Xtst, and GLES2 were disabled for this validation build. Missing development headers, a bundled/system JPEG header mismatch, and an Xrandr link configuration issue were resolved without unrelated source edits. Existing deprecated-library warnings remain.

The headless test harness uses a temporary filesystem, opens no game window, and does not save user preferences or mappings. It is separate from the sanitizer-enabled standalone tests.

Interactive audio/video playback was not tested. Before shipping, play a video-backed song at 0.8x, 1.0x, and 1.2x; check alignment, holds/mines/lifts, pause/resume, failure/back/retry/course transitions, and any custom scripts that explicitly use independent real-time clocks. These checks are documented, not claimed as passed.

## Windows x64 delivery checkpoint

The feature revision was pushed to `chihduo/stepmania`, branch `5_1-new`, and built by the existing Windows portable workflow.

- Source revision: `f5e98c70a49833710044027b098784bdb5743df3`.
- [Successful Windows build and smoke test](https://github.com/chihduo/stepmania/actions/runs/40217217284): MSVC 2022, x64, Release, static C/C++ runtime.
- [Download artifact `StepMania-windows-x64-8.1`](https://github.com/chihduo/stepmania/actions/runs/40217217284/artifacts/12607404103). GitHub currently retains this artifact until 2026-11-24.
- Local ZIP: `out/github-actions/live-rate-40217217284/StepMania-5.1-windows-x64-portable.zip` (73,454,816 bytes).
- The checksum file, build logs, and `verification.json` are saved alongside the ZIP in the ignored output directory.
- Windows portable-layout and x64 PE checks passed. The runtime Lua export smoke test reported `StepMania5.1-git-f5e98c7`.
- The downloaded ZIP's SHA-256 matches both the accompanying checksum file and the CI log. ZIP CRC checks passed for all 3,546 files.
- The package contains the expected executable version, the new live-rate message, media DLLs, themes, noteskins, scripts, and song assets. It has no saved preferences or logs from the smoke test.

SHA-256 of the inner portable ZIP:

```text
c7deb5fe8704c865203075286fb0e2fe9c0ab402a7a33e461026a6e27af06d0e
```

Extract the GitHub artifact, then extract its inner portable ZIP and run `StepMania/Program/StepMania.exe`. Keep the complete `StepMania` folder together. This Windows package includes the media dependencies omitted from the earlier Linux validation build. In-game notes/music/video synchronization still requires the manual checks in `Docs/LiveGameplayRate.md`.

## Follow-up revision progress

- [x] Record the user's successful gameplay feedback and the three requested changes.
- [x] Inspect the input snapshot, temporary rate state, and bundled system-message overlays.
- [x] Implement 0.1x steps and reset-to-1x chords in both orders.
- [x] Move playback feedback to the bottom in both bundled themes.
- [x] Extend and run rate/input regression checks and rebuild the local validation executable.
- [x] Review and commit the revision locally.
- Windows ZIP rebuilding and publishing are deferred at the user's request.

The user's report confirms the existing speed controls work in gameplay. It does not establish that every previously listed exit, timing, and custom-script check has been completed.

Follow-up checkpoint commits:

- `7f194902cf` — plan and progress checkpoint.
- `eefff0def8` — `feat: refine live playback controls and feedback`.

Follow-up validation:

- **PASS:** focused rate/input checks with AddressSanitizer and UndefinedBehaviorSanitizer. Covered 0.1x arithmetic, bounds and repeated adjustments, reset-to-1x from slower/faster/nonstandard starting rates, both opposite pairs, both press/release orders, repeat suppression, stale captures, and preserving ordinary held notes.
- **PASS:** tests linked against the real engine. Covered both physical Shift keys and all ordered pairs of distinct arrows; only opposite pairs reset. Captured arrows remain excluded from note polling while separately mapped controllers work. Resetting to 1.0x retains original song options, Haste/autosync restoration, and practice-score behavior.
- **PASS:** local CMake Debug application compilation and link, with the existing validation build configuration. Logs are in `build/live-rate/followup-build.log` and `build/live-rate/followup-engine-tests.log`.
- **PASS:** diff whitespace check and source review of both bundled message overlays. Playback messages move the entire 30-pixel bar to the bottom, and ordinary messages reset its placement.
- **PENDING:** visual confirmation of the bottom bar and interactive Windows testing of the new controls. Windows build, ZIP packaging, and publishing remain deferred at the user's request.

## Follow-up: judgment-style speed display

- [x] Inspect the FLAWLESS sprite, theme fonts, W1 animation, and gameplay actor lifecycle.
- [x] Replace the playback message bar with the themed bottom-left multiplier.
- [x] Verify Lua syntax, local compilation, and display cleanup; update documentation.
- [x] Commit locally, keeping ZIP packaging and publishing deferred.

The FLAWLESS word is a sprite. The changing multiplier uses the default theme's bold italic font with its W1 color, outline, and existing FLAWLESS animation. The display is scoped to gameplay and resets with the current song.

Display checkpoint: `29f73a9876`, `feat: show playback speed with themed judgment animation`.

Display validation:

- **PASS:** the new actor parses with the repository's bundled Lua 5.1 runtime.
- **PASS:** local CMake Debug application compilation and link; log is in `build/live-rate/speed-display-build.log`.
- **PASS:** source review of actor command propagation, default/fallback font redirects and glyph textures, W1 color/animation references, stable bottom-left anchoring, rapid-change tween replacement, and song/screen cleanup paths.
- **PASS:** diff whitespace check. No rate arithmetic, keyboard handling, synchronization, or score behavior changed in this display revision.
- **PENDING:** interactive appearance and animation verification. The selected default is a pop-and-fade indicator like FLAWLESS; no Windows ZIP or remote build was produced.

## Follow-up: single-player life-history graph

- [x] Inspect the linked screenshot and identify the life/health history plot.
- [x] Inspect the legacy assets, life-record clock, evaluation statistics, and current layouts.
- [x] Record the implementation and validation plan.
- [x] Add the shared live/evaluation graph with single-player gating and bottom-right placement.
- [x] Validate time ranges, geometry, live updates, evaluation selection, and local compilation.
- [x] Review and commit locally; leave ZIP packaging and publishing deferred.

Reference: https://perfectpotato.wordpress.com/wp-content/uploads/2016/02/screen-shot-2016-02-25-at-11-07-58-am.png . The image shows the life graph, not timing-offset or note-density data. The legacy graphics remain in the repository. The live widget will sample the engine's existing records and preserve their elapsed-play time scale.


Life-history checkpoints:

- `e86bc185b8` — plan and progress checkpoint.
- `2613625a1d` — `feat: restore single-player life-history graphs`.

The shared default-theme actor now renders live gameplay, evaluation, and evaluation summary. It uses the original legacy grid and fill assets, samples 128 life values, and updates existing geometry on life-record events and elapsed-time changes. The graph reads evaluation's selected StageStats and includes the last life-record timestamp, covering failure tails and summary gaps. The engine's existing history and scoring behavior remain unchanged.

The preferred plot is 200 by 64 theme pixels, 16 pixels from the right and 56 pixels above the bottom. During gameplay its width adapts to the ordinary note field's right edge; captions shrink with narrow panels. Single-player P2 modifier text moves left on evaluation. Two-player, demonstration, and endless sessions do not show the added graph. The screen-owned updater stops on exit. See `Docs/LifeHistoryGraph.md` for behavior and the manual verification checklist.

Life-history validation:

- **PASS:** focused tests with the bundled Lua 5.1 runtime. Covered P1/P2 eligibility, two-player/demo/endless exclusion, life clamping and geometry endpoints, empty geometry and zero-duration stage handling, failure/summary time ranges, immediate same-time life changes, paused/unchanged update caching, 1,000 updates with fixed vertex counts, fresh-stage/next-song refresh, exit cleanup, evaluation data selection, and narrow P2 corner placement.
- **PASS:** real-engine checks. Covered empty history, last-record timestamps, a final zero-life record, the one-second aggregation gap, the new Lua accessor, and an inclusive 128-sample endpoint. Existing rate, option-restoration, input-suppression, and practice-score tests also passed. Log: `build/live-rate/life-history-engine-tests.log`.
- **PASS:** local CMake Debug application compilation and link. Log: `build/live-rate/life-history-build.log`. The existing Linux validation configuration and its previously documented media limitations are unchanged.
- **PASS:** legacy asset byte comparison, diff whitespace check, and source review of draw modes/vertex format, Lua APIs, screen lifecycle, summary wiring, and layout anchors.
- **PENDING:** interactive Windows appearance and gameplay checks, including narrow panels, reverse/custom note modifiers, failure, retry, and finite courses. No rendered-screen test was performed. No Windows ZIP, push, or remote workflow was produced; these remain deferred at the user's request.


## Windows x64 delivery: speed refinements and life history

The user requested a new ZIP after the life-history implementation, authorizing packaging of the completed changes.

- Source revision: `2613625a1d968a4a001349e75f38fe310919aeb2`.
- [Windows build](https://github.com/chihduo/stepmania/actions/runs/40266781474).
- [x] Push the completed source to the fork's `5_1-new` branch.
- [x] Complete the Windows Release build, portable-layout checks, and startup smoke test.
- [x] Download and verify the artifact, checksum, executable, and bundled feature assets.
- [x] Record the delivery and commit the documentation checkpoint.

This package includes 0.1x speed adjustments, opposite-arrow reset chords, the bottom-left themed speed display, and the single-player bottom-right life-history graph. Interactive gameplay and visual checks remain separate from the automated packaging checks.


Delivery results:

- **PASS:** MSVC 2022 x64 Release build with static C/C++ runtime; [workflow run](https://github.com/chihduo/stepmania/actions/runs/40266781474).
- **PASS:** Windows portable-layout and x64 PE checks; startup Lua-export smoke test reported `StepMania5.1-git-2613625`.
- **PASS:** downloaded SHA-256 matches the accompanying checksum file and CI log; ZIP CRC checks passed for all 3,554 files.
- **PASS:** executable version and the new life-history API are present. Bundled speed-display Lua/fonts, life-history Lua/textures, screen wiring, metrics, language strings, and feature documentation match the committed source. Media DLLs and other required assets are present; runtime preferences, caches, logs, and smoke-test state are absent.
- [Download artifact `StepMania-windows-x64-9.1`](https://github.com/chihduo/stepmania/actions/runs/40266781474/artifacts/12626746745). GitHub currently retains it until 2026-11-25.
- Local ZIP: `out/github-actions/life-history-40266781474/StepMania-5.1-windows-x64-portable.zip` (73,462,569 bytes).
- The checksum file, build logs, metadata, verification script, and `verification.json` are saved beside the ZIP in the ignored output directory.

SHA-256 of the inner portable ZIP:

```text
c03eea6cec759376a1759970d6d472d110d290fee4d6167989fda41a93fced9a
```

Extract the downloaded GitHub artifact, then extract its inner portable ZIP and run `StepMania/Program/StepMania.exe`. Keep the full extracted folder together. This delivery includes all completed speed-control, speed-display, and life-history changes. Interactive appearance and gameplay verification remain pending; use `Docs/LiveGameplayRate.md` and `Docs/LifeHistoryGraph.md` for the manual checks.


## Follow-up: persistent speed and larger life history

- [x] Inspect the speed fade-out, graph dimensions, and nearby gameplay/evaluation layout.
- [x] Record the requested changes and validation plan.
- [x] Keep the latest speed visible through the current song.
- [x] Double the graph's preferred width/height and raise its bottom edge by 15 pixels, keeping nearby information readable.
- [x] Run focused checks, update documentation, and commit locally.

The earlier Windows ZIP remains the delivered build of `2613625a1d`; this follow-up will be a separate local revision.


Presentation follow-up checkpoints:

- `fa0c693ec9` — plan and progress checkpoint.
- `1f3b21c9c9` — `feat: keep playback speed visible and enlarge life history`.

The speed display now uses a dedicated `GameplayRateDisplay/ChangedCommand` in the default and fallback themes. It preserves the W1-style pop, color, font, and glow, with no scheduled fade-out. Existing reset/exit commands still clear it on song completion, retry, course advance, and screen exit.

The graph's preferred dimensions are now 400 by 128 theme pixels, and its bottom margin is 71 instead of 56. The existing gameplay width limit still prevents ordinary note-lane overlap in tight layouts. Evaluation uses the preferred width; narrow evaluation layouts compact the judgment rows and fit the song title beside the graph. P2-only life meters and lower evaluation details use the otherwise unused left area to avoid the enlarged plot. Two-player layouts are unchanged.

Presentation validation:

- **PASS:** bundled Lua 5.1 syntax checks for the speed-display actor, graph actor, and shared graph/layout helper.
- **PASS:** the existing focused life-history checks with the new dimensions. Covered live updates, pause caching, screen cleanup, evaluation statistics selection, narrow P2 gameplay, and clearance between the enlarged graph and evaluation rows/song titles. Widescreen and two-player evaluation spacing remain unchanged.
- **PASS:** source review of the default/fallback persistent animation commands, rapid-value replacement, existing song/screen reset paths, P2 life-meter/frame positions and entrance/exit direction, and diff whitespace checks.
- **PENDING:** interactive appearance, persistent speed visibility, and the larger graph in Windows gameplay/evaluation. This revision changes theme code/configuration and documentation only; no executable rebuild, Windows ZIP, push, or remote workflow was performed.


## Windows x64 delivery: persistent speed and larger life history

The user requested packaging of the latest revision.

- Source revision: `1f3b21c9c9878d9b7edf80ee8e85eed867df0384`.
- [Windows build](https://github.com/chihduo/stepmania/actions/runs/40283970563).
- [x] Push the completed source revision to the fork's `5_1-new` branch.
- [x] Complete the Windows Release build, portable-layout checks, and startup smoke test.
- [x] Download and verify the ZIP, checksum, executable version, and latest theme files.
- [x] Record the delivery and commit the documentation checkpoint.

This delivery includes persistent speed feedback, the 400 by 128 preferred life-history graph raised by 15 pixels, the surrounding layout adjustments, and all previously completed controls and graph behavior. Interactive gameplay and visual verification remain separate from the packaging checks.


Latest delivery results:

- **PASS:** MSVC 2022 x64 Release build with static C/C++ runtime; [workflow run](https://github.com/chihduo/stepmania/actions/runs/40283970563).
- **PASS:** Windows portable-layout and x64 PE checks; startup Lua-export smoke test reported `StepMania5.1-git-1f3b21c`.
- **PASS:** downloaded SHA-256 matches the checksum file and CI log; ZIP CRC checks passed for all 3,554 files.
- **PASS:** executable version and life-history API are present. Bundled default/fallback metrics, persistent speed-display actor/fonts, enlarged graph helper/textures/layouts, and feature documentation match revision `1f3b21c9c9`. Required media DLLs and game assets are present; generated settings, profiles, caches, logs, and smoke-test state are absent.
- [Download artifact `StepMania-windows-x64-10.1`](https://github.com/chihduo/stepmania/actions/runs/40283970563/artifacts/12633815670). GitHub currently retains it until 2026-11-25.
- Local ZIP: `out/github-actions/persistent-display-40283970563/StepMania-5.1-windows-x64-portable.zip` (73,463,393 bytes).
- The checksum file, build logs, metadata, verification script, and `verification.json` are saved beside the ZIP in the ignored output directory.

SHA-256 of the inner portable ZIP:

```text
741df7fdd49915d78745764d5af9f1ba67d32b642a44a0d737f7949779cc4035
```

Extract the GitHub artifact and then its inner portable ZIP. For an existing installation of these portable builds, close StepMania and merge the new `Program`, `Scripts`, and `Themes` folders into the existing game folder, replacing matching files; `Docs` is optional. Preserve the existing `Save`, `Songs`, `Logs`, `Screenshots`, and `Portable.ini`. Back up historical logs separately before launching because normal startup rewrites standard log files. Launch `StepMania/Program/StepMania.exe` with its full folder intact.

This ZIP supersedes the previous `2613625a1d` package and includes the persistent speed number and enlarged graph. Interactive gameplay and appearance checks remain pending; automated packaging checks do not render gameplay.


## Follow-up: bottom-left graph beneath gameplay information

- [x] Inspect screen draw order, existing underlays, graph opacity, and life-bar bounds.
- [x] Record the user's clarification to preserve the current transparency.
- [x] Record the placement, layering, and validation plan.
- [x] Move the graph to the bottom-left underlay with life-bar clearance.
- [x] Adjust evaluation layout and verify position, layer order, unchanged opacity, and existing live behavior.
- [x] Update documentation and commit locally.

Keep the existing transparency. The latest delivered Windows ZIP remains the `1f3b21c9c9` build until this follow-up is packaged separately.


Bottom-left graph checkpoints:

- `3695588aad` — plan and progress checkpoint.
- `ac7b31a225` — `feat: draw life history beneath gameplay at bottom left`.

The graph now loads in the gameplay underlay instead of the overlay. Its local draw order puts it above the screen filter, while the screen underlay itself remains below notes, judgments, life meters, messages, and the persistent speed display. Evaluation and summary use a lower graph draw order than their statistics. The normal evaluation graph was moved from the overlay into the underlay.

The graph starts 96 theme pixels from the left, leaving 24 pixels beyond the existing left life-meter frame. It retains its 400 by 128 preferred dimensions and 71-pixel bottom margin. It no longer narrows around note lanes because those lanes now draw above it. Single-player evaluation details move to the right, and row/title clearance follows the left-side plot. Life-bar positions and two-player layouts remain unchanged.

The user's transparency clarification is applied: the backing, body, line, and text opacity settings remain unchanged. No additional 60% opacity multiplier was added.

Bottom-left graph validation:

- **PASS:** bundled Lua 5.1 syntax checks for the shared actor, helper, and gameplay underlay.
- **PASS:** existing focused graph checks, updated for bottom-left placement and full width for P1/P2 at 4:3 and 16:9, life-bar clearance, gameplay/evaluation sibling draw order, unchanged opacity, evaluation row/title clearance, live changes, pause caching, fresh-stage refresh, and exit cleanup.
- **PASS:** source review of screen draw order (`background -200`, `underlay -100`, ordinary notes/decorations `0`, overlay `100`), actor-frame sorting, evaluation/summary wiring, and removal of old overlay graph instances. Diff whitespace checks passed.
- **PENDING:** interactive Windows appearance and layering checks with actual notes, judgments, messages, backgrounds, and life bars. This is a theme revision; no executable rebuild, Windows ZIP, push, or remote workflow was performed.


## Follow-up: line and label opacity

The user now requests 80% opacity for the graph's white line and labels. Added `self:diffusealpha(0.8)` to the `Line`, `Title`, `Value`, `StartTime`, and `EndTime` InitCommands. This shared actor applies the change during gameplay and evaluation. Backing/fill opacity, position, draw order, and live-update behavior remain unchanged.

Validation **PASS**: bundled Lua 5.1 syntax check, existing focused life-history checks, and diff whitespace review. Local checkpoint: `style: soften life-history line and labels`. No new tests, executable rebuild, ZIP, push, or workflow is needed for this small theme edit; visual verification remains pending.


## Follow-up: song changes unexpectedly while cycling sorts

- [x] Inspect the Left+Right handler, wheel movement, Artist grouping, and screen-message timing.
- [x] Identify a stale-preference path: the wheel changes sort before the pending song-change notification updates `m_pPreferredSong`.
- [x] Record the implementation and regression plan.
- [x] Reproduce the selection loss using the real engine code.
- [x] Preserve the highlighted song across sorts, retaining existing fallbacks.
- [x] Build, run focused regression checks, and review the diff.
- [x] Record validation and commit the fix locally.

Artist sorting uses the existing artist metadata; the suspected fault is selection restoration shared by all sorts. No new Windows ZIP has been requested for this follow-up.


Confirmed cause: `Screen::Update` updates the wheel before delivering queued `SM_SongChanged` messages. When the two arrows arrive in adjacent frames, the first arrow can update the preferred song to a neighbor. The second arrow returns the wheel to the original song, but the old sort-restoration code chooses that stale preference before the return notification is handled. This affects all song sorts; Artist makes the mismatch easier to notice.

`MusicWheel::UpdateSwitch` now tries the highlighted song's identity in the new sort first. If that song is missing, or the selected item is a section heading, it retains the existing preferred/current/first-song-or-course fallback. The sort sequence, shortcut definitions, ordinary arrow navigation, and mode-menu branch are unchanged.

Validation:

- **REPRODUCED before the fix:** the headless regression expected `Target / Artist M` after BPM -> Artist but selected `Before / Artist Z`. The fixture emulates the screen's arrow-handling and notification order through the real engine's wheel methods; it supplies cached song lists and minimal actors without loading a renderer or touching real user data.
- **PASS after the fix:** both arrow orders, same-frame and split-frame handling, two complete sort cycles per case, direct sort changes with stale preferred/current songs, section-heading fallback, and preferred/current/first-available fallback when the original song is absent. The checks cover 62 sort transitions.
- **PASS:** complete local Linux Debug application build. Existing `StdString` deprecation warnings remain unrelated to this change.
- **PASS:** existing gameplay-rate, option restoration, input suppression, practice-score, and life-history engine regression checks. The shared runner still accepts its original arguments and now also accepts an optional test source.
- **PASS:** diff/whitespace review and preservation of the production file's CRLF line endings.
- **PENDING:** physical keyboard and rendered Windows verification. No Windows ZIP, remote push, or workflow was performed for this revision.

Commands:

```text
/tmp/stepmania-live-rate-tools/cmake/data/bin/cmake --build build/live-rate --parallel 4
python3 src/tests/run_gameplay_rate_engine.py build/live-rate src/tests/test_music_wheel_sort.cpp
python3 src/tests/run_gameplay_rate_engine.py build/live-rate
```

For the next Windows build, highlight a song and note its title and artist. Release both arrows between Left+Right chords and cycle through Artist and back to the starting sort; verify that the title and artist stay the same. Repeat with Right pressed slightly before Left, then verify that ordinary individual arrow presses still change songs. This C++ fix requires an updated executable; editing Lua or metrics alone will not apply it.

Checkpoints: `888acf1876` records the plan; the implementation checkpoint is `fix: preserve highlighted song when changing sort`.


## Windows x64 delivery: song-sort fix and latest graph presentation

The user requested a new portable ZIP from source `00e67ffb848e78b5ac275a5e511bb7af324b2e88`.

- [x] Push the completed source and identify its Windows workflow run.
- [x] Complete the Windows Release build, portable-layout validation, and startup smoke test.
- [x] Download and verify the ZIP, checksum, source revision, and current theme assets.
- [x] Record the download and validation results; commit and push the delivery documentation.

This delivery includes the sort-selection fix, bottom-left life-history underlay, 80% white-line/label opacity, persistent playback-rate display, and previous speed controls. Interactive Windows gameplay and visual verification remain pending.

[Windows workflow run 40327867336](https://github.com/chihduo/stepmania/actions/runs/40327867336) builds source `00e67ffb848e78b5ac275a5e511bb7af324b2e88`.


Delivery results:

- **PASS:** MSVC 2022 x64 Release build with static C/C++ runtime, portable-layout validation, and Windows startup Lua-export smoke test. Executable version: `StepMania5.1-git-00e67ff`.
- **PASS:** downloaded SHA-256 matches the checksum file and CI log. ZIP CRC checks passed for all 3,554 files.
- **PASS:** x64 PE architecture and executable version match source `00e67ffb848e78b5ac275a5e511bb7af324b2e88`. The bundled default/fallback theme files, bottom-left gameplay/evaluation underlays, 80% line/label opacity, graph helper/textures, persistent speed display, and feature documentation match the committed source.
- **PASS:** required media DLLs and game assets are present. Generated preferences, profiles, logs, caches, screenshots, and smoke-test output are absent.
- [Download artifact `StepMania-windows-x64-12.1`](https://github.com/chihduo/stepmania/actions/runs/40327867336/artifacts/12653640995). GitHub currently retains this artifact until 2026-11-25.
- Local ZIP: `out/github-actions/song-sort-40327867336/StepMania-5.1-windows-x64-portable.zip` (73,463,335 bytes).
- Build metadata, logs, checksum file, `verify_archive.py`, and `verification.json` are saved beside the archive in the ignored output directory.

SHA-256 of the inner portable ZIP:

```text
da4a14d543c09e5c4555f3862e472051b58c83aa13c476dfd1e504b451b7d3ca
```

Extract the GitHub artifact, then its inner portable ZIP. To update the existing portable installation, close StepMania and merge the new `Program`, `Scripts`, and `Themes` folders into the game folder, replacing matching files; `Docs` is optional. Preserve `Save`, `Songs`, `Logs`, `Screenshots`, and `Portable.ini`, and retain a separate copy of historical logs before launching because startup rewrites standard log files. Personal theme/script edits at matching paths will be replaced.

This delivery supersedes the recorded `1f3b21c9c9` ZIP and includes all revisions through `00e67ffb84`. Interactive Windows checks remain pending, particularly preserving the highlighted song while cycling sorts and the graph's visual layering. Automated build, startup, and archive checks do not render gameplay.
