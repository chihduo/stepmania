# Live gameplay speed progress

## Status

Planning checkpoint complete. Implementing gameplay controls and per-song lifetime.

## Checkpoints

- [x] Investigate playback, video clocks, modifier lifetime, input routing, and score handling.
- [x] Write the edit plan and progress file.
- [x] Commit planning checkpoint (`docs: plan live gameplay speed controls`).
- [ ] Implement Shift+arrow controls and suppress shortcut note inputs.
- [ ] Implement temporary rate lifetime and practice score/replay handling.
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

- Source inspection only so far. No implementation or in-game verification has occurred.

## Remaining work

Follow `EDIT_PLAN.md`, updating this file at each implementation checkpoint. Identify the available build toolchain and document any runtime verification limitations.
