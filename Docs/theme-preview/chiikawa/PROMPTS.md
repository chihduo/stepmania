# Generation prompts and provenance

All ten illustrations were generated with the **built-in `image_gen` tool**, using the exact prompt files linked below. No fallback CLI, external image API script, manual recoloring, or post-generation cutout was used. Images are copied byte-for-byte from tool output. The gallery scales them for display and draws functional text and controls separately using the original theme assets.

| Image | Exact prompt | Input references | Transparency requested |
| --- | --- | --- | --- |
| [Character trio](images/01-character-trio.png) | [01-character-trio.txt](prompts/01-character-trio.txt) | None; new illustration | Yes |
| [Title background](images/02-title-background.png) | [02-title-background.txt](prompts/02-title-background.txt) | Character trio, for identity/style | No |
| [Selection background](images/03-selection-background.png) | [03-selection-background.txt](prompts/03-selection-background.txt) | Character trio and title background, for identity/palette | No |
| [Results/replay illustration](images/04-results-replay.png) | [04-results-replay.txt](prompts/04-results-replay.txt) | Character trio, with changed poses | Yes |
| [Listening](images/05-listening.png) | [05-listening.txt](prompts/05-listening.txt) | Character trio, for identity/style only | Yes |
| [Acoustic](images/06-acoustic.png) | [06-acoustic.txt](prompts/06-acoustic.txt) | Character trio, for identity/style only | Yes |
| [Leap](images/07-leap.png) | [07-leap.txt](prompts/07-leap.txt) | Character trio, for identity/style only | Yes |
| [Picnic rehearsal](images/08-picnic.png) | [08-picnic.txt](prompts/08-picnic.txt) | Character trio, for identity/style only | No |
| [Peek](images/09-peek.png) | [09-peek.txt](prompts/09-peek.txt) | Character trio, for identity/style only | Yes |
| [Rest](images/10-rest.png) | [10-rest.txt](prompts/10-rest.txt) | Character trio, for identity/style only | Yes |

The requested cast and palette were explicitly selected by the user. The [official anime site](https://www.anime-chiikawa.jp/) was consulted for the character identities; no downloaded official illustration is bundled here. These are AI-generated fan-art previews, not official art or gameplay captures. Original fonts, notes, and judgment sprites are copied directly from this repository and were never passed through the generator.

## Composition review

The six new prompts explicitly avoid reusing the same frontal lineup. Listening uses one seated character; Acoustic adds a diagonal instrument; Leap uses a tall airborne silhouette; Picnic arranges three interacting characters at unequal heights; Peek uses staggered partial-body views; Rest uses two curled sleeping figures. Each preserves recognizable character markings and the warm pastel outline treatment. Cutouts have real transparency, and no functional UI text is baked into the illustrations.

The prompt files preserve the actual requests, including constraints the generator approximated. Acoustic is closer to a front three-quarter pose than a side view; its diagonal guitar and solo seated silhouette still distinguish it from the first set. Picnic extends further left than requested, so the preview puts the title and menu in its clear upper-left area. Peek deliberately crops parts of bodies at implied panel edges; final integration should place it beside panel boundaries. Rest uses a low oval blanket composition with both faces visible.

In the first set, the requested sitting pose for the selection vignette became a small dancing pose, while the intended empty interface area was preserved. The title group occupies more vertical space than initially requested; its menu clears the faces. These differences remain documented for final layout decisions.

The generator does not expose a seed/model identifier through this tool. Re-running a prompt can produce a different image. Actual dimensions, alpha ranges, file sizes, reference relationships, SHA-256 hashes, and inspection notes are recorded in [manifest.json](manifest.json).
