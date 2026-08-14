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
    (Join-Path $root 'src\bx-mantle\bx_ntvdm_machine_stage_v1.cc'),
    (Join-Path $root 'tests\bx-mantle\t200_s22_machine_stage_fixture.cc')
)
foreach ($input in $inputs) {
    if (-not (Test-Path -LiteralPath $input)) { throw "Required S22 input missing: $input" }
}

$config = Join-Path $base 'native-core\config.h'
$includes = @('src', 'src\bochs', 'src\bochs\instrument\stubs', 'src\bx-core',
    'src\bx-core\cpu', 'src\bx-mantle', 'src\bx-vdm', 'src\bochs\iodev') |
    ForEach-Object { '/I "' + (Join-Path $root $_) + '"' }
$compile = 'cl.exe /nologo /c /std:c++14 /EHsc /MT /Gy /DWIN32 ' +
    ($includes -join ' ') + ' /FI "' + $config + '" '
$sources = @(
    @{ Object = 'machine-stage.obj'; Source = 'src\bx-mantle\bx_ntvdm_machine_stage_v1.cc' },
    @{ Object = 'fixture.obj'; Source = 'tests\bx-mantle\t200_s22_machine_stage_fixture.cc' }
)
$batch = Join-Path $build 'compile.cmd'
$lines = [System.Collections.Generic.List[string]]::new()
$lines.Add('@echo off')
$lines.Add('call "' + $vs + '" -arch=x64 -host_arch=x64 >nul')
$lines.Add('if errorlevel 1 exit /b %errorlevel%')
foreach ($source in $sources) {
    $lines.Add($compile + '/Fo"' + (Join-Path $build $source.Object) + '" "' +
        (Join-Path $root $source.Source) + '"')
    $lines.Add('if errorlevel 1 exit /b %errorlevel%')
}
$lines.Add('exit /b 0')
$lines | Set-Content -LiteralPath $batch -Encoding ascii
& cmd.exe /d /s /c ('call "' + $batch + '"') 2>&1 |
    Tee-Object -FilePath (Join-Path $build 'compile.log')
if ($LASTEXITCODE -ne 0) { throw "S22 fixture compile failed: $LASTEXITCODE" }

$exe = Join-Path $build 't200-s22-machine-stage.exe'
$response = Join-Path $build 'link.rsp'
$fixture = Join-Path $build 'fixture.obj'
$link = foreach ($line in Get-Content -LiteralPath (Join-Path $base 'link.rsp')) {
    if ($line -match '\\fixture\.obj"$') { '"' + $fixture + '"'; continue }
    if ($line -match '^/OUT:') { '/OUT:"' + $exe + '"'; continue }
    $line
}
$link += '"' + (Join-Path $build 'machine-stage.obj') + '"'
$link | Set-Content -LiteralPath $response -Encoding ascii
& cmd.exe /d /s /c ('call "' + $vs + '" -arch=x64 -host_arch=x64 >nul && link.exe @"' + $response + '"') 2>&1 |
    Tee-Object -FilePath (Join-Path $build 'link.log')
if ($LASTEXITCODE -ne 0) { throw "S22 fixture link failed: $LASTEXITCODE" }

& cmd.exe /d /s /c ('"' + $exe + '" 2>&1') |
    Tee-Object -FilePath (Join-Path $build 'run.log')
$runExit = $LASTEXITCODE
$record = [ordered]@{
    schema = 'ntdos64.t200.s22.machine-stage.v1'
    architecture = 'x64'
    runtimeLibrary = '/MT'
    baseMachineClosure = 'T198 S74 retained full CPU5 native-core object closure'
    operation = 'initialize -> initial action -> capture -> startup action -> restore'
    machineInitialized = $true
    guestExecution = $false
    cpuLoopEntered = $false
    runExitCode = $runExit
    expectedRunExitCode = 0
    passed = ($runExit -eq 0)
}
$record | ConvertTo-Json -Depth 4 |
    Set-Content -LiteralPath (Join-Path $build 't200-s22-machine-stage.json') -Encoding utf8
if ($runExit -ne 0) { throw "S22 machine-stage fixture failed: $runExit" }
Write-Host "T200 S22 machine-stage probe passed: $build"
