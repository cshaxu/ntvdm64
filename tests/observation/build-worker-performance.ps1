param(
    [string]$Cache='build/M0-T427/S2/r001',
    [Parameter(Mandatory)][string]$BuildRoot,
    [string]$Environment='build/M0-T427/S4/r049/msvc-x86.cmd',
    [switch]$WorkerOnly
)
$ErrorActionPreference='Stop'
function Get-Sha256([string]$Path) {
    $sha=[Security.Cryptography.SHA256]::Create()
    try {
        return ([BitConverter]::ToString($sha.ComputeHash([IO.File]::ReadAllBytes($Path)))).Replace('-','')
    } finally {
        $sha.Dispose()
    }
}
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$cachePath=(Resolve-Path $Cache).Path
$environmentPath=(Resolve-Path $Environment).Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)){throw 'Build output must remain under build/'}
if(Test-Path $root){throw 'Fresh measurement build required'}
$null=New-Item -ItemType Directory -Path $root
$graph=Get-Content (Join-Path $cachePath 'build.ninja')
$entry=@($graph | Where-Object {$_ -match '^build ntvdm.exe \| ntvdm.lib: worker_link '})
if($entry.Count -ne 1){throw 'Selected worker link missing/ambiguous'}
$inputs=($entry[0] -replace '^.*: worker_link ','') -split ' '
$objects=@()
$compileArgs=@('/nologo','/c','/MT','/O2','/W4','/WX','/wd4201','/std:c11',
    '/D_NO_CRT_STDIO_INLINE','/DWIN32','/DWINNT','/DOPENNT_ADAPTER_NT_ALERT_THREAD',
    '/DMVDM_SOFTPC_NO_HOST_BOOT_FILE_MUTATION','/DNTVDM','/DCPU_40_STYLE','/DNEW_CPU',
    '/DCCPU','/DC_VID','/DSPC386','/DSIM32','/DV7VGA','/DANSI','/DPROD',
    "/FI$repo/src/opennt-abi/host-compat/include/nt.h",
    "/FI$repo/src/ntvdm-exe/softpc/include/mvdm_softpc_symbol_compat.h",
    "/I$repo/src","/I$repo/src/ntcon-exe","/I$cachePath/obj/basesrv",
    "/I$repo/src/vdmredir-dll/include","/I$repo/src/ntvdm-exe/redir/include",
    "/I$repo/src/opennt-host/netapi/xactsrv","/I$repo/src/ntvdm-exe/vdd/include",
    "/I$repo/src/opennt-abi/host-compat/include","/I$repo/src/mvdm/softpc.new/base/inc",
    "/I$repo/src/opennt-host/public/sdk/inc","/I$repo/src/opennt-abi/source/public/sdk/inc",
    "/I$repo/src/opennt-abi/source/public/internal/base/inc",
    "/I$repo/src/opennt-abi/source/public/internal/windows/inc",
    "/I$repo/src/opennt-abi/source/public/internal/ds/inc",
    "/I$repo/src/opennt-abi/source/public/internal/net/inc",
    "/I$repo/src/opennt-abi/source/private/inc",
    "/I$repo/src/opennt-abi/source/private/ds/netapi/rpcxlate",
    "/I$repo/src/opennt-abi/source/private/windows/inc",
    "/I$repo/src/opennt-abi/source/public/ddk/inc","/I$repo/src/mvdm/inc",
    "/I$repo/src/ntvdm-exe/softpc/include/generated/x86/prod","/I$repo/src/mvdm/xms.486",
    "/I$repo/src/mvdm/dpmi32","/I$repo/src/mvdm/vdmredir",
    "/I$repo/src/mvdm/softpc.new/base/ccpu386","/I$repo/src/mvdm/softpc.new/host/inc",
    "/I$repo/src/mvdm/softpc.new/base/cvidc","/I$repo/src/mvdm/dos/dem",
    "/I$repo/src/ntvdm-exe/softpc/include","/I$repo/src/ntvdm-exe/command/include",
    "/I$repo/src/ntvdm-exe/monitor/include","/I$repo/src/wow32-dll/include",
    "/I$repo/src/ntvdm-exe/wow/include","/I$repo/src/ntvdm-exe/session")
$measurementSourceNames=@('worker_performance','mouse_bridge_measured','console_client_measured','console_text_measured','console_graphics_measured','frontend_session_measured')
if($WorkerOnly){$measurementSourceNames=@($measurementSourceNames | Where-Object {$_ -ne 'frontend_session_measured'})}
foreach($source in $measurementSourceNames){
    $object=Join-Path $root ($source+'.obj')
    & $environmentPath cl.exe @compileArgs "/Fo$object" (Join-Path $PSScriptRoot ($source+'.c'))
    if($LASTEXITCODE){throw "Measurement compile failed: $source"}
    if($source -ne 'frontend_session_measured'){$objects+=$object}
}
$linkInputs=@($inputs | ForEach-Object {(Resolve-Path (Join-Path $cachePath $_)).Path})
$output=Join-Path $root 'ntvdm-performance-observer.exe'
$map=$output+'.map'
& $environmentPath link.exe /nologo /subsystem:console /opt:ref "/out:$output" "/map:$map" "/def:$cachePath/generated/ntvdm-wow32-provider.def" @objects @linkInputs rpcrt4.lib kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib libcmt.lib libvcruntime.lib libucrt.lib
if($LASTEXITCODE){throw 'Measurement link failed'}
& 'C:/Users/neko/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/bin/node.exe' "$repo/tools/audit/Verify-VdmTibStorage.mjs" $map "$cachePath/obj/adapter-monitor/mvdm_vdm_tib.obj"
if($LASTEXITCODE){throw 'VDM_TIB identity failed'}
$frontendInputs=@();$frontendOutput=$null
if(!$WorkerOnly) {
    $frontendEntry=@($graph | Where-Object {$_ -match '^build ntcon.exe: frontend_link '})
    if($frontendEntry.Count -ne 1){throw 'Selected frontend link missing/ambiguous'}
    $frontendInputs=@(($frontendEntry[0] -replace '^.*: frontend_link ','') -split ' ' |
        Where-Object {$_ -ne 'obj/frontend/frontend_session.obj'} | ForEach-Object {(Resolve-Path (Join-Path $cachePath $_)).Path})
    $frontendOutput=Join-Path $root 'ntcon-performance-observer.exe'
    & $environmentPath link.exe /nologo /subsystem:console /entry:wmainCRTStartup /opt:ref "/out:$frontendOutput" "/map:$frontendOutput.map" (Join-Path $root 'worker_performance.obj') (Join-Path $root 'frontend_session_measured.obj') @frontendInputs rpcrt4.lib ntdll.lib kernel32.lib shell32.lib user32.lib gdi32.lib advapi32.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib
    if($LASTEXITCODE){throw 'Measured frontend link failed'}
}
[ordered]@{
    Source=(& git rev-parse HEAD);Graph=(Get-Sha256 (Join-Path $cachePath 'build.ninja'))
    MeasurementSources=@(@('worker_performance.c','worker_performance.h')+@($measurementSourceNames | ForEach-Object { $_+'.c' }) | ForEach-Object {
        [ordered]@{Path=$_;Hash=(Get-Sha256 (Join-Path $PSScriptRoot $_))}
    })
    Profile='MSVC x86 /MT /O2 CCPU40; selected cache link; test-only producer/transport/frontend wrappers'
    Output=(Get-Sha256 $output)
    FrontendOutput=if($frontendOutput){(Get-Sha256 $frontendOutput)}else{$null}
    Inputs=@($linkInputs | ForEach-Object {[ordered]@{Path=$_;Hash=(Get-Sha256 $_)}})
    FrontendInputs=@($frontendInputs | ForEach-Object {[ordered]@{Path=$_;Hash=(Get-Sha256 $_)}})
    Measurements=@('worker mouse queue enqueue/merge/IRQ consumption','worker read batches','text assembly','complete video transfer','frontend video commit and nested Console buffer create/read/write/resize','frontend decode/present calls','handoff barrier and I/O-close acknowledgement')
    Excluded=@('raw-input hardware latency','guest callback execution time','physical display latency')
}|ConvertTo-Json -Depth 6|Set-Content (Join-Path $root 'manifest.json') -Encoding UTF8
Write-Output "PASS built test-only measured worker: $output"
