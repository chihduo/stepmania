# Native Chiikawa theme captures

These are actual StepMania screens rendered on Linux on 2026-11-08, showing the [finished theme](../README.md). Captures use the isolated synthetic Live Speed Demo chart and test profiles, software OpenGL, and null audio. Gameplay uses autoplay to exercise layout and the live rate display. The refreshed gameplay capture uses a background-free copy of the chart, the new silent dance movie, 45% background brightness, and 1.1x playback speed. It was rendered with the engine's FFmpeg movie decoder enabled; other captures retain their original fixture. The engine version string remains its inherited StepMania 5.1 build identifier.

![Title with the flute-practice illustration](title.png)

![P1 song selection with a flute duet and the unused P2 difficulty pane hidden](selection.png)

![Silent dancing-trio background beneath original notes, judgment, graph and persistent speed display](gameplay.png)

Gameplay with the **dancing background movie**, captured at 1.1x speed and 45% background brightness. [Watch the 30-second silent background movie](../../../../Themes/chiikawa/Movies/ddr-trio-loop.mp4).

![4:3 evaluation with a celebration beside the results and readable graph labels](evaluation-4x3.png)

![4:3 replay browser with the violin illustration](replay-browser-4x3.png)

The screenshots are unedited native captures. Random choices differ on future visits. The playable theme inherits the original functional panels, notes, fonts, and judgments. Windows x64, physical controller input, and audible media synchronization need checking on the next Windows build; these captures do not verify them.

[Install and customize the theme](../../../../Themes/chiikawa/README.md). Extended native captures and test logs remain under ignored `out/chiikawa-native/` in the development workspace. The selection capture was refreshed through normal title-menu navigation after correcting inactive-player pane visibility; its layout and player-join checks are under `out/chiikawa-pane-fix/`.

The dance-movie capture and native movie verification logs are retained under ignored `out/chiikawa-movie/`. [Preview the full silent loop](../../../../Themes/chiikawa/Movies/ddr-trio-loop.mp4). Its source clips and assembly notes are retained on the [development branch](../../../ChiikawaDevelopment.md).
