# Adjusting playback speed during a song

Hold either Shift key and press an arrow:

| Shortcut | Playback-rate change |
| --- | --- |
| Shift + Up or Shift + Right | +0.1x |
| Shift + Down or Shift + Left | -0.1x |
| Shift + Up + Down or Shift + Left + Right | Set to 1.0x |

For example, tapping and releasing Up, Down, Down while holding Shift changes 1.0x to 1.1x, then 1.0x, then 0.9x. Each new arrow press changes the rate once. Holding an arrow does not repeat the adjustment. The rate is limited to 0.2x through 3.0x. The latest multiplier (for example, `1.1x`) pops up at the bottom-left with the theme's W1/FLAWLESS color and glow, then remains visible for the rest of the song. The default theme uses its bold italic font. Each adjustment immediately replaces the value and restarts the animation.

To reset, hold either Shift and press both arrows in an opposite pair. The reset takes effect when the second arrow is pressed, in either order. Holding or releasing the pair makes no further changes. This sets the current play to exactly 1.0x even if its original rate was different; the original setting still returns when the play ends.

Release Shift to continue playing at the selected rate. Arrows pressed without Shift remain normal note inputs. An arrow used for a shortcut remains excluded from note inputs until it is released, even if Shift is released first. Dance pads and other separately mapped keys continue to work.

Notes, music, and standard song videos follow the same song clock. CMod and MMod temporarily retain their original note spacing so visible note movement changes with playback speed. Saved scrolling settings, preferred music rate, and song files are unchanged.

The original settings return after finishing, failing, cancelling, restarting, or advancing to the next song in a course. The speed display clears when the song or gameplay screen ends and on retry/course advance. Changing the rate marks the whole stage as practice, including a course containing that song. Practice plays do not save normal high scores, replays, or score exports, even if the rate is changed back to its original value. Ordinary play-count and activity totals still accumulate.

The shortcuts work during active local gameplay. They do not change the rate while paused, in the editor, in demonstrations, in the calibration screen, or in a network session. During manual rate control, Haste is overridden and autosync sampling is suspended; normal song settings resume afterward.

## Developer checks

Run the focused regression checks from the repository root:

```sh
g++ -std=c++11 -Wall -Wextra -Werror -pedantic -Isrc src/tests/test_gameplay_rate.cpp -o /tmp/test_gameplay_rate
/tmp/test_gameplay_rate
```

After building with the Unix Makefiles generator and `CMAKE_EXPORT_COMPILE_COMMANDS=ON`, run the tests against the compiled engine objects:

```sh
python3 src/tests/run_gameplay_rate_engine.py build/live-rate
```

These checks use a temporary filesystem and verify modifier restoration, both Shift keys, each arrow, both reset pairs in either order, controller inputs sharing a column, original-rate restoration after a reset, and practice-score aggregation. They do not open a game window or change saved user settings.

Check the bottom-left display after rapid adjustments and reset chords, including its font and glow. Wait several seconds after an adjustment and confirm the number stays visible, including after resetting to 1.0x; it should clear when the song ends or restarts. Also check an actual song with a video. Verify audio/video alignment while switching between 0.9x, 1.0x, and 1.1x and resetting to 1.0x from either direction; ordinary arrows, holds, mines, and lifts; both Shift keys and release orders; XMod/CMod/MMod; and resetting on each exit/retry/course path. Custom song scripts that explicitly use their own real-time clocks need separate verification.
