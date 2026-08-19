param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$BuildRoot,
    [Parameter(Mandatory = $true)][string]$CompositionBuildRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = [IO.Path]::GetFullPath($BuildRoot)
$inputComposition = [IO.Path]::GetFullPath($CompositionBuildRoot)
$composition = Join-Path $build 'composition'
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$baseline = Join-Path $root 'artifacts\build\t198-s74-dem-pdb-termination-r1'
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build directory: $build" }
foreach ($path in @($vs, $baseline, $inputComposition,
    (Join-Path $inputComposition 'current-objects'),
    (Join-Path $inputComposition 'prepared\profile-v7.json'))) {
    if (-not (Test-Path -LiteralPath $path)) { throw "Missing T213 S4 input: $path" }
}
New-Item -ItemType Directory -Path $composition | Out-Null
Copy-Item -LiteralPath (Join-Path $inputComposition 'current-objects') -Destination $composition -Recurse
New-Item -ItemType Directory -Path (Join-Path $composition 'prepared') | Out-Null
Copy-Item -LiteralPath (Join-Path $inputComposition 'prepared\profile-v7.json') -Destination (Join-Path $composition 'prepared\profile-v7.json')
$config = Join-Path $baseline 'native-core\config.h'
$includes = @('src','src\cli','refs\bochs','refs\bochs\instrument\stubs','src\bx-core','src\bx-core\cpu','src\bx-mantle','src\bx-vdm') | ForEach-Object { '/I "' + (Join-Path $root $_) + '"' }
$objects = Join-Path $composition 'current-objects'
$sources = @(
    @{Language='C++'; Source='src\bx-mantle\bx_ntvdm_cancellation_controller_v1.cc'},
    @{Language='C'; Source='src\bx-vdm\bop-v1\bx_ntvdm_command_lifecycle_provider_v1.c'},
    @{Language='C'; Source='src\bx-vdm\bop-v1\bx_ntvdm_command_initial_environment_v1.c'},
    @{Language='C'; Source='src\bx-vdm\bop-v1\bx_ntvdm_boot_namespace_composition_v1.c'},
    @{Language='C'; Source='src\bx-vdm\bop-v1\bx_ntvdm_composition_runtime_v1.c'}
)
$commands = @('@echo off', ('call "' + $vs + '" -arch=x64 -host_arch=x64 >nul'), 'if errorlevel 1 exit /b %errorlevel%')
foreach ($entry in $sources) {
    $object = Join-Path $objects (([IO.Path]::GetFileNameWithoutExtension($entry.Source)) + '.obj')
    $compiler = if ($entry.Language -eq 'C++') { 'cl.exe /nologo /TP /c /std:c++14 /W4 /WX /MT' } else { 'cl.exe /nologo /TC /c /std:c11 /W4 /WX /MT /DWIN32 /D_CRT_SECURE_NO_WARNINGS' }
    $commands += $compiler + ' ' + ($includes -join ' ') + ' /FI "' + $config + '" /Fo"' + $object + '" "' + (Join-Path $root $entry.Source) + '"'
    $commands += 'if errorlevel 1 exit /b %errorlevel%'
}
$batch = Join-Path $build 'compile-current.cmd'
$commands | Set-Content -LiteralPath $batch -Encoding ascii
& cmd.exe /d /c $batch 2>&1 | Tee-Object -LiteralPath (Join-Path $build 'compile-current.log')
if ($LASTEXITCODE -ne 0) { throw "T213 S4 current-object compile failed: $LASTEXITCODE" }
& (Join-Path $root 'tools\probe\Invoke-T200S27NativeCliProbe.ps1') `
    -RepositoryRoot $root -BuildRoot (Join-Path $build 'native-cli') -CompositionBuildRoot $composition `
    -AdditionalCliSources @('src\cli\ntdos64_lifecycle_v1.c',
        'src\cli\ntdos64_console_cancellation_v1.c',
        'src\cli\ntdos64_engine_worker_v1.c') -ExpectedRunExitCode 3
$native = Join-Path $build 'native-cli'
$base = Get-Content -LiteralPath (Join-Path $native 't200-s27-native-cli.json') -Raw | ConvertFrom-Json
$runLog = Get-Content -LiteralPath (Join-Path $native 'run.log') -Raw
if (-not $base.passed -or $runLog -notmatch 'terminal=2 detail=1 lifecycle=3 presentation=3 cancellation=0') {
    throw 'T213 S4 observation did not retain its declared composition-rejection baseline.'
}
$record = [ordered]@{schema='ntdos64.t213.s4.native-terminal-observation.v1';architecture='x64';runtimeLibrary='/MT';rebuiltCurrentObjects=@($sources.Source);runExitCode=3;reachedCommandTerminal=$false;result='pre-execution composition rejection';passed=$true}
$record | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $build 't213-s4-native-terminal-observation.json') -Encoding utf8
Write-Host "T213 S4 native terminal observation passed: $build"
