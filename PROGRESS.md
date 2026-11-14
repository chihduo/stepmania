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


## Follow-up: normal gameplay regardless of live speed changes

The user requests that live speed changes count as normal gameplay. This supersedes the earlier practice-only behavior and its historical validation results.

- [x] Trace score, award, replay, export, and disqualification restrictions.
- [x] Record the revised behavior and implementation plan.
- [x] Remove live-rate-specific restrictions and the obsolete stage flag.
- [x] Verify normal score recording, reset/cleanup, summaries, and existing unrelated eligibility rules.
- [x] Build locally, update documentation, and commit the completed revision.

The latest delivered ZIP still contains the old scoring restriction until this revision is packaged. No new ZIP has been requested in this follow-up.


Removed the `m_bUsedGameplayRate` stage flag and all associated restrictions. The live shortcut no longer sets player disqualification. Score finalization, high-score recognition, awards, replay saving, Padmiss exports, and upload-file generation now follow their ordinary paths and existing settings. Normal autoplay, give-up, disqualification, and save-score rules remain in place. Course and summary aggregation no longer carries a live-rate practice state.

Updated `Docs/LiveGameplayRate.md` to describe normal scoring. Replay exports retain the existing format, which does not encode a timeline of playback-rate changes; this revision restores the normal save path without adding a replay format.

Validation:

- **PASS:** complete local Linux Debug application build after removing the stage field and score gates.
- **PASS:** new headless score checks linked to the actual engine. Rates 0.2x, 0.8x, 1.1x, and 3.0x each cover an active override, reset to 1.0x, and song-end option restoration. `StageStats::FinalizeScores` records normal personal scores in all 12 cases. These finalization cases use failed-play fixtures, which normally save personal scores without requiring a graphical theme's grading configuration.
- **PASS:** successful-score fixtures enter both normal personal and machine lists through `ProfileManager::AddStepsScore`, and the engine recognizes the machine high score while a live override is active. Personal and machine song-score XML survives a real save/read/reload round trip in the temporary filesystem.
- **PASS:** save-score disabled, autoplay, and ordinary disqualification still prevent score insertion; summary aggregation preserves score totals without introducing disqualification.
- **PASS:** existing engine checks for rate arithmetic, input suppression, option restoration, and life history, plus the song-selection sort regression.
- **PASS:** source review of shortcut disqualification, award, replay, and export paths; no `m_bUsedGameplayRate` references or practice-only guards remain in source. Diff whitespace and original line endings were checked.
- **PENDING:** an end-to-end Windows play, evaluation/award display, and replay/export output with these changes. The tests exercise engine scoring and persistence with fixtures, not physical keyboard input or rendered gameplay.

Commands:

```text
/tmp/stepmania-live-rate-tools/cmake/data/bin/cmake --build build/live-rate --parallel 4
python3 src/tests/run_gameplay_rate_engine.py build/live-rate src/tests/test_gameplay_rate_scores.cpp
python3 src/tests/run_gameplay_rate_engine.py build/live-rate
python3 src/tests/run_gameplay_rate_engine.py build/live-rate src/tests/test_music_wheel_sort.cpp
```

On the next Windows build, finish a song after changing speed and verify its normal score record after restarting the game. Repeat after resetting to 1.0x and with a result that beats the previous best. The existing profile/save settings still apply. The most recently delivered `00e67ffb84` ZIP remains unchanged; this C++ revision needs a new executable before Windows gameplay uses the new behavior.

Checkpoints: `183653c22b` records the revised policy; the implementation checkpoint is `fix: score live-rate gameplay normally`. No Windows ZIP, remote push, or workflow was performed for this follow-up.


## Windows x64 delivery: normal scoring after live speed changes

The user requests the latest portable ZIP from source `0535f3e9d9b5ae3470d43d6b5d3ed4d778133e63`. This delivery includes normal scoring after live speed changes and all previously implemented controls, graph presentation, and song-sort fixes.

- [x] Push the completed source and identify its Windows workflow run.
- [x] Complete the Windows Release build, portable-layout validation, and startup smoke test.
- [x] Download and verify the ZIP, checksum, source revision, and bundled feature files.
- [x] Record download and validation results; commit and push the delivery documentation.

Existing local build and regression results for this source are recorded above. Interactive Windows gameplay verification remains pending.

[Windows workflow run 40876612910](https://github.com/chihduo/stepmania/actions/runs/40876612910) builds source `0535f3e9d9b5ae3470d43d6b5d3ed4d778133e63`.


Delivery results:

- **PASS:** MSVC 2022 x64 Release build with static C/C++ runtime, portable-layout validation, and Windows startup Lua-export smoke test. Executable version: `StepMania5.1-git-0535f3e`.
- **PASS:** downloaded SHA-256 matches the checksum file and CI log. ZIP CRC checks passed for all 3,554 files.
- **PASS:** x64 executable and version match source `0535f3e9d9b5ae3470d43d6b5d3ed4d778133e63`. Bundled speed controls/display, bottom-left life-history graph underlays, line/label opacity, theme assets, and updated normal-scoring documentation match the committed source.
- **PASS:** required media DLLs and game assets are present. Generated preferences, profiles, logs, caches, screenshots, and smoke-test output are absent.
- [Download artifact `StepMania-windows-x64-13.1`](https://github.com/chihduo/stepmania/actions/runs/40876612910/artifacts/12899620835). GitHub currently retains it until 2026-11-28.
- Local ZIP: `out/github-actions/normal-scoring-40876612910/StepMania-5.1-windows-x64-portable.zip` (73,463,537 bytes).
- Build metadata, logs, checksum file, `verify_archive.py`, and `verification.json` are saved beside the archive in the ignored output directory.

SHA-256 of the inner portable ZIP:

```text
5bf12a8d8e9c614394c27a25e26959d112377c8271dcff19dffd33e17e34f973
```

Extract the GitHub artifact, then its inner portable ZIP. For an existing portable installation, close StepMania and merge the new `Program`, `Scripts`, and `Themes` folders into the game folder, replacing matching files; `Docs` is optional. Preserve `Save`, `Songs`, `Logs`, `Screenshots`, and `Portable.ini`. Archive historical logs separately before launching because normal startup rewrites standard log files. Personal theme/script edits at matching paths will be replaced.

This delivery supersedes the recorded `00e67ffb84` ZIP. Live speed changes now use the ordinary score, high-score, award, replay, and export paths; existing save settings and unrelated eligibility rules still apply. Interactive Windows gameplay verification remains pending. To verify the scoring fix, finish a song after changing speed, then restart and check its saved score; repeat after resetting to 1.0x. Automated build, startup, archive, and previously recorded engine tests do not replace an end-to-end Windows play.

Delivery documentation is committed with `[skip ci]` so the downloadable executable remains tied to the verified source revision above.


## Follow-up: fork README and gameplay demonstrations

- [x] Review the existing README, implemented features, Windows workflow, and capture prerequisites.
- [x] Record the documentation and authentic-media plan.
- [x] Capture gameplay and evaluation in an isolated setup if available.
- [x] Replace the README and add media provenance and links.
- [x] Validate links, claims, media, and diff; commit and push the documentation.

The local Linux Debug executable is available, but no display is running. FFmpeg and X11 support are installed. Investigate a temporary virtual display and a generated WAV/chart fixture; keep all generated game state under the ignored output directory.


Completed the fork README and authentic demonstration media. The README now explains all live playback controls, the persistent bottom-left multiplier, normal scoring after rate changes, single-player life history, song-sort selection preservation, Windows portable downloads/updates, source builds, and upstream credits. It uses this fork's workflow badge and a stable Actions link instead of upstream CI badges or an expiring artifact as the sole download route.

Added `Docs/media/gameplay.png`, `evaluation.png`, a 17-second inline GIF, a 52-second silent MP4, and `Docs/media/README.md` with capture provenance and limits. Updated the life-history guide to link the Linux visual captures. Total media size is approximately 6.0 MB.

Capture and validation:

- **PASS:** refreshed and built the local Linux Debug executable. Embedded version is `StepMania5.1-git-ed202f9327`; source `ed202f932724d8b8f3124887c951f6dee71b09bc` has the same feature code as `0535f3e9d9b5ae3470d43d6b5d3ed4d778133e63`.
- **PASS:** actual regular gameplay and evaluation rendered under Xvfb/Mesa using the unmodified default theme and dance noteskin. A generated 120 BPM chart, original synthetic WAV, and neutral background avoid importing external song media. Autoplay with brief misses produces the life curve; XTest keyboard events exercise increases, decreases, and both reset chords.
- **PASS:** visually inspected the gameplay/evaluation stills and slowdown/reset animation/video frames. The speed label persists, note sprites render above the graph, life history updates, and evaluation shows the completed curve.
- **PASS:** both PNGs decode at 1280 × 720. All 204 GIF frames decode at 800 × 450 for 17 seconds. The MP4 decodes without errors as H.264 at 960 × 540, 24 fps, 52 seconds, with no audio stream.
- **PASS:** all 27 relative links in the README, media notes, and updated graph guide resolve; all four Markdown images (including the build badge) have alt text. Reviewed claims against feature documentation/source and checked diff whitespace.
- **PASS:** verified the repository default branch is `5_1-new` and the existing normal-scoring Windows artifact is still available. The README points to the workflow for future downloads and explains the 30-day retention.
- **LIMIT:** these are silent Linux captures using the Null audio driver and autoplay, not end-to-end Windows audio/video synchronization or human score-saving verification. Those limits and autoplay's normal scoring restrictions are stated with the media.

The isolated game tree, raw capture, automation, build log, metadata, and media verification report are retained under ignored `out/readme-demo/`. No personal profile or song directory was used for the demo. The temporary virtual display is stopped after capture. No gameplay source or theme behavior changed.

Checkpoint `ed202f9327` records the plan. The completed README/media checkpoint is `docs: introduce fork features with gameplay demos [skip ci]`; push the documentation to the fork without starting another Windows build.


## Follow-up: single-player recording and replay

The user authorizes implementation and checkpoint commits after the feasibility investigation. The old exporter is incomplete: loading/controller support is disabled, hold results are empty, and no live-rate timeline is stored.

- [x] Inspect existing replay stubs, clocks, judgments, profile writes, and theme entry points.
- [x] Record the implementation and validation plan.
- [x] Implement replay storage/validation and recording; verify and commit.
- [x] Implement playback, state isolation, and theme controls/browser.
- [x] Validate engine behavior and actual rendered replay in an isolated setup.
- [x] Update documentation and commit the finished feature.

Normal original plays must continue scoring normally after live speed changes. Replay viewing is a separate mode that does not add another play to saved history.

### Checkpoint: versioned replay storage

Implemented a bounded version 1 JSON replay journal (`.smr.json`) with the resolved chart, timing fingerprint, initial settings, music-clock frames, original note/hold results, score counters, inputs, judgments, rates, and life-history events. Files use StepMania's safe-write path with checked write/flush/size results. The reader checks schema, enum values, column/row references, event counts, chronological order, file size, and JSON nesting before playback.

Validation: the Linux Debug engine builds successfully. `test_replay_data.cpp` passes real-engine round trips for taps, holds, mines, lifts, original score/combo/judgment counts, and timing-sensitive fingerprints; malformed JSON, unsupported versions, absent notes, invalid columns/results/rates, duplicate notes, fractional counters, out-of-order frames, deep nesting, and missing files are rejected. Gameplay capture and playback integration are next; recording is not exposed in-game at this checkpoint.


### Checkpoint: recording, isolated playback, and controls

Connected the version 1 journal to standard single-player gameplay. Capture records the resolved chart, filtered logical inputs, absolute speed changes, note/hold outcomes, score/combo counters, and original life-history updates. A moving window limits note scanning during play. Recording has explicit size/event limits and rejects unsupported contexts, calibration changes, attacks, and keysounds; recording failure does not block original score saving. The old incomplete XML exporter is replaced.

Added the Save Replays song-option row (default off), default-theme recording feedback and F10 hints, evaluation playback, and a current-chart replay browser. The viewer uses the normal music clock, notefield, theme judgments, rate control, and life graph while applying recorded outcomes instead of rejudging inputs. Space/Start pauses, R restarts, and Esc/Back returns. It restores the original stage/index, player state, song options, seed, calibration offset, and evaluation; persistent score/profile/play-count entry points are guarded during viewing.

Validation so far: Linux Debug builds; replay storage and session tests pass, along with existing rate-control, ordinary rate-adjusted score-saving, song-sort, and life-history checks. Real rendered P1 recording/playback reproduced the score (97,435,899), max combo (36), speed sequence (0.9x/1.0x/1.1x/1.2x), holds/rolls, and life graph; pause/restart and return to the original evaluation worked. Human P2-only failed-play recording/playback worked. Aborted recordings, browser playback, immediate exit, and repeated viewing were exercised. Full `Save` directory hashes were unchanged by replay viewing/exit in the isolated fixture. Corrected the browser background, stage display, and several invalid-file edge cases found during these checks.

Artifacts and graphical scripts are retained under ignored `out/replay-tests/`; no personal songs or profiles were used. Final documentation and the verification ledger follow in the next checkpoint. No Windows ZIP has been built or published.


### Final review correction

Preserve the timing offset and held-miss flag from the actual row judgment when a jump's judgment comes from a different column. Playback no longer substitutes the first column's tap offset. The journal accepts the earlier five-field judgment entries and the new entries with the explicit held-miss flag. Replay data round-trip and malformed-flag tests pass after the correction.


### Completed validation and documentation

Added `Docs/SinglePlayerReplays.md` with recording/viewer controls, file storage and compatibility, original-score behavior, supported contexts, and a Windows verification procedure. Updated the fork README and speed-control guide. Added an unedited, actual replay screenshot with capture provenance in `Docs/media/README.md`.

- **PASS:** complete Linux Debug application build after the final judgment correction. Existing compiler deprecation warnings remain.
- **PASS:** `test_replay_data.cpp` verifies journal round trips, chart/timing identity, taps/holds/mines/lifts and score counters, judgment offsets/held flags, and malformed input rejection. It was rerun after the final correction.
- **PASS:** `test_replay_session.cpp` verifies missing/changed assets, nonfinite numeric modifiers, safe file/browser filtering, unsupported two-player styles, profile/play-count guards, restart, early exit, and restoration of stage/options/seed/calibration. It also verifies repeated cleanup.
- **PASS:** existing engine rate-control, normal rate-adjusted scoring/persistence, song-sort, and Lua life-history checks. Syntax checks pass for all three changed default-theme overlays.
- **PASS:** actual rendered P1 recording/playback reproduces score **97,435,899**, max combo **36**, the recorded 0.9x/1.0x/1.1x/1.2x rates, and life history. Pause/resume, restart, browser selection, completed/failed/aborted plays, immediate return, and repeated viewing were exercised. Human P2-only failed-play recording was replayed and returned to its original evaluation.
- **PASS:** every file under the isolated `Save/` directory was compared before/after replay viewing and return, with no changes. The committed `b21530a9ad` reader/viewer was also exercised through the browser and returned without changing saved files.
- **PASS:** the new replay PNG decodes at 1280 × 720; local documentation links and diff whitespace were checked. The screenshot is from the actual committed viewer and is labeled as a Linux capture.
- **LIMIT:** graphical tests use Xvfb/Mesa, generated music/chart assets, Null audio, and a build without video decoding. Windows x64 runtime, audible music/video synchronization, physical controller behavior, and unusual song scripts still need platform verification. The final jump-offset/held-flag correction was built and covered by data tests, not a separate rendered jump capture.

Focused engine-test logs are retained under `/tmp/stepmania-*replay*.log` and `/tmp/stepmania-final-{rate-engine,rate-scores,song-sort}.log`. Reproducible graphical scripts, Save-file comparison reports, raw captures, and game logs remain in the ignored `out/replay-tests/` directory. The temporary game/display processes were stopped after capture; no personal profile or song folder was used.

Local checkpoints: `17115d3d9a` (plan), `d73c53afbf` (validated journal), `b21530a9ad` (recording/viewer and integration checks), and `402f628460` (jump judgment details). The final documentation checkpoint closes this revision. No remote push, Windows workflow, or ZIP packaging was performed; the previously delivered Windows executable does not contain replay playback.


## Follow-up: StepMania 6 README title and record/replay section

Changed the README title to **StepMania 6** and retained attribution as a community fork of StepMania 5.1. Expanded record/replay into a main section, linked from the introduction, covering Save Replays, F10 access, playback controls, recorded speed/results, completed/failed/aborted plays, file storage, and normal original-score behavior. The existing authentic screenshot and detailed guide are linked from the section.

Validation: the requested title and single record/replay heading are present, all local README links and the new section anchor resolve, images have alt text, and `git diff --check` passes. This documentation-only checkpoint does not change executable branding or gameplay. No build, ZIP, or remote push was performed.

## Chiikawa theme release

The finished theme includes cream/pastel menus, optional menu motion, eleven paired/group cutouts with random selection per screen, native replay-browser styling, and the original fonts, arrows, judgments, speed controls and life graph. The inactive song-selection pane correction remains in the default theme.

The missing-song-art fallback is a 30-second silent movie assembled from the three supplied clips. It contains 720 frames at 1280×720 / 24 fps, preserves every decoded source frame, and uses the engine's normal movie clock, rate, pause, looping, brightness and layering. Static mode uses the extracted poster. Song-provided images and scripted movies keep their precedence; missing-art gameplay omits duplicate character decorations.

Prior verification at `a74362c422`: local Linux build with the pinned FFmpeg decoder passes; focused theme/fallback Lua checks pass. Native checks cover missing/own/scripted backgrounds, 16:9/4:3, P1/P2/centered layouts, speed changes, gameplay/replay pause/resume, replay restart, looping, static/disabled backgrounds and the unchanged original-theme fallback. Authentic screenshots and the release asset manifest remain in `Docs/theme-preview/chiikawa/`. Windows x64, physical controllers and audible synchronization remain unverified.

## Chiikawa branch separation

The full theme development history and package are preserved on `feature/chiikawa-theme`, with a separate worktree at `../stepmania-chiikawa`. Detailed historical plans/progress are retained there as `Docs/theme-preview/chiikawa/DEVELOPMENT_PLAN.md` and `DEVELOPMENT_PROGRESS.md`.

Main retains the published history through `0e53193abf`, then receives one finished-results commit incorporating the six unpublished movie commits' runtime output. The main tree keeps final assets, required shared hooks/tests and release docs; experiments, duplicate preview assets, source clips and detailed journals stay on the feature branch. Existing Git history is not erased. `Docs/ChiikawaDevelopment.md` documents selective promotion and baseline synchronization for future work.

Separation validation:

- Runtime theme files, the fallback effect, shared engine/default-theme code and regression tests are byte-identical to the tested `a74362c422` revision.
- Both focused Lua checks pass in the release tree. All 15 runtime media hashes match the release manifest; selected character originals, all three raw movie sources and native screenshots match the preserved feature-worktree files.
- All 71 local links in the seven release/workflow guides resolve. Diff whitespace checks pass.
- Before removing 57 development files (19.40 MiB) inherited from the published tree, verified each matched its feature-worktree copy byte-for-byte. The newer original movies and keyframes remain feature-only; the final movie and poster are included in main.
- `AGENTS.md` records the branch policy for future work. The feature branch's first separation checkpoint is `83f33edd3e`; its initial source snapshot remains `a74362c422`.

The main release snapshot is committed as `feat: promote finished Chiikawa theme and isolate development [skip ci]`. The feature journal records baseline reconciliation after this checkpoint. No remote push or Windows ZIP was performed.


## Rename the default branch to main

Prepared the local rename from `5_1-new` to `main` and updated active README, build/clone links, contributor instructions and the branch workflow. Verified GitHub administrator permission, that `main` is unused, and that no open repository pull requests or branch-specific CI filters need adjustment. The remote published tip is `0e53193abf`; newer local theme work remains unpublished. GitHub renamed the default branch successfully and still points to exactly `0e53193abfb663a0efdb9c99818bd06b644e6df4`. The old remote branch name is absent. Local `main` now tracks `fork/main`, and `fork/HEAD` resolves to `fork/main`; the stale old tracking ref was removed.

Updated the badge, workflow filters in README URLs, clone command, contributor policy and active branch-promotion instructions. The Actions workflow itself already applies to all branches. Historical progress entries retain their original names. No gameplay code or assets changed, and no pending local theme commits were pushed. The feature worktree receives these documentation changes through a normal merge of `main`.


## Movie-background gameplay preview

The release preview uses the native 1280×720 gameplay capture with the silent dancing-trio movie, original notes/judgments, life graph and persistent 1.1x display. It matches the original `out/chiikawa-movie/gameplay-rate-wide.png` byte-for-byte, with SHA-256 `a49840da22b495df18a8ad63c2ceebf0dd39eab8471ca3ad4175768e860b9184`. Visual review and all 15 released media hashes pass. Added an adjacent caption and direct 30-second MP4 link to `Docs/theme-preview/chiikawa/native/README.md`.

This publication includes the prepared final movie/theme release and the `main` branch documentation updates. Development originals remain on `feature/chiikawa-theme`. The GitHub page previously pointed to the older capture because the release commits were still local. The final source-side publication verification is recorded in the feature progress journal. All commits use `[skip ci]`; no Windows ZIP is requested.


## Chiikawa default and refreshed README release

Prepared the completed theme revision from `56789e3115` for selective main promotion. It adds the 3.2-second transparent title dance, Chiikawa defaults for new installations and the **Classic** display label for the original theme. The internal `Themes/default` folder remains for inheritance and saved choices. Earlier native checks verify fresh startup, missing-theme recovery, retained Classic/Legacy selections and switching in both directions.

Fresh 2026-11-09 Linux captures show the current title, movie-background gameplay, evaluation and replay. The README now uses these screenshots plus a new 12.01-second gameplay GIF and 40-second silent MP4. The same synthetic play's replay confirms every illustrated rate change and both reset chords. Full capture metadata and hashes are in `Docs/media/manifest.json`; PNG pixels are unedited. The finished transparent title preview is included, while source atlases, prompts, assembly scripts and detailed development journals remain feature-only.

Release verification passes:

- All 42 selected runtime/check files match the reviewed feature snapshot. All 17 runtime media hashes, two configuration hashes and nine final README/native media hashes match their manifests.
- The title-animation, theme-selection/layout and fallback-movie Lua checks pass in the release tree.
- The exact integration tree starts in Chiikawa without a saved or forced theme. Its title animation renders and loads with no Lua errors; isolated evidence is under `out/chiikawa-release/`.
- All 106 local links in changed release guides resolve, and diff whitespace checks pass. Source illustrations, prompts, original movies, browser comparisons and detailed development journals are absent from the release tree.

Promoted source checkpoint: `56789e3115`. The focused release commit is `feat: make Chiikawa the main default and refresh README captures [skip ci]`. The feature branch is reconciled after main advances. Linux native verification uses software OpenGL and Null audio; Windows hardware, physical controls and audible synchronization remain unverified. No executable rebuild, Windows ZIP or remote push is part of this promotion.


## Windows version and saved-theme diagnosis

The delivered `d961b87` Windows executable reports `StepMania5.1-git-d961b87`: the README/theme logo was renamed but CMake's executable version and portable ZIP name were still inherited. `Data/Defaults.ini` selects Chiikawa on fresh installs; a valid saved `Theme=default` continues to select the original theme, now labeled Classic. The user-data identifier must remain `StepMania 5.1` because non-portable saves use that directory.

Prepared the 6.0 runtime/package and Windows metadata correction, a Windows startup assertion for the default Chiikawa theme, and upgrade troubleshooting that preserves Save. Local validation passes: the Linux Debug build succeeds; actual rendered startup selects Chiikawa with fresh settings, preserves saved Classic, and saves a menu-driven switch from Classic to Chiikawa. All three runs report `StepMania6.0` and have no Lua errors. The title screenshot visually confirms the 6.0 label. Evidence is isolated under `out/version6-native/`; build logs are `out/version6-{configure,build}.log`. Changed documentation links resolve and diff whitespace checks pass. The user confirmed copying the four runtime folders over an older installation; its saved theme choice explains the retained Classic appearance. The Appearance Options switch was verified, and `Data/Defaults.ini` is the additional upgrade file.

Published the fix as `8977aa3f0c85f06ce85c6b2841d2ab5896c52221`. [Windows workflow run 42825456584](https://github.com/chihduo/stepmania/actions/runs/42825456584) passed in 6m19s with MSVC 2022 x64 Release. The executable has Windows file/product version 6.0, and the startup test reports `StepMania6.0-git-8977aa3; fresh default theme: chiikawa` without a saved Theme override.

Downloaded and verified [artifact StepMania-windows-x64-15.1](https://github.com/chihduo/stepmania/actions/runs/42825456584/artifacts/13769337195). The ZIP's SHA-256, x64 PE, embedded build version, 28 theme/media/configuration manifest entries, and full archive CRC pass. Windows text comparisons normalize CRLF. Smoke-test state and feature-only source assets are absent.

- ZIP: `out/windows-x64-8977aa3f0c-42825456584/StepMania-6.0-windows-x64-portable.zip` (128,833,373 bytes).
- SHA-256: `9778414de062a3e58de6d8f931a3fa32e8af6a05cb74c46c4009e0790e6c8162`.
- The same output directory retains the checksum file, workflow metadata/log, verification script and report.

This checkpoint records the completed build; its executable/runtime files match the published implementation commit.


## Chiikawa display name

Promoted feature checkpoint `8dbfd4a8e1` so the theme selection menu and current guides use **Chiikawa**. Updated the release manifest's name and source revision. The internal `chiikawa` ID remains compatible with saved preferences. Configuration/manifest consistency, the existing focused Lua theme check, 49 local guide links and diff whitespace pass. This text-only update can be installed by copying `Themes/chiikawa/ThemeInfo.ini`; no executable rebuild or new ZIP is required.


## Gameplay decoration overlap fixed

Promoted feature checkpoint `eb2cd3f52b`: gameplay and recorded playback no longer place an extra character cutout over song images, song movies or the fallback movie/poster. Classic's screen filter and life graph remain inherited. Selection, evaluation and replay-browser illustrations keep their existing behavior. Updated installation guidance, regression expectations and release provenance; older gallery concepts stay on the feature branch.

All three focused Lua checks pass. Native Linux checks cover song images, song movies with cover art, fallback movie/poster settings, P1/P2 at 16:9/4:3, selection/evaluation decorations and recorded movie playback. Both movie frame comparisons show motion, final logs contain no Lua command errors, and visual inspection confirms the notes, judgments and life graph. Runtime media/configuration hashes are unchanged. Detailed evidence remains in the feature worktree under `out/gameplay-background-layers/`. Windows hardware and audible synchronization were not retested.

Existing Windows installations can replace `Themes/chiikawa/BGAnimations/ScreenGameplay underlay.lua` and `Themes/chiikawa/Scripts/06 Chiikawa.lua`, then restart. No executable rebuild or new Windows ZIP was required.


## README introduction for new players

Rewrote the README as a product introduction: gameplay overview, feature summary, Windows setup, speed controls, recording/replay and performance review. Chiikawa is presented as the default interface; appearance settings occupy a short section linked to the full guide. Removed theme-migration comparisons, old executable-version explanations and detailed implementation history. Retained authentic gameplay/replay/evaluation media, saved-data preservation, source-build instructions and attribution. Validation passes: 21 local links, 5 section links and 4 nonempty media embeds resolve; all images have alt text and code fences are balanced. Cross-checked controls, recording scope, download names and defaults against the current guides/configuration. The diff contains only README and plan/progress documentation; no gameplay tests or build are needed.

Replaced the opening gameplay screenshot with the existing native title-menu capture and descriptive alt text. The animated gameplay demo remains in the playback-controls section. Both media paths resolve; only documentation changed.


## Larger evaluation decoration

Promoted feature checkpoint `4f6e010d8f`. Evaluation illustrations are 1.5× their previous size on widescreen, using a maximum 345 × 270 box at y=270. A 12-pixel gap beside the judgment panel and 16-pixel outer margin constrain width to 252 on 4:3; P2 placement mirrors P1. Native Linux captures for both players and aspect ratios show clear banners, judgment rows, scores, details and life history, with no Lua command errors. The existing theme-layout check and all 20 guide links pass. Evidence is under the feature worktree’s `out/evaluation-art-size/`.

Existing installations can copy `Themes/chiikawa/Scripts/06 Chiikawa.lua` and restart. No executable rebuild or new Windows ZIP was required.


## Gameplay timing display fix

Removed the small gameplay accuracy readout. Per-lane millisecond labels now render in front of the score frame through overlay proxies, preserving player/judgment transforms and the original label updates. Native Linux gameplay and replay checks pass, including P2, centered 4:3, reverse/Mini, timing off and Blind; evaluation accuracy remains visible. Logs contain no Lua command errors. The focused Lua check, 22 guide links and 19 release asset/configuration hashes pass.

Existing Windows installations need `Themes/default/BGAnimations/ScreenGameplay overlay/default.lua` and `Themes/default/Graphics/Player judgment/default.lua`, followed by a restart. Chiikawa inherits these Classic actors. No executable rebuild or Windows ZIP; Windows rendering was not retested. Detailed evidence stays on the feature branch.


## Consecutive hold visibility

The renderer visits holds from later rows to earlier rows and draws each body, tail and head together. Consequently, an earlier body or bottom cap can cover a later head when holds are close together. Preparing a two-pass correction at the column level so bodies/tails for all note owners precede hold heads, while preserving the existing tap pass. Native fixtures and the original executable are kept under ignored `out/hold-note-layers/`.

Implemented two hold-rendering passes in `NoteDisplay`: draw every visible hold/roll body and tail first, then their heads, followed by the existing tap pass. The passes include all player-owned and unassigned notes before advancing to heads. Hold-ghost state is updated once per frame; existing visibility, noteskin flags, reverse/active positioning, fading and scoring logic are retained.

Validation completed with the rebuilt Linux Debug executable:

- Reproduced the original defect with 0.75x scroll and holds ending one sixteenth before the next hold. Before/after captures pause at beat 6 of the same chart. All four later heads are mostly hidden before the fix and fully recognizable afterward. An image-based check finds 0% red head-center coverage in the original, versus 83–87% with corrected normal/fast rendering; its pass threshold is 75%.
- Native checks pass for normal and fast rendering, reverse, active and missed holds, P2 at 4:3 with Mini/Bumpy depth effects, and the Delta noteskin. A completed play records 48/48 holds, 16/16 rolls and the expected 100,000,000 score. Recorded playback shows the corrected hold heads, and returning restores evaluation. All final logs have no Lua command errors.
- The build succeeds with existing deprecation warnings; diff whitespace passes. Captures, private chart, scripts, logs, binary/source hashes and verification records are under ignored `out/hold-note-layers/`. Native game/Xvfb processes stopped. Tests use Linux software OpenGL and Null audio; Windows rendering was not retested.

This is an engine change and requires a rebuilt Windows executable. No theme asset edits or new Windows ZIP were needed for this verification.


## Windows executable updated with hold visibility fix

[Workflow #16](https://github.com/chihduo/stepmania/actions/runs/43066153222) built `69772bd100f38e482cb3daaa7ce83bd597b07283` successfully in 6m47s using MSVC 2022 x64 Release. Windows layout, x64 PE and 6.0 file/product metadata checks pass. The startup smoke test reports `StepMania6.0-git-69772bd; fresh default theme: chiikawa`.

Downloaded and verified [artifact StepMania-windows-x64-16.1](https://github.com/chihduo/stepmania/actions/runs/43066153222/artifacts/13875908360). SHA-256, full archive CRC, embedded build identity, all 28 released media/configuration entries and the current timing-label/gameplay/evaluation Lua files match. The ZIP contains no smoke-test Save/Logs/Cache files or feature-only source assets.

- ZIP: `out/windows-x64-69772bd100-43066153222/StepMania-6.0-windows-x64-portable.zip` (128,834,011 bytes).
- SHA-256: `0224241e1fff295164089f68e87f72f879657d4bcb0cc43b0e55edb630e17a1c`.
- The same directory contains extracted `StepMania/Program/` files, the checksum, CI logs/metadata and local verification reports.

Compared with Windows build #15, only `Program/StepMania.exe` and `Program/StepMania.vdi` changed within Program; bundled DLLs match. Replace `Program/` and `Themes/` from the ZIP to apply the engine fix and all subsequent theme changes while preserving personal data. Windows graphics/controller play was not exercised by the startup smoke test; native rendering checks for the fix are recorded in the preceding section.


## Manual dashboard upload

Planning checkpoint: inspected both repositories, statistics export identifiers, profile selection, title-menu hooks and the current gh-pages deployment. Manual upload is confirmed; implementation and verification are starting. The existing published snapshot contains 7,131 lifetime plays and its raw source history is not tracked. Full migration must validate the matching historical Save data before replacing the live site.


Implementation checkpoint: added explicit dashboard preferences, allowlisted profile/event/metadata export, content-addressed banner envelopes, a persistent outbox, GitHub tree/commit/ref upload with conflict handling, Windows WinHTTP and Credential Manager support, and the title-menu upload/status screen. Only selecting the screen starts work. Repeated already-published data skips rebuilding; accepted-but-unpublished batches can be dispatched again. The engine uses existing score/log records without changing rate/replay scoring.

Validation passes: Linux Debug build; engine tests against an in-memory GitHub API for GUID/profile/privacy filtering, repeat uploads, failed dispatch, ref conflicts, regressed totals, immutable event conflicts, denied credentials and cancellation; native export accepted by the Python importer; native title selection, return, setup screen at 16:9/4:3, and Linux export. Native testing found and fixed the missing title-menu translation. Theme colors are verified in the feature worktree and await focused promotion. The dashboard's nine importer tests pass locally and on GitHub (run 43249599943). Windows compilation and real-user first upload are not yet verified.

Promoted only the verified runtime metrics from Chiikawa checkpoint 6715745330. The upload screen uses the original theme font with cream, brown and mint colors. Detailed theme planning remains on the feature branch; no generation assets were imported.

Windows build 43251169504 successfully compiled the native WinHTTP/Credential Manager implementation and passed the executable/version/portable checks. Final review added pacing for GitHub write requests so a large first import stays below secondary write limits, and an explicit accepted-upload message when publication dispatch fails. A final Windows rebuild verifies that small follow-up.

Contract review also covers malformed legacy UTF-8 and XML control characters: the export normalizes them like the dashboard’s existing reader while leaving the original files untouched. The engine test verifies valid UTF-8 and escaped ampersands.


Final runtime revision: `f77f87a76d`. Windows x64 workflow #19 (`43253427102`) completed successfully. The expanded engine contract test now includes bitmap-loader initialization and an actual PNG banner; its exported profile, play, song metadata, escaped ampersands, Unicode/legacy text and image all pass through the Python dashboard builder. The added test changes no runtime code.

The dashboard repository is published at `a56b081`. Actions bootstrap `43251375360` migrated the live site without changing its 7,131-play data or any of its 1,356 website assets, including 1,353 banner images. Four sampled live banners and the data JSON also match. Pages now uses Actions and permits the trusted main workflow; gh-pages remains as a rollback snapshot. Hosted importer checks pass at `43254295118`.

The player's first upload still requires their matching Windows profile, retained Upload history, and repository-scoped token entered locally in the game. No personal token or new play data was supplied here. The completed code, integration guides and native evidence are available; final downloaded ZIP verification is being recorded separately.


Final package verification passes: the downloaded ZIP matches its CI SHA-256, all archive entries pass CRC checks, the new upload screen/menu/guide match the release sources after Windows line-ending normalization, and all 19 release manifest entries match (two existing INI files differ only in line endings). The package contains no Save, Logs, Cache or Screenshots directories. Windows CI confirms `StepMania6.0-git-f77f87a`, x64 PE/version metadata and fresh Chiikawa startup.

- ZIP: `out/windows-x64-f77f87a76d-43253427102/StepMania-6.0-windows-x64-portable.zip` (128,915,844 bytes).
- SHA-256: `e03361321b7df1469a5e270e7d7b21edaf6efa55630c9f97f6f5b009170773fd`.
- GitHub artifact: `StepMania-windows-x64-19.1`, ID `13960947152`, workflow `43253427102`.
- The same output directory holds CI logs, smoke results, artifact metadata and the local verification report. The first real upload remains a player-side setup/verification step; it requires the matching saved profile/history and a token stored locally in Windows Credential Manager.


## Dashboard menu feedback

Investigation found that blank DashboardRepository makes both browser actions return silently, while Upload / Retry repeats the unchanged setup error when uploads are disabled. Saving the token does not set the connection preferences. Reproducing the dialog/menu path and adding visible action feedback before packaging the correction.

Reproduced the same symptoms in the original executable using disabled uploads and an empty repository. The token dialog returned normally. Added persistent token/action results, setup instructions with restart guidance, validation for both browser destinations, browser failure URLs, and explicit busy/retry feedback. Long status messages wrap to the available width. Connection preferences remain separate from token storage.

The Linux build and focused engine tests pass, including invalid/missing repository values, startup failure and retry, plus the existing export/publication suite. Native keyboard checks pass for dialog submission/cancellation, disabled uploads, both missing-repository actions, exact browser-hook URLs, failed browser launch, repeated exports and retry after the dialog, Back, and 16:9/4:3 layout. Browser hooks were intercepted and no test records or credentials were uploaded. Evidence is under ignored out/dashboard-menu/. Actual Windows token storage/upload was not exercised; the Windows package check follows.

Windows build #20 (43468764239) successfully built runtime revision efd31f4371. The x64/6.0 metadata and startup smoke test pass, reporting StepMania6.0-git-efd31f4 with Chiikawa. All 14 native menu cases also pass against the final source. Downloaded artifact StepMania-windows-x64-20.1 (14059309878); checksum, full ZIP CRC, executable build identity, updated guide and all 20 release entries pass, accounting for the existing Windows INI line endings.

- ZIP: `out/windows-x64-efd31f4371-43468764239/StepMania-6.0-windows-x64-portable.zip` (128917316 bytes).
- SHA-256: `cfccdaf31150b4af3655fcfa76f09548b6052bf97fa2aeda25cb4f50cf25f020`.
- Compared with build #19, only Program/StepMania.exe, Program/StepMania.vdi and Docs/DashboardUpload.md changed. Replace Program/ and optionally Docs/; retain Save/ and the existing Windows credential.

The correction exposes configuration and browser errors; it does not enable uploads or choose a profile automatically. The existing executable can upload after the required Preferences.ini settings are supplied and the game restarted. The reported Windows installation and its actual token/upload remain unavailable for direct verification.


## Readable upload menu and status

Confirmed the title menu uses Roboto Condensed Bold 48px at 1.0 selected and 0.75 otherwise, while the upload menu used Common Normal 24px at 0.65. Preparing separate action actors and a large status headline with severity colors, shadow, and wrapped detail text.

The rebuilt Linux engine and uploader tests pass, including typed setup errors. Native verification passed 18 cases: exact title-menu font-height/focus-scale equality, all four status colors, 16:9/4:3 text bounds, token dialog return/cancellation, missing configuration, exact browser destinations, browser failure, repeated exports, and busy/cancel/return warnings. A temporary delay in the private fixture’s export writes exercised actual running/cancelling jobs; browser launches were intercepted and no data was uploaded. All three focused Lua checks pass. Theme styling and final capture promotion follow this engine checkpoint.

Promoted verified theme checkpoint dfba79087a with title-menu-sized action fonts, a blue/green/amber/rose status headline and shadow, readable details, and a final native screenshot. The shared engine checkpoint is 7c8b62f24f. Only the completed runtime aliases/metrics, user guides, native capture and release manifest are promoted; detailed fixtures and journals remain on the feature branch.

The user requested that the Windows build wait. Cancelled workflow 43474967953; GitHub confirms the run is cancelled with zero artifacts. No new Windows ZIP was produced or downloaded. The source/UI revision remains committed and published at 9ea3fa6163, with 18 native Linux cases, engine tests, focused Lua checks, release-path equality and guide links verified. Do not start another Windows build until the user requests it.

## First Windows dashboard upload investigation

Run 43470498036 accepted data revision 27a1d0ea34ac2eda56ba72950837e9860f38aad7
but rejected a legacy calibration entry at Themes/default/Other/. Dashboard
checkpoint a48c704 accepts safe calibration paths only in cumulative snapshots,
keeps their existing exclusion from song rankings, and passes 15 regression tests.
A full local import now reveals omitted historical logs: 827 uploaded versus
5,918 already published. The unchanged live dashboard remains protected. Older
retained scores lack PlayerGuid; local-profile uploads filter such logs, so the
local pending.json counter and older source logs are needed to confirm ownership
and completeness before publication can proceed.

Also found and corrected the native metadata lookup: serialized SongID paths omit
the leading slash expected by SongManager. Resolve through SongID::ToSong(), as
the profile system does, to export titles, charts and banners. The local Linux
build and isolated native capture/import pass: a real loaded song exports the
correct title, artist, Hard/9 chart and hashed PNG; the importer preserves totals,
excludes the calibration entry from rankings, and verifies the banner bytes.
The fixture also confirms that an unrelated ID and an empty PlayerGuid are both
excluded and counted in pending.json. Evidence: out/dashboard-repair/verification.json.
Dashboard hosted checks pass in run 43477814383. No real data was uploaded by the
fixture; no Windows build was run. Publication remains blocked pending the older
source logs/counter, and the existing live site's data hash is unchanged.

## Migrate the owner's older machine-profile play logs

The user confirmed sole ownership and provided `/tmp/StepMania 5.rar` for direct
repair. Updated only blank PlayerGuid fields in 5,203 song logs and 160 course
logs. All 6,190 unique event IDs, every other byte, and the original archive are
preserved. Packaged all 6,309 active Upload XML files with instructions and a
hash report in `/tmp/StepMania-play-logs-repaired.zip`; ZIP verification passes.

The existing native exporter accepts all 6,030 song plays with zero skipped
records; all 827 previously uploaded events remain byte-identical. The dashboard
importer passes the full published-history checks and produces 7,249 lifetime
plays and 6,030 song event records locally. The user will merge the repaired logs
into Save/Upload and select Upload/Retry. No real data was uploaded during this
verification, no new preference was added, and no Windows build was run.

## Dashboard banner recovery

The first repaired-log upload published successfully in dashboard run 43550339204.
The site has 1,110 songs without banners and 29 missing images among the latest
150 plays. Its upload contains no song metadata yet; the SongID lookup fix has
not yet been packaged in a new Windows build. Fixed a second lookup defect:
the exporter now uses ImageCache's Cache/Banner directory. The Linux build and
native uploader test pass, including recovery with only a cached image available.

Audited the supplied Windows folder and exact matching Songs 2 archive paths.
Prepared `/tmp/StepMania-dashboard-banner-repair.zip` with 354 recovered images:
341 banners (including two video stills), nine backgrounds, and four jackets.
313 recoveries come from Songs 2. All 29 missing recent-play images are restored;
756 older entries lack matching folders or usable artwork. The ZIP includes an
unresolved-song list and source report, and changes only dashboard outbox copies.

The current uploader retains the patch. Full importer checks pass with all 6,030
original event files, every score list, totals, and existing banners unchanged.
The mounted game was read only and no data was published. The dashboard builder
also now detects Cache/Banner while retaining support for staged Cache/Banners;
all 20 Python tests pass. No Windows build was run.
