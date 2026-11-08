# Chiikawa artwork preview for StepMania 6

**Open [index.html](index.html) in a browser** to compare the artwork and animated menu concepts. It works locally without a server or network access.

Every active illustration shows **at least two recognizable characters**. The vertical peekers have been removed, and the three solo candidates have been replaced with paired compositions. There are now **nine active illustrations**, all in warm cream, pastel pink, mint, and sky blue.

The screen concepts preserve the original bitmap fonts, arrows/receptors, and judgment artwork. Text tint and surrounding panels adopt the palette; note and judgment textures retain their original colors and shading. Layouts use sample data and remain an artwork preview for later playable-theme integration.

## Compare the illustrations

| Candidate | Characters | Composition |
| --- | --- | --- |
| [11 · Listening together](images/11-listening-together.png) | Chiikawa + Hachiware | Close seated pair in different headphones |
| [12 · Acoustic duet](images/12-acoustic-duet.png) | Hachiware + Usagi | Guitar and tambourine, unequal head heights |
| [13 · Shared jump](images/13-shared-jump.png) | Chiikawa + Usagi | Two airborne characters meeting raised paws |
| [08 · Picnic rehearsal](images/08-picnic.png) | All three | Interacting triangular group around a blanket |
| [10 · Rest](images/10-rest.png) | Chiikawa + Hachiware | Low sleeping pair on a mint blanket |
| [01 · Character trio](images/01-character-trio.png) | All three | Welcoming group and identity reference |
| [02 · Title background](images/02-title-background.png) | All three | Painted meadow scene with space for menus |
| [03 · Selection background](images/03-selection-background.png) | All three | Small corner group with open interface space |
| [04 · Results/replay](images/04-results-replay.png) | All three | Cheerful celebration group |

Use **Artwork choice** for Recommended per screen, Random per screen, or Manual. Random mode chooses a suitable decoration on screen entry and avoids its previous choice; **Show another** compares another eligible illustration. Manual mode enables **Manual decoration** for direct comparison. Artwork stays fixed throughout gameplay. See [how artwork is chosen](ARTWORK_CHOICES.md) for the per-screen recommendations and random pools. Use **Show candidates** to see the five varied group/pair scenes, the first four candidates, or all nine. Switch between 16:9 and 4:3, and inspect cutouts on cream, checkerboard, or dark backgrounds.

All nine full-resolution files are in `images/`. Six have real alpha transparency; three are opaque backgrounds. Generated source pixels are preserved. Retired candidates are available only in Git history and are excluded from the current preview and image folder.

## Animated background preview

The default **Animated character scene** title combines the two-character cutout with slow pastel rotating layers and drifting particles behind it. Selection, evaluation, and replay concepts use the same background treatment where their artwork leaves it visible. Gameplay decoration stays still. The painted title-scene alternatives remain static.

The **Background motion** button pauses/resumes the animation. The preview starts paused when the browser requests reduced motion. These are browser demonstrations of the original theme's Lua background approach; implementing the same effect in StepMania needs theme Lua/graphics changes, **not recompilation**. See the [animation feasibility and integration notes](ANIMATION.md).

## Files and validation

- [Artwork and implementation plan](ARTWORK_PLAN.md)
- [Recommendations and random selection](ARTWORK_CHOICES.md)
- [Exact generation prompts and inspection notes](PROMPTS.md)
- [Image manifest](manifest.json)
- [Original theme asset mappings and checksums](original-theme/README.md)

The illustrations were generated with the built-in image tool. Image decoding, true alpha, source hashes, local links, and script syntax were checked. All 17 copied original font/INI/sprite/background files match their repository sources. All five concepts render at 1280 × 720 and 1024 × 768 without missing sample glyphs. The browser checks exercise moving, paused, resumed, and reduced-motion states, and verify that gameplay has no animated decoration.

This preview does not reproduce every engine font fallback, rendering effect, or gameplay behavior; the life graph remains a layout sketch. Actual theme installation and Windows x64 verification are the next implementation stage.

Random selection was also checked for eligible per-screen choices, no immediate repeats, stable artwork while staying on a screen, fixed painted titles, and manual selection. The randomization controls do not change the illustration files.
