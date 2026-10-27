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
- Treat live speed changes as normal gameplay: use the ordinary score, award, replay, and export rules, including after a reset or in courses/summaries.
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
- Check live speed changes do not suppress normal score recording, awards, replays, or exports, including after restoring 1.0x and in course/summary statistics.
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

## Follow-up revision: single-player life-history graph

Restore the life/health-over-time graph shown in the user's historical screenshot. Show it at the bottom-right of default-theme gameplay and evaluation, including evaluation summary, when exactly one human player and one enabled player are present. Reuse the legacy grid and green/yellow/red fill texture. Place the compact graph above the bottom status area and keep single-player P2 modifier text clear of its evaluation position. During gameplay, reduce the panel width when the ordinary note field leaves less space at the right edge; keep the preferred width on evaluation.

Use one shared theme actor with bounded sampled geometry rather than repeatedly reinitializing the evaluation-only legacy graph actor. Sample the engine's existing life record and use its elapsed-play time domain so live music-rate changes do not distort history. Update immediately on recorded-life events and as elapsed play time advances; pause leaves the time domain unchanged. Evaluation reads the displayed screen's StageStats, including summary aggregation. Expose the last life-record timestamp to Lua so failure records and summary gaps are included in the graph's time range.

Preserve the engine's policy of not recording history for endless courses; hide the plot there. Keep two-player layouts and scoring unchanged. The actor is owned by each screen and stops updating when that screen exits.

Validation covers single-player eligibility, history endpoint/geometry, clamped life values, empty/zero-duration history, rapid changes, summary/failure timestamps, live and evaluation data selection, and bounded geometry. Run focused Lua and engine checks, compile locally, review the layout, and commit at suitable checkpoints. Do not push or build a Windows ZIP until the user requests it.


## Windows x64 delivery of the current feature set

The user has now requested the ZIP, lifting the earlier packaging deferral. Build the committed speed-control, themed speed-display, and life-history changes with the existing Windows portable pipeline. Push the completed source revision, wait for its Windows build and startup smoke test, download the artifact, and verify its checksum, ZIP integrity, x64 executable, bundled feature assets, and clean portable layout. Record the source revision, run, artifact link, and verification results in `PROGRESS.md`, then commit the delivery record without triggering another build.

Delivery complete: source `2613625a1d` passed the Windows pipeline and downloaded-archive checks. The run, download link, and SHA-256 are recorded in `PROGRESS.md`.


## Follow-up revision: persistent speed and larger life history

Keep the playback multiplier visible after its first adjustment for the rest of the current song. Give it a dedicated theme command that preserves the W1 pop, font, color, and glow without the judgment's fade-out. New adjustments replace the value and restart the pop; existing song completion, retry, course-advance, and screen-exit cleanup still clears it.

Double the graph's preferred dimensions from 200 by 64 to 400 by 128 theme pixels and increase its bottom margin from 56 to 71, moving its lower edge upward by 15 pixels. Preserve automatic narrowing beside gameplay note lanes unless the user chooses the full-size alternative. Keep evaluation's larger graph clear of the score rows, song title, and P2-only details; use the unused left-side area for the P2-only life meter and lower evaluation details where needed. Two-player layouts stay as they are.

Record the plan checkpoint, make the theme changes, run Lua syntax and the existing focused graph checks, review the persistent display's cleanup and narrow evaluation layouts, and commit locally. These are theme changes; a new executable or Windows ZIP is not required for local validation and has not been requested in this turn.


## Windows x64 delivery of the presentation follow-up

The user requested packaging of revision `1f3b21c9c9`, including the persistent speed number and enlarged, raised life-history graph. Use the existing Windows portable workflow, verify the downloaded ZIP against the build's checksum and source revision, and check that its bundled theme files include the new default/fallback animation commands and graph layout. Record the download and verification results in `PROGRESS.md`, then commit the delivery record without starting another build.

Presentation follow-up delivery complete: source `1f3b21c9c9` passed the Windows pipeline and downloaded-archive checks. The artifact link and SHA-256 are recorded in `PROGRESS.md`.


## Follow-up revision: bottom-left graph beneath gameplay information

Move the life-history graph to the bottom-left in gameplay, evaluation, and evaluation summary. Preserve the existing transparency, as clarified by the user; do not apply the initially proposed 60% opacity. Keep the 400 by 128 preferred dimensions and 71-pixel bottom margin.

Use the actual screen underlay for gameplay, after its background filter and before notes, judgments, life meters, overlays, and messages. Give evaluation's graph a lower sibling draw order than its statistics. Use a 96-pixel left inset: the existing left life-meter frame is centered at 40 and extends to 72, leaving a 24-pixel gap. Keep the graph at its preferred width now that notes draw above it. Preserve existing life-bar positions and the bottom-left persistent speed number.

Mirror the evaluation layout clearance around the left-side plot: place lower single-player details on the available right side, keep judgment rows above the graph when needed, and fit song titles beside it. Keep two-player layouts and scoring unchanged.

Commit this plan checkpoint, make the theme changes, update the existing focused graph checks for position/layering/life-bar clearance and unchanged opacity, and review the lifecycle and screen wiring. Update documentation and commit locally. Do not create a new Windows ZIP or push until requested.


## Follow-up revision: line and label opacity

Set `self:diffusealpha(0.8)` in the shared graph's `Line`, `Title`, `Value`, `StartTime`, and `EndTime` InitCommands. Preserve the backing, fill, placement, and draw order. Check Lua syntax and the existing focused graph checks, update the documentation, and commit locally; no recompilation or ZIP build is needed for this theme edit.


## Follow-up revision: preserve the highlighted song when changing sort

Investigate the reported song/artist jump while cycling sorts with Left+Right. The wheel currently restores `m_pPreferredSong` at the end of a sort transition, even when the highlighted song is newer than that preference. The two arrows can land in adjacent frames: the first arrow updates the preference to a neighbor, the second returns the highlight to the original song, and the wheel changes sort before the screen processes that return notification.

Add a focused headless regression that reproduces this timing through the engine's actual wheel movement and sort-transition code. Preserve the highlighted song by identity when it is present in the new sort; retain the existing preferred/current/first-song fallback for section headings or songs absent from the new list. Check both arrow orders, same-frame and split-frame processing, repeated sort cycling through Artist, and fallback behavior. Keep ordinary navigation and the existing shortcut definitions unchanged.

Commit this plan checkpoint, reproduce the failure, make the smallest engine fix, build and run the focused checks, then record the results and commit locally. A Windows executable rebuild will be required to deliver this C++ fix; do not package or push a new ZIP until requested.

Completed: reproduced the stale-preference failure, preserved the highlighted song before falling back, passed the local build and focused engine regressions, and recorded the remaining Windows verification in `PROGRESS.md`.


## Windows x64 delivery: song-sort fix and latest graph presentation

The user requests a new ZIP. Build committed revision `00e67ffb848e78b5ac275a5e511bb7af324b2e88` with the existing Windows portable workflow. This delivery includes the song-selection preservation fix, the life-history graph beneath gameplay at the bottom-left, the 80% white-line/label opacity, and all earlier playback-rate and graph changes.

Push the completed source, verify the Windows Release build and startup smoke test, then download the archive and check its SHA-256, ZIP integrity, x64 executable/source revision, bundled theme files, required media DLLs, and absence of generated user data. Record the run and artifact links and commit the delivery documentation without triggering another build. Interactive Windows gameplay remains a separate verification step.

Delivery complete: source `00e67ffb84` passed the Windows build/startup pipeline and downloaded-archive checks. The artifact link, checksum, and remaining in-game verification are recorded in `PROGRESS.md`.


## Follow-up revision: normal scoring after live speed changes

The user explicitly requests normal gameplay regardless of speed changes. This supersedes all earlier practice-only scoring decisions in this plan and the historical progress entries.

Remove the live-rate practice flag and its score/high-score, award, replay, and export restrictions. Stop marking players disqualified merely because a live rate change occurred. Preserve the game's existing unrelated eligibility rules, user save/replay/export settings, synchronized timing, input handling, and per-song option restoration. Replays continue using the existing export format; this change does not introduce a playback-rate timeline into that format.

Replace the practice-only regression with checks that rate-adjusted plays enter the normal score lists and that normal save-score, autoplay, and disqualification settings still behave as before. Verify reset-to-1.0x and song-option cleanup do not prevent saving, and cover summary aggregation. Run the existing engine checks and a local build, update the gameplay documentation, and commit at the plan and implementation checkpoints. Do not create or publish another Windows ZIP until requested.

Completed: removed live-rate practice/disqualification gates, verified normal profile recording and XML persistence plus existing eligibility rules, passed the local build and engine regressions, and documented the remaining Windows checks in `PROGRESS.md`.
