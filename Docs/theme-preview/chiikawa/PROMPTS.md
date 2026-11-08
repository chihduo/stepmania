# Generation prompts and provenance

All nine active illustrations were generated with the **built-in `image_gen` tool**. Exact prompts are linked below. Tool outputs are copied byte-for-byte, with no manual recoloring or cutout. The gallery scales the illustrations and renders functional text, arrows, and judgment labels using the original theme assets.

| Image | Exact prompt | Reference inputs | Transparency |
| --- | --- | --- | --- |
| [Character trio](images/01-character-trio.png) | [01-character-trio.txt](prompts/01-character-trio.txt) | None | Yes |
| [Title background](images/02-title-background.png) | [02-title-background.txt](prompts/02-title-background.txt) | Character trio | No |
| [Selection background](images/03-selection-background.png) | [03-selection-background.txt](prompts/03-selection-background.txt) | Character trio and title background | No |
| [Results/replay](images/04-results-replay.png) | [04-results-replay.txt](prompts/04-results-replay.txt) | Character trio | Yes |
| [Picnic rehearsal](images/08-picnic.png) | [08-picnic.txt](prompts/08-picnic.txt) | Character trio, identity/style only | No |
| [Rest](images/10-rest.png) | [10-rest.txt](prompts/10-rest.txt) | Character trio, identity/style only | Yes |
| [Listening together](images/11-listening-together.png) | [11-listening-together.txt](prompts/11-listening-together.txt) | Character trio, identity/style only | Yes |
| [Acoustic duet](images/12-acoustic-duet.png) | [12-acoustic-duet.txt](prompts/12-acoustic-duet.txt) | Character trio, identity/style only | Yes |
| [Shared jump](images/13-shared-jump.png) | [13-shared-jump.txt](prompts/13-shared-jump.txt) | Character trio, identity/style only | Yes |

The user selected recognizable Chiikawa, Hachiware, and Usagi with a warm pastel palette, and now requires at least two characters in every illustration. The [official anime site](https://www.anime-chiikawa.jp/) was consulted for identity in the initial preview. No downloaded official illustration is bundled. These are AI-generated fan-art previews. Original fonts, notes, judgments, and background texture shapes are copied directly from this repository and were never passed through the generator.

## Latest composition review

Listening together shows Chiikawa and Hachiware sitting close in different headphones. Acoustic duet shows Hachiware and Usagi with guitar and tambourine. Shared jump shows Chiikawa and Usagi at unequal heights meeting raised paws. Each has two recognizable complete characters, real transparency, and a different action/silhouette. Faces and ears fit within the canvas. All three were inspected in the gallery over cream and checkerboard and in the screen compositions.

The prompts preserve the actual requests, including details approximated by generation. The duet is more front-facing than requested and includes three small music notes. The instrument hardware is simplified cartoon detail. These differences are accepted for the preview because the paired activity, readable silhouettes, and clear interface area are preserved.

For retained candidates, Picnic extends further left than requested, so its title/menu stays in the clear upper-left area. The first title group's greater-than-requested height and the selection background's dancing pose were previously accepted and remain documented in the manifest. All retained illustrations contain two or three characters.

## Retired candidates

The user rejected vertical peekers (09). Solo listening (05), acoustic (06), and leap (07) were superseded by the two-character requirement. These four files and their prompts have been removed from the active preview; previous versions are in Git checkpoint `ff1919e188`. Historical metadata in the manifest is explicitly marked retired and must not be used for future layouts.

The generator does not expose a seed/model identifier through this tool. Actual sizes, alpha ranges, source filenames, character counts, reference relationships, hashes, and inspection notes are recorded in [manifest.json](manifest.json). The animated preview uses original texture shapes and browser transforms, as described in [ANIMATION.md](ANIMATION.md); no AI-generated video is involved.
