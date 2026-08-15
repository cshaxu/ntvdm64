param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$BuildRoot,
    [Parameter(Mandatory = $true)][string]$CompositionBuildRoot
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = [IO.Path]::GetFullPath($BuildRoot)
$composition = [IO.Path]::GetFullPath($CompositionBuildRoot)
if (Test-Path -LiteralPath $build) {
    throw "Refusing to overwrite existing build directory: $build"
}
New-Item -ItemType Directory -Path $build | Out-Null

$baseline = Join-Path $root 'artifacts\build\t198-s74-dem-pdb-termination-r1'
$manifestPath = Join-Path $root 'tools\build\t198-s50-bx-vdm-composition-manifest.json'
$objects = Join-Path $composition 'current-objects'
$profile = Join-Path $composition 'prepared\profile-v7.json'
$byobRoot = Join-Path $root 'artifacts\inputs\t194-v6-normal-return-root-r1'
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
foreach ($input in @($baseline, $manifestPath, $objects, $profile, $byobRoot, $vs,
    (Join-Path $root 'src\bx-mantle\bx_ntvdm_engine_contract_v1.c'),
    (Join-Path $root 'src\bx-mantle\bx_ntvdm_engine_run_v1.c'),
    (Join-Path $root 'src\bx-vdm\bx_ntvdm_vdm_generic_ud_bridge_v1.c'),
    (Join-Path $root 'src\bx-vdm\bx_ntvdm_normal_terminal_sequence_observation_v1.c'),
    (Join-Path $root 'tests\bx-mantle\bx_ntvdm_engine_direct_composition_v1_test.c'))) {
    if (-not (Test-Path -LiteralPath $input)) { throw "Required S22 input missing: $input" }
}

$manifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
$current = @{}
foreach ($relative in @($manifest.compileSources | Where-Object { $_ -notin @($manifest.compileOnlySources) })) {
    $base = [IO.Path]::GetFileNameWithoutExtension($relative)
    $object = Join-Path $objects ($base + '.obj')
    if (-not (Test-Path -LiteralPath $object)) { throw "Missing current composition object: $object" }
    $current[$base] = $object
}

$config = Join-Path $baseline 'native-core\config.h'
$includes = @('src', 'refs\bochs', 'refs\bochs\instrument\stubs', 'src\bx-core',
    'src\bx-core\cpu', 'src\bx-mantle', 'src\bx-vdm', 'src\cli') |
    ForEach-Object { '/I "' + (Join-Path $root $_) + '"' }
$compile = 'cl.exe /nologo /TC /c /std:c11 /W4 /WX /MT /DWIN32 /D_CRT_SECURE_NO_WARNINGS ' +
    ($includes -join ' ') + ' /FI "' + $config + '" '
$sources = @(
    @{ Object = 'engine-contract.obj'; Source = 'src\bx-mantle\bx_ntvdm_engine_contract_v1.c' },
    @{ Object = 'engine-run.obj'; Source = 'src\bx-mantle\bx_ntvdm_engine_run_v1.c' },
    @{ Object = 'generic-ud-bridge.obj'; Source = 'src\bx-vdm\bx_ntvdm_vdm_generic_ud_bridge_v1.c' },
    @{ Object = 'terminal-observation.obj'; Source = 'src\bx-vdm\bx_ntvdm_terminal_observation_v1.c' },
    @{ Object = 'normal-terminal-sequence-observation.obj'; Source = 'src\bx-vdm\bx_ntvdm_normal_terminal_sequence_observation_v1.c' },
    @{ Object = 'fixture.obj'; Source = 'tests\bx-mantle\bx_ntvdm_engine_direct_composition_v1_test.c' }
)
$batch = Join-Path $build 'compile.cmd'
$commands = [System.Collections.Generic.List[string]]::new()
$commands.Add('@echo off')
$commands.Add('call "' + $vs + '" -arch=x64 -host_arch=x64 >nul')
$commands.Add('if errorlevel 1 exit /b %errorlevel%')
foreach ($source in $sources) {
    $commands.Add($compile + '/Fo"' + (Join-Path $build $source.Object) + '" "' +
        (Join-Path $root $source.Source) + '"')
    $commands.Add('if errorlevel 1 exit /b %errorlevel%')
}
$commands.Add('exit /b 0')
$commands | Set-Content -LiteralPath $batch -Encoding ascii
& cmd.exe /d /s /c ('call "' + $batch + '"') 2>&1 |
    Tee-Object -FilePath (Join-Path $build 'compile.log')
if ($LASTEXITCODE -ne 0) { throw "S22 engine fixture compile failed: $LASTEXITCODE" }
$current['bx_ntvdm_terminal_observation_v1'] = Join-Path $build 'terminal-observation.obj'
$current['bx_ntvdm_normal_terminal_sequence_observation_v1'] = Join-Path $build 'normal-terminal-sequence-observation.obj'

$exe = Join-Path $build 't200-s22-engine-machine-stage.exe'
$response = Join-Path $build 'link.rsp'
$emitted = @{}
$link = foreach ($line in Get-Content -LiteralPath (Join-Path $baseline 'link.rsp')) {
    if ($line -match '\\fixture\.obj"$') { '"' + (Join-Path $build 'fixture.obj') + '"'; continue }
    if ($line -match '\\bridge\.obj"$') { '"' + (Join-Path $build 'generic-ud-bridge.obj') + '"'; continue }
    if ($line -match '^/OUT:') { '/OUT:"' + $exe + '"'; continue }
    if ($line -match '\\([^\\"]+)\.obj"$') {
        $base = [IO.Path]::GetFileNameWithoutExtension($matches[1])
        if ($current.ContainsKey($base)) { $emitted[$base] = $true; '"' + $current[$base] + '"'; continue }
    }
    $line
}
foreach ($base in ($current.Keys | Sort-Object)) {
    if (-not $emitted.ContainsKey($base)) { $link += '"' + $current[$base] + '"' }
}
foreach ($source in $sources | Where-Object { $_.Object -notin @('fixture.obj', 'generic-ud-bridge.obj', 'terminal-observation.obj', 'normal-terminal-sequence-observation.obj') }) {
    $link += '"' + (Join-Path $build $source.Object) + '"'
}
# The current direct-host namespace uses normal user-mode ntdll imports.  The
# original S22 response predates that source closure.
if ($link -notcontains 'ntdll.lib') { $link += 'ntdll.lib' }
$link | Set-Content -LiteralPath $response -Encoding ascii
& cmd.exe /d /s /c ('call "' + $vs + '" -arch=x64 -host_arch=x64 >nul && link.exe @"' + $response + '"') 2>&1 |
    Tee-Object -FilePath (Join-Path $build 'link.log')
if ($LASTEXITCODE -ne 0) { throw "S22 engine fixture link failed: $LASTEXITCODE" }

$start = [Diagnostics.ProcessStartInfo]::new()
$start.FileName = $exe
$start.Arguments = '"' + $profile + '" "' + $byobRoot + '"'
$start.WorkingDirectory = $root
$start.UseShellExecute = $false
$start.RedirectStandardOutput = $true
$start.RedirectStandardError = $true
# The engine contract copies a bounded OEM environment.  This fixture selects
# its small input explicitly, so a developer's unrelated shell state cannot
# turn an engine-lifecycle test into an admission failure.
$start.Environment.Clear()
$start.Environment['ComSpec'] = Join-Path $env:SystemRoot 'System32\cmd.exe'
$start.Environment['PROMPT'] = '$P$G'
$process = [Diagnostics.Process]::new()
$process.StartInfo = $start
if (-not $process.Start()) { throw 'S22 engine fixture did not start.' }
$stdout = $process.StandardOutput.ReadToEnd()
$stderr = $process.StandardError.ReadToEnd()
$process.WaitForExit()
($stdout + $stderr) | Set-Content -LiteralPath (Join-Path $build 'run.log') -Encoding utf8
$runExit = $process.ExitCode
$record = [ordered]@{
    schema = 'ntdos64.t200.s22.engine-machine-stage.v1'
    architecture = 'x64'
    runtimeLibrary = '/MT'
    currentComposition = $composition
    sourceClosure = 'T198 S94 current manifest object closure plus current engine objects'
    machineInitialized = $true
    guestExecution = $true
    cpuLoopEntered = $true
    runExitCode = $runExit
    expectedRunExitCode = 0
    passed = ($runExit -eq 0)
}
$record | ConvertTo-Json -Depth 4 |
    Set-Content -LiteralPath (Join-Path $build 't200-s22-engine-machine-stage.json') -Encoding utf8
if ($runExit -ne 0) { throw "S22 engine machine-stage fixture failed: $runExit" }
Write-Host "T200 S22 engine machine-stage probe passed: $build"
