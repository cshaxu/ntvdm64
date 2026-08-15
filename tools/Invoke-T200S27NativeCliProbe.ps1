param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$BuildRoot,
    [Parameter(Mandatory = $true)][string]$CompositionBuildRoot,
    [string[]]$AdditionalCliSources = @(),
    [int]$ExpectedRunExitCode = 0
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = [IO.Path]::GetFullPath($BuildRoot)
$composition = [IO.Path]::GetFullPath($CompositionBuildRoot)
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build directory: $build" }
New-Item -ItemType Directory -Path $build | Out-Null
$baseline = Join-Path $root 'artifacts\build\t198-s74-dem-pdb-termination-r1'
$manifest = Get-Content -LiteralPath (Join-Path $root 'tools\t198-s50-bx-vdm-composition-manifest.json') -Raw | ConvertFrom-Json
$objects = Join-Path $composition 'current-objects'
$profile = Join-Path $composition 'prepared\profile-v7.json'
$byobRoot = Join-Path $root 'artifacts\inputs\t194-v6-normal-return-root-r1'
$target = Join-Path $byobRoot 'TARGET.EXE'
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
foreach ($path in @($baseline, $objects, $profile, $byobRoot, $target, $vs,
    (Join-Path $root 'src\cli\ntdos64_native_cli.c'),
    (Join-Path $root 'src\cli\byob_target_selection.c'),
    (Join-Path $root 'src\bx-mantle\bx_ntvdm_engine_contract_v1.c'),
    (Join-Path $root 'src\bx-mantle\bx_ntvdm_engine_run_v1.c'),
    (Join-Path $root 'src\bx-vdm\bx_ntvdm_vdm_generic_ud_bridge_v1.c'))) {
    if (-not (Test-Path -LiteralPath $path)) { throw "Required S27 input missing: $path" }
}
foreach ($relative in $AdditionalCliSources) {
    if ([IO.Path]::IsPathRooted($relative) -or $relative -notmatch '^src\\cli\\[^\\]+\.c$') {
        throw "Additional CLI source must be a relative src\\cli C source: $relative"
    }
    if (-not (Test-Path -LiteralPath (Join-Path $root $relative))) {
        throw "Additional CLI source missing: $relative"
    }
}
$current = @{}
foreach ($relative in @($manifest.compileSources | Where-Object { $_ -notin @($manifest.compileOnlySources) })) {
    $base = [IO.Path]::GetFileNameWithoutExtension($relative)
    $object = Join-Path $objects ($base + '.obj')
    if (-not (Test-Path -LiteralPath $object)) { throw "Missing current composition object: $object" }
    $current[$base] = $object
}
$config = Join-Path $baseline 'native-core\config.h'
$includes = @('src','src\bochs','src\bochs\instrument\stubs','src\bx-core','src\bx-core\cpu','src\bx-mantle','src\bx-vdm','src\cli') | ForEach-Object { '/I "' + (Join-Path $root $_) + '"' }
$compile = 'cl.exe /nologo /TC /c /std:c11 /W4 /WX /MT /DWIN32 /D_CRT_SECURE_NO_WARNINGS ' + ($includes -join ' ') + ' /FI "' + $config + '" '
$sources = @(
    @{ Object='native-cli.obj'; Source='src\cli\ntdos64_native_cli.c' },
    @{ Object='target-selection.obj'; Source='src\cli\byob_target_selection.c' },
    @{ Object='engine-contract.obj'; Source='src\bx-mantle\bx_ntvdm_engine_contract_v1.c' },
    @{ Object='engine-run.obj'; Source='src\bx-mantle\bx_ntvdm_engine_run_v1.c' },
    @{ Object='generic-ud-bridge.obj'; Source='src\bx-vdm\bx_ntvdm_vdm_generic_ud_bridge_v1.c' }
)
foreach ($relative in $AdditionalCliSources) {
    $base = [IO.Path]::GetFileNameWithoutExtension($relative)
    $sources += @{ Object=($base + '.obj'); Source=$relative }
}
$batch = Join-Path $build 'compile.cmd'; $commands = @('@echo off',('call "' + $vs + '" -arch=x64 -host_arch=x64 >nul'),'if errorlevel 1 exit /b %errorlevel%')
foreach ($source in $sources) { $commands += $compile + '/Fo"' + (Join-Path $build $source.Object) + '" "' + (Join-Path $root $source.Source) + '"'; $commands += 'if errorlevel 1 exit /b %errorlevel%' }
$commands | Set-Content -LiteralPath $batch -Encoding ascii
& cmd.exe /d /c $batch 2>&1 | Tee-Object -FilePath (Join-Path $build 'compile.log')
if ($LASTEXITCODE -ne 0) { throw "S27 compile failed: $LASTEXITCODE" }
$exe = Join-Path $build 'ntdos64-native.exe'; $response = Join-Path $build 'link.rsp'; $emitted=@{}
$lines = foreach ($line in Get-Content -LiteralPath (Join-Path $baseline 'link.rsp')) {
    if ($line -match '\\fixture\.obj"$') { '"' + (Join-Path $build 'native-cli.obj') + '"'; continue }
    if ($line -match '\\bridge\.obj"$') { '"' + (Join-Path $build 'generic-ud-bridge.obj') + '"'; continue }
    if ($line -match '^/OUT:') { '/OUT:"' + $exe + '"'; continue }
    if ($line -match '\\([^\\"]+)\.obj"$') { $base=[IO.Path]::GetFileNameWithoutExtension($matches[1]); if($current.ContainsKey($base)){ $emitted[$base]=$true; '"'+$current[$base]+'"'; continue } }
    $line
}
foreach ($base in ($current.Keys | Sort-Object)) { if(-not $emitted.ContainsKey($base)) { $lines += '"'+$current[$base]+'"' } }
foreach ($name in ($sources | Where-Object { $_.Object -ne 'native-cli.obj' -and $_.Object -ne 'generic-ud-bridge.obj' } | ForEach-Object { $_.Object })) { $lines += '"' + (Join-Path $build $name) + '"' }
# The current host namespace is a documented user-mode NTDLL seam.  Preserve
# this import in the native CLI closure rather than relying on an old response.
$lines += 'ntdll.lib'
$lines += 'user32.lib'
$lines | Set-Content -LiteralPath $response -Encoding ascii
& cmd.exe /d /s /c ('call "' + $vs + '" -arch=x64 -host_arch=x64 >nul && link.exe @"' + $response + '"') 2>&1 | Tee-Object -FilePath (Join-Path $build 'link.log')
if ($LASTEXITCODE -ne 0) { throw "S27 link failed: $LASTEXITCODE" }
& cmd.exe /d /s /c ('"' + $exe + '" --byob-profile "' + $profile + '" --byob-root "' + $byobRoot + '" --include-drives c "' + $target + '" 1>"' + (Join-Path $build 'run.stdout.log') + '" 2>"' + (Join-Path $build 'run.stderr.log') + '"')
$runExit=$LASTEXITCODE; Get-Content (Join-Path $build 'run.stdout.log'),(Join-Path $build 'run.stderr.log') | Tee-Object -FilePath (Join-Path $build 'run.log')
@{schema='ntdos64.t200.s27.native-cli.v1';architecture='x64';runtimeLibrary='/MT';environmentTransport=$false;legacyBochsShell=$false;runExitCode=$runExit;expectedRunExitCode=$ExpectedRunExitCode;passed=($runExit -eq $ExpectedRunExitCode)} | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $build 't200-s27-native-cli.json') -Encoding utf8
if ($runExit -ne $ExpectedRunExitCode) { throw "S27 native CLI failed: $runExit (expected $ExpectedRunExitCode)" }
Write-Host "T200 S27 native CLI probe passed: $build"
