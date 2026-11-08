# Generation prompts and provenance

All four illustrations were generated with the **built-in `image_gen` tool**, using the exact prompt files linked below. No fallback CLI, external image API script, manual recoloring, or post-generation cutout was used. The images are copied byte-for-byte from the tool output. The preview gallery scales them in the browser and supplies ordinary text/panels separately.

| Image | Exact prompt | Input references | Transparency requested |
| --- | --- | --- | --- |
| [Character trio](images/01-character-trio.png) | [01-character-trio.txt](prompts/01-character-trio.txt) | None; new illustration | Yes |
| [Title background](images/02-title-background.png) | [02-title-background.txt](prompts/02-title-background.txt) | Character trio, for identity/style | No |
| [Selection background](images/03-selection-background.png) | [03-selection-background.txt](prompts/03-selection-background.txt) | Character trio and title background, for identity/palette | No |
| [Results/replay illustration](images/04-results-replay.png) | [04-results-replay.txt](prompts/04-results-replay.txt) | Character trio, with changed poses | Yes |

The requested cast and palette were explicitly selected by the user. The [official anime site](https://www.anime-chiikawa.jp/) was consulted for the character identities; no downloaded official illustration is bundled here. These are AI-generated fan-art previews, not official art or gameplay captures.

The prompt files preserve the actual generation requests, including constraints that the generator may approximate. The requested sitting pose for the selection vignette became a small dancing pose, while the intended empty interface area was preserved. The title group occupies more vertical space than initially requested; the gallery keeps its native menu above the character faces. Those differences are acceptable for this preview and should be revisited if the final layout needs more room.

The generator does not expose a seed/model identifier through this tool. Re-running a prompt can produce a different image. Actual dimensions, alpha ranges, file sizes, reference relationships, and SHA-256 hashes are recorded in [manifest.json](manifest.json).
