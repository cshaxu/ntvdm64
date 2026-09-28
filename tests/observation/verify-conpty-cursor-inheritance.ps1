[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$BuildRoot,
    [Parameter(Mandatory)][string]$LogPath
)
$ErrorActionPreference = 'Stop'
$repository = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$output = [IO.Path]::GetFullPath((Join-Path $repository $BuildRoot))
$prefix = [IO.Path]::GetFullPath((Join-Path $repository 'build')) + [IO.Path]::DirectorySeparatorChar
if (!$output.StartsWith($prefix, [StringComparison]::OrdinalIgnoreCase)) { throw 'BuildRoot must be beneath build/' }
if (Test-Path -LiteralPath $output) { throw 'Use a new run root' }
$log = [IO.Path]::GetFullPath($LogPath)
if ((Test-Path -LiteralPath $log) -or [IO.Path]::GetDirectoryName($log) -notin @('O:\winnt\logs', 'O:\winnt\Logs2')) {
    throw 'Use a new log in the approved runtime log directory'
}
& (Join-Path $repository 'tools/build/Build-FrontendTerminalLibrary.ps1') -BuildRoot (Join-Path $BuildRoot 'upstream')
$library = Join-Path $output 'upstream'
$include = Join-Path $library 'libvterm-0.3.3/include'
$executable = Join-Path $output 'cursor-inheritance.exe'
Push-Location $repository
try {
    # Startup and retained-resource cases use the production resource/parser.
    & cl.exe /nologo /W4 /MT /DNTKVM_CONPTY_TEST_RELEASE /std:c11 /Isrc "/I$include" `
        tests/observation/conpty_cursor_inheritance_test.c src/ntkvm-exe/native_terminal.c `
        src/ntkvm-exe/native_conpty.c src/ntkvm-exe/native_console_launch.c (Join-Path $library 'libvterm.lib') `
        "/Fo$output\" "/Fe$executable" /link /incremental:no *> (Join-Path $output 'build.log')
    if ($LASTEXITCODE) { throw "Build failed; see $output/build.log" }
    & $executable *> $log
    $result = $LASTEXITCODE
    Get-Content -LiteralPath $log
    if ($result) { throw "Cursor inheritance probe failed with $result" }
    foreach ($marker in @(
        'INHERIT-CURSOR flag=0 retained=0 positioned=1 child=37 error=0 launch=0 PASS',
        'INHERIT-CURSOR flag=1 retained=1 positioned=1 child=37 error=0 launch=0 PASS'
        'REUSED-CURSOR local-moved=1 first=37 second=37 error=0 remote-still=11,4 PASS'
        'REUSED-ADDRESSING absolute=0 PASS'
        'REUSED-ADDRESSING absolute=1 PASS'
    )) {
        if (!(Select-String -LiteralPath $log -SimpleMatch $marker -Quiet)) { throw "Missing assertion: $marker" }
    }
} finally { Pop-Location }
