# StepMania 6: Chiikawa artwork and theme plan

## Confirmed direction

Use recognizable **Chiikawa, Hachiware, and Usagi**, with **warm cream, pastel pink, mint, and sky blue**. The user selected this direction on 2026-11-08. Create AI-generated artwork for preview before integrating a new theme. Keep the current playable theme unchanged during this artwork stage. The follow-up instruction locks the original theme fonts, arrow/receptor artwork, and judgment-label artwork; change their colors only where this preserves readability and the existing meaning of each color.

The initial cast uses the familiar white rounded Chiikawa, blue-and-white Hachiware, and pale-yellow long-eared Usagi. Use the [official anime character page](https://www.anime-chiikawa.jp/) as an identity reference. The illustrations in this folder are newly AI-generated fan-art concepts, not official artwork or actual game captures.

## Visual rules

- Use simple rounded silhouettes, clean dark hand-drawn outlines, tiny facial details, blush, and restrained flat pastel fills. Keep character proportions and markings consistent across images.
- Make the interface feel like a quiet illustrated stationery set: cream surfaces, softly bordered panels, a few small musical or star accents, and plenty of empty space.
- Draw all functional text, scores, chart details, judgments, control hints, and arrows with the engine. Do not bake functional text or controls into generated illustrations.
- Let characters decorate title, selection, evaluation, and replay screens. Gameplay decoration stays small and quiet, outside the active note field, life bar, graph, speed label, and recording status.
- Keep player, difficulty, judgment, selected, disabled, and warning states distinct. Color must not be the only way to identify selection or gameplay state.
- Preserve the original noteskin sprites, receptor shapes, judgment sprite sheet, font faces, and glyph shapes. Do not replace them with schematic arrows, newly drawn labels, or a new typeface. Keep subdivision/judgment colors distinct; where a texture already contains color, retain it unless a controlled tint preserves contrast and shading.

## Shared palette and components

| Role | Starting color | Use |
| --- | --- | --- |
| Canvas | `#FFF8EE` | Warm cream screen backgrounds |
| Surface | `#FFFCF7` | Readable cards and menu panels |
| Main text | `#4A3C40` | Titles, labels, score digits, outlines |
| Secondary text | `#796970` | Supporting labels; validate contrast at final sizes |
| Pink | `#F7BBC8` | Selection fill and cheerful accents |
| Mint | `#CBE7D7` | Positive states and secondary panels |
| Sky blue | `#BFDCF0` | Replay, information, and Hachiware accents |
| Soft yellow | `#F5DE9B` | Usagi and occasional celebration accents |

These are target UI colors, not a promise that every generated pixel is an exact hex value. Use dark text on pastel surfaces. Main text `#4A3C40` has contrast ratios of 6.41–10.20 against the listed backgrounds. Reserve secondary text `#796970` for cream/near-white surfaces (4.90–5.04); use the main text color on pastel fills. Share palette, spacing, border widths, shadow, and motion rules in Lua. Retain the original font mappings and glyph artwork, including Roboto text/combo fonts, the Open Sans header font, Overpass score digits, and the existing fallback glyphs for international song titles. Use quiet 150–250 ms interface transitions; keep large decorative animation out of gameplay.

## Initial preview set

Each illustration is generated separately with the built-in image tool. Requested dimensions are compositional targets; record the actual output size and alpha channel in the preview manifest. Keep full-resolution originals without post-generation recoloring or manual cutouts.

| File | Artwork | Target and intended use |
| --- | --- | --- |
| `images/01-character-trio.png` | Chiikawa, Hachiware, and Usagi together, welcoming the player | Transparent landscape cutout; reusable menu/header decoration and consistency reference |
| `images/02-title-background.png` | A quiet cream-and-pastel scene with the trio near the lower edge | Wide background; clear center/upper space for the real StepMania 6 title and menu |
| `images/03-selection-background.png` | Restrained illustrated border with a small character group at the lower-left | Wide background; leave the center and right clear for song details and the wheel |
| `images/04-results-replay.png` | Small cheerful trio with restrained musical accents | Transparent landscape cutout; evaluation or replay header decoration, with final placement chosen against the real statistics layout |

The preview gallery will show the standalone images and illustrative UI compositions using real HTML text/panels. Any such compositions are design mockups, not screenshots or implemented screens. Transparent images will be shown over a checkerboard and a cream background to inspect edges.

## Implementation after artwork selection

1. **Theme foundation.** Create `Themes/stepmania6/` with `FallbackTheme=default` and a separate display name. Establish shared palette, spacing, and panel helpers while inheriting the original font mappings and gameplay artwork. Preserve the current theme as a selectable option.
2. **Shared screen elements.** Apply the new logo treatment, backgrounds, headers, footers, menu rows, focus/disabled states, dialogs, and option screens. Use generated art as decorative textures; use Lua actors for reusable controls.
3. **Song selection.** Restyle the wheel, chart list, banner frame, BPM/time labels, profile status, and F10 hint. Check long song/artist names and fast navigation.
4. **Gameplay.** Restyle score/combo frames, life-meter framing, speed text, recording status, and graph labels using the shared palette. Retain graph placement beneath the notes, existing opacity behavior, and note readability. Suppress decorative characters when they cannot fit outside a centered/double note field.
5. **Evaluation and replay.** Apply the same panels, fonts, chart treatment, and control hints. Move the replay browser/viewer presentation settings from hardcoded C++ into theme metrics/Lua as needed; that part needs a new executable. Normal theme assets and Lua/metrics changes do not need recompilation.
6. **Optional later artwork.** Generate additional character poses only after this preview establishes proportions and line treatment. Possible additions: ready/cleared encouragement, an empty replay list illustration, and a loading decoration. Keep judgment lettering, icons, and note shapes crisp and functional rather than generated text.

## Validation and checkpoints

- Commit the artwork plan and progress record before generating the preview set.
- Inspect every generated image for recognizable markings, character count, facial consistency, extra limbs, unwanted text, busy UI zones, cropping, and true alpha where requested. Regenerate a specific asset if necessary.
- Save exact prompts, generation mode, source references, actual sizes, SHA-256 hashes, and inspection notes beside the previews.
- Validate local gallery links and image decoding. Review UI compositions at 16:9 and 4:3, with special attention to note/graph/life-bar/label clearance. Composition previews do not replace in-game verification.
- Commit the completed preview set and documentation. The preview stage does not install the theme or publish a Windows ZIP.
- After the user selects or refines the direction, integrate at theme-foundation, screen-layout, and final-verification checkpoints. Check P1/P2, centered play, reverse scroll, longer titles, several resolutions, replay pause/restart, and the actual Windows x64 renderer.

## Progress

- [x] Confirm recognizable characters and the warm pastel palette.
- [x] Inspect the current theme, graph/speed placement, and replay styling boundary.
- [x] Write the artwork and implementation plan.
- [x] Generate and inspect the four preview illustrations.
- [x] Create a local preview gallery and record prompts/asset metadata.
- [x] Validate and commit the completed preview package.
- [ ] Integrate the selected artwork into a playable theme in a later revision.


## Preview review results

Four full-resolution images and the local `index.html` gallery are complete. The gallery uses shared colors, text, panels, focus styles, and the generated illustrations to demonstrate five screens in both 16:9 and 4:3. Browser captures are retained under ignored `out/chiikawa-preview/`.

The images decode, match their saved hashes, and have the expected alpha/opaque formats. The cutouts were inspected on cream and checkerboard. The title/menu placement clears the characters; the selection artwork leaves the list clear; gameplay decoration uses the empty side area; evaluation/replay art clears the statistics and list. The selection background's outer musical accents should be considered when finalizing a production header. Minor pose/size differences from the generation requests are recorded in `PROMPTS.md` and `manifest.json`.

Preview scope is complete. The remaining unchecked integration item belongs to the later playable-theme revision after artwork review.


## Revision: preserve original gameplay assets and vary the illustrations

The user requests the same fonts, arrows, and judgment labels as the original theme, allowing color changes where practical. This supersedes the first gallery's browser-font and schematic-arrow examples. Update the browser compositions to render the actual original bitmap font atlases and sprite frames, and document that browser layout is still only a preview. Copy these assets unchanged with source hashes; do not regenerate their shapes with AI. Match the new palette through text tint and surrounding surfaces. Initially keep colored arrow and judgment textures intact so their shading and timing meanings remain readable.

Add six new illustrations with independent compositions rather than moving the same frontal trio to a different corner:

| Variant | Cast and action | Composition / use |
| --- | --- | --- |
| 05 · Listening | Chiikawa alone with headphones | Compact seated three-quarter portrait; transparent cutout |
| 06 · Acoustic | Hachiware seated with a small guitar | Side-oriented musical vignette; transparent cutout |
| 07 · Leap | Usagi alone in a joyful jump | Tall diagonal action silhouette; transparent cutout |
| 08 · Picnic | Three friends interacting on a blanket | Wide scene with depth and triangular grouping; clear UI area |
| 09 · Peek | The trio peeking at different heights | Tall edge decoration; partial-body poses, transparent background |
| 10 · Rest | Chiikawa and Hachiware resting together | Quiet close view with curved silhouettes; transparent cutout |

Keep the established character identity, warm palette, and outline treatment, but vary cast size, viewing angle, scale, action, silhouette, and spatial arrangement. Preserve the original four candidates. Add gallery controls to compare new artwork in the existing layouts and inspect each standalone image. Record prompts, input references, alpha/size checks, and composition review notes. Commit the revised plan, then the completed preview update; theme installation, publishing, and Windows packaging remain outside this artwork revision.
