# Dancing-trio gameplay background

The three supplied movies are now combined into a **30-second silent loop** and installed as the Chiikawa theme's missing-song-art background.

[Preview the combined movie (MP4, 12.5 MB)](../../../../Themes/chiikawa/Movies/ddr-trio-loop.mp4) · [Native gameplay screenshot](../native/gameplay.png) · [Theme settings and installation](../../../../Themes/chiikawa/README.md#dancing-trio-gameplay-background)

![First-frame poster from the installed dance movie](../../../../Themes/chiikawa/Graphics/Common%20fallback%20background.png)

## Movie and sources

The installed MP4 contains exactly **720 frames, 1280×720, 24 fps, 30.000 seconds**, encoded as H.264 High/yuv420p. It has one video stream and **no audio streams**. Video packets are copied without re-encoding, and all decoded frame hashes match the source clips in this order:

1. [Characters_dancing_on_DDR_pads_67d4dfe6.mp4](Characters_dancing_on_DDR_pads_67d4dfe6.mp4)
2. [Characters_dancing_on_DDR_pads_9e35d8f4.mp4](Characters_dancing_on_DDR_pads_9e35d8f4.mp4)
3. [Characters_dancing_on_DDR_pads_d0d6f534.mp4](Characters_dancing_on_DDR_pads_d0d6f534.mp4)

Each source has 240 video frames / 10 seconds. Explicit ten-second segment durations exclude the slightly longer AAC audio tails. The shared opening/closing composition allows direct joins; small encoding/render differences remain between endpoint frames. The original uploads, including their original audio, are retained unchanged in this folder. Only the combined runtime movie is silent.

Rebuild from the repository root with Python 3, FFmpeg and ffprobe installed:

```sh
python3 Docs/theme-preview/chiikawa/dance-movie/build-loop.py
```

The [recipe](build-loop.py) assembles the MP4, verifies all frames and the absence of audio, and extracts its first frame as the common fallback PNG. [movie-manifest.json](movie-manifest.json) records source order, hashes, dimensions, codec and output metadata. The user generated the source videos externally; their generation service and exact per-clip prompts were not provided.

## Gameplay behavior

The optional `Background/FallbackMovie` theme metric substitutes this MP4 only when the normal `StretchNormal` effect receives the common fallback poster. The engine keeps its usual music clock, live rate handling, looping, pause, brightness and draw order. Song artwork and scripted background changes retain their precedence. Static-background mode or disabled song backgrounds uses the poster; thumbnails also use the poster. Songs without a background image omit the extra random character cutout.

The choreography is decorative. Playback follows the song clock, but a reusable dance loop cannot match every chart's steps or tempo. See the [theme guide](../../../../Themes/chiikawa/README.md) for settings and the two paths to copy when updating an existing portable installation.

## Keyframes and generation brief

The [center-tile keyframe](ddr-trio-keyframe-v2.png) places Chiikawa, Hachiware and Usagi on their own blank center tiles. It is an unchanged output of the built-in image editor. [manifest.json](manifest.json) preserves its dimensions, SHA-256 and previous-version provenance. These images and movies are AI-generated fan artwork, not official Chiikawa artwork.

For future variations, use the center-tile image for both the start and end frame. The [movie prompt](movie-prompt.txt) describes side steps, short runs across the pad, a two-foot jump and a complete spin returning to center, with vibrant arcade lighting and a smooth camera orbit. It was written for an eight-second generation; the three supplied clips are ten seconds each. Inspect foot travel and loop seams after generation.

The [image-edit prompt](keyframe-v2-prompt.txt), [original keyframe](ddr-trio-keyframe.png), [original image prompt](keyframe-prompt.txt), and [original movie prompt](movie-prompt-v1.txt) remain available for comparison.

## Verification

The combined file fully decodes and preserves all 720 source frames. A Linux native build using the engine's pinned FFmpeg version successfully loads and loops it. Checks cover live rate adjustment, gameplay/replay pause and resume, replay restart, own-art/scripted-video precedence, static/disabled-background settings, 16:9 and 4:3 layouts, and the unchanged default-theme fallback. Paused movie regions remain pixel-identical and advance after resume. The [updated gameplay screenshot](../native/gameplay.png) is an unedited engine capture using a truly background-free chart.

Windows x64 playback and audible synchronization remain unverified here. The native fixture uses software OpenGL and null audio. No Windows ZIP was built for this movie revision.
