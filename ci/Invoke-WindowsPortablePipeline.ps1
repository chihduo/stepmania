[CmdletBinding()]
param(
    [string] $BuildDirectory = "build/windows-x64",
    [string] $OutputDirectory = "out",
    [ValidateSet("Release")]
    [string] $Configuration = "Release",
    [ValidateRange(1, 64)]
    [int] $Parallel = 2
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$repoRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot ".."))

function Get-RepositoryOutputPath {
    param(
        [Parameter(Mandatory = $true)]
        [string] $Path
    )

    $fullPath = if ([System.IO.Path]::IsPathRooted($Path)) {
        [System.IO.Path]::GetFullPath($Path)
    }
    else {
        [System.IO.Path]::GetFullPath((Join-Path $repoRoot $Path))
    }

    $rootPrefix = $repoRoot.TrimEnd(
        [System.IO.Path]::DirectorySeparatorChar,
        [System.IO.Path]::AltDirectorySeparatorChar
    ) + [System.IO.Path]::DirectorySeparatorChar

    if (-not $fullPath.StartsWith(
        $rootPrefix,
        [System.StringComparison]::OrdinalIgnoreCase
    )) {
        throw "Pipeline output paths must remain inside the repository: $fullPath"
    }

    return $fullPath
}

function Invoke-ExternalCommand {
    param(
        [Parameter(Mandatory = $true)]
        [string] $FilePath,
        [Parameter(Mandatory = $true)]
        [string[]] $ArgumentList
    )

    Write-Host "> $FilePath $($ArgumentList -join ' ')"
    & $FilePath @ArgumentList
    if ($LASTEXITCODE -ne 0) {
        throw "$FilePath exited with code $LASTEXITCODE."
    }
}

$cmake = Get-Command cmake -ErrorAction Stop
$buildPath = Get-RepositoryOutputPath -Path $BuildDirectory
$outputPath = Get-RepositoryOutputPath -Path $OutputDirectory
$stagingPath = Join-Path $outputPath "StepMania"
$archivePath = Join-Path $outputPath "StepMania-5.1-windows-x64-portable.zip"
$checksumPath = "$archivePath.sha256"

New-Item -ItemType Directory -Path $buildPath -Force | Out-Null
New-Item -ItemType Directory -Path $outputPath -Force | Out-Null

if (Test-Path -LiteralPath $stagingPath) {
    Remove-Item -LiteralPath $stagingPath -Recurse -Force
}
if (Test-Path -LiteralPath $archivePath) {
    Remove-Item -LiteralPath $archivePath -Force
}
if (Test-Path -LiteralPath $checksumPath) {
    Remove-Item -LiteralPath $checksumPath -Force
}

Invoke-ExternalCommand -FilePath $cmake.Source -ArgumentList @(
    "-S", $repoRoot,
    "-B", $buildPath,
    "-G", "Visual Studio 17 2022",
    "-A", "x64",
    "-DWITH_FULL_RELEASE=OFF",
    "-DWITH_IRC_POST_HOOK=OFF",
    "-DWITH_STATIC_LINKING=ON"
)

Invoke-ExternalCommand -FilePath $cmake.Source -ArgumentList @(
    "--build", $buildPath,
    "--config", $Configuration,
    "--parallel", "$Parallel"
)

Invoke-ExternalCommand -FilePath $cmake.Source -ArgumentList @(
    "--install", $buildPath,
    "--config", $Configuration,
    "--prefix", $stagingPath
)

# StepMania switches to self-contained user data when this marker exists.
Set-Content -LiteralPath (Join-Path $stagingPath "Portable.ini") -Value @(
    "; This file enables StepMania portable mode."
) -Encoding Ascii

& (Join-Path $PSScriptRoot "Test-WindowsPortable.ps1") `
    -PortableRoot $stagingPath

Push-Location $outputPath
try {
    Compress-Archive `
        -LiteralPath (Split-Path $stagingPath -Leaf) `
        -DestinationPath $archivePath `
        -CompressionLevel Optimal
}
finally {
    Pop-Location
}

$archiveHash = (Get-FileHash -LiteralPath $archivePath -Algorithm SHA256).Hash.ToLowerInvariant()
$checksumLine = "$archiveHash  $(Split-Path $archivePath -Leaf)`n"
[System.IO.File]::WriteAllText(
    $checksumPath,
    $checksumLine,
    [System.Text.UTF8Encoding]::new($false)
)

$archiveSize = (Get-Item -LiteralPath $archivePath).Length
Write-Host "Portable archive: $archivePath"
Write-Host "Archive size: $archiveSize bytes"
Write-Host "SHA-256: $archiveHash"

if ($env:GITHUB_STEP_SUMMARY) {
    $summary = @"
## Windows portable build

- Build: MSVC 2022, x64, static C/C++ runtime, Release
- Automated test: portable layout, PE architecture, and runtime Lua export smoke test
- Archive: $(Split-Path $archivePath -Leaf)
- SHA-256: $archiveHash
"@
    $summary | Add-Content -LiteralPath $env:GITHUB_STEP_SUMMARY -Encoding UTF8
}
