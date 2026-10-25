# Live gameplay speed progress

## Status

Gameplay controls and lifetime implemented. Finishing synchronized presentation and build validation.

## Checkpoints

- [x] Investigate playback, video clocks, modifier lifetime, input routing, and score handling.
- [x] Write the edit plan and progress file.
- [x] Commit planning checkpoint (`docs: plan live gameplay speed controls`).
- [x] Implement Shift+arrow controls and suppress shortcut note inputs.
- [x] Implement temporary rate lifetime and practice score/replay handling.
- [ ] Implement synchronized scrolling/video behavior and rate feedback.
- [ ] Build and run focused regression checks.
- [ ] Review and commit the completed revision.

## Evidence and findings

- Baseline: `b3561fc60e` on `5_1-new`; the working tree was clean before this task.
- `ScreenGameplay::Update` already applies the current music rate to audio. Song position comes from the audio playback position.
- Standard background-video effects use `effectclock("music")`, and movie decoding advances from that clock.
- CMod/MMod compensate for music rate by design, so live controls need separate temporary compensation to make visible note speed change proportionally.
- Theme callbacks run after gameplay input. Held-note checks also poll `InputMapper`, so consuming a screen event alone is insufficient.
- Existing slowdown disqualification checks preferred options; a temporary change needs a persistent practice marker for this play.
- No repository `AGENTS.md` files were found. The command sandbox cannot initialize in this environment; repository commands require the managed escalation path.

## Validation log

- Planning commit: `7ec3f6389f`.
- Standalone C++ regression checks pass with GCC 13, `-Wall -Wextra -Werror -pedantic`, AddressSanitizer, and UndefinedBehaviorSanitizer. Coverage includes the requested rate sequence, bounds, 10,000 increase/decrease cycles, non-1x starts, reset, all arrow aliases, repeats, both release orders, pre-existing holds, concurrent arrows, and recovery after a lost release.
- `git diff --check` passes.
- Initial CMake configuration stopped at the missing libmad dependency. CMake was installed under `/tmp`; development headers are being downloaded into the ignored build directory to attempt a build without changing system packages. No in-game check has been performed.

## Implementation checkpoint

- Shortcuts are intercepted before input-code queues and normal dispatch. Captured keys stay excluded from press, held-duration, and analog-level queries; other mapped devices remain available.
- The override is applied only to current song options and reapplied after normal option updates. Preferred, stage, and song targets remain intact. Song end, reload, and screen destruction clear the override.
- Manual control temporarily takes precedence over Haste and suspends autosync sampling. These settings return to the normal song targets afterward.
- A stage-level practice flag survives restoring the original rate and statistics aggregation. It blocks normal high scores, replay files, Padmiss score exports, and upload files.
- This checkpoint is `feat: add temporary Shift-arrow gameplay rate controls`. Fixed-scroll compensation, feedback, and broader validation follow in the next checkpoint.

## Remaining work

Finish CMod/MMod compensation, video/animation timing review, rate feedback, build validation, and user documentation. Record any interactive checks that remain unavailable.
