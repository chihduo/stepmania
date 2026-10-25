# Live gameplay speed progress

## Status

Implemented and committed. Automated regression checks, the Linux validation build, and the Windows x64 Release build pass. The Windows portable ZIP has been downloaded and verified. Interactive audio/video verification remains pending.

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

- Either Shift + Up/Right increases playback rate by 0.2x; either Shift + Down/Left decreases it by 0.2x. Each arrow press changes the rate once. New rates are limited to 0.2x through 3.0x. Existing out-of-range options cannot make a shortcut change speed in the opposite direction.
- Shortcuts are intercepted before input-code queues and normal dispatch. Captured keys remain excluded from press, held-duration, and analog-level queries until release, including when Shift is released first. Other mapped devices remain usable.
- Only current song options receive the temporary override. Preferred, stage, and song targets remain intact. The override survives normal modifier updates and clears on song end, reload, screen exit, or destruction. Persistent screens and lesson-page input routing are covered.
- Manual control temporarily takes precedence over Haste and suspends autosync sampling. Normal song targets resume afterward.
- CMod/MMod retain their original spacing while the song clock accelerates. Saved scrolling settings are untouched.
- Standard video effects already use the music clock. Timer-based background/foreground animations follow song time while live control is active. Videos receive no additional rate multiplier.
- A localized message displays the new rate and identifies practice play.
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
