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
- Implementation checkpoint — `feat: keep playback speed visible and enlarge life history` (this commit).

The speed display now uses a dedicated `GameplayRateDisplay/ChangedCommand` in the default and fallback themes. It preserves the W1-style pop, color, font, and glow, with no scheduled fade-out. Existing reset/exit commands still clear it on song completion, retry, course advance, and screen exit.

The graph's preferred dimensions are now 400 by 128 theme pixels, and its bottom margin is 71 instead of 56. The existing gameplay width limit still prevents ordinary note-lane overlap in tight layouts. Evaluation uses the preferred width; narrow evaluation layouts compact the judgment rows and fit the song title beside the graph. P2-only life meters and lower evaluation details use the otherwise unused left area to avoid the enlarged plot. Two-player layouts are unchanged.

Presentation validation:

- **PASS:** bundled Lua 5.1 syntax checks for the speed-display actor, graph actor, and shared graph/layout helper.
- **PASS:** the existing focused life-history checks with the new dimensions. Covered live updates, pause caching, screen cleanup, evaluation statistics selection, narrow P2 gameplay, and clearance between the enlarged graph and evaluation rows/song titles. Widescreen and two-player evaluation spacing remain unchanged.
- **PASS:** source review of the default/fallback persistent animation commands, rapid-value replacement, existing song/screen reset paths, P2 life-meter/frame positions and entrance/exit direction, and diff whitespace checks.
- **PENDING:** interactive appearance, persistent speed visibility, and the larger graph in Windows gameplay/evaluation. This revision changes theme code/configuration and documentation only; no executable rebuild, Windows ZIP, push, or remote workflow was performed.
