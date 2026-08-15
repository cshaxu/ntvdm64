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
foreach ($path in @($inputComposition, $vs, $baseline,
    (Join-Path $inputComposition 'current-objects'),
    (Join-Path $inputComposition 'prepared\profile-v7.json'),
    (Join-Path $root 'src\bx-mantle\bx_ntvdm_cancellation_controller_v1.cc'))) {
    if (-not (Test-Path -LiteralPath $path)) { throw "Missing T212 S6 native input: $path" }
}
New-Item -ItemType Directory -Path $composition | Out-Null
Copy-Item -LiteralPath (Join-Path $inputComposition 'current-objects') -Destination $composition -Recurse
New-Item -ItemType Directory -Path (Join-Path $composition 'prepared') | Out-Null
Copy-Item -LiteralPath (Join-Path $inputComposition 'prepared\profile-v7.json') -Destination (Join-Path $composition 'prepared\profile-v7.json')
$controllerObject = Join-Path $composition 'current-objects\bx_ntvdm_cancellation_controller_v1.obj'
$config = Join-Path $baseline 'native-core\config.h'
$includes = @('src','refs\bochs','refs\bochs\instrument\stubs','src\bx-core','src\bx-core\cpu','src\bx-mantle','src\bx-vdm','src\cli') | ForEach-Object { '/I "' + (Join-Path $root $_) + '"' }
$compile = 'call "' + $vs + '" -arch=x64 -host_arch=x64 >nul && cl.exe /nologo /TP /c /std:c++14 /W4 /WX /MT ' + ($includes -join ' ') + ' /FI "' + $config + '" /Fo"' + $controllerObject + '" "' + (Join-Path $root 'src\bx-mantle\bx_ntvdm_cancellation_controller_v1.cc') + '"'
& cmd.exe /d /s /c $compile 2>&1 | Tee-Object -LiteralPath (Join-Path $build 'controller-compile.log')
if ($LASTEXITCODE -ne 0) { throw "T212 S6 current controller compile failed: $LASTEXITCODE" }
& (Join-Path $root 'tools\probe\Invoke-T200S27NativeCliProbe.ps1') `
    -RepositoryRoot $root -BuildRoot (Join-Path $build 'native-cli') -CompositionBuildRoot $composition `
    -AdditionalCliSources @('src\cli\ntdos64_lifecycle_v1.c',
        'src\cli\ntdos64_console_cancellation_v1.c',
        'src\cli\ntdos64_engine_worker_v1.c') -ExpectedRunExitCode 3
$nativeBuild = Join-Path $build 'native-cli'
$base = Get-Content -LiteralPath (Join-Path $nativeBuild 't200-s27-native-cli.json') -Raw | ConvertFrom-Json
if (-not $base.passed -or $base.runExitCode -ne 3) {
    throw 'T212 S6 native CLI did not retain the declared pre-execution composition rejection.'
}
$runLog = Get-Content -LiteralPath (Join-Path $nativeBuild 'run.log') -Raw
if ($runLog -notmatch 'terminal=.+lifecycle=.+presentation=.+cancellation=0') {
    throw 'T212 S6 native CLI did not emit its uncancelled joined lifecycle result.'
}
$record = [ordered]@{schema='ntdos64.t212.s6.native-cli-cancellation.v1';architecture='x64';runtimeLibrary='/MT';sourceClosure=@('src/cli/ntdos64_native_cli.c','src/cli/ntdos64_console_cancellation_v1.[hc]','src/cli/ntdos64_engine_worker_v1.[hc]','src/cli/ntdos64_lifecycle_v1.[hc]','existing current composition objects');engineHandleTransfer=$false;engineCallbackTransfer=$false;guestExecution=$false;runExitCode=3;expectedRunExitCode=3;passed=$true}
$record | ConvertTo-Json -Depth 3 | Set-Content -LiteralPath (Join-Path $build 't212-s6-native-cli-cancellation.json') -Encoding utf8
Write-Host "T212 S6 native CLI cancellation probe passed: $build"
