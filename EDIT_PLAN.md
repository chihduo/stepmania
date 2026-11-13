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


## Windows x64 delivery: normal scoring after live speed changes

The user requests a ZIP of the latest build. Package source `0535f3e9d9b5ae3470d43d6b5d3ed4d778133e63` with the existing Windows portable workflow, including the change that lets live speed adjustments follow normal scoring and saving rules. All earlier speed controls, graph presentation, and song-sort fixes are included.

Push the completed source, verify the Windows Release build and startup smoke test, download the portable archive, and check its checksum, ZIP integrity, x64 executable/source revision, bundled feature assets and documentation, and clean portable layout. Record the run, artifact, checksum, and validation limits in `PROGRESS.md`; commit and push the delivery record without triggering another build.

Delivery complete: source `0535f3e9d9` passed the Windows build/startup pipeline and downloaded-archive checks. The download link, checksum, and remaining in-game scoring verification are recorded in `PROGRESS.md`.


## Follow-up: fork README and gameplay demonstrations

Replace the upstream-only README with an introduction to this fork, its synchronized per-song speed controls, persistent speed display, normal scoring policy, single-player life-history graph, and song-sort selection fix. Document Windows x64 portable installation and updates, source builds, links to detailed feature documentation, and upstream credits/licenses. Use the fork's actual build workflow and avoid depending on an expiring artifact as the only download route.

Capture real gameplay and evaluation from the committed engine and bundled default theme in an isolated temporary game directory if this environment can run a virtual display. Use a generated demo chart and synthetic audio so screenshots/animation can be redistributed without importing third-party songs. Clearly identify the capture platform and demo setup. Prefer a compact inline animation plus readable still images; do not present fabricated UI as a gameplay capture.

Commit the plan checkpoint, write and verify the README and any media, record capture provenance and limitations, and commit/push the documentation with `[skip ci]`. Validate relative links, media formats/sizes, source-supported feature claims, and the final diff. No new Windows executable is needed for documentation changes.

Completed: replaced the upstream README, captured real Linux gameplay/evaluation with the unmodified theme, added PNG/GIF/MP4 demonstrations and capture notes, validated links/media and feature descriptions, and recorded the results in `PROGRESS.md`.


## Follow-up: single-player recording and replay

Implement the authorized in-game record/replay feature for local single-player regular songs. Preserve live speed changes, note results, score/combo/life presentation, and ordinary score saving for the original play. Replays use installed song assets and verify chart/timing identity. Watching a replay must not write scores, awards, activity totals, play counts, replay recordings, or modified player preferences.

Use a versioned replay journal with initial chart/options metadata, timestamped logical input and speed events, and recorded judgment/note-state/statistic changes. Replay presentation consumes the recorded outcomes so different frame rates do not rejudge the original performance. Reuse the engine song clock, media rate control, theme judgments, and life graph. Bound parser/file sizes and reject missing/changed charts and unsupported recordings with readable errors. Preserve settings and stage/session state across exit/restart.

Enable recording through the song options, provide a Watch Replay action on evaluation and a replay browser for the selected song/chart, and provide pause/resume, restart, and exit while watching. Start with single-player regular songs; courses, network play, multiplayer, editor/calibration, and dynamic note attacks are outside this initial scope and must be handled explicitly. Keep seeking and MP4 recording outside this in-game replay feature.

Checkpoints:

1. Commit the plan/progress before implementation.
2. Implement and test replay data storage, identity validation, event ordering, and recording.
3. Implement playback, profile/state isolation, theme controls/browser, and lifecycle handling.
4. Build, run focused engine/regression tests, exercise actual rendered recording/playback in an isolated portable setup, document controls/limits, and commit the verified result.

Cover taps, simultaneous inputs, misses, holds/rolls/releases/mines/lifts, rate changes/reset chords, pauses, failure/early exit, missing or changed charts, malformed files, P1/P2 single-player, repeat playback, and original-score/profile preservation. Record automated and interactive validation limits separately. Do not publish a new Windows ZIP until requested.


Completed: implemented single-player recording and playback with the live-rate timeline, native recorded results, chart validation, state/profile isolation, and default-theme controls/browser. Passed the local Linux build and focused engine/regression checks, exercised rendered P1/P2 replay and Save-file preservation in an isolated setup, and added the guide plus an authentic screenshot. Verification details and Windows/audio/video limits are recorded in `PROGRESS.md`. All checkpoints are local; no new Windows ZIP was built or published.


## Follow-up: StepMania 6 README title and record/replay section

Use **StepMania 6** as the README title while retaining the StepMania 5.1 community-fork attribution. Make record/replay a main README section with recording setup, F10 entry points, viewer controls, saved-file location, original-score behavior, and the existing screenshot/guide. Validate local links and whitespace, update the progress record, and commit this documentation checkpoint.

Completed: updated the title and main record/replay section, validated README links/formatting, and recorded the documentation checkpoint in `PROGRESS.md`.

## Chiikawa release and development separation

Keep the finished Chiikawa theme, its required shared hooks/tests, user guides, native screenshots and asset manifest on `main`. Keep prompts, alternatives, the browser gallery, source videos, assembly scripts and detailed development journals on `feature/chiikawa-theme` in its separate worktree. Preserve published history and consolidate only the six unpublished movie commits into the current release result.

Verify that all runtime files and focused tests match the tested movie revision, that final assets retain their hashes, and that release documentation has no broken local links. Document future selective promotion in `Docs/ChiikawaDevelopment.md`, commit the release snapshot, and reconcile the feature baseline while retaining its source package. No push or Windows ZIP is part of this task.

The theme's detailed plans are now in `Docs/theme-preview/chiikawa/DEVELOPMENT_PLAN.md` on the feature branch.


Release snapshot complete: runtime equivalence, media provenance, focused Lua checks and local links pass. The main snapshot contains the finished movie/theme and release docs; the full development tree is preserved in the feature worktree. Branch reconciliation is recorded in the feature journal after this checkpoint.


## Rename the default branch to main

Rename the local and GitHub default branch from `5_1-new` to `main`, preserving commit history and the current published tip. Update local tracking, fork HEAD, build/clone links and active branch-workflow instructions in both worktrees. Keep historical branch names in past progress entries. Verify the remote default name and exact commit, local upstream settings, document links and clean worktrees. Existing unpublished theme commits remain pending publication; a rename does not publish them.


Completed: GitHub and local branch names are `main`, the remote commit is unchanged, tracking and fork HEAD point to `fork/main`, and active branch references are updated. The pending theme release remains local. Feature-worktree synchronization follows this documentation checkpoint.


## Publish the movie-background gameplay preview

Publish the completed native gameplay capture and its adjacent silent-movie link on `main`, using the already prepared theme release. Preserve the exact captured pixels and existing provenance. Publish the feature branch first to retain development sources separately, then verify the README, screenshot and MP4 on GitHub. No new game build is needed for the preview update.


## Promote Chiikawa defaults and current README captures

Promote the finished transparent title loop, `Data/Defaults.ini` Chiikawa defaults, Classic display label, packaging requirements and focused theme check from feature revision `56789e3115`. Keep original theme directory names for inheritance and saved preferences. Include the refreshed native title/gameplay/evaluation/replay PNGs, current gameplay GIF and silent MP4, final title preview and capture provenance. Retain artwork sources, prompts, browser comparisons and detailed journals on the feature branch.

Validate the selected release files against the feature snapshot, run the focused Lua checks, verify all release/media/configuration hashes and local links, and commit a focused release before fast-forwarding main. Reconcile main back into the feature branch. No Windows ZIP or remote push is requested.

Completed: selected the finished runtime/default/Classic changes and final media from `56789e3115`, verified their equality and manifests, passed the three focused Lua checks and actual release-tree default startup, and checked 106 release links. Main receives one focused result commit; development materials remain on the feature branch.


## Correct the Windows fork version and clarify theme upgrades

Update the displayed/build/package version to 6.0 and Windows file metadata to match. Preserve the legacy application identifier so existing user-data locations remain readable. Extend the Windows smoke check to reject an old runtime version and to verify that a fresh installation actually selects Chiikawa. Explain how an upgraded installation retains Classic until the user selects Chiikawa, and how to detect a nested extraction or old shortcut. Validate the local build and real startup with fresh and saved theme settings, commit the fix, and produce a corrected Windows portable build.

Completed: published the compatibility-preserving version fix as `8977aa3f0c`, verified native fresh/saved/switch behavior, and built the corrected Windows x64 ZIP in run `42825456584`. Windows metadata and runtime/default-theme checks, the downloaded SHA-256, all 28 release manifest entries, and full archive CRC pass.


## Simplify the theme display name

Promote the completed display-name change from feature checkpoint `8dbfd4a8e1`: use **Chiikawa** in `ThemeInfo.ini`, current user guides and the release manifest. Keep the internal theme ID and saved selections compatible. Validate the configuration, focused theme check, selected release paths and links; commit the finished result and reconcile the feature baseline. No executable or ZIP rebuild is needed.


## Keep song backgrounds unobstructed

Promote the completed gameplay-decoration fix from feature checkpoint `eb2cd3f52b`. Remove the extra cutout from gameplay and replay playback while retaining the inherited screen filter and life graph, menu/evaluation artwork, and normal song/fallback background selection. Include the existing-check updates, current user guides and release manifest only. Verify release-path equality, focused Lua checks and links, then reconcile and publish the branches. Native image/movie/replay verification is complete on the feature branch; no executable or ZIP rebuild is needed.

Completed: the reviewed release files match feature checkpoint `eb2cd3f52b`; all three focused Lua checks, 19 runtime media/configuration hashes, 59 release-document links and whitespace checks pass. The change is ready to publish with the native verification recorded above.


## Introduce the fork to new players

Rewrite the repository README around what StepMania 6 does, how to install and start playing, live speed controls, recording/replay and performance review. Treat Chiikawa as the normal default interface, keep authentic gameplay media, and link to detailed customization rather than recounting theme migrations. Preserve data-safe update instructions, source builds and credits. Check feature/control accuracy, Markdown links and image references, then commit and publish the documentation checkpoint.

Completed: rewrote the introduction and reader flow, retained current gameplay media and practical instructions, and validated links, anchors, image references, feature details and documentation-only scope.

README image follow-up completed: use the existing title-menu capture as the opening screenshot and retain the animated gameplay demonstration. Verified both media references.


## Enlarge evaluation artwork

Promote the verified illustration layout from feature checkpoint `4f6e010d8f`: increase the maximum box from 230 × 180 to 345 × 270, shift it upward/outward, and fit its width around the central judgment panel on narrower displays. Preserve mirrored P2 placement and single-player eligibility. Native P1/P2 checks at 16:9 and 4:3 are complete; verify the selected Lua/guide and release metadata, then reconcile and publish the branches. No executable or ZIP rebuild is needed.

Completed: selected runtime/guide files match `4f6e010d8f`, the focused Lua layout check passes in main, and all 19 released media/configuration hashes remain unchanged.


## Readable gameplay timing labels

Promote the verified shared-theme change from `951d424e85`: remove the small gameplay accuracy readout and draw per-lane timing offsets in the overlay above the score frame. Preserve their existing receptor positions and animations. Completed native Linux checks cover both players, 16:9/4:3, centered/reverse/Mini, timing off, Blind, Classic, evaluation and replay/restart/return. No executable or ZIP rebuild.


## Keep consecutive hold heads visible

Reproduce closely spaced same-lane holds/rolls in a private native fixture. Split engine rendering into a pass for all visible hold bodies/tails and a subsequent pass for their heads, preserving reverse/active/fade transforms and noteskin flags. Build the Linux executable and compare paused before/after gameplay, including reverse, active/missed holds, both rendering modes and replay. Keep this engine fix on main; record verification and commit at completion. A Windows executable rebuild is required, but no ZIP is requested.

Completed: split hold/roll rendering into bodies/tails followed by heads across all note owners in each column. The Linux debug build and eight native scenarios pass, including recorded playback and return. A pixel check rejects the original occluded heads and passes the corrected normal/fast rendering in all four lanes. No Windows ZIP was built.


## Windows executable with the hold-visibility fix

Build published engine/runtime revision `69772bd100` with the Windows x64 Release workflow. Require portable layout, x64 PE, 6.0 version and fresh Chiikawa startup checks. Download the package, verify its checksum/CRC and current theme/media contents, and provide the updated executable plus portable ZIP. Keep saved user data out of the package. Record the finished build without changing its runtime sources.

Completed: Windows workflow #16 (`43066153222`) successfully built `69772bd100`. Windows x64/6.0 metadata, fresh Chiikawa startup, package checksum/CRC, 28 manifest entries and current theme Lua checks pass. Downloaded the portable ZIP and extracted Program files; this completion record changes no runtime source.


## Manual dashboard upload

Implement the approved title-menu-only GitHub Pages integration, following [the implementation plan](Docs/DashboardIntegrationPlan.md). Separate data from code, preserve existing history, reuse the Python dashboard calculations in Actions, and keep credentials in Windows Credential Manager. Commit importer/workflow, engine, UI and validation checkpoints.

Completed the manual-upload engine, dashboard importer/workflow, Chiikawa screen promotion, setup documentation and tested history-preserving Pages migration. Linux engine/native/contract checks and Windows x64 build #19 pass. The first real Windows profile upload requires the player’s local configuration and credential.


## Dashboard menu feedback

Reproduce the reported inactive actions after token entry in a private native fixture. Verify missing connection settings and return from the token dialog, make setup/browser/busy failures visible, preserve credential storage and title-menu-only uploads, and add focused regression checks. Build and verify the corrected Windows executable, then commit and publish the completed fix.

Implemented and verified the feedback correction in the native Linux UI and engine tests. The original behavior is reproducible with default connection settings; no input lock was found on return from the token dialog. Preparing the Windows package and final upgrade guidance.

Completed and published runtime fix efd31f4371. Windows build #20, startup checks, downloaded ZIP verification and all 14 native menu cases pass. Only Program/ and the optional setup guide differ from the previous package.


## Readable upload menu and status

Match the title menu with individual bold action rows and selected/unselected sizing. Separate a large, colored, shadowed status headline from readable details; distinguish progress, success, warnings and errors. Make every unavailable activation report its reason, including busy and leaving states. Develop Chiikawa styling on its feature branch, verify native widescreen/4:3 layouts and action feedback, then selectively promote completed runtime files and documentation.

Promoted verified theme checkpoint dfba79087a with title-menu-sized action fonts, a blue/green/amber/rose status headline and shadow, readable details, and a final native screenshot. The shared engine checkpoint is 7c8b62f24f. Only the completed runtime aliases/metrics, user guides, native capture and release manifest are promoted; detailed fixtures and journals remain on the feature branch.
