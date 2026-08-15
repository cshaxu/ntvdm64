param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$BuildRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = [IO.Path]::GetFullPath($BuildRoot)
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build directory: $build" }
New-Item -ItemType Directory -Path $build | Out-Null
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (-not (Test-Path -LiteralPath $vs)) { throw "Missing MSVC environment: $vs" }
$sources = @(
    'src\cli\ntdos64_lifecycle_v1.c',
    'src\bx-mantle\bx_ntvdm_engine_contract_v1.c',
    'tests\runner\ntdos64_lifecycle_v1_test.c'
) | ForEach-Object { Join-Path $root $_ }
foreach ($source in $sources) { if (-not (Test-Path -LiteralPath $source)) { throw "Missing S2 source: $source" } }
$exe = Join-Path $build 't212-s2-cli-lifecycle-policy.exe'
$command = 'call "' + $vs + '" -arch=x64 -host_arch=x64 >nul && cl.exe /nologo /TC /std:c11 /W4 /WX /MT /I "' +
    (Join-Path $root 'src\cli') + '" /I "' + (Join-Path $root 'src\bx-mantle') +
    '" /Fe:"' + $exe + '" "' + ($sources -join '" "') + '"'
& cmd.exe /d /s /c $command 2>&1 | Tee-Object -LiteralPath (Join-Path $build 'compile.log')
if ($LASTEXITCODE -ne 0) { throw "T212 S2 compile failed: $LASTEXITCODE" }
& $exe 2>&1 | Tee-Object -LiteralPath (Join-Path $build 'run.log')
$runExit = $LASTEXITCODE
$record = [ordered]@{
    schema = 'ntdos64.t212.s2.cli-lifecycle-policy.v1'
    architecture = 'x64'
    runtimeLibrary = '/MT'
    sourceClosure = @('src/cli/ntdos64_lifecycle_v1.[hc]', 'src/bx-mantle/bx_ntvdm_engine_contract_v1.c', 'tests/runner/ntdos64_lifecycle_v1_test.c')
    forbiddenDependencies = @('bochs.h', 'src/bx-core', 'src/bx-vdm', 'guest execution', 'Windows HANDLE')
    runExitCode = $runExit
    expectedRunExitCode = 0
    passed = ($runExit -eq 0)
}
$record | ConvertTo-Json -Depth 3 | Set-Content -LiteralPath (Join-Path $build 't212-s2-cli-lifecycle-policy.json') -Encoding utf8
if ($runExit -ne 0) { throw "T212 S2 fixture failed: $runExit" }
Write-Host "T212 S2 CLI lifecycle policy probe passed: $build"
