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
$executable = Join-Path $output 'terminal-test.exe'
Push-Location $repository
try {
    & cl.exe /nologo /W4 /MT /DNTKVM_CONPTY_TEST_RELEASE /std:c11 /Isrc "/I$include" `
        tests/observation/native_terminal_test.c src/ntkvm-exe/native_terminal.c `
        src/ntkvm-exe/native_conpty.c src/ntkvm-exe/native_console_launch.c `
        (Join-Path $library 'libvterm.lib') "/Fo$output\" "/Fe$executable" `
        /link /incremental:no *> (Join-Path $output 'build.log')
    if ($LASTEXITCODE) { throw "Terminal build failed; see $output/build.log" }
    & $executable $output *> $log
    if ($LASTEXITCODE) { throw "Terminal test failed; see $log" }
    if (!(Select-String -LiteralPath $log -Pattern '^NATIVE-TERMINAL checks=215 failures=0$' -Quiet)) {
        throw 'Incomplete terminal assertions'
    }
    Get-Content -LiteralPath $log
    & $executable --screen-import *>> $log
    if ($LASTEXITCODE) { throw "Screen import test failed; see $log" }
    if (!(Select-String -LiteralPath $log -Pattern '^SCREEN-IMPORT checks=28 failures=0$' -Quiet)) {
        throw 'Incomplete screen-import assertions'
    }
    & $executable --console-handoff *>> $log
    if ($LASTEXITCODE) { throw "Console handoff test failed; see $log" }
    if (!(Select-String -LiteralPath $log -Pattern '^CONSOLE-HANDOFF checks=15 failures=0$' -Quiet)) {
        throw 'Incomplete Console-handoff assertions'
    }
    & $executable --console-unicode *>> $log
    if ($LASTEXITCODE) { throw "Console Unicode test failed; see $log" }
    if (!(Select-String -LiteralPath $log -Pattern '^CONSOLE-UNICODE checks=17 failures=0$' -Quiet)) {
        throw 'Incomplete Unicode preservation assertions'
    }
    & $executable --console-concurrent *>> $log
    if ($LASTEXITCODE) { throw "Concurrent output test failed; see $log" }
    if (!(Select-String -LiteralPath $log -Pattern '^CONSOLE-CONCURRENT checks=13 failures=0$' -Quiet)) {
        throw 'Incomplete concurrent output assertions'
    }
    $backendExecutable = Join-Path $output 'backend-test.exe'
    & cl.exe /nologo /W4 /MT /DNTKVM_CONPTY_TEST_RELEASE /std:c11 /Isrc "/I$include" `
        tests/observation/native_backend_test.c src/ntkvm-exe/native_console_backend.c `
        src/ntkvm-exe/native_terminal.c src/ntkvm-exe/native_conpty.c `
        src/ntkvm-exe/native_console_launch.c (Join-Path $library 'libvterm.lib') `
        "/Fo$output\" "/Fe$backendExecutable" /link /incremental:no *> (Join-Path $output 'backend-build.log')
    if ($LASTEXITCODE) { throw "Backend build failed; see $output/backend-build.log" }
    & $backendExecutable *>> $log
    if ($LASTEXITCODE) { throw "Backend test failed; see $log" }
    if (!(Select-String -LiteralPath $log -Pattern '^NATIVE-BACKEND PASS error=0 exit=37$' -Quiet)) {
        throw 'Missing real backend assertions'
    }
    if (!(Select-String -LiteralPath $log -SimpleMatch 'BACKEND-SCROLL PASS first=19 last=79 lines=61 history=50 rows=62 cursor=0,61 exit=37 error=0' -Quiet)) {
        throw 'Missing real bounded-scrollback assertion'
    }
    Get-Content -LiteralPath $log | Select-Object -Last 3
} finally { Pop-Location }
