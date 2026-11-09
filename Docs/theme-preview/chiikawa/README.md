# Chiikawa artwork preview for StepMania 6

The finished title animation, Chiikawa default and Classic label are promoted to **main** in `d961b87fb9`. This branch retains the artwork sources and comparisons. [Current native gameplay, evaluation and replay captures](../../media/README.md) match that release.

**The title menu now has a transparent dancing-trio illustration.** [View its looping GIF](title-dance/title-dance.gif), [open the interactive animation preview](title-dance/preview.html), or [see the native title capture](native/title.png). All three characters dance on their own cartoon pads. Source drawings, prompts and build steps are in [title-dance/](title-dance/README.md).

**Open [index.html](index.html) in a browser** to compare the earlier artwork and animated menu concepts. It works locally without a server or network access. Its title comparisons retain the older random stills; the playable title now always uses the dancing trio.

Every active illustration shows **at least two recognizable characters**. The vertical peekers have been removed, and the three solo candidates have been replaced with paired compositions. The comparison gallery contains **fourteen illustrations**, all in warm cream, pastel pink, mint, and sky blue, alongside the separate title-animation package.

The screen concepts preserve the original bitmap fonts, arrows/receptors, and judgment artwork. Text tint and surrounding panels adopt the palette; note and judgment textures retain their original colors and shading. Layouts use sample data. The [default Chiikawa theme](../../../Themes/chiikawa/README.md) uses these decorations with random selection on entry outside the animated title menu.

## Compare the illustrations

| Candidate | Characters | Composition |
| --- | --- | --- |
| [14 · Piano Duet](images/14-piano-duet.png) | Chiikawa + Hachiware | Diagonal pink piano and shared mint bench |
| [15 · Drum Circle](images/15-drum-circle.png) | All three | Triangular drum circle with clapping listeners |
| [16 · Violin Serenade](images/16-violin-serenade.png) | Hachiware + Chiikawa | Standing violinist and seated listener |
| [17 · Flute Practice](images/17-flute-practice.png) | Chiikawa + Hachiware | Flute practice with a shared music book |
| [18 · Saxophone Swing](images/18-saxophone-swing.png) | Usagi + Chiikawa | Leaning saxophonist and dancing partner |
| [11 · Listening together](images/11-listening-together.png) | Chiikawa + Hachiware | Close seated pair in different headphones |
| [12 · Acoustic duet](images/12-acoustic-duet.png) | Hachiware + Usagi | Guitar and tambourine, unequal head heights |
| [13 · Shared jump](images/13-shared-jump.png) | Chiikawa + Usagi | Two airborne characters meeting raised paws |
| [08 · Picnic rehearsal](images/08-picnic.png) | All three | Interacting triangular group around a blanket |
| [10 · Rest](images/10-rest.png) | Chiikawa + Hachiware | Low sleeping pair on a mint blanket |
| [01 · Character trio](images/01-character-trio.png) | All three | Welcoming group and identity reference |
| [02 · Title background](images/02-title-background.png) | All three | Painted meadow scene with space for menus |
| [03 · Selection background](images/03-selection-background.png) | All three | Small corner group with open interface space |
| [04 · Results/replay](images/04-results-replay.png) | All three | Cheerful celebration group |

The gallery defaults to **Random per screen**. Use **Artwork choice** for Recommended per screen, Random per screen, or Manual. Random mode chooses a suitable decoration on screen entry and avoids its previous choice; **Show another** compares another eligible illustration. Manual mode enables **Manual decoration** for direct comparison. Artwork stays fixed throughout gameplay. See [how artwork is chosen](ARTWORK_CHOICES.md) for the per-screen recommendations and random pools. Use **Show candidates** to see the five new instrument scenes, five earlier group/pair scenes, the first four candidates, or all fourteen. Switch between 16:9 and 4:3, and inspect cutouts on cream, checkerboard, or dark backgrounds.

All fourteen full-resolution files are in `images/`. Eleven have real alpha transparency; three are opaque backgrounds. Generated source pixels are preserved. Retired candidates are available only in Git history and are excluded from the current preview and image folder.

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

This preview does not reproduce every engine font fallback, rendering effect, or gameplay behavior; the life graph remains a layout sketch. Native theme installation is described in the [theme guide](../../../Themes/chiikawa/README.md). Windows x64 verification remains to be done with a future build.

Random selection was also checked for eligible per-screen choices, no immediate repeats, stable artwork while staying on a screen, fixed painted titles, and manual selection. The randomization controls do not change the illustration files.
