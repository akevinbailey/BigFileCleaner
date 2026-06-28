$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$repositoryDir = Split-Path -Parent $PSScriptRoot
$buildDir = Join-Path -Path $repositoryDir -ChildPath "cmake-build-release-visualstudio"
$releaseDir = Join-Path -Path $buildDir -ChildPath "Release"

& (Join-Path -Path $PSScriptRoot -ChildPath "copy_and_package.ps1") `
    -SourceDir $releaseDir `
    -DestDir (Join-Path -Path $buildDir -ChildPath "temp") `
    -ExecutableName "BigFileCleaner.exe" `
    -ExtraPaths @(
        "icons",
        "imageformats",
        "platforms",
        "styles",
        "Qt6Core.dll",
        "Qt6Gui.dll",
        "Qt6Widgets.dll"
    ) `
    -ArchivePath (Join-Path -Path $buildDir -ChildPath "BigFileCleaner_win_x64.zip")
