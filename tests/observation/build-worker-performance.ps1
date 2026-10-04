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
foreach($source in @('worker_performance','mouse_bridge_measured','console_client_measured')){
    $object=Join-Path $root ($source+'.obj')
    & $environmentPath cl.exe /nologo /c /MT /W4 /WX /TC "/I$repo/src" "/I$cachePath/obj/basesrv" "/Fo$object" (Join-Path $PSScriptRoot ($source+'.c'))
    if($LASTEXITCODE){throw "Measurement compile failed: $source"}
    $objects+=$object
}
$linkInputs=@($inputs | ForEach-Object {(Resolve-Path (Join-Path $cachePath $_)).Path})
$output=Join-Path $root 'ntvdm-performance-observer.exe'
$map=$output+'.map'
& $environmentPath link.exe /nologo /subsystem:console /opt:ref "/out:$output" "/map:$map" "/def:$cachePath/generated/ntvdm-wow32-provider.def" @objects @linkInputs rpcrt4.lib kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib libcmt.lib libvcruntime.lib libucrt.lib
if($LASTEXITCODE){throw 'Measurement link failed'}
& 'C:/Users/neko/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/bin/node.exe' "$repo/tools/audit/Verify-VdmTibStorage.mjs" $map "$cachePath/obj/adapter-monitor/mvdm_vdm_tib.obj"
if($LASTEXITCODE){throw 'VDM_TIB identity failed'}
[ordered]@{
    Source=(& git rev-parse HEAD);Graph=(Get-FileHash (Join-Path $cachePath 'build.ninja')).Hash
    MeasurementSources=@(@('worker_performance.c','worker_performance.h','mouse_bridge_measured.c','console_client_measured.c') | ForEach-Object {
        [ordered]@{Path=$_;Hash=(Get-FileHash (Join-Path $PSScriptRoot $_)).Hash}
    })
    Profile='MSVC x86 /MT CCPU40; selected cache link; three test-only TUs ahead of archives'
    Output=(Get-FileHash $output).Hash
    Inputs=@($linkInputs | ForEach-Object {[ordered]@{Path=$_;Hash=(Get-FileHash $_).Hash}})
    Measurements=@('worker mouse queue enqueue/merge/IRQ consumption','worker read batches','complete video transfer','handoff barrier and I/O-close acknowledgement')
    Excluded=@('raw-input hardware latency','guest callback execution time','frontend reconstruction versus painting split')
}|ConvertTo-Json -Depth 6|Set-Content (Join-Path $root 'manifest.json') -Encoding UTF8
Write-Output "PASS built test-only measured worker: $output"
