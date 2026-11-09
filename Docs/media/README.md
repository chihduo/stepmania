# Gameplay media

These gameplay, evaluation and replay captures show the **current default Chiikawa theme**, recorded on **2026-11-09** at **1280 × 720** on Linux x86_64. Original notes, judgments, speed text and the life graph appear over the silent dance movie. PNGs are unedited native captures; the gameplay GIF and MP4 are resized encodings of the same play.

| File | Size and duration | Purpose |
| --- | --- | --- |
| [gameplay.png](gameplay.png) | 1280 × 720 | Movie background, persistent 0.9x speed and a life-history dip. |
| [evaluation.png](evaluation.png) | 1280 × 720 | Cream evaluation layout, paired decoration and completed life history. |
| [replay.png](replay.png) | 1280 × 720 | The same play viewed as a replay, with the movie and recorded 1.2x speed. |
| [live-speed-demo.gif](live-speed-demo.gif) | 800 × 450; 12.01 seconds; 8.1 MB | Slowing to 0.9x, resetting to 1.0x and increasing to 1.1x. |
| [live-speed-demo.mp4](live-speed-demo.mp4) | 960 × 540; 40 seconds; 4.3 MB | Silent recording of increases, decreases and both reset chords. |
| [title-dance.gif](title-dance.gif) | 660 × 480; 3.2 seconds | Transparent illustration preview rendered from the native sprite poses and Lua motion. |

[Actual title menu](../theme-preview/chiikawa/native/title.png) · [More native screens](../theme-preview/chiikawa/native/README.md) · [Media hashes and capture metadata](manifest.json)

## Capture setup

- **Theme:** runtime revision `fdd9180efa`, including the dancing title, Chiikawa defaults and Classic label, selectively promoted from the feature branch.
- **Engine:** existing Linux debug build `StepMania5.1-git-09b8817104` dated 2026-11-08, with FFmpeg decoding. Its engine sources match this theme-only revision. The inherited version string remains visible in the title capture.
- **Rendering:** Xvfb, Mesa software OpenGL and Null audio. Gameplay/replay use the theme movie at **45% brightness** because the chart has no background. Engine and theme files were not modified for capture.
- **Chart:** an original generated **120 BPM, 32-measure dance-single** chart, **Live Speed Demo**, with taps and a synthetic WAV pulse track. No external song pack is used.
- **Play:** one joined player, **1.5x note scroll**, FailOff, replay saving and built-in autoplay. Two brief F8 interruptions create life-history dips. The result is 122 Flawless judgments, six misses, max combo 59 and score 97,638,084.
- **Controls:** actual X11 XTest Shift+arrow events. The saved replay confirms `1.0 → 1.1 → 1.2 → 1.1 → 1.0 → 0.9 → 1.0 → 1.1 → 1.0`, including both reset chords.
- **Isolation:** a separate portable directory contains the synthetic chart, settings, logs and replay; no personal profile is used. Source video, scripts and logs remain under ignored `out/readme-chiikawa/` in the feature workspace.

The evaluation's **1.5x** is the note-scroll modifier, separate from live playback speed. The GIF covers seconds 18–30 at nominal eight frames per second; centisecond delay rounding gives 12.01 seconds. The silent MP4 retains the first 40 seconds at 24 frames per second.

## Replay screenshot

`replay.png` shows the actual viewer opened with **F10** from the captured evaluation. It plays the same completed recording, with the saved judgments, speed timeline and life history. Its **REPLAY** label distinguishes it from a new play. The GIF/MP4 show the original play, not replay viewing.

## Verification and limits

PNGs match their raw captures. The gameplay GIF has 96 decoded frames; the MP4 fully decodes to 960 H.264 frames with no audio. Native logs confirm FFmpeg loads the dance movie in gameplay and replay, with no Lua errors. All screenshots and representative slowdown/reset frames were inspected.

The title GIF and [4:3 title capture](../theme-preview/chiikawa/native/title-4x3.png) were completed on 2026-11-08. The GIF is an isolated transparent animation preview; the title PNGs show the actual menu. Artwork sources and assembly tools remain on the [development branch](../ChiikawaDevelopment.md).

Null audio and software rendering do not verify audible synchronization, physical controllers or Windows x64 behavior. Autoplay's usual score restrictions apply, so evaluation is not evidence of a saved human score. Earlier functional checks are recorded in [the progress file](../../PROGRESS.md).

Theme, noteskin and generated fan-art credits remain in their guides and [license notices](../Licenses.txt). Earlier Classic-theme captures remain in Git history.
