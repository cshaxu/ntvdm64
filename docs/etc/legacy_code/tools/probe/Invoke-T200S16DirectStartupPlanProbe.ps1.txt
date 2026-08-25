param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$BuildRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = [IO.Path]::GetFullPath($BuildRoot)
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build directory: $build" }
$s14 = Join-Path $root 'tools\probe\Invoke-T200S14DirectCompositionProbe.ps1'
$baseline = Join-Path $root 'artifacts\build\t198-s74-dem-pdb-termination-r1'
$manifestPath = Join-Path $root 'tools\build\t198-s50-bx-vdm-composition-manifest.json'
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
foreach ($path in @($s14, $baseline, $manifestPath, $vs,
    (Join-Path $root 'src\bx-vdm\bx_ntvdm_engine_contract_v1.c'),
    (Join-Path $root 'src\bx-vdm\bx_ntvdm_vdm_generic_ud_bridge_v1.c'),
    (Join-Path $root 'tests\bx-mantle\bx_ntvdm_engine_direct_startup_plan_v1_test.c'))) {
    if (-not (Test-Path -LiteralPath $path)) { throw "Required S16 input missing: $path" }
}

$base = Join-Path $build 's14-base'
& $s14 -RepositoryRoot $root -BuildRoot $base
if ($LASTEXITCODE -ne 0) { throw "S16 retained direct-composition base failed: $LASTEXITCODE" }
$prepared = Join-Path $base 'prepared'
$profile = Join-Path $prepared 'profile-v7.json'
$byobRoot = Join-Path $root 'artifacts\inputs\t194-v6-normal-return-root-r1'
$config = Join-Path $baseline 'native-core\config.h'
$objects = Join-Path $base 'current-objects'
$testObject = Join-Path $build 'direct-startup-plan-test.obj'
$contractObject = Join-Path $build 'engine-contract.obj'
$genericBridgeObject = Join-Path $build 'generic-ud-bridge.obj'
$includes = @('src', 'refs\bochs', 'refs\bochs\instrument\stubs', 'src\bx-core', 'src\bx-core\cpu', 'src\bx-mantle', 'src\bx-vdm', 'src\cli') | ForEach-Object { '/I "' + (Join-Path $root $_) + '"' }
$compile = 'cl.exe /nologo /TC /c /std:c11 /W4 /WX /MT /DWIN32 /D_CRT_SECURE_NO_WARNINGS ' + ($includes -join ' ') + ' /FI "' + $config + '" '
$batch = Join-Path $build 'compile-s16.cmd'
$batchLines = [System.Collections.Generic.List[string]]::new()
$batchLines.Add('@echo off')
$batchLines.Add('call "' + $vs + '" -arch=x64 -host_arch=x64 >nul')
$batchLines.Add($compile + '/Fo"' + $contractObject + '" "' + (Join-Path $root 'src\bx-vdm\bx_ntvdm_engine_contract_v1.c') + '"')
$batchLines.Add('if errorlevel 1 exit /b %errorlevel%')
$batchLines.Add($compile + '/Fo"' + $genericBridgeObject + '" "' + (Join-Path $root 'src\bx-vdm\bx_ntvdm_vdm_generic_ud_bridge_v1.c') + '"')
$batchLines.Add('if errorlevel 1 exit /b %errorlevel%')
$batchLines.Add($compile + '/Fo"' + $testObject + '" "' + (Join-Path $root 'tests\bx-mantle\bx_ntvdm_engine_direct_startup_plan_v1_test.c') + '"')
$batchLines.Add('if errorlevel 1 exit /b %errorlevel%')
$batchLines.Add('exit /b 0')
$batchLines | Set-Content -LiteralPath $batch -Encoding ascii
& cmd.exe /d /s /c ('call "' + $batch + '"') 2>&1 | Tee-Object -FilePath (Join-Path $build 'compile.log')
if ($LASTEXITCODE -ne 0) { throw "S16 fixture compile failed: $LASTEXITCODE" }

$manifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
$current = @{}
foreach ($relative in @($manifest.compileSources | Where-Object { $_ -notin @($manifest.compileOnlySources) })) {
    $baseName = [IO.Path]::GetFileNameWithoutExtension($relative)
    $current[$baseName] = Join-Path $objects ($baseName + '.obj')
}
$response = Join-Path $build 'direct-startup-plan-link.rsp'
$exe = Join-Path $build 't200-s16-direct-startup-plan.exe'
$emitted = @{}
$lines = foreach ($line in Get-Content -LiteralPath (Join-Path $baseline 'link.rsp')) {
    if ($line -match '\\fixture\.obj"$') { '"' + $testObject + '"'; continue }
    if ($line -match '\\bridge\.obj"$') { continue }
    if ($line -match '^/OUT:') { '/OUT:"' + $exe + '"'; continue }
    if ($line -match '\\([^\\"]+)\.obj"$') {
        $baseName = [IO.Path]::GetFileNameWithoutExtension($matches[1])
        if ($current.ContainsKey($baseName)) { $emitted[$baseName] = $true; '"' + $current[$baseName] + '"'; continue }
    }
    $line
}
foreach ($baseName in ($current.Keys | Sort-Object)) { if (-not $emitted.ContainsKey($baseName)) { $lines += '"' + $current[$baseName] + '"' } }
$lines += '"' + $contractObject + '"'; $lines += '"' + $genericBridgeObject + '"'
$lines += 'ntdll.lib'
$lines | Set-Content -LiteralPath $response -Encoding ascii
& cmd.exe /d /s /c ('call "' + $vs + '" -arch=x64 -host_arch=x64 >nul && link.exe @"' + $response + '"') 2>&1 | Tee-Object -FilePath (Join-Path $build 'link.log')
if ($LASTEXITCODE -ne 0) { throw "S16 direct startup-plan link failed: $LASTEXITCODE" }
& $exe $profile $byobRoot 2>&1 | Tee-Object -FilePath (Join-Path $build 'run.log')
$runExit = $LASTEXITCODE
$record = [ordered]@{
    schema = 'ntdos64.t200.s16.direct-startup-plan.v1'; architecture = 'x64'; runtimeLibrary = '/MT'
    profile = 's14-base/prepared/profile-v7.json'; byobRoot = 'artifacts/inputs/t194-v6-normal-return-root-r1'
    sourceClosure = 'T200 S14 direct composition closure plus pointer-free direct startup-plan provider'
    machineInitialized = $false; guestExecution = $false; cpuLoopEntered = $false
    runExitCode = $runExit; expectedRunExitCode = 0; passed = ($runExit -eq 0)
}
$record | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $build 't200-s16-direct-startup-plan.json') -Encoding utf8
if ($runExit -ne 0) { throw "S16 direct startup-plan fixture failed: $runExit" }
Write-Host "T200 S16 direct startup-plan probe passed: $build"
