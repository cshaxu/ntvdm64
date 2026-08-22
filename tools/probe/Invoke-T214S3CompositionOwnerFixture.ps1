param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$BuildRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = [IO.Path]::GetFullPath($BuildRoot)
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build directory: $build" }
$s94 = Join-Path $root 'tools\probe\Invoke-T198S94SourceBuiltNormalReturnNativeProbe.ps1'
$baseline = Join-Path $root 'artifacts\build\t198-s74-dem-pdb-termination-r1'
$manifestPath = Join-Path $root 'tools\build\t198-s50-bx-vdm-composition-manifest.json'
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
foreach ($path in @($s94, $baseline, $manifestPath, $vs,
    (Join-Path $root 'src\bx-vdm\bx_ntvdm_engine_contract_v1.c'),
    (Join-Path $root 'src\bx-vdm\bx_ntvdm_engine_run_v1.c'),
    (Join-Path $root 'tests\bx-mantle\t214_s3_composition_owner_fixture.c'))) {
    if (-not (Test-Path -LiteralPath $path)) { throw "Missing T214 S3 input: $path" }
}
& $s94 -RepositoryRoot $root -BuildRoot $build -CompileOnly
if ($LASTEXITCODE -ne 0) { throw "T214 S3 full source closure compile failed: $LASTEXITCODE" }
$prepared = Join-Path $build 'prepared'
$profile = Join-Path $prepared 'profile-v7.json'
$configSource = Join-Path $prepared 'config.nt'
$autoexecSource = Join-Path $prepared 'autoexec.nt'
$oem = [Text.Encoding]::GetEncoding(437)
[IO.File]::WriteAllText($configSource, "FILES=20`r`n", $oem)
[IO.File]::WriteAllText($autoexecSource, "", $oem)
$byobRoot = Join-Path $root 'artifacts\inputs\t194-v6-normal-return-root-r1'
$config = Join-Path $baseline 'native-core\config.h'
$objects = Join-Path $build 'current-objects'
$contractObject = Join-Path $build 'engine-contract.obj'
$entryObject = Join-Path $build 'engine-run.obj'
$testObject = Join-Path $build 't214-s3-composition-owner-fixture.obj'
$genericBridgeObject = Join-Path $build 'generic-ud-bridge.obj'
$includes = @('src', 'refs\bochs', 'refs\bochs\instrument\stubs', 'src\bx-core', 'src\bx-core\cpu', 'src\bx-mantle', 'src\bx-vdm', 'src\cli') | ForEach-Object { '/I "' + (Join-Path $root $_) + '"' }
$compile = 'cl.exe /nologo /TC /c /std:c11 /W4 /WX /MT /DWIN32 /D_CRT_SECURE_NO_WARNINGS ' + ($includes -join ' ') + ' /FI "' + $config + '" '
$commands = [System.Collections.Generic.List[string]]::new()
$commands.Add($compile + '/Fo"' + $contractObject + '" "' +
    (Join-Path $root 'src\bx-vdm\bx_ntvdm_engine_contract_v1.c') + '"')
$commands.Add($compile + '/Fo"' + $entryObject + '" "' +
    (Join-Path $root 'src\bx-vdm\bx_ntvdm_engine_run_v1.c') + '"')
$commands.Add($compile + '/Fo"' + $testObject + '" "' +
    (Join-Path $root 'tests\bx-mantle\t214_s3_composition_owner_fixture.c') + '"')
$commands.Add($compile + '/Fo"' + $genericBridgeObject + '" "' +
    (Join-Path $root 'src\bx-vdm\bx_ntvdm_vdm_generic_ud_bridge_v1.c') + '"')
$batch = Join-Path $build 'compile-owner-fixture.cmd'
$batchLines = [System.Collections.Generic.List[string]]::new()
$batchLines.Add('@echo off')
$batchLines.Add('call "' + $vs + '" -arch=x64 -host_arch=x64 >nul')
$batchLines.Add('if errorlevel 1 exit /b %errorlevel%')
foreach ($command in $commands) {
    $batchLines.Add($command)
    $batchLines.Add('if errorlevel 1 exit /b %errorlevel%')
}
$batchLines | Set-Content -LiteralPath $batch -Encoding ascii
& cmd.exe /d /c $batch 2>&1 | Tee-Object -LiteralPath (Join-Path $build 'fixture-compile.log')
if ($LASTEXITCODE -ne 0) { throw "T214 S3 fixture compile failed: $LASTEXITCODE" }
$manifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
$current = @{}
foreach ($relative in @($manifest.compileSources | Where-Object { $_ -notin @($manifest.compileOnlySources) })) {
    $base = [IO.Path]::GetFileNameWithoutExtension($relative)
    $current[$base] = Join-Path $objects ($base + '.obj')
}
$response = Join-Path $build 'owner-fixture-link.rsp'
$exe = Join-Path $build 't214-s3-composition-owner-fixture.exe'
$emitted = @{}
$lines = foreach ($line in Get-Content -LiteralPath (Join-Path $baseline 'link.rsp')) {
    if ($line -match '\\fixture\.obj"$') { '"' + $testObject + '"'; continue }
    if ($line -match '\\bridge\.obj"$') { continue }
    if ($line -match '^/OUT:') { '/OUT:"' + $exe + '"'; continue }
    if ($line -match '\\([^\\"]+)\.obj"$') {
        $base = [IO.Path]::GetFileNameWithoutExtension($matches[1])
        if ($current.ContainsKey($base)) { $emitted[$base] = $true; '"' + $current[$base] + '"'; continue }
    }
    $line
}
foreach ($base in ($current.Keys | Sort-Object)) { if (-not $emitted.ContainsKey($base)) { $lines += '"' + $current[$base] + '"' } }
$lines += '"' + $contractObject + '"'; $lines += '"' + $entryObject + '"'; $lines += '"' + $genericBridgeObject + '"'; $lines += 'ntdll.lib'; $lines += 'user32.lib'
$lines | Set-Content -LiteralPath $response -Encoding ascii
& cmd.exe /d /s /c ('call "' + $vs + '" -arch=x64 -host_arch=x64 >nul && link.exe @"' + $response + '"') 2>&1 | Tee-Object -LiteralPath (Join-Path $build 'link.log')
if ($LASTEXITCODE -ne 0) { throw "T214 S3 fixture link failed: $LASTEXITCODE" }
$runLog = Join-Path $build 'run.log'
$savedConfigSource = $env:NTDOS64_STARTUP_CONFIG_SOURCE
$savedAutoexecSource = $env:NTDOS64_STARTUP_AUTOEXEC_SOURCE
$env:NTDOS64_STARTUP_CONFIG_SOURCE = $configSource
$env:NTDOS64_STARTUP_AUTOEXEC_SOURCE = $autoexecSource
try {
    & cmd.exe /d /c ('"' + $exe + '" "' + $profile + '" "' + $byobRoot + '" > "' + $runLog + '" 2>&1')
    $run = $LASTEXITCODE
} finally {
    $env:NTDOS64_STARTUP_CONFIG_SOURCE = $savedConfigSource
    $env:NTDOS64_STARTUP_AUTOEXEC_SOURCE = $savedAutoexecSource
}
Get-Content -LiteralPath $runLog
$record = [ordered]@{schema='ntdos64.t214.s3.composition-owner-fixture.v1';architecture='x64';runtimeLibrary='/MT';fullCurrentSourceClosure=$true;runExitCode=$run;expectedRunExitCode=0;terminalKind='controlled-guest-terminal';terminalDetail=1;repeatedReset=$false;cpuEntryReached=$true;guestExecutionClaim=$false;passed=($run -eq 0)}
$record | ConvertTo-Json -Depth 3 | Set-Content -LiteralPath (Join-Path $build 't214-s3-composition-owner-fixture.json') -Encoding utf8
if ($run -ne 0) { throw "T214 S3 composition owner fixture failed: $run" }
Write-Host "T214 S3 composition owner fixture passed: $build"
