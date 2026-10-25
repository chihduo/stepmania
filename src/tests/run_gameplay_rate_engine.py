#!/usr/bin/env python3
"""Run headless engine checks using an existing Unix Makefiles CMake build."""
import json
from pathlib import Path
import shlex
import subprocess
import sys
import tempfile

if len(sys.argv) != 2:
    sys.exit("Usage: run_gameplay_rate_engine.py <cmake-build-directory>")
build = Path(sys.argv[1]).resolve()
source = Path(__file__).with_name("test_gameplay_rate_engine.cpp").resolve()
commands = json.loads((build / "compile_commands.json").read_text())
entry = next(item for item in commands if Path(item["file"]).name == "Main.cpp")
link_file = build / "src/CMakeFiles/StepMania.dir/link.txt"
if not link_file.exists():
    sys.exit("Use a Unix Makefiles build with CMAKE_EXPORT_COMPILE_COMMANDS=ON.")

with tempfile.TemporaryDirectory(prefix="gameplay-rate-test-", dir=build) as temporary:
    fixture = Path(temporary)
    obj = fixture / "test.o"
    binary = fixture / "test"
    args = shlex.split(entry["command"])
    args[args.index("-o") + 1] = str(obj)
    args[args.index("-c") + 1] = str(source)
    args.append("-UNDEBUG")
    subprocess.run(args, cwd=entry["directory"], check=True)
    args = shlex.split(link_file.read_text())
    args = [arg for arg in args if not arg.startswith("-Wl,--dependency-file=")]
    main = next(i for i, arg in enumerate(args) if arg.endswith("/Main.cpp.o"))
    args[main] = str(obj)
    args[args.index("-o") + 1] = str(binary)
    subprocess.run(args, cwd=build / "src", check=True)
    root = fixture / "root"
    root.mkdir()
    subprocess.run([str(binary), str(root)], cwd=root, check=True)
