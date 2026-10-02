[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,[Parameter(Mandatory)][string]$LogPath)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$output=[IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
$prefix=[IO.Path]::GetFullPath((Join-Path $repo 'build'))+[IO.Path]::DirectorySeparatorChar
if(!$output.StartsWith($prefix,[StringComparison]::OrdinalIgnoreCase)){throw 'BuildRoot must be under build/'}
$log=[IO.Path]::GetFullPath($LogPath)
if([IO.Path]::GetDirectoryName($log) -notin @('O:\winnt\logs','O:\winnt\Logs2')){throw 'Use approved log directory'}
if((Test-Path $output) -or (Test-Path $log)){throw 'Use fresh evidence paths'}
[void](Get-Command cl.exe -ErrorAction Stop)
[void](New-Item -ItemType Directory -Path $output)
Push-Location $repo
try {
    $exe=Join-Path $output 'ntvwm-console-state.exe'
    & cl.exe /nologo /W4 /MT /Isrc tests/observation/ntvwm_console_state_test.c `
        src/ntvwm-exe/console_state.c `
        "/Fo$output\" "/Fe$exe" /link /incremental:no user32.lib *> (Join-Path $output 'build.log')
    if($LASTEXITCODE){throw "Compile failed: $output/build.log"}
    & $exe *> $log
    $result=$LASTEXITCODE
    Get-Content $log
    if($result -or !(Select-String -LiteralPath $log -SimpleMatch 'NTVWM-STATE PASS' -Quiet)){throw 'Console state fixture failed'}
} finally {Pop-Location}
