# Gameplay media

These files are actual captures of this fork's default theme, recorded on **2026-11-07** at **1280 × 720** on Linux x86_64. The gameplay engine and theme render the notes, judgments, speed number, life history, and evaluation screen. The GIF and MP4 are trimmed/resized encodings of that recording.

| File | Size and duration | Purpose |
| --- | --- | --- |
| [gameplay.png](gameplay.png) | 1280 × 720 | Gameplay with the current speed and life-history graph beneath the notes. |
| [replay.png](replay.png) | 1280 × 720 | Recorded gameplay being watched, including judgments, a roll, the life graph, and the recorded 1.2x speed. |
| [evaluation.png](evaluation.png) | 1280 × 720 | The completed life history on the evaluation screen. |
| [live-speed-demo.gif](live-speed-demo.gif) | 800 × 450; 17 seconds; 2.9 MB | Inline preview of slowing down and resetting to 1.0x. |
| [live-speed-demo.mp4](live-speed-demo.mp4) | 960 × 540; 52 seconds; 2.3 MB | Silent recording of increases, decreases, and both reset chords. |

## Capture setup

This setup describes the live-speed gameplay/evaluation images and GIF/MP4. The replay screenshot has its own capture notes below.

- **Build:** Linux Debug from source [`ed202f9327`](https://github.com/chihduo/stepmania/commit/ed202f932724d8b8f3124887c951f6dee71b09bc). Its gameplay code is the same as feature revision [`0535f3e9d9`](https://github.com/chihduo/stepmania/commit/0535f3e9d9b5ae3470d43d6b5d3ed4d778133e63); intervening commits update documentation.
- **Rendering:** Xvfb and Mesa software OpenGL, with the bundled default theme and dance noteskin. Game code and theme files were not modified for the recording.
- **Song:** an original generated 120 BPM, 32-measure dance-single chart named **Live Speed Demo**, with a synthetic WAV pulse track and a plain gradient background. No external song pack is needed for these media.
- **Play:** one joined player, 1.5x note-scroll modifier, FailOff, and the built-in autoplay controller. Two brief pauses in autoplay produce the visible dips in life. The on-screen **AutoPlay** label remains visible.
- **Controls:** actual Shift+arrow keyboard events sent through X11's XTest extension. The longer video shows Up/Right increases, Down/Left decreases, and both opposite-arrow reset chords.
- **Isolation:** a separate portable game directory holds the demo chart, preferences, logs, and generated state. The capture does not use a personal player profile.

The **1.5x** shown in evaluation is the note-scroll modifier; it is separate from the live playback multiplier at the bottom-left during gameplay. Live playback starts at 1.0x and changes during the recording.

The capture is silent and uses the engine's Null sound driver. It demonstrates the rendered controls and graph, but does not verify audible synchronization, video-background synchronization, or Windows device behavior. The demo uses autoplay, whose usual score restrictions still apply; its evaluation image is not evidence of a saved human score. Normal score-saving checks and their limits are recorded in [the progress file](../../PROGRESS.md).

The underlying StepMania theme and noteskin retain their original credits and [license notices](../Licenses.txt). The chart, pulse track, and neutral background were generated for this demonstration.


## Replay screenshot

`replay.png` was captured from Linux Debug build `b21530a9ad2398c86062146e8ab903a4ab5a2eb4` on the same date, using the actual replay viewer and default theme. It replays a 12-measure variant of the generated 120 BPM chart, with holds, rolls, mines, and lifts. The original recording used autoplay with brief misses and live speed changes; the viewer applies those recorded outcomes. The REPLAY label distinguishes it from a fresh play, and viewing does not create another score.

The screenshot is an unedited 1280 × 720 capture from the isolated `out/replay-tests/` fixture. The song uses the same original WAV/background assets described above. Null audio and disabled video decoding mean this screenshot verifies rendering, not audible/video synchronization or Windows behavior. The existing GIF/MP4 demonstrate live speed controls, not replay playback.
