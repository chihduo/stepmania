# Chiikawa development progress

## Branch separation

- [x] Verify the remote default branch is `5_1-new`, currently published at `0e53193abf`.
- [x] Preserve the full latest tree at `a74362c422` on `feature/chiikawa-theme` and create the separate development worktree.
- [x] Prepare and validate the finished-results snapshot on the main branch.
- [x] Commit the release snapshot and reconcile the feature baseline without losing its sources.

The six movie-related commits after the published tip are retained on this branch. Published history will remain intact. Raw inputs and trial artwork are removed only from the main branch's current tree; they remain here and in existing Git history.

Completed: `5_1-new` now points to release commit `7ba3066551`, whose parent is the unchanged published tip `0e53193abf`. It contains the finished movie, poster, theme behavior, required hooks/tests, user guides, native screenshots, asset manifest and branch workflow. Its current tree omits development-only assets; the six original movie commits remain on this feature branch.

Release validation passed both focused Lua checks, byte-for-byte runtime/source equivalence with `a74362c422`, all 15 runtime media hashes and original clip hashes, native screenshot preservation, 71 local document links and diff whitespace checks. The original development package is retained unchanged while reconciling the new main baseline. The main root plan/progress files are now concise release records; these feature-only files retain the complete theme journals.

Baseline validation: all 78 files from the original development package at `a74362c422` remain byte-identical. Runtime theme, shared/default-theme source and tests match main exactly; no merge conflicts remain. All 153 local links in the feature-worktree documentation resolve.

Future engine updates can be merged from `main` into this feature branch normally. Promote only reviewed runtime/doc paths back to main as described in `Docs/ChiikawaDevelopment.md`. The worktrees are `/home/codex/stepmania` (main) and `/home/codex/stepmania-chiikawa` (theme development). No remote push or ZIP was performed.

## Default branch rename

Renamed the default branch from `5_1-new` to `main` locally and on GitHub. GitHub still points to the original published commit `0e53193abf`; pending theme changes remain local. The main checkout tracks `fork/main`, and `fork/HEAD` resolves there. Main documentation checkpoint `27910ae890` updates active build/clone links, contributor instructions and promotion commands; this merge brings those changes into the feature worktree. Historical branch names in the completed separation records remain as originally observed.

No runtime files, media or tests changed. The branch-specific plan and active future-merge instruction now use `main`. No pending commits were pushed and no ZIP was built.

## Earlier theme progress

### Chiikawa artwork preview


The user requests an artwork plan plus AI-generated images in a preview folder, and confirms recognizable Chiikawa characters with a warm cream/pink/mint/sky-blue palette. The imagegen skill's built-in generation path is selected. No artwork was generated before the user answered the character/palette questions.

- [x] Confirm the visual direction and inspect current theme constraints.
- [x] Write `Docs/theme-preview/chiikawa/ARTWORK_PLAN.md`.
- [x] Generate and inspect four standalone illustrations.
- [x] Add the preview gallery, exact prompts, provenance, and asset checks.
- [x] Commit the completed preview package.

This stage prepares artwork for review; the current game theme remains unchanged.


Completed the four-image preview set using the built-in image tool. `Docs/theme-preview/chiikawa/images/` contains a transparent character trio, title background, selection background, and transparent results/replay illustration (about 5.4 MiB total). The latter three use earlier generated artwork as identity/style references. Full-resolution source pixels are preserved, and all exact prompts, output dimensions, alpha ranges, reference relationships, checksums, and inspection notes are recorded alongside them.

Added a self-contained `index.html` gallery with five illustrative screen layouts, 16:9/4:3 switching, and cream/checkerboard/dark cutout previews. Functional UI text and panels are browser elements; generated artwork contains no baked-in UI text. The gallery is explicitly labeled as a concept with sample data, not implemented gameplay.

Validation: all four PNGs decode and match their saved SHA-256 hashes; both cutouts have real transparency. Local document/gallery links resolve, image alt text is present, JavaScript syntax and diff whitespace pass. All five compositions were visually inspected in Linux Chrome at 1280 × 720 and 1024 × 768. Cream and checkerboard cutout rendering were inspected. Palette contrast checks reserve the secondary text color for cream/near-white surfaces and use dark text on pastel fills. The plan records pose/size deviations and the selection header's corner accents for later refinement.

Checkpoint `4c344e7bd9` records the plan; the completed preview checkpoint is `art: add Chiikawa theme illustrations and preview gallery [skip ci]`. Browser captures/logs remain in ignored `out/chiikawa-preview/`. No engine/theme installation, executable build, ZIP packaging, or remote push was performed. Playable-theme integration and Windows checks remain a later revision after artwork review.


## Follow-up: original gameplay visuals and varied Chiikawa artwork

The user asks to retain the original fonts, arrows, and judgment labels, changing only colors where practical, and requests more varied illustration designs/compositions. The first preview's schematic arrows and browser typefaces will be replaced with actual bundled font/sprite assets. Colored notes/judgments retain their existing texture colors initially; dark UI text and pastel surfaces establish the palette.

- [x] Inspect original theme font maps, noteskin, and judgment frames.
- [x] Record the revised constraints and six-variation plan.
- [x] Generate and inspect six new illustrations with different casts, poses, and layouts.
- [x] Reuse original gameplay assets in the browser preview and add artwork comparison controls.
- [x] Validate images, provenance, and layouts; commit the completed revision.

The current playable theme remains unchanged during this preview revision.


Completed the expanded preview with six new illustrations: solo Chiikawa listening, Hachiware with a guitar, Usagi leaping, a triangular picnic group, vertical peekers, and a resting pair. The first four images remain available. All new illustrations use the first trio only as a character/style reference; poses, cast sizes, silhouettes, and arrangements vary. Exact built-in image-generation prompts and composition inspection notes are saved beside the full-resolution images.

The gallery now renders actual original bitmap fonts, arrows/receptors, and judgment frames. Fifteen copied font/INI/sprite files match their repository sources byte-for-byte. Text alpha is tinted to the palette; colored notes and judgments retain their original shading and meanings. Artwork controls compare title scenes and cutouts across the five screen concepts. The long-label rendering was corrected to wrap words without squeezing glyphs.

Validation: all ten generated PNGs decode and match both manifest hashes and generator outputs; seven have real transparency. All 15 original component copies match their source hashes. All 101 local preview references resolve, image descriptions are present, and the metadata/renderer/gallery JavaScript passes syntax checks. All five concepts render at 1280 × 720 and 1024 × 768 with no missing sample glyphs; typography, decoration clearance, and full cream/checkerboard galleries were visually reviewed. `git diff --check` passes. Captures, browser logs, and the validation report remain under ignored `out/chiikawa-preview-v2/`.

Plan checkpoint: `04c95843dc`. Completion checkpoint: `art: vary Chiikawa previews and reuse original theme assets [skip ci]`. This completes the requested preview revision. The graph remains a layout sketch, and browser previews do not reproduce every engine font fallback or animation effect. Playable-theme integration and Windows verification remain later work. No live theme files, executable build, ZIP, or remote push are part of this checkpoint.


## Follow-up: paired illustrations and animated backgrounds

The user rejects the vertical peekers, requires at least two characters in every illustration, and asks whether the original animated backgrounds can be adapted. Inspected the original title/shared-menu actors: they rotate texture layers, update moving particles, and honor the existing `FancyUIBG` option. The same approach can be implemented with theme Lua/graphics without an engine rebuild.

- [x] Inspect original animated background actors and record the revised constraints.
- [x] Generate and inspect three replacement pair illustrations.
- [x] Remove rejected/superseded candidates and update gallery choices.
- [x] Add and verify a pastel animated title/menu preview with a motion toggle.
- [x] Update provenance and native integration notes; validate and commit the revision.

This task updates the artwork preview; native theme installation remains a later stage.


Completed three new transparent paired illustrations: Listening together (Chiikawa/Hachiware), Acoustic duet (Hachiware/Usagi), and Shared jump (Chiikawa/Usagi). Every active illustration now contains two or three characters. Removed the three solo candidates and rejected vertical peekers from active images, prompts, selectors, and gallery cards; their previous content remains in Git history, with manifest metadata explicitly marked retired. Nine active candidates remain, including six transparent cutouts.

Added a browser demonstration of the original title/menu animation approach. It reuses exact copies of the original tunnel and particle texture shapes, tinted pink/mint/blue, with slow opposing rotations and drifting particles behind the paired artwork and original fonts. The title defaults to the acoustic duet. Background motion can be paused/resumed and starts paused for reduced-motion preferences. Hidden screens pause, and gameplay has no decorative animation. `ANIMATION.md` links the original Lua actors and documents an inherited-theme implementation using the existing `FancyUIBG` option without recompiling the engine.

Validation: all nine PNGs decode and match generator-output hashes; manually reviewed character counts are at least two for every active candidate. Six cutouts contain real transparency. All 17 original-asset copies match their repository sources; 107 local/document/gallery references resolve. JavaScript syntax and diff whitespace checks pass. Local Chrome rendered all five screen concepts at 1280 × 720 and 1024 × 768 with no missing sample glyphs or runtime exceptions. Browser checks confirm animation advances, pause holds it still, resume restarts it, reduced-motion starts paused, and gameplay has zero animated decoration. Both ratios and cream/checkerboard artwork previews were reviewed. Captures and check reports remain under ignored `out/chiikawa-preview-v3/`.

Plan checkpoint: `4fe1e4815b`. Completion checkpoint: `art: pair Chiikawa illustrations and preview animated menus [skip ci]`. This completes the artwork-preview revision. Native theme installation and Windows renderer verification remain the later integration stage; no engine build, ZIP, or remote push is included.


## Follow-up: choosing and randomizing decorations

The user asks how to choose among the illustrations and whether they can be shown randomly. The existing theme already uses `math.random` in its Lua actors. Use curated choices by screen: welcoming title art, musical selection art, calm gameplay pairs, celebration on evaluation, and quiet replay-list art.

- [x] Review existing controls and confirm Lua random-selection support.
- [x] Add recommended/random/manual gallery modes and stable per-screen choices.
- [x] Document eligible artwork and when a new choice is made.
- [x] Verify preview behavior and commit the completed change.


Completed Recommended, Random, and Manual modes in the preview gallery. Each screen has a curated pool: welcoming/musical title and selection art, calm gameplay pairs, celebration on evaluation, and quiet replay-browser art. Random mode selects on screen entry and excludes the previous choice. Show another explicitly compares a different eligible candidate. Manual mode applies a fixed cutout; painted title scenes retain their own illustration. The animation toggle and artwork choice are independent.

Added `ARTWORK_CHOICES.md` with recommendations, timing, controls, and the native Lua route. Moved gallery controls into `gallery.js` as part of the selection change. No artwork files or playable-theme code changed.

Focused browser verification passes for recommended defaults, eligible random choices on all five screens, no immediate repeats on re-entry or Show another, stable gameplay artwork while resizing/changing ratio/toggling motion/waiting, manual choices, painted-title behavior, and query links. No missing sample glyphs or runtime exceptions were reported. Local links, script syntax, and diff whitespace pass. Captures and results are retained under ignored `out/chiikawa-preview-v4/`.

Completion checkpoint: `art: add per-screen random decoration preview [skip ci]`. Native theme integration remains the later stage.


## Follow-up: playable Chiikawa random artwork theme

The user authorizes implementing the random-per-screen policy and asks for five more instrument illustrations. This moves the work from browser concepts into a selectable inherited theme. Use piano, drums, violin, flute, and saxophone scenes with two or three characters; preserve original fonts, arrows, and judgments.

- [x] Inspect native theme fallback, layout, replay entry points, and isolated rendering tools.
- [x] Record implementation/artwork plan.
- [x] Generate and inspect five instrument scenes; extend the pool, gallery, and provenance; commit.
- [x] Implement the native theme and stable per-screen random artwork.
- [x] Verify Lua behavior, actual layouts, gameplay/replay stability, and fallback compatibility.
- [x] Document installation and validation limits; commit the finished change.

Artwork checkpoint: added piano duet, drum circle, violin serenade, flute practice, and saxophone swing. Each scene has two or three recognizable characters. All five PNGs retain generator bytes and real alpha; the gallery now has fourteen candidates and eleven cutouts. Title/selection use all five additions; calm scenes also enter gameplay/replay pools, and drums/saxophone enter evaluation. Random per screen is the preview default. Exact prompts, source hashes, character counts, and composition notes are recorded.

Focused Chrome checks pass for all curated pools, no immediate repeats, stable active-screen/gameplay choices, manual and recommended modes, query links, and all image decoding. No missing sample glyphs or runtime errors. Cream/checkerboard renders were inspected; captured evidence is under ignored `out/chiikawa-preview-v5/`. Native implementation follows this artwork checkpoint.

Native implementation completed in `Themes/chiikawa`, inheriting `default`. Eleven transparent paired/group cutouts use the same curated pools as the gallery. A private random stream avoids consuming/reseeding gameplay randomness and excludes the previous choice per screen category. Selection happens in each screen underlay's `OnCommand`, including cached title re-entry; constructing/preloading actors does not select. Rate changes, wheel movement, gameplay pause, and replay pause/resume/restart leave the texture alone. Returning to an existing browser preserves its illustration; reopening the browser selects again.

Added cream/pastel title and shared menu backgrounds with three rotating original texture layers and nine slowly drifting particles, controlled by `FancyUIBG`. Characters remain static. Original fonts, notes/receptors, judgments/effects, gameplay media, graph, score and replay behavior are inherited. Gameplay decoration uses the empty side for P1/P2 and is omitted for two-player, centered, double/shared, demonstration and wider-column fields. Single-song evaluation uses the unused grade area. The graph's evaluation labels become dark for cream-background contrast while retaining 80% opacity; line/fill opacity and gameplay labels remain inherited.

The replay browser had a hardcoded opaque backdrop and unnamed widgets. Nine added C++ lines name its four actors and load optional theme commands after their defaults. The Chiikawa theme uses those commands for a readable cream browser with art beside the list. Native inspection confirms the original default browser keeps its original dark centered layout. Pushed browsers own their backdrop; the shared background does not change when pushing a screen.

Validation:

- Linux Debug build passed with the existing local CMake toolchain. Only existing deprecated `std::binary_function` warnings were emitted for the compiled translation unit.
- `src/tests/test_chiikawa_theme.lua` passes eligible choices, no immediate repeat, coverage of all pool entries, deterministic private seeds, empty/single-item pools, gameplay RNG isolation, no preload selection, entry/message lifecycle, P1/P2 clearance at both ratios, and crowded-layout suppression. Existing life-history tests also pass.
- Isolated native rendering at 1280×720 and 1024×768 covers title, selection, gameplay, evaluation and replay, plus P2-only and centered gameplay. Cached title returns choose again. Shift+arrow rate/reset and native gameplay pause preserve artwork; the rate display persists and the graph stays beneath notes. Native P2 evaluation puts art in the unused left grade area.
- Replay pause/resume/restart and return preserve the appropriate artwork. Profile, replay and upload hashes match before/after viewing. Closing/reopening the browser changes its choice. Motion-off captures are identical in the background/art region; motion-on captures advance.
- All fourteen active illustrations decode, have at least two inspected characters, and retain source hashes. All thirteen native PNGs (eleven cutouts and two original motion textures) match their preview sources byte-for-byte. Gallery/native pools match. All 170 checked local document/gallery links resolve; Lua/JavaScript syntax and diff whitespace pass. Retained native logs have no Lua, broken-reference, or missing-texture errors.
- Browser policy tests passed in the artwork checkpoint. Final documentation-only gallery text changes pass syntax and local-link checks. Authentic unedited native screenshots are saved in `Docs/theme-preview/chiikawa/native/`; detailed logs, captures and test summaries remain under ignored `out/chiikawa-native/`.

Plan checkpoint: `ce573e07ab`. Artwork checkpoint: `b752db6a95`. Final implementation checkpoint: `feat: add Chiikawa theme with random screen artwork [skip ci]`. The guide documents selecting the theme and retaining `default`/`_fallback`, along with Lua customization. Existing install rules already include the entire Themes tree.

Limits: native checks use Linux software OpenGL and null audio; this build cannot verify audible OGG/media behavior, video synchronization, Windows x64 rendering, physical controllers or unusual song scripts. Two-player/double/wider-column suppression is covered by Lua checks; centered and P2-only layouts were additionally rendered. Windows verification remains pending for the next requested Windows build. No Windows ZIP or remote push was performed for this revision.


## Follow-up: black panel on song selection

Traced the reported screenshot panel to the default theme's unused P2 difficulty frame. The direct-screen capture path sets a temporary versus style in `ScreenSelectMusic::Init`; the pane's `IsHumanPlayer` test happens before `BeginScreen` selects compatible single-player styles. This is a stale visibility decision, not a missing graphic. Both inherited panes now use joined sides and refresh visibility on entry/join/leave while preserving active-player information, colors and animations.

- [x] Identify the actor and startup-state cause.
- [x] Correct both inherited player-pane visibility lifecycles.
- [x] Verify native layouts/joins and replace the screenshot.
- [x] Record validation and commit the correction.

Validation in the isolated Linux native fixture:

- Direct P1 song-selection startup at 1280×720 and 1024×768 hides the unused P2 pane. Joining P2 restores both populated difficulty/statistics panels at both sizes.
- Normal title → mode → music navigation works for P1-only and P2-only at 1280×720. Joining P1 from P2-only restores the left chart panel.
- The original default theme also hides the unused P2 panel on direct startup, confirming the shared decoration correction works outside Chiikawa.
- No theme runtime or missing-reference errors occurred in the retained engine logs. The former panel interior's neutral dark pixels dropped from 28,196 to zero in the replacement native screenshot.
- Updated `Docs/theme-preview/chiikawa/native/selection.png` with an unedited capture reached through normal P1 navigation. Native captures/logs are retained in ignored `out/chiikawa-pane-fix/`; `git diff --check` passed.

Checkpoint: `fix: hide inactive song-selection difficulty panes [skip ci]`. This is a Lua-only correction; no executable rebuild or ZIP is needed. Windows rendering remains untested by this Linux verification.

## Follow-up: dancing-trio gameplay fallback movie

- [x] Confirm theme fallback resolution in `Background.cpp` and movie music-clock support in `BackgroundEffects/StretchNormal.lua`.
- [x] Discover a video-generation provider; Runway is available but not connected. Ask whether to enable it or use a locally rendered illustrated animation.
- [x] Prepare and inspect the trio DDR keyframe and motion brief.
- [x] Produce the actual movie, integrate it, and verify gameplay behavior. Completed using the three supplied clips in the later movie-integration checkpoint below.

The built-in image generator cannot produce full-motion video. Artwork and source investigation can proceed while the animation route is pending; no movie has been generated or installed yet.

Saved the 1672×941 keyframe, exact image/movie prompts, provenance manifest and source-backed integration notes in `Docs/theme-preview/chiikawa/dance-movie/`. Visual inspection confirms all three recognizable characters, distinct active poses, three DDR pads and the intended vibrant palette. The image is an unchanged built-in-generator output; its SHA-256 and all package links pass validation. It is a still reference, not a movie.

Also confirmed that the linked gameplay screenshot uses the demo song's own dark `#BACKGROUND:background.png`, rather than the missing-image fallback. The next native capture must remove that background in an isolated test chart to exercise the new fallback properly.

Plan checkpoint: `b52a3b5228`. Artwork checkpoint: `art: prepare Chiikawa DDR movie reference and brief [skip ci]`. Runtime theme files and the existing native gameplay capture remain unchanged while the user chooses a video-generation route. No Windows ZIP was built.

## Follow-up: center-tile dance reference and varied choreography

The user has generated trial videos externally. Their reported issue is repetitive dancing/jumping on the up-arrow panel, consistent with the original reference positions. Those trial videos have not been supplied for direct inspection. This supersedes the earlier pending animation-route choice.

- [x] Edit and inspect a versioned keyframe with both feet on each neutral center tile.
- [x] Add jumps, running steps and spins with explicit travel across the pad to the movie prompt.
- [x] Update provenance, preview/use instructions and commit the correction.

No runtime movie has been installed; a chosen clip is still needed for integration and playback verification.

Saved `ddr-trio-keyframe-v2.png` (1671×941) using the built-in image-edit tool. Visual inspection confirms both feet of each character rest on their own existing blank center tile with contact shadows; Usagi is grounded and the old jump/impact marks are removed. Character identities, order, arcade composition and lighting remain consistent.

The revised 8-second movie prompt specifies left/right stepping, short runs across the rear/up and front/down panels, a two-foot jump and rebound, and one complete 360-degree spin returning to center. The README directs replacing both frame inputs with v2. Original image and image prompt are unchanged; the old movie prompt is archived as `movie-prompt-v1.txt` and the manifest retains its provenance.

Validation: both PNGs decode and match their recorded dimensions/hashes; all package links and prompt/reference paths resolve; archived content matches the previous commit byte-for-byte; `git diff --check` passes. The edited reference is a still image, and the next generated movie's choreography/loop remain unverified. Checkpoint: `art: center DDR trio and expand dance choreography [skip ci]`. No runtime theme or executable changes, ZIP or remote push.

## Follow-up: supplied dance movies and silent gameplay fallback

- [x] Probe all three uploaded clips: H.264 High, yuv420p, 1280×720, 24 fps, 240 frames each; AAC audio extends each container by about 27 ms.
- [x] Inspect endpoint frames: the same centered trio composition is retained, with only small encoding/render differences. Direct cuts preserve the full 30-second sequence without adding crossfade ghosting.
- [x] Assemble a video-only MP4, extract the poster and record reproducible provenance.
- [x] Integrate the fallback movie, static poster and cutout suppression.
- [x] Verify media and native gameplay behavior, refresh documentation/capture and commit.

The existing Linux layout-test executable was built without FFmpeg. Actual in-engine movie verification requires a decoder-enabled test build; Windows packaging is not part of this request.

Media checkpoint: `Themes/chiikawa/Movies/ddr-trio-loop.mp4` contains exactly 30.000 seconds / 720 frames at 1280×720, 24 fps, H.264 High/yuv420p, with exactly one video stream and no audio. All decoded frame hashes match the original clips concatenated in filename order, so no visual re-encoding or frame loss occurred. A matching first-frame PNG is the theme's common fallback poster. `build-loop.py` reproduces both assets and `movie-manifest.json` records original/output hashes; the three uploads remain unchanged.


Implementation: `BackgroundEffects/StretchNormal.lua` now supports an optional `Background/FallbackMovie` metric. Chiikawa points it to the combined MP4 only when the common fallback would otherwise be used. Static-background mode, disabled song backgrounds and an absent movie keep the PNG poster; other themes need no new metric. Keeping the common fallback as a bitmap also preserves Sprite/Banner thumbnail consumers. The normal background music clock, texture-rate and focus handlers remain intact. Gameplay omits the extra random cutout for songs without an image; course song changes hide/show an already selected cutout without rerolling it.

Validation:

- Media: exactly 30.000 seconds, 720 video frames, 1280×720, 24 fps, one H.264 High/yuv420p stream and zero audio streams; complete decode succeeds. All 720 decoded frame hashes equal the three original sequences in order. Source files are unchanged. Poster and movie SHA-256 values are recorded in `movie-manifest.json`.
- Lua: `test_fallback_movie.lua` passes no-art selection, own-image/scripted-movie preservation, static/disabled/missing-file/empty-metric fallback, themes without the metric, music clock, initial rate and focus callbacks. `test_chiikawa_theme.lua` passes selection/RNG/layout checks and no-art cutout suppression across song changes without rerolling.
- Build: enabled FFmpeg in the local Linux verification build and rebuilt successfully. Its decoder is the repository's pinned FFmpeg n2.1.3 (`eda6effcabcf9c238e4635eb058d72371336e09b`), built with H.264/MOV support and portable C decoding for this host. This confirms the MP4 is accepted by that older decoder generation. No C++ source changes were needed.
- Native missing-art gameplay: FFmpeg loads the combined movie, displays it behind the original notes/judgments/life graph, and loops it after 30 seconds. Shift+Up displays 1.1x while the movie continues playing. A double-Escape pause freezes the background region exactly across captures 1.5 seconds apart; resuming changes it again.
- Native fallback rules: Static Background and `SongBackgrounds=0` load the PNG, not the MP4. Static captures are pixel-identical in the background region. A song's existing image retains its static artwork and normal random cutout; a scripted MP4 remains the active background. The original default theme still loads its original fallback bitmap.
- Native layout/replay: 1280×720 P1, 1024×768 centered P1 and P2/scripted-video layouts render with the original gameplay layers. Standard fill cropping trims the outer movie edges at 4:3. Replay loads the movie, pauses with an unchanged background region across captures, advances on resume and reloads on restart.
- Final checks: all 64 local links in the four updated guides resolve; source/movie/poster/keyframe hashes match their manifests; the assembly recipe parses; the refreshed screenshot matches the raw capture byte-for-byte. Retained native logs contain no Lua runtime/theme errors, and `git diff --check` passes. Existing install rules include both Themes and BackgroundEffects.
- Preview/documentation: replaced `Docs/theme-preview/chiikawa/native/gameplay.png` with an unedited 1280×720 capture of the background-free chart at 1.1x and 45% brightness. Updated the movie, theme and repository guides, including copying `Themes/chiikawa/` plus `BackgroundEffects/StretchNormal.lua` into existing installations. No executable rebuild is needed for the existing Windows movie decoder.

The isolated fixture, build records, native screenshots and logs are under ignored `out/chiikawa-movie/`; user songs, profiles and settings were not used. Windows x64 rendering, physical controller play and audible synchronization remain untested. Native checks use software OpenGL and null audio.

Plan checkpoint: `09b8817104`. Media checkpoint: `e9ca447d2d`. Implementation checkpoint: `feat: use silent dance movie for Chiikawa fallback [skip ci]`. No Windows ZIP or remote push was performed for this revision.


## Movie-background capture publication

The requested GitHub page still showed the pre-movie capture because the finished theme release had remained local. Reused the already verified 1280×720 native capture from `out/chiikawa-movie/gameplay-rate-wide.png`: the trio dances behind original notes, judgments and the life graph, with 1.1x speed and 45% brightness. Visual inspection confirms the movie version, and both worktrees' PNGs match the raw native capture byte-for-byte.

Screenshot SHA-256: `a49840da22b495df18a8ad63c2ceebf0dd39eab8471ca3ad4175768e860b9184` (828,469 bytes). All 15 runtime media hashes still match the release manifest. Added a clear caption and direct MP4 link beside the screenshot. Published the development branch first and then `main` at `eca4da9033`. GitHub now serves the movie-background capture, caption and direct silent-MP4 link at `Docs/theme-preview/chiikawa/native/README.md`. Verified the live README, screenshot and MP4 Git blob hashes against the local release, including exact README text. Both remote branch tips matched the pushed commits at verification. This follow-up publishes the previously local final theme and branch-workflow documentation as well as the requested preview.

Merged the published main baseline back into this feature branch; the development sources and distinct preview documentation remain here. No Windows build or ZIP was requested or started.


## Transparent title-menu dance loop

- [x] Confirmed the requested location is the right-side illustration on the Game Start / Options title menu.
- [x] Confirmed native GIF loading reads only the first image; native PNG sprite states support alpha and animation.
- [x] Record the feature-only plan and exact artwork prompts.
- [x] Generate and inspect character poses and cartoon pad artwork.
- [x] Implement the transparent native title animation and looping GIF preview.
- [x] Verify native layout/lifecycle/motion settings, update documents and commit.

Baseline: `373e795d6f` on `feature/chiikawa-theme`; main remains `eca4da9033`. Use the built-in image generator for the new drawings and preserve its transparent source outputs. Existing movie backgrounds and random decorations on other screens are outside this change.


Artwork checkpoint: built-in image generation produced twelve recognizable character poses on a 1448×1086 RGBA source atlas and three matching cartoon pads on a 2172×724 RGBA source. Both have genuine 0–255 alpha. The source atlas has uneven visual spacing, so `build-assets.py` crops the inspected pose bounds and aligns them losslessly into twelve 416×416 cells with a common foot baseline. Every packed crop matches its original RGBA pixels exactly; the pad source is copied unchanged. Source outputs, exact prompts, packing coordinates and runtime hashes are in `title-dance/`.

Implementation: the title underlay now uses `Chiikawa.TitleDance()` from `Scripts/07 TitleDance.lua`. Three sprites share a twelve-state PNG sheet above one fixed pad layer. A deterministic 3.2-second clock drives side steps, hops and a small shuffle, with phase offsets per character. Pose boundaries use a rounded phase to keep exact periodicity across floating-point wraparound. `OffCommand` removes the update callback; returning to the title restarts it. `FancyUIBG=0` selects neutral poses with no update callback. Other screen pools and gameplay backgrounds remain unchanged.

Preview: `title-dance/build-preview.mjs` exports the actual Lua choreography, renders 96 transparent canvas frames and encodes a looping GIF. Source artwork, exact prompts, packing recipe, runtime hashes, interactive preview, GIF and native 4:3 capture are under `title-dance/`. The current native `title.png` is an unedited 1280×720 capture after returning from Options. The gallery explicitly labels its older static title comparisons.

Verification:

- `test_chiikawa_title_dance.lua` passes pose coverage, valid state ranges, periodicity and boundary continuity, gameplay RNG isolation, fixed pads, 16:9/4:3 sizing, motion off/on and cached return lifecycle. Existing `test_chiikawa_theme.lua` also passes.
- Native Linux captures fit both 1280×720 and 1024×768. Options opens and returns to the animated title; Game Start enters the normal caution screen. Moving captures differ, while the illustration region remains pixel-identical across frozen captures 1.5 seconds apart. No Lua errors appear in the saved logs.
- The transparent GIF has 96 frames, 3,200 ms total duration and infinite looping. Every decoded frame's opacity mask matches its rendered PNG with zero extra or missing opaque pixels. The rendered wraparound frame is byte-identical to the start.
- All 17 runtime media hashes and preview input/output hashes match. JavaScript syntax, 190 local documentation links and `git diff --check` pass. `title-dance/verification.json` records test output and capture/log hashes; `preview-manifest.json` records reproducible preview inputs and outputs.

The fixture uses the existing Linux debug executable, software OpenGL and null audio; its inherited unsupported-OGG warnings do not affect this silent animation. Windows x64 rendering is still unverified. Intermediate frames, native fixture and detailed logs stay under ignored `out/title-dance/`.

Checkpoints: plan `497bd6f5ae`; artwork `4f608b03db`; final implementation `feat: animate the Chiikawa title illustration [skip ci]`. All three are on `feature/chiikawa-theme` only. Main remains clean at `eca4da9033`. No executable rebuild, Windows ZIP or remote push.


## Chiikawa as the default theme

- [x] Trace startup, per-game preferences, invalid-theme recovery and packaged Data files.
- [x] Configure the shipped default and label the original theme Classic without changing its directory.
- [x] Verify native startup and saved theme choices, refresh documentation and commit on the feature branch.

`PrefsManager` reads `Data/Defaults.ini` before saved preferences. `ThemeManager` uses the configured `Theme` default when the initial `_fallback` or a removed theme is not selectable. This supports new installations without C++ changes and keeps explicit user selections. The original folder remains required by the theme inheritance chain.

Completed the default through `Data/Defaults.ini`, with Classic's display label in `Themes/default/ThemeInfo.ini`. The portable package check now requires `Data/Defaults.ini` and `Themes/chiikawa`; existing CMake installation rules already include both trees. Updated upgrade instructions to copy the defaults file while preserving Save and explicitly choosing Chiikawa once for an older installation. The README now shows the current Chiikawa gameplay capture and identifies older examples as Classic. The release manifest records the two configuration hashes alongside the unchanged 17 media assets.

Native verification used the existing Linux executable in an isolated installation, with no command-line theme override:

- An empty Save folder selects Chiikawa and renders its dancing-trio title.
- Saved `Theme=default` and `Theme=legacy` per-game choices remain Classic and Legacy, respectively.
- A missing saved theme resolves to Chiikawa even when the old `DefaultTheme=default` preference remains.
- The appearance menu displays **Chiikawa - StepMania 6** and **Classic**. Switching to Classic writes `Theme=default`; switching back writes `Theme=chiikawa`. Both transitions and startup cases have no Lua errors.
- Both runtime configuration hashes, all 17 media hashes, 155 local links and diff whitespace pass. Native captures, logs and the verification record remain under ignored `out/default-theme/`.

No new engine or theme-logic tests were needed for this configuration-only change. The Windows package script's required-path lists were updated and checked against the existing install rules; PowerShell and Windows runtime checks were not run on this Linux host. Commit: `feat: make Chiikawa the default StepMania 6 theme [skip ci]`, on `feature/chiikawa-theme` only. Main remains at `eca4da9033`; no executable rebuild, ZIP or remote push.


## Default-theme promotion and current README captures

- [x] Confirm main is already an ancestor of the feature baseline; both worktrees are clean.
- [x] Identify the reviewed title/default/Classic runtime changes and feature-only source assets.
- [x] Capture Chiikawa gameplay, evaluation and replay; refresh the inline GIF and silent MP4.
- [x] Update release-safe README/capture documentation and provenance.
- [x] Commit the feature checkpoint, selectively promote to main, verify and reconcile the feature baseline.

Baseline: main `eca4da9033`; feature `fdd9180efa`. The user now explicitly authorizes main-branch promotion, replacing the earlier feature-only restriction for this finished release.

Capture checkpoint: new unedited 1280×720 title, 0.9x gameplay, completed evaluation and 1.2x replay screenshots use runtime `fdd9180efa` and the existing FFmpeg-enabled Linux binary. The demo chart has no background, so the theme movie is visible at 45% brightness. The same synthetic 32-measure play supplies the new 40-second silent MP4 and 12.01-second, 96-frame GIF. The saved replay confirms all eight rate changes/reset events and completed status; evaluation reports 122 Flawless, six misses, max combo 59 and score 97,638,084.

Validation: all PNGs decode and match their raw captures; the MP4 fully decodes with 960 H.264 frames and no audio; the new GIF decodes and loops. Native logs show FFmpeg movie playback in both gameplay and replay with no Lua errors. Inspected every screenshot and slowdown/reset GIF samples. All current media/runtime/configuration hashes and 94 local links in changed feature guides pass. Capture provenance is in `Docs/media/manifest.json`; intermediate media and scripts stay ignored under `out/readme-chiikawa/`. Native capture processes were stopped. Windows hardware and audible synchronization remain unverified.

Prepared shared runtime guides and capture documentation with release-safe links. `Docs/media/title-dance.gif` is the completed transparent illustration preview; prompts, packing scripts and source atlases stay in the feature-only package. Main promotion is the remaining step.

Completed promotion: feature capture checkpoint `56789e3115`; main release `d961b87fb9`. Main contains the finished title/default/Classic revision, current README media, checks and release docs. The three focused Lua checks pass from the release tree; all 42 selected runtime/check files match the feature snapshot, all output/configuration hashes match, and 106 release links resolve. An isolated main-tree startup with no saved/forced theme renders Chiikawa's title without Lua errors.

Merged the main release baseline back into this feature branch, preserving its full gallery, prompts, original movies, source atlases, scripts and detailed journals. Resolved the preview README to retain feature comparisons with a release note, and adopted main's final release manifest. No engine rebuild, Windows ZIP or remote push.


## Simplify the theme display name

Synchronized main through `f2f8d5f616` to retain the corrected Windows 6.0 branding and upgrade guidance. Changed the theme's displayed name to **Chiikawa**, updated current user guides and the release manifest, and retained the internal `chiikawa` ID. Historical progress entries keep the names actually shown at the time. Validation passes: the INI and manifest agree on the display name and stable ID, the existing focused Lua theme check passes, all 49 local links in the updated guides resolve, and diff whitespace checks pass. Promoted only the finished runtime label, current guides and release manifest to main as `c274d020c3`, then reconciled that release baseline here. Development artwork and journals remain feature-only. No executable or ZIP rebuild was needed.


## Gameplay background overlap

The gameplay underlay instantiates `Chiikawa.Decoration("game")`, which is rendered after the engine song background. Its eligibility rule suppresses the cutout only when no song background image exists, leaving song images and scripted movies with cover art obscured. Replay playback inherits this same underlay. Main `c274d020c3` is already included in the feature baseline. An isolated reproduction and fix verification are prepared under `out/gameplay-background-layers/`.

Removed the gameplay cutout actor while preserving Classic's screen filter and life graph. Deleted its unused gameplay layout and song-change visibility handler; selection, single-song evaluation and replay-browser decoration behavior remains intact. The title/game pools are retained for the older browser concepts, now labeled as such. Current user guides explain the two-file Windows update; the release manifest records `gameplay_cutouts: false`.

Validation passes:

- Reproduced the original cutout over a song-provided background before editing; before/after PNGs and logs are retained under `out/gameplay-background-layers/`.
- All three focused Lua checks pass: selection/layout/gameplay suppression, title animation, and fallback movie behavior. The existing layout check now exercises suppression with and without song artwork for P1/P2 at both aspect ratios.
- Eight native cases pass: song image, P2 image at 4:3, fallback movie, static poster, song-selection decoration, song-provided movie with cover art, evaluation decoration, and recorded movie replay. Separate frame comparisons confirm motion in both the song movie and replay. The screen filter, notes, judgments and life graph remain visible.
- The initial isolated fixture omitted saved timing-option files and exposed warnings in inherited timing actors. Restored the complete synthetic UserPrefs, expanded the log check to include `Error playing command`, and reran all cases. Final logs contain no Lua command errors. No unrelated runtime code was changed.
- All 19 released media/configuration hashes, 82 local documentation links, gallery JavaScript syntax and diff whitespace pass. Native game/display processes were stopped.

Native verification uses Linux software OpenGL and Null audio; Windows hardware and audible synchronization were not retested. No executable rebuild or new Windows ZIP was needed. Selective main promotion follows this feature checkpoint.

Completed selective promotion: main `6fefd60c81` contains the finished runtime fix, existing-check update, current guides and release manifest from feature checkpoint `eb2cd3f52b`. All three focused checks, 19 runtime hashes, 59 release links and exact selected-file comparisons pass in main. Reconciled the feature baseline, preserving its gallery README and source assets while adopting the final release manifest. No executable or ZIP rebuild was needed.
