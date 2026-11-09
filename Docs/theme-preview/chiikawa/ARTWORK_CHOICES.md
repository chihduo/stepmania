# Choosing character decorations

The current native title menu always shows the [transparent dancing trio](title-dance/README.md). Its older static title pool remains available in the comparison gallery below. Other decorated screens retain random selection.

Use the screen's purpose and available space to choose artwork. Keep quiet, compact poses near gameplay and lists; use more energetic poses for welcoming or celebrating. All eligible illustrations contain at least two characters and fit the existing decoration areas.

| Screen | Recommended default | Random choices | Reason |
| --- | --- | --- | --- |
| Animated title | Acoustic duet | Acoustic duet, Character trio, Listening together, Piano duet, Drum circle, Violin serenade, Flute practice, Saxophone swing | Welcoming music theme; readable beside the title/menu |
| Song selection | Listening together | Listening together, Acoustic duet, Character trio, Piano duet, Drum circle, Violin serenade, Flute practice, Saxophone swing | Music-related decoration that leaves the song list clear |
| Gameplay | Resting pair | Resting pair, Listening together, Piano duet, Violin serenade, Flute practice | Calm silhouettes outside the note field |
| Evaluation | Celebration trio | Celebration trio, Shared jump, Drum circle, Saxophone swing | Supportive celebration beside the results |
| Replay browser | Resting pair | Resting pair, Listening together, Acoustic duet, Piano duet, Violin serenade, Flute practice | Quiet decoration beside a list |

## Preview controls

Open [index.html](index.html) and use **Artwork choice**:

- **Recommended per screen** shows the defaults in the table.
- **Random per screen** chooses from that screen's short list when you enter it. It excludes the previous choice for that screen, so consecutive visits vary. **Show another** explicitly chooses a different eligible illustration for comparison.
- **Manual** enables **Manual decoration**, letting you compare any supported cutout on all five screen concepts.

Random mode is not a timed slideshow. The image stays fixed while you remain on a screen, including throughout gameplay. Clicking the already-active tab, resizing, changing the aspect ratio, or pausing/resuming background motion does not reroll the artwork. Manual selection is available when you want a fixed favorite.

The full painted title scenes (Picnic rehearsal and Original meadow) already contain their characters and retain them in every mode. Select **Animated character scene** to try recommended/random/manual title decorations. Show another is disabled for a painted title. Returning to the animated title in Random mode makes a fresh eligible choice. The original painted song-selection background remains a standalone candidate; the interactive selection concept shows the chosen cutout itself.

Direct preview links can use `?choice=random`, `?choice=recommended`, or `?choice=manual&art=12-acoustic-duet`. Existing `?art=...` links select Manual mode unless an explicit choice mode is supplied.

## Native theme implementation

The default [Chiikawa theme](../../../Themes/chiikawa/README.md) uses the same pools in [artwork-pools.json](artwork-pools.json) for its random decorations; the title pool is retained only for gallery comparisons. The gallery now defaults to Random per screen.

The five new scenes add piano, drums, violin, flute, and saxophone. All five join the gallery title pool and native song selection. Piano, violin, and flute also join gameplay and the replay browser. Drums and saxophone join evaluation. The first entry of each pool remains its recommended preview choice.

Native random selection runs in the decoration's `OnCommand`, when its screen becomes visible. Cached title returns restart the dance loop. A private random generator and last-choice table avoid immediate repeats without consuming the engine's random sequence. Gameplay keeps its texture through rate changes and pauses; replay restart retains the same choice. No selection runs from update, judgment, or wheel-change events.

The random policy and artwork need only Lua and PNG files. The replay browser uses the new optional actor styling hooks in the engine, so its matching layout requires a build containing this revision. Other themes retain their original replay appearance.

Native gameplay art is shown for one human player using a non-centered one-side style with at most four columns. It moves to the unused side for P2. Two-player, centered, double/shared-side, demonstration, and wider-column fields omit it. Single-song evaluation uses the unused grade area; course/summary layouts retain their inherited presentation. Returning from a replay to the existing browser preserves its current decoration; closing and reopening the browser picks again. The last-choice memory lasts until theme reload or game exit.

Recommended and Manual are gallery comparison controls. The playable theme uses Random per screen outside the animated title menu.
