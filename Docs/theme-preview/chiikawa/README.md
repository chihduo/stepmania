# Chiikawa theme results

Chiikawa is the default StepMania 6 theme for new installations. The original appearance remains selectable as **Classic**; existing saved theme choices are preserved. This branch contains the finished [theme](../../../Themes/chiikawa/README.md), its playable artwork and movie, user documentation and actual engine captures.

- [Transparent title-menu dance preview](../../media/title-dance.gif)
- [Current gameplay, evaluation and replay captures](../../media/README.md)
- [Installation, controls and customization](../../../Themes/chiikawa/README.md)
- [Native title, selection, gameplay, evaluation and replay screenshots](native/README.md)
- [Included character artwork](../../../Themes/chiikawa/Art/README.md)
- [30-second silent dance movie](../../../Themes/chiikawa/Movies/ddr-trio-loop.mp4)
- [Released asset hashes and provenance](release-manifest.json)
- [Per-screen illustration pools](artwork-pools.json)

The title menu shows all three characters dancing on transparent cartoon pads in a 3.2-second loop. **Fancy UI Background → Off** freezes the dancers and menu motion. Song selection, single-song evaluation and the replay browser keep their curated random pools. Gameplay and replay playback add no extra character cutouts over song images, song movies or fallback media. The title/game entries in the pool file are retained for older gallery comparisons.

The gameplay movie is 1280×720 at 24 fps, with 720 video frames and no audio track. Song-supplied backgrounds keep their normal precedence, and static mode uses the matching poster. The theme preserves the original fonts, notes, judgments and gameplay features.

The browser comparison gallery, alternative illustrations, generation prompts, original video clips, assembly recipe and detailed development journals live on **`feature/chiikawa-theme`** at the same `Docs/theme-preview/chiikawa/` path. See the [branch workflow](../../ChiikawaDevelopment.md) to open that workspace or promote a finished revision. Published history is retained.

These are AI-generated fan illustrations and movies, not official Chiikawa artwork. Native verification uses Linux software OpenGL and null audio; Windows x64 playback, physical controllers and audible synchronization remain to be checked.
