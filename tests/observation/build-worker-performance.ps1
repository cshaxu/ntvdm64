param(
    [string]$Cache='build/M0-T427/S2/r001',
    [Parameter(Mandatory)][string]$BuildRoot,
    [string]$Environment='build/M0-T427/S4/r049/msvc-x86.cmd'
)
$ErrorActionPreference='Stop'
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
foreach($source in @('worker_performance','mouse_bridge_measured','console_client_measured','console_text_measured','frontend_session_measured')){
    $object=Join-Path $root ($source+'.obj')
    & $environmentPath cl.exe /nologo /c /MT /W4 /WX /wd4201 /std:c11 /D_CRT_SECURE_NO_WARNINGS /TC "/I$repo/src" "/I$repo/src/ntcon-exe" "/I$cachePath/obj/basesrv" "/Fo$object" (Join-Path $PSScriptRoot ($source+'.c'))
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
$frontendEntry=@($graph | Where-Object {$_ -match '^build ntcon.exe: frontend_link '})
if($frontendEntry.Count -ne 1){throw 'Selected frontend link missing/ambiguous'}
$frontendInputs=@(($frontendEntry[0] -replace '^.*: frontend_link ','') -split ' ' |
    Where-Object {$_ -ne 'obj/frontend/frontend_session.obj'} | ForEach-Object {(Resolve-Path (Join-Path $cachePath $_)).Path})
$frontendOutput=Join-Path $root 'ntcon-performance-observer.exe'
& $environmentPath link.exe /nologo /subsystem:console /entry:wmainCRTStartup /opt:ref "/out:$frontendOutput" "/map:$frontendOutput.map" (Join-Path $root 'worker_performance.obj') (Join-Path $root 'frontend_session_measured.obj') @frontendInputs rpcrt4.lib ntdll.lib kernel32.lib shell32.lib user32.lib gdi32.lib advapi32.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib
if($LASTEXITCODE){throw 'Measured frontend link failed'}
[ordered]@{
    Source=(& git rev-parse HEAD);Graph=(Get-FileHash (Join-Path $cachePath 'build.ninja')).Hash
    MeasurementSources=@(@('worker_performance.c','worker_performance.h','mouse_bridge_measured.c','console_client_measured.c','console_text_measured.c','frontend_session_measured.c') | ForEach-Object {
        [ordered]@{Path=$_;Hash=(Get-FileHash (Join-Path $PSScriptRoot $_)).Hash}
    })
    Profile='MSVC x86 /MT CCPU40; selected cache link; test-only producer/transport/frontend wrappers'
    Output=(Get-FileHash $output).Hash
    FrontendOutput=(Get-FileHash $frontendOutput).Hash
    Inputs=@($linkInputs | ForEach-Object {[ordered]@{Path=$_;Hash=(Get-FileHash $_).Hash}})
    FrontendInputs=@($frontendInputs | ForEach-Object {[ordered]@{Path=$_;Hash=(Get-FileHash $_).Hash}})
    Measurements=@('worker mouse queue enqueue/merge/IRQ consumption','worker read batches','text assembly','complete video transfer','frontend video commit and nested Console buffer create/read/write/resize','frontend decode/present calls','handoff barrier and I/O-close acknowledgement')
    Excluded=@('raw-input hardware latency','guest callback execution time','physical display latency')
}|ConvertTo-Json -Depth 6|Set-Content (Join-Path $root 'manifest.json') -Encoding UTF8
Write-Output "PASS built test-only measured worker: $output"
