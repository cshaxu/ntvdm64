param([Parameter(Mandatory)][string]$BuildRoot,
      [ValidateSet('x86','x64')][string]$Architecture='x86')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Require a fresh build-only evidence directory'}
New-Item -ItemType Directory $root | Out-Null
@("$repo/src/mvdm-host/softpc.new/host/src/nt_graph.c","$repo/test/ntvdm-exe/softpc/blocked_video_tick_test.c") | ForEach-Object {
    [pscustomobject]@{Path=$_;Sha256=(Get-FileHash $_).Hash}
} | ConvertTo-Json | Set-Content "$root/inputs.json"
& "$repo/tools/build-fixtures.cmd" $Architecture blocked-video-tick *> "$root/build.log"
if($LASTEXITCODE){throw 'Blocked tick fixture build failed'}
$native=Join-Path $repo "build/.ninja/fixtures-$Architecture"
Copy-Item "$native/blocked_video_tick.inc" "$root/blocked_video_tick.inc"
& "$native/blocked-video-tick.exe" *> "$root/result.txt"
if($LASTEXITCODE){throw 'Blocked tick fixture failed'}
Get-Content "$root/result.txt"
