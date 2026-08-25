[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$BuildRoot,
    [switch]$Resume
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = [IO.Path]::GetFullPath($BuildRoot)
if ((Test-Path -LiteralPath $build) -and !$Resume) {
    throw "Refusing to overwrite existing build root: $build"
}
if ($Resume -and !(Test-Path -LiteralPath $build -PathType Container)) {
    throw "Cannot resume a missing build root: $build"
}

$seed = Join-Path $root 'tools\probe\Invoke-T197S6MinimalMachineLinkProbe.ps1'
$manifestPath = Join-Path $root 'tools\build\t198-s50-bx-vdm-composition-manifest.json'
$actionSource = Join-Path $root 'src\bx-mantle\bx_ntvdm_mechanical_action_v1.cc'
$genericUdBridgeSource = Join-Path $root 'src\bx-mantle\bx_ntvdm_generic_ud_bridge.cc'
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
foreach ($input in @($seed, $manifestPath, $actionSource, $genericUdBridgeSource, $vs)) {
    if (-not (Test-Path -LiteralPath $input -PathType Leaf)) {
        throw "Required T220 S5 source-closure input missing: $input"
    }
}

$nativeCore = Join-Path $build 'native-core'
if (!$Resume) {
    & powershell.exe -ExecutionPolicy Bypass -File $seed -RepositoryRoot $root `
        -BuildRoot $nativeCore -WholeCpu5Core -HostArchitecture x64
    if ($LASTEXITCODE -ne 0) { throw "T220 S5 CPU5 source seed failed: $LASTEXITCODE" }
} elseif (!(Test-Path -LiteralPath (Join-Path $nativeCore 't197-s6-minimal-machine-link-probe.json'))) {
    throw "Cannot resume without a completed T197 CPU5 source seed: $nativeCore"
}

$config = Join-Path $nativeCore 'config.h'
$objects = Join-Path $build 'composition-objects'
New-Item -ItemType Directory -Force -Path $objects | Out-Null
$manifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
$seedOwnedSources = @(
    'src/bx-mantle/bx_ntvdm_a20_capability_v1.cc',
    'src/bx-core/memory/misc_mem.cc'
)
$sources = @($manifest.compileSources | Where-Object {
    $_ -notin @($manifest.compileOnlySources) -and $_ -notin $seedOwnedSources
})
$includes = @('src', 'src\cli', 'src\bx-vdm', 'src\bx-mantle', 'refs\bochs',
    'refs\bochs\instrument\stubs', 'src\bx-core', 'src\bx-core\cpu',
    'refs\bochs\iodev') | ForEach-Object { '/I "' + (Join-Path $root $_) + '"' }
$current = [ordered]@{}
$commands = [System.Collections.Generic.List[string]]::new()
foreach ($relative in $sources) {
    $source = Join-Path $root $relative
    if (-not (Test-Path -LiteralPath $source -PathType Leaf)) {
        throw "Declared composition source missing: $relative"
    }
    $base = [IO.Path]::GetFileNameWithoutExtension($relative)
    if ($current.Contains($base)) { throw "Duplicate composition object base: $base" }
    $object = Join-Path $objects ($base + '.obj')
    $language = if ([IO.Path]::GetExtension($relative) -ieq '.c') {
        '/TC /std:c11 /W4 /WX /D_CRT_SECURE_NO_WARNINGS'
    } else {
        '/TP /std:c++14 /EHsc /Gy'
    }
    $commands.Add('cl.exe /nologo /c /MT /DWIN32 ' + $language + ' ' +
        ($includes -join ' ') + ' /FI "' + $config + '" /Fo"' + $object +
        '" "' + $source + '"')
    $commands.Add('if errorlevel 1 exit /b %errorlevel%')
    $current[$base] = $object
}
$actionObject = Join-Path $build 'mechanical-action.obj'
$commands.Add('cl.exe /nologo /c /MT /DWIN32 /TP /std:c++14 /EHsc /Gy ' +
    ($includes -join ' ') + ' /FI "' + $config + '" /Fo"' + $actionObject +
    '" "' + $actionSource + '"')
$commands.Add('if errorlevel 1 exit /b %errorlevel%')
$genericUdBridgeObject = Join-Path $build 'generic-ud-bridge.obj'
$commands.Add('cl.exe /nologo /c /MT /DWIN32 /TP /std:c++14 /EHsc /Gy ' +
    ($includes -join ' ') + ' /FI "' + $config + '" /Fo"' + $genericUdBridgeObject +
    '" "' + $genericUdBridgeSource + '"')
$commands.Add('if errorlevel 1 exit /b %errorlevel%')
$batch = Join-Path $build 'compile-composition.cmd'
@('@echo off', ('call "' + $vs + '" -arch=x64 -host_arch=x64 >nul'),
    'if errorlevel 1 exit /b %errorlevel%') + @($commands) |
    Set-Content -LiteralPath $batch -Encoding ascii
& cmd.exe /d /s /c ('call "' + $batch + '"') 2>&1 |
    Tee-Object -FilePath (Join-Path $build 'compile.log')
if ($LASTEXITCODE -ne 0) { throw "T220 S5 composition source compilation failed: $LASTEXITCODE" }

$coreObjects = Get-ChildItem -LiteralPath $nativeCore -Filter '*.obj' -File |
    Where-Object { $_.Name -ne 'minimal_machine_link_probe.obj' } |
    ForEach-Object { $_.FullName }
if ($coreObjects.Count -eq 0) { throw 'T220 S5 source seed emitted no reusable core objects' }
$dll = Join-Path $build 't220-s5-source-composition-audit.dll'
$response = Join-Path $build 'link.rsp'
@('/nologo', '/DLL', '/NOENTRY', ('/OUT:"' + $dll + '"'), '/OPT:REF') +
    @($coreObjects) + @($current.Values) + @($actionObject, $genericUdBridgeObject) + @(
        'libcmt.lib', 'libvcruntime.lib', 'libucrt.lib',
        'legacy_stdio_definitions.lib', 'kernel32.lib', 'bcrypt.lib', 'ntdll.lib',
        'user32.lib') | ForEach-Object {
            if ($_ -match '\.obj$') { '"' + $_ + '"' } else { $_ }
        } | Set-Content -LiteralPath $response -Encoding ascii
& cmd.exe /d /s /c ('call "' + $vs + '" -arch=x64 -host_arch=x64 >nul && link.exe @"' + $response + '"') 2>&1 |
    Tee-Object -FilePath (Join-Path $build 'link.log')
$linkExit = $LASTEXITCODE
$record = [ordered]@{
    schema = 'ntdos64.t220.s5.source-composition-closure.v1'
    architecture = 'x64'
    runtimeLibrary = '/MT'
    machineProfile = 'CPU5/Pentium-MMX, non-x86-64'
    coreSeed = 'Invoke-T197S6MinimalMachineLinkProbe.ps1 -WholeCpu5Core -HostArchitecture x64'
    compositionManifest = 'tools/build/t198-s50-bx-vdm-composition-manifest.json'
    compiledCompositionSources = $sources.Count
    nativeExecution = $false
    forbiddenInputs = @('artifacts/build/t198-s74-dem-pdb-termination-r1', 'bochs.exe', 'main.cc', 'config.cc', 'GUI/plugin/device archives')
    linkExitCode = $linkExit
    linkSucceeded = ($linkExit -eq 0)
    logs = @{ compile = 'compile.log'; link = 'link.log'; response = 'link.rsp' }
}
$record | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $build 't220-s5-source-composition-closure.json') -Encoding utf8
if ($linkExit -ne 0) { throw "T220 S5 source composition link failed: $linkExit" }
Write-Host "T220 S5 source-only composition closure linked: $dll"
