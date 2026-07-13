# Windows portable pipeline

The repository's `Windows portable build and test` GitHub Actions workflow
creates a self-contained, unsigned Windows x64 build. It does not create or
use an installer.

## Pipeline stages

The single Windows job performs these stages in order:

1. Configure a Release build with Visual Studio 2022, x64, and the static
   C/C++ runtime.
2. Compile StepMania and its bundled libraries.
3. Install the executable, runtime DLLs, themes, noteskins, songs, and data into
   a clean staging tree.
4. Add `Portable.ini`, which keeps user data inside the extracted tree.
5. Validate the required portable layout and the executable's x64 PE headers.
6. Launch `StepMania.exe --ExportLuaInformation` with a 60-second timeout and
   require a zero exit code plus valid generated XML.
7. Remove smoke-test state, create a ZIP archive, calculate its SHA-256 digest,
   and upload both files as one workflow artifact.

The smoke test covers executable and DLL loading, portable filesystem mounting,
preferences, Lua, themes, and early Windows initialization. StepMania has no
registered CTest/unit-test suite. Graphics, audio, input devices, timing, and
gameplay still require manual validation in the Windows 11 guest.

## Running the workflow

The workflow runs for pushes and pull requests. It can also be started from the
GitHub Actions page with `workflow_dispatch`.

Download the artifact named `StepMania-windows-x64-<run>.<attempt>`. It contains:

- `StepMania-5.1-windows-x64-portable.zip`
- `StepMania-5.1-windows-x64-portable.zip.sha256`

Extract the ZIP in the Windows 11 guest and launch
`StepMania/Program/StepMania.exe`. Keep `Portable.ini` at the root of the
extracted `StepMania` directory.

## Reusing the same pipeline script on Windows

On a Windows machine with Visual Studio 2022 C++ tools, CMake, Git, and
PowerShell 7 available, run:

```powershell
pwsh -File ci/Invoke-WindowsPortablePipeline.ps1
```

Generated files are placed in `build/windows-x64` and `out`; both directories
are ignored by Git.
