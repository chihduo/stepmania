# Single-player life-history graph

The default theme displays the life/health history at the bottom-right during gameplay and on evaluation. It uses the older theme's grid and green/yellow/red fill, with a white line showing life from empty to full. The percentage above the plot is the latest recorded life.

During gameplay, recorded life changes update the plot immediately. Its horizontal range grows from the start of play to the current elapsed play time. The entire history stays in view, so earlier events move closer together as the song progresses. Evaluation shows the completed history; evaluation summary uses the statistics selected by that screen.

The graph follows the engine's elapsed-play clock. Changing playback speed does not stretch or rewind existing records. Failure records and the engine's gaps between accumulated stages are included in evaluation's time range.

The display is enabled automatically for exactly one human player and one enabled player, including P2-only play. During gameplay, the panel narrows if the note field leaves less space at the right edge. Evaluation keeps the full panel width, and P2-only modifier text moves to the otherwise unused left side. Two-player and demonstration screens omit the graph. Endless courses omit it because the engine does not retain their life histories.

## In-game verification

Use a build containing this change and select the **default** theme.

1. Start a song with only P1 joined. Find the LIFE plot at the bottom-right, above the bottom status area. Hit some notes and miss others; the rightmost point and percentage should follow the life meter immediately.
2. Adjust playback speed with Shift+arrows. Confirm that life history continues smoothly and elapsed time moves forward. If using a pause control, confirm the plot's time range stops advancing while paused.
3. Finish the song. Confirm evaluation shows the completed curve in the same corner. Also check a failed song for the final drop to zero.
4. Start another song or retry. Confirm a fresh play starts with fresh history. A finite course should retain its continuous course history, and final evaluation summary should show its selected accumulated stages.
5. Repeat with only P2 joined, checking both 16:9 and 4:3 layouts. The gameplay panel should fit beside the ordinary note lanes; evaluation's modifier text should appear on the left, clear of the graph.
6. Join both players. Confirm neither gameplay nor evaluation displays this additional graph.

Interactive appearance and Windows gameplay verification remain pending; the local automated checks do not render the screens.

## Theme settings and automated checks

The `[LifeHistory]` section of `Themes/default/metrics.ini` controls the preferred width, height, and right/bottom margins. The shared actor is `Themes/default/Graphics/LifeHistory graph.lua`. It samples 128 points and reuses fixed-size vertex arrays, avoiding the legacy graph actor's repeated child creation.

Run the focused Lua checks from the repository root with Lua 5.1:

```sh
lua5.1 src/tests/test_life_history.lua
```

The real-engine checks include the life-history timestamp API and inclusive Lua sampling endpoint, alongside the existing rate-control checks. After building the application with the existing Unix Makefiles CMake setup:

```sh
python3 src/tests/run_gameplay_rate_engine.py build/live-rate
```
