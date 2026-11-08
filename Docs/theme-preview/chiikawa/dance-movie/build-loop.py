#!/usr/bin/env python3
"""Assemble the supplied DDR clips without re-encoding or carrying audio."""
from pathlib import Path
import hashlib
import json
import subprocess
import tempfile

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[3]
SOURCES = [HERE / ("Characters_dancing_on_DDR_pads_" + suffix + ".mp4")
           for suffix in ("67d4dfe6", "9e35d8f4", "d0d6f534")]
MOVIE = ROOT / "Themes/chiikawa/Movies/ddr-trio-loop.mp4"
POSTER = ROOT / "Themes/chiikawa/Graphics/Common fallback background.png"


def probe(path):
    return json.loads(subprocess.check_output([
        "ffprobe", "-v", "error", "-show_format", "-show_streams", "-of", "json", str(path)]))


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def frame_hashes(path):
    data = subprocess.check_output([
        "ffmpeg", "-v", "error", "-i", str(path), "-map", "0:v:0", "-f", "framemd5", "-"], text=True)
    return [line.rsplit(",", 1)[1].strip() for line in data.splitlines()
            if line and not line.startswith("#")]


def main():
    inputs = []
    for source in SOURCES:
        info = probe(source)
        video = next(s for s in info["streams"] if s["codec_type"] == "video")
        assert (video["codec_name"], video["width"], video["height"], video["avg_frame_rate"],
                int(video["nb_frames"])) == ("h264", 1280, 720, "24/1", 240), source
        assert abs(float(video["duration"]) - 10) < 0.0001, source
        inputs.append({"file": source.name, "sha256": digest(source), "video_seconds": 10,
                       "video_frames": 240, "audio_streams": sum(s["codec_type"] == "audio" for s in info["streams"])})
    MOVIE.parent.mkdir(parents=True, exist_ok=True)
    POSTER.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="chiikawa-loop-") as temporary:
        concat = Path(temporary) / "clips.ffconcat"
        # Explicit video durations avoid the 27 ms AAC tail in each source.
        concat.write_text("ffconcat version 1.0\n" + "".join(
            "file '" + str(p).replace("'", "'\\''") + "'\nduration 10.0\n" for p in SOURCES))
        subprocess.run(["ffmpeg", "-hide_banner", "-loglevel", "error", "-y", "-f", "concat",
                        "-safe", "0", "-i", str(concat), "-map", "0:v:0", "-c:v", "copy", "-an",
                        "-map_metadata", "-1", "-map_chapters", "-1", "-movflags", "+faststart",
                        "-video_track_timescale", "24000", str(MOVIE)], check=True)
    output = probe(MOVIE)
    assert len(output["streams"]) == 1 and output["streams"][0]["codec_type"] == "video"
    assert int(output["streams"][0]["nb_frames"]) == 720
    assert abs(float(output["format"]["duration"]) - 30) < 0.0001
    expected = [frame for p in SOURCES for frame in frame_hashes(p)]
    actual = frame_hashes(MOVIE)
    assert len(actual) == 720 and actual == expected, "Decoded video frames changed during assembly"
    subprocess.run(["ffmpeg", "-hide_banner", "-loglevel", "error", "-y", "-i", str(MOVIE),
                    "-frames:v", "1", "-update", "1", str(POSTER)], check=True)
    manifest = {"sources_in_playback_order": inputs,
                "movie": {"file": str(MOVIE.relative_to(ROOT)), "sha256": digest(MOVIE),
                          "bytes": MOVIE.stat().st_size, "duration_seconds": 30, "frames": 720,
                          "fps": 24, "size": [1280, 720], "codec": "h264", "profile": "High",
                          "pixel_format": "yuv420p", "audio_streams": 0},
                "poster": {"file": str(POSTER.relative_to(ROOT)), "sha256": digest(POSTER),
                           "source_frame": 0},
                "assembly": "Direct concatenation in listed order; video stream copy, explicit 10-second segments, all audio and source metadata excluded, faststart MP4",
                "validation": "All 720 decoded frame hashes match the three source clips in order; exactly one video stream, no audio, exactly 30 seconds",
                "rebuild": "python3 Docs/theme-preview/chiikawa/dance-movie/build-loop.py"}
    (HERE / "movie-manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
    print("Created", MOVIE.relative_to(ROOT), "(30.000 seconds, 720 unchanged frames, no audio)")
    print("Created", POSTER.relative_to(ROOT))


if __name__ == "__main__":
    main()
