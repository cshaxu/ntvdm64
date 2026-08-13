[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$BuildRoot,
    [ValidateSet('x64')][string]$HostArchitecture = 'x64'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$repository = [IO.Path]::GetFullPath($RepositoryRoot)
$build = [IO.Path]::GetFullPath($BuildRoot)
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build root: $build" }
$vsDevCmd = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$coreProbe = Join-Path $repository 'tools\Invoke-T197S6MinimalMachineLinkProbe.ps1'
$legacyResponse = Join-Path $repository 'artifacts\build\t198-s25-native-command-r1\link.rsp'
$compositionManifest = Join-Path $repository 'tools\t198-s50-bx-vdm-composition-manifest.json'
$ntioBytes = Join-Path $repository 'artifacts\build\t198-s23-native-ntio-r1\ntio_bytes.cc'
$ntdosBytes = Join-Path $repository 'artifacts\build\t198-s25-native-ntdos-r1\ntdos_bytes.cc'
foreach ($input in @($vsDevCmd, $coreProbe, $legacyResponse, $compositionManifest, $ntioBytes, $ntdosBytes)) {
    if (-not (Test-Path -LiteralPath $input)) { throw "Required S48 input missing: $input" }
}

New-Item -ItemType Directory -Path $build | Out-Null
New-Item -ItemType Directory -Path (Join-Path $build 'obj') | Out-Null

# The shared CPU5 closure remains the authoritative source inventory.  The
# history switch is a trace-only compile option required by the unchanged S48
# fixture; it introduces no guest or host behavior.
$savedCl = $env:CL
try {
    $env:CL = (($savedCl, '/DBX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY=1' | Where-Object { $_ }) -join ' ')
    & powershell.exe -ExecutionPolicy Bypass -File $coreProbe -RepositoryRoot $repository -BuildRoot (Join-Path $build 'native-core') -WholeCpu5Core -InstructionHistory -HostArchitecture $HostArchitecture
    if ($LASTEXITCODE -ne 0) { throw "S48 native CPU5 closure failed: $LASTEXITCODE" }
} finally { $env:CL = $savedCl }

$compileLog = Join-Path $build 'compile.log'
$oldProviderRoot = Join-Path $repository 'artifacts\build\t198-s25-command-launch-r2\obj'
$composition = Get-Content -LiteralPath $compositionManifest -Raw | ConvertFrom-Json
if ($composition.schema -ne 'ntdos64.t198.s50.bx-vdm-composition.v1') { throw 'Unexpected S50 composition manifest schema' }
$providerSources = @($composition.compileSources | ForEach-Object {
    Join-Path $repository $_.Replace('/', '\\')
})
foreach ($sourcePath in $providerSources) {
    if (-not (Test-Path -LiteralPath $sourcePath)) { throw "S50 composition source missing: $sourcePath" }
    $source = Get-Item -LiteralPath $sourcePath
    $object = Join-Path $build ('obj\' + [IO.Path]::GetFileNameWithoutExtension($source.Name) + '.obj')
    $providerCommand = 'call "' + $vsDevCmd + '" -arch=' + $HostArchitecture + ' -host_arch=x64 >nul && cl.exe /nologo /TC /c /std:c11 /W4 /WX /MT /DWIN32 /I "' + (Join-Path $repository 'src') + '" /I "' + (Join-Path $repository 'src\cli') + '" /I "' + (Join-Path $repository 'src\bx-vdm') + '" /I "' + (Join-Path $repository 'src\bx-mantle') + '" /Fo"' + $object + '" "' + $source.FullName + '"'
    & cmd.exe /d /s /c $providerCommand 2>&1 | Tee-Object -FilePath $compileLog -Append
    if ($LASTEXITCODE -ne 0) { throw "S50 provider source compile failed: $($source.Name)" }
}

$config = Join-Path $build 'native-core\config.h'
$includes = @('src', 'src\bochs', 'src\bochs\instrument\stubs', 'src\bx-core', 'src\bx-core\cpu', 'src\bx-mantle', 'src\bx-vdm', 'src\cli') | ForEach-Object { '/I "' + (Join-Path $repository $_) + '"' }
function Add-Cxx([hashtable]$item) {
    $object = Join-Path $build $item.Name
    $command = 'call "' + $vsDevCmd + '" -arch=' + $HostArchitecture + ' -host_arch=x64 >nul && cl.exe /nologo /c /std:c++14 /EHsc /MT /Gy /DWIN32 ' + ($includes -join ' ') + ' /FI "' + $config + '" ' + (($item.Defines | ForEach-Object { $_ }) -join ' ') + ' /Fo"' + $object + '" "' + $item.Source + '"'
    Write-Host "S48 C++ compile: $($item.Name)"
    & cmd.exe /d /s /c $command 2>&1 | Tee-Object -FilePath $compileLog -Append
    if ($LASTEXITCODE -ne 0) { throw "S48 C++ compile failed: $($item.Name)" }
}
function Add-C([hashtable]$item) {
    $object = Join-Path $build $item.Name
    $command = 'call "' + $vsDevCmd + '" -arch=' + $HostArchitecture + ' -host_arch=x64 >nul && cl.exe /nologo /TC /c /std:c11 /W4 /WX /MT /DWIN32 ' + ($includes -join ' ') + ' ' + (($item.Defines | ForEach-Object { $_ }) -join ' ') + ' /Fo"' + $object + '" "' + $item.Source + '"'
    Write-Host "S48 C compile: $($item.Name)"
    & cmd.exe /d /s /c $command 2>&1 | Tee-Object -FilePath $compileLog -Append
    if ($LASTEXITCODE -ne 0) { throw "S48 C compile failed: $($item.Name)" }
}

Add-Cxx (@{ Name = 'finite_run.obj'; Source = (Join-Path $repository 'src\bx-mantle\bx_ntvdm_finite_run.cc'); Defines = @() })
Add-Cxx (@{ Name = 'mechanical_action.obj'; Source = (Join-Path $repository 'src\bx-mantle\bx_ntvdm_mechanical_action_v1.cc'); Defines = @() })
Add-Cxx (@{ Name = 'generic_ud_bridge.obj'; Source = (Join-Path $repository 'src\bx-mantle\bx_ntvdm_generic_ud_bridge.cc'); Defines = @('/DBX_NTVDM_EXTERNAL_GENERIC_UD_BRIDGE=1') })
Add-Cxx (@{ Name = 'exception_mantle_ud.obj'; Source = (Join-Path $repository 'src\bx-core\cpu\exception.cc'); Defines = @('/DBX_NTVDM_ENABLE_MANTLE_UD_BRIDGE=1', '/DBX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY=1') })
Add-Cxx (@{ Name = 'builder.obj'; Source = (Join-Path $repository 'src\bx-vdm\bx_ntvdm_ntio_preentry_v1.cc'); Defines = @() })
Add-C (@{ Name = 'preentry_input.obj'; Source = (Join-Path $repository 'src\bx-vdm\bx_ntvdm_preentry_input_v1.c'); Defines = @() })
Add-Cxx (@{ Name = 'bytes.obj'; Source = $ntioBytes; Defines = @() })
Add-Cxx (@{ Name = 'ntdos_bytes.obj'; Source = $ntdosBytes; Defines = @() })
Add-Cxx (@{ Name = 'fixture.obj'; Source = (Join-Path $repository 'tests\bx-vdm\t198_s23_native_ntio_boundary_fixture.cc'); Defines = @('/DBX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY=1') })
Add-C (@{ Name = 'bridge.obj'; Source = (Join-Path $repository 'tests\bx-vdm\t198_s23_native_ntio_boundary_bridge.c'); Defines = @() })

$oldCore = Join-Path $repository 'artifacts\build\t198-s23-native-ntio-run-r1\native-core'
$oldRun = Join-Path $repository 'artifacts\build\t198-s23-native-ntio-run-r1'
$oldSnapshot = Join-Path $repository 'artifacts\build\t198-s24-terminal-snapshot-r1'
$oldNtio = Join-Path $repository 'artifacts\build\t198-s23-native-ntio-r1'
$oldNtdos = Join-Path $repository 'artifacts\build\t198-s25-native-ntdos-r1'
$response = Join-Path $build 'link.rsp'
$rewritten = Get-Content -LiteralPath $legacyResponse | ForEach-Object {
    $_.Replace($oldCore, (Join-Path $build 'native-core')).Replace($oldProviderRoot, (Join-Path $build 'obj')).Replace($oldRun, $build).Replace($oldSnapshot, $build).Replace($oldNtio, $build).Replace($oldNtdos, $build)
} | Where-Object { $_ -notmatch 'native-core\\whole_cpu_exception\.obj' }
$linkedProviderLeaves = @($rewritten | Where-Object { $_ -match '\\obj\\[^\\"]+\.obj"$' } | ForEach-Object {
    [IO.Path]::GetFileName($_.Trim('"'))
})
$compileOnlyLeaves = @($composition.compileOnlySources | ForEach-Object {
    [IO.Path]::GetFileNameWithoutExtension($_) + '.obj'
})
foreach ($sourcePath in $providerSources) {
    $leaf = [IO.Path]::GetFileNameWithoutExtension($sourcePath) + '.obj'
    if ($leaf -notin $linkedProviderLeaves -and $leaf -notin $compileOnlyLeaves) {
        $rewritten += '"' + (Join-Path $build ('obj\' + $leaf)) + '"'
    }
}
$rewritten += '"' + (Join-Path $build 'native-core\instruction_history.obj') + '"'
$rewritten += '"' + (Join-Path $build 'preentry_input.obj') + '"'
$rewritten | Set-Content -LiteralPath $response -Encoding ascii
$linkLog = Join-Path $build 'link.log'
& cmd.exe /d /s /c ('call "' + $vsDevCmd + '" -arch=' + $HostArchitecture + ' -host_arch=x64 >nul && link.exe @"' + $response + '"') 2>&1 | Tee-Object -LiteralPath $linkLog
if ($LASTEXITCODE -ne 0) { throw "S48 link failed: $LASTEXITCODE" }

$exe = Join-Path $build 'command-r1.exe'
$runLog = Join-Path $build 'run.log'
$savedErrorActionPreference = $ErrorActionPreference
try {
    # Bochs' ordinary diagnostic channel is stderr.  Preserve it as evidence;
    # it is not a PowerShell failure before the child has returned its status.
    $ErrorActionPreference = 'Continue'
    & $exe 2>&1 | Tee-Object -LiteralPath $runLog
    $runExit = $LASTEXITCODE
} finally { $ErrorActionPreference = $savedErrorActionPreference }
$record = [ordered]@{
    schema = 'ntdos64.t198.s48.clean-ntio-observation.v1'
    architecture = $HostArchitecture
    runtimeLibrary = '/MT'
    cpuProfile = 'CPU5/Pentium-MMX, non-x86-64'
    legacyFilesUsedAs = 'source-membership manifests only; no legacy object is linked'
    ntioInput = @{ path = $ntioBytes; sha256 = (Get-FileHash -LiteralPath $ntioBytes -Algorithm SHA256).Hash }
    ntdosInput = @{ path = $ntdosBytes; sha256 = (Get-FileHash -LiteralPath $ntdosBytes -Algorithm SHA256).Hash }
    runExitCode = $runExit
    logs = @{ compile = 'compile.log'; link = 'link.log'; run = 'run.log' }
}
$record | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $build 't198-s48-clean-ntio-observation.json') -Encoding utf8
Write-Host "S48 source-built NTIO observation completed with exit ${runExit}: $exe"
exit $runExit
