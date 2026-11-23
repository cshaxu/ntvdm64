param([Parameter(Mandatory)][string]$BuildCache)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$cache=(Resolve-Path (Join-Path $repo $BuildCache)).Path
if(!$cache.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase)) {
    throw 'Build-owned cache required'
}
& "$repo/tools/build-fixtures.cmd" x86 win101-control-fixtures $cache *> "$cache/control-fixtures.log"
if($LASTEXITCODE){throw 'Control fixture rebuild failed'}
'PASS control fixtures rebuilt against the current application identity'
