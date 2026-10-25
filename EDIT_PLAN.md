# Live gameplay speed edit plan

## Requested behavior

During a song, hold either Shift key and press Up or Right to increase the playback rate by 0.1x, or Down or Left to decrease it by 0.1x. Shift with Up+Down or Left+Right resets the current play to 1.0x. Each new arrow press makes one change; key repeat does not. Releasing Shift retains the chosen rate. Notes, music, and song video must follow the same song timeline. Restore the original rate on completion, cancellation, retry, and the next song. Ordinary arrow inputs retain their gameplay behavior.

## Implementation decisions

- Use the existing music-rate/audio-position pipeline; do not edit chart BPM or song files.
- Clamp live changes to 0.2x through 3.0x and round arithmetic to avoid accumulated floating-point drift.
- Recognize physical keyboard arrows with either Shift key, using the event's modifier snapshot. Consume shortcut presses, repeats, and releases, including when Shift is released first. Exclude captured arrows from held-note polling while preserving other mapped devices.
- Keep the override scoped to the current song and preserve saved preferences. Handle option rebuilding and exits explicitly.
- Make visible note movement scale with the live rate even when CMod/MMod is selected. Preserve the player's original scrolling settings.
- Keep standard music-clock video playback synchronized without applying the rate twice. Inspect background/foreground animation timing for paths using elapsed real time.
- Show the latest playback multiplier at the bottom-left after a change, using the theme font, W1 color, and FLAWLESS animation.
- Mark any play using live changes as practice: do not save normal high scores or replays, even after returning to the starting rate. Preserve this status in course/summary statistics.
- Enable controls for local gameplay, excluding editor, demonstration, calibration, and network sessions where independent rate changes are inappropriate.

## Commit checkpoints

1. **Plan and progress:** commit this plan and `PROGRESS.md` before implementation.
2. **Gameplay controls and lifetime:** implement Shift+arrow handling, input suppression, per-song rate state/reset, and practice score/replay handling; record checks and commit a coherent change.
3. **Synchronized presentation and verification:** finish fixed-scroll compensation, video/animation timing, rate feedback, regression coverage, and user documentation; update progress and commit.

Checkpoint boundaries may be combined or refined if compilation dependencies require it. Each implementation commit must describe its validation and remaining work in the progress file.

## Validation

- Build the affected C++ targets, preferably the complete application using the available build setup.
- Exercise both Shift keys and all four arrows, ordinary arrows, repeats, Shift-first release, arrow-first release, pre-existing holds, and a separately mapped controller.
- Check 1.0 -> 1.1 -> 1.0 -> 0.9, bounds, repeated adjustments, and non-1.0 starting rates. Verify reset-to-1.0x shortcuts in both key orders without losing the original rate.
- Check reset on completion, back/cancel, failure, retry, course advance, and destruction; preferred options must remain unchanged.
- Check practice status remains set after restoring 1.0x and survives course/statistics aggregation.
- Verify note movement for XMod, CMod, and MMod and the music-clock video path without double scaling.
- Run focused regression tests for the stateful behavior. Record any interactive audio/video checks that cannot be performed in this environment rather than claiming they passed.
- Review the final diff and working tree; commit only files belonging to this feature.

## Follow-up revision: finer control, reset chords, and bottom feedback

The user reports that the Windows speed controls work and requests these changes:

1. Change each increase/decrease to 0.1x; keep the 0.2x–3.0x limits.
2. With either Shift held, Up+Down or Left+Right sets the current play to exactly 1.0x. Detect the reset when the second arrow is pressed, in either order, using the event's physical key snapshot. Continue consuming captured presses/repeats/releases. Keep existing ordinary held notes intact.
3. Place the playback-rate notification bar at the bottom in both bundled themes, using an explicit message placement flag. Other system messages retain their normal placement.

Resetting to 1.0x must preserve the original saved rate for cleanup and the existing practice flag. Holding or releasing a reset chord must not change speed again. Tests will cover 0.1x arithmetic and bounds, both reset pairs/orderings/Shift keys, key snapshots, held-key suppression, and restoring non-1x song settings after a reset.

Commit the plan/progress checkpoint, implement and verify locally, then commit the revision. The user explicitly requested no Windows ZIP build yet because more changes are coming. Do not push commits or trigger the Windows workflow during this revision.

## Follow-up revision: judgment-style speed display

Replace the wide playback message bar with a compact multiplier at the bottom-left. The default theme uses its bold italic font, the W1 judgment color and outline, and the existing `JudgmentW1Command` animation. FLAWLESS itself is a pre-rendered sprite, so a theme font supplies the changing numerals. Use the fallback theme's normal font when that theme is active.

Own the display inside the gameplay screen so it ends with gameplay. Reset its animation on song completion, retries, course advances, and persistent-screen exits. Keep the corner position on a parent actor so judgment commands that change the text's position cannot move it to another part of the screen. Rapid adjustments must replace the value and restart its animation. Keep up to three decimal places for nonstandard rates and show ordinary rates as `1.1x`.

Use a pop-and-fade display like FLAWLESS unless the user chooses persistent visibility. Keep all rate controls, audio timing, and practice-score behavior unchanged. Validate the actor's Lua syntax, rebuild locally, review cleanup paths, and commit locally. Windows ZIP packaging and publishing remain deferred.
