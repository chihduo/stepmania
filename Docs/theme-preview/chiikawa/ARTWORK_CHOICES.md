# Choosing character decorations

Use the screen's purpose and available space to choose artwork. Keep quiet, compact poses near gameplay and lists; use more energetic poses for welcoming or celebrating. All eligible illustrations contain at least two characters and fit the existing decoration areas.

| Screen | Recommended default | Random choices | Reason |
| --- | --- | --- | --- |
| Animated title | Acoustic duet | Acoustic duet, Character trio, Listening together | Welcoming music theme; readable beside the title/menu |
| Song selection | Listening together | Listening together, Acoustic duet, Character trio | Music-related decoration that leaves the song list clear |
| Gameplay | Resting pair | Resting pair, Listening together | Calm silhouettes outside the note field |
| Evaluation | Celebration trio | Celebration trio, Shared jump | Supportive celebration beside the results |
| Replay browser | Resting pair | Resting pair, Listening together, Acoustic duet | Quiet decoration beside a list |

## Preview controls

Open [index.html](index.html) and use **Artwork choice**:

- **Recommended per screen** shows the defaults in the table.
- **Random per screen** chooses from that screen's short list when you enter it. It excludes the previous choice for that screen, so consecutive visits vary. **Show another** explicitly chooses a different eligible illustration for comparison.
- **Manual** enables **Manual decoration**, letting you compare any supported cutout on all five screen concepts.

Random mode is not a timed slideshow. The image stays fixed while you remain on a screen, including throughout gameplay. Clicking the already-active tab, resizing, changing the aspect ratio, or pausing/resuming background motion does not reroll the artwork. Manual selection is available when you want a fixed favorite.

The full painted title scenes (Picnic rehearsal and Original meadow) already contain their characters and retain them in every mode. Select **Animated character scene** to try recommended/random/manual title decorations. Show another is disabled for a painted title. Returning to the animated title in Random mode makes a fresh eligible choice. The original painted song-selection background remains a standalone candidate; the interactive selection concept shows the chosen cutout itself.

Direct preview links can use `?choice=random`, `?choice=recommended`, or `?choice=manual&art=12-acoustic-duet`. Existing `?art=...` links select Manual mode unless an explicit choice mode is supplied.

## Native theme implementation

This can be implemented in theme Lua without recompiling. The default theme already uses `math.random` in its [particle loader](../../../Themes/default/BGAnimations/ScreenTitleMenu%20background/_particleLoader.lua).

A shared theme helper should keep each screen's eligible filenames and last choice. When the screen's decoration actors are constructed, choose a filename from the eligible list after excluding the previous one, save the choice, then load that image with `LoadActor`. Keep the chosen texture for the actor's lifetime. With only one eligible image, use it directly.

For gameplay, choose once before the song starts and keep it through pauses and live speed adjustments. Do not choose from an update loop, song-wheel movement, judgments, or music beats. Keep this visual choice separate from chart/replay state and do not reseed the global random generator. A theme preference can expose Recommended, Random, and a fixed favorite in the playable theme.

The current implementation is in [gallery.js](gallery.js) for preview. Actual theme integration remains the next stage.
