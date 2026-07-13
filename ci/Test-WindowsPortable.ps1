[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string] $PortableRoot,
    [ValidateRange(5, 300)]
    [int] $TimeoutSeconds = 60
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$portablePath = (Resolve-Path -LiteralPath $PortableRoot).Path
$programPath = Join-Path $portablePath "Program"
$executablePath = Join-Path $programPath "StepMania.exe"

$requiredFiles = @(
    "Portable.ini",
    "Program/StepMania.exe",
    "Program/StepMania.vdi",
    "Program/avcodec-55.dll",
    "Program/avformat-55.dll",
    "Program/avutil-52.dll",
    "Program/swscale-2.dll",
    "Program/parallel_lights_io.dll",
    "Data/splash.png",
    "Songs/instructions.txt"
)

$requiredDirectories = @(
    "Announcers",
    "Data",
    "NoteSkins",
    "Scripts",
    "Songs/StepMania 5",
    "Themes/_fallback",
    "Themes/default"
)

$missing = [System.Collections.Generic.List[string]]::new()
foreach ($relativePath in $requiredFiles) {
    $candidate = Join-Path $portablePath $relativePath
    if (-not (Test-Path -LiteralPath $candidate -PathType Leaf)) {
        $missing.Add($relativePath)
    }
}
foreach ($relativePath in $requiredDirectories) {
    $candidate = Join-Path $portablePath $relativePath
    if (-not (Test-Path -LiteralPath $candidate -PathType Container)) {
        $missing.Add("$relativePath/")
    }
}

if ($missing.Count -gt 0) {
    throw "Portable layout is missing required paths:`n - $($missing -join "`n - ")"
}

# Validate the executable without relying on a Visual Studio environment in PATH.
$stream = [System.IO.File]::OpenRead($executablePath)
$reader = [System.IO.BinaryReader]::new($stream)
try {
    if ($reader.ReadUInt16() -ne 0x5a4d) {
        throw "StepMania.exe does not have an MZ executable header."
    }

    $stream.Position = 0x3c
    $peOffset = $reader.ReadInt32()
    if ($peOffset -lt 0x40 -or $peOffset -gt ($stream.Length - 6)) {
        throw "StepMania.exe has an invalid PE header offset: $peOffset"
    }

    $stream.Position = $peOffset
    if ($reader.ReadUInt32() -ne 0x00004550) {
        throw "StepMania.exe does not have a PE signature."
    }

    $machine = $reader.ReadUInt16()
    if ($machine -ne 0x8664) {
        throw ("StepMania.exe is not an x64 PE image (machine 0x{0:x4})." -f $machine)
    }
}
finally {
    $reader.Dispose()
    $stream.Dispose()
}

Write-Host "Portable layout and x64 PE validation passed."

$smokeXmlPath = Join-Path $portablePath "Lua.xml"
$runtimeGeneratedPaths = @(
    $smokeXmlPath,
    (Join-Path $portablePath "Cache"),
    (Join-Path $portablePath "Logs"),
    (Join-Path $portablePath "Save"),
    (Join-Path $portablePath "Screenshots"),
    (Join-Path $portablePath "crashinfo.txt")
)

foreach ($generatedPath in $runtimeGeneratedPaths) {
    if (Test-Path -LiteralPath $generatedPath) {
        throw "The freshly staged portable tree is not clean: $generatedPath already exists."
    }
}

# Avoid creating a GUI splash window in the hosted runner. Because Portable.ini
# is present, this preference file is also evidence that user state is being
# read from the staged tree instead of the runner profile.
$smokeSavePath = Join-Path $portablePath "Save"
New-Item -ItemType Directory -Path $smokeSavePath -Force | Out-Null
[System.IO.File]::WriteAllText(
    (Join-Path $smokeSavePath "Preferences.ini"),
    "[Options]`nShowLoadingWindow=0`n",
    [System.Text.UTF8Encoding]::new($false)
)

$process = $null
try {
    $process = Start-Process `
        -FilePath $executablePath `
        -ArgumentList "--ExportLuaInformation" `
        -WorkingDirectory $portablePath `
        -PassThru

    if (-not $process.WaitForExit($TimeoutSeconds * 1000)) {
        $process.Kill($true)
        $process.WaitForExit()
        throw "StepMania runtime smoke test timed out after $TimeoutSeconds seconds."
    }

    if ($process.ExitCode -ne 0) {
        throw "StepMania runtime smoke test exited with code $($process.ExitCode)."
    }

    if (-not (Test-Path -LiteralPath $smokeXmlPath -PathType Leaf)) {
        throw "StepMania exited successfully but did not create Lua.xml."
    }

    [xml] $luaDocument = Get-Content -LiteralPath $smokeXmlPath -Raw
    $namespaceUri = "http://www.stepmania.com"
    $namespaceManager = [System.Xml.XmlNamespaceManager]::new($luaDocument.NameTable)
    $namespaceManager.AddNamespace("sm", $namespaceUri)
    $versionNode = $luaDocument.SelectSingleNode("/sm:Lua/sm:Version", $namespaceManager)
    if (
        $luaDocument.DocumentElement.LocalName -ne "Lua" `
        -or $luaDocument.DocumentElement.NamespaceURI -ne $namespaceUri `
        -or $null -eq $versionNode `
        -or [string]::IsNullOrWhiteSpace($versionNode.InnerText)
    ) {
        throw "The generated Lua.xml has an unexpected schema or no populated Version element."
    }

    Write-Host "Runtime smoke test passed: $($versionNode.InnerText)"
}
finally {
    if ($null -ne $process -and -not $process.HasExited) {
        $process.Kill($true)
        $process.WaitForExit()
    }

    # Keep transient smoke-test state out of the distributed portable archive.
    foreach ($generatedPath in $runtimeGeneratedPaths) {
        if (Test-Path -LiteralPath $generatedPath) {
            Remove-Item -LiteralPath $generatedPath -Recurse -Force
        }
    }
}
