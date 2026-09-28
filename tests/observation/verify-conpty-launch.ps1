[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$BuildRoot,
    [Parameter(Mandatory)][string]$LogPath
)
$ErrorActionPreference = 'Stop'
$repository = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$output = [IO.Path]::GetFullPath((Join-Path $repository $BuildRoot))
$buildPrefix = [IO.Path]::GetFullPath((Join-Path $repository 'build')) + [IO.Path]::DirectorySeparatorChar
if (!$output.StartsWith($buildPrefix, [StringComparison]::OrdinalIgnoreCase)) {
    throw 'BuildRoot must be beneath repository build/'
}
if (Test-Path -LiteralPath $output) { throw 'Use a new run root; retained evidence is immutable' }
$log = [IO.Path]::GetFullPath($LogPath)
if (Test-Path -LiteralPath $log) { throw 'Use a new log path' }
$logParent = [IO.Path]::GetDirectoryName($log)
if ($logParent -notin @('O:\winnt\logs', 'O:\winnt\Logs2')) {
    throw 'Runtime logs must use the approved runtime log directory'
}
[void](Get-Command cl.exe -ErrorAction Stop)
[void](New-Item -ItemType Directory -Path $output)
$executable = Join-Path $output 'conpty-launch.exe'
Push-Location $repository
try {
    # Match the current frontend graph's target/CRT and declaration baseline.
    & cl.exe /nologo /W4 /MT /DNTKVM_CONPTY_TEST_RELEASE /D_WIN32_WINNT=0x0601 /Isrc `
        tests/observation/conpty_launch_test.c src/ntkvm-exe/native_console_launch.c src/ntkvm-exe/native_conpty.c `
        "/Fo$output\" "/Fe$executable" /link /incremental:no *> (Join-Path $output 'build.log')
    if ($LASTEXITCODE) { throw "Build failed; see $output/build.log" }
    & $executable $output *> $log
    $result = $LASTEXITCODE
    Get-Content -LiteralPath $log
    if ($result -ne 0) { throw "ConPTY launch tests failed with $result" }
    if (!(Select-String -LiteralPath $log -SimpleMatch 'CONPTY-LAUNCH failures=0' -Quiet)) {
        throw 'Missing positive completion marker'
    }
    $lifecycle = Join-Path $output 'conpty-lifecycle.exe'
    & cl.exe /nologo /W4 /MT /DNTKVM_CONPTY_TEST_RELEASE /std:c11 /D_WIN32_WINNT=0x0601 /Isrc `
        tests/observation/conpty_lifecycle_test.c `
        (Join-Path $output 'native_console_launch.obj') (Join-Path $output 'native_conpty.obj') `
        "/Fo$output\" "/Fe$lifecycle" /link /incremental:no *> (Join-Path $output 'lifecycle-build.log')
    if ($LASTEXITCODE) { throw "Lifecycle build failed; see $output/lifecycle-build.log" }
    & $lifecycle *>> $log
    if ($LASTEXITCODE) { throw "ConPTY production lifecycle failed; see $log" }
    $passed = @(Select-String -LiteralPath $log -Pattern 'conpty case=.* pass=yes ')
    if ($passed.Count -ne 11) { throw 'Incomplete production lifecycle cases' }
    Write-Output 'PASS production ConPTY lifecycle/input 11 cases'
    $admission = Join-Path $output 'conpty-admission.exe'
    & cl.exe /nologo /W4 /MT /DNTKVM_CONPTY_TEST_RELEASE /Isrc tests/observation/conpty_admission_test.c `
        (Join-Path $output 'native_console_launch.obj') (Join-Path $output 'native_conpty.obj') `
        "/Fo$output\" "/Fe$admission" /link /incremental:no *> (Join-Path $output 'admission-build.log')
    if ($LASTEXITCODE) { throw "Admission build failed; see $output/admission-build.log" }
    & $admission *>> $log
    if ($LASTEXITCODE -or !(Select-String -LiteralPath $log -SimpleMatch 'CONPTY-RELEASE-ADMISSION PASS' -Quiet)) {
        throw "ConPTY post-release admission failed; see $log"
    }
    & $admission --retain *>> $log
    if ($LASTEXITCODE -or !(Select-String -LiteralPath $log -SimpleMatch 'PASS mode=retained-real-io' -Quiet)) {
        throw "ConPTY retained admission real I/O failed; see $log"
    }
    Write-Output 'PASS released admission refusal; retained admission real I/O; final natural EOF'
} finally { Pop-Location }
