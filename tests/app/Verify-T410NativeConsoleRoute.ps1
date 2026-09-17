param(
    [string]$RepositoryRoot = (Split-Path -Parent (Split-Path -Parent $PSScriptRoot))
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
function Read-TrackedText([string]$relativePath) {
    return Get-Content -LiteralPath (Join-Path $root $relativePath) -Raw
}

foreach ($relativePath in @(
    'src/app/presentation_window.c',
    'src/app/presentation_window.h',
    'tests/app/presentation_window_fixture.c',
    'tests/app/presentation_window_contract_fixture.c',
    'tests/app/presentation_window_graphics_stub.c',
    'tests/observation/presentation_mouse_automation_launcher.c'
)) {
    if (Test-Path -LiteralPath (Join-Path $root $relativePath)) {
        throw "T410 removed independent-window path is still present: $relativePath"
    }
}

$entry = Read-TrackedText 'src/app/entry.c'
if ($entry -match 'presentation_window|app_presentation_window') {
    throw 'T410 app entry still binds the independent presentation window.'
}

$appBuild = Read-TrackedText 'tools/build/New-T310OriginalSoftpcNinja.ps1'
if ($appBuild -match 'presentation_window\.c') {
    throw 'T410 formal product graph still selects presentation_window.c.'
}

$appSources = @(Get-ChildItem -LiteralPath (Join-Path $root 'src/app') -File -Filter '*.c')
foreach ($source in $appSources) {
    if ((Get-Content -LiteralPath $source.FullName -Raw) -match
            'WriteConsoleInput(?:W|A)?\s*\(') {
        throw "T410 app source still synthesizes Console input: $($source.Name)"
    }
}

$console = Read-TrackedText 'src/ntvdm-exe/win32/console_compat.c'
foreach ($marker in @('RegisterConsoleVDM', 'present_text_invalidation',
                       'WriteConsoleOutputA')) {
    if ($console -notmatch [regex]::Escape($marker)) {
        throw "T410 native text Console marker is missing: $marker"
    }
}

Write-Host 'T410 native Console route selection passed.'
