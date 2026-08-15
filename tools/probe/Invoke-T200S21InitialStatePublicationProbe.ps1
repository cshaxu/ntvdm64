param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$BuildRoot
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = [IO.Path]::GetFullPath($BuildRoot)
if (Test-Path -LiteralPath $build) {
    throw "Refusing to overwrite existing build directory: $build"
}
New-Item -ItemType Directory -Path $build | Out-Null

$base = Join-Path $root 'artifacts\build\t198-s74-dem-pdb-termination-r1'
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$inputs = @(
    $vs, (Join-Path $base 'link.rsp'), (Join-Path $base 'native-core\config.h'),
    (Join-Path $root 'src\bx-mantle\bx_ntvdm_mechanical_action_v1.cc'),
    (Join-Path $root 'src\bx-vdm\bx_ntvdm_initial_state_abi.c'),
    (Join-Path $root 'src\bx-vdm\bx_ntvdm_initial_state_action_v1.c'),
    (Join-Path $root 'tests\bx-mantle\t200_s21_initial_state_publication_fixture.cc')
)
foreach ($input in $inputs) {
    if (-not (Test-Path -LiteralPath $input)) { throw "Required S21 input missing: $input" }
}

$config = Join-Path $base 'native-core\config.h'
$includeRoots = @('src', 'refs\bochs', 'refs\bochs\instrument\stubs', 'src\bx-core',
    'src\bx-core\cpu', 'src\bx-mantle', 'src\bx-vdm', 'refs\bochs\iodev') |
    ForEach-Object { '/I "' + (Join-Path $root $_) + '"' }
$cCompile = 'cl.exe /nologo /TC /c /std:c11 /W4 /WX /MT /DWIN32 ' +
    ($includeRoots -join ' ') + ' /FI "' + $config + '" '
$cxxCompile = 'cl.exe /nologo /c /std:c++14 /EHsc /MT /Gy /DWIN32 ' +
    ($includeRoots -join ' ') + ' /FI "' + $config + '" '
$sources = @(
    @{ Object = 'initial-state.obj'; Source = 'src\bx-vdm\bx_ntvdm_initial_state_abi.c'; Command = $cCompile },
    @{ Object = 'initial-state-action.obj'; Source = 'src\bx-vdm\bx_ntvdm_initial_state_action_v1.c'; Command = $cCompile },
    @{ Object = 'fixture.obj'; Source = 'tests\bx-mantle\t200_s21_initial_state_publication_fixture.cc'; Command = $cxxCompile }
)
$batch = Join-Path $build 'compile.cmd'
$batchLines = [System.Collections.Generic.List[string]]::new()
$batchLines.Add('@echo off')
$batchLines.Add('call "' + $vs + '" -arch=x64 -host_arch=x64 >nul')
$batchLines.Add('if errorlevel 1 exit /b %errorlevel%')
foreach ($source in $sources) {
    $batchLines.Add($source.Command + '/Fo"' + (Join-Path $build $source.Object) + '" "' +
        (Join-Path $root $source.Source) + '"')
    $batchLines.Add('if errorlevel 1 exit /b %errorlevel%')
}
$batchLines.Add('exit /b 0')
$batchLines | Set-Content -LiteralPath $batch -Encoding ascii
& cmd.exe /d /s /c ('call "' + $batch + '"') 2>&1 |
    Tee-Object -FilePath (Join-Path $build 'compile.log')
if ($LASTEXITCODE -ne 0) { throw "S21 fixture compile failed: $LASTEXITCODE" }

$exe = Join-Path $build 't200-s21-initial-state-publication.exe'
$response = Join-Path $build 'link.rsp'
$fixtureObject = Join-Path $build 'fixture.obj'
$lines = foreach ($line in Get-Content -LiteralPath (Join-Path $base 'link.rsp')) {
    if ($line -match '\\fixture\.obj"$') { '"' + $fixtureObject + '"'; continue }
    if ($line -match '^/OUT:') { '/OUT:"' + $exe + '"'; continue }
    $line
}
foreach ($source in $sources | Where-Object { $_.Object -ne 'fixture.obj' }) {
    $lines += '"' + (Join-Path $build $source.Object) + '"'
}
$lines | Set-Content -LiteralPath $response -Encoding ascii
& cmd.exe /d /s /c ('call "' + $vs + '" -arch=x64 -host_arch=x64 >nul && link.exe @"' + $response + '"') 2>&1 |
    Tee-Object -FilePath (Join-Path $build 'link.log')
if ($LASTEXITCODE -ne 0) { throw "S21 fixture link failed: $LASTEXITCODE" }

$runCommand = '"' + $exe + '" 2>&1'
& cmd.exe /d /s /c $runCommand | Tee-Object -FilePath (Join-Path $build 'run.log')
$runExit = $LASTEXITCODE
$record = [ordered]@{
    schema = 'ntdos64.t200.s21.initial-state-publication.v1'
    architecture = 'x64'
    runtimeLibrary = '/MT'
    baseMachineClosure = 'T198 S74 retained full CPU5 native-core object closure'
    publication = 'bx-vdm copied initial state -> selector-blind mantle write action'
    machineInitialized = $true
    guestExecution = $false
    cpuLoopEntered = $false
    runExitCode = $runExit
    expectedRunExitCode = 0
    passed = ($runExit -eq 0)
}
$record | ConvertTo-Json -Depth 4 |
    Set-Content -LiteralPath (Join-Path $build 't200-s21-initial-state-publication.json') -Encoding utf8
if ($runExit -ne 0) { throw "S21 initial-state publication fixture failed: $runExit" }
Write-Host "T200 S21 initial-state publication probe passed: $build"
