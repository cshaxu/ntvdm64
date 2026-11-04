[CmdletBinding()]
param([Parameter(Mandatory)][string]$WorkerBuildRoot)
$ErrorActionPreference='Stop'
$root=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$graph=Get-Content (Join-Path $WorkerBuildRoot 'build.ninja') -Raw
$line=[regex]::Match($graph,'(?m)^build VDMREDIR\.dll[^\r\n]*')
if(!$line.Success -or $line.Value -notmatch 'common-worker-thread\.lib' -or
    $line.Value -notmatch 'lib-thread\.lib' -or
    $graph -match 'src/ntvdm-exe/win32/thread_start_compat\.c') {
    throw 'VDMREDIR thread source closure is not shared'
}
function Assert-Independent([string]$text) {
    if($text -match 'ntvdm-exe/|worker-base/|common/') {
        throw 'Generic thread library depends on product code'
    }
}
$thread=(Get-Content (Join-Path $root 'src/lib-win32/thread/thread.c') -Raw) +
    (Get-Content (Join-Path $root 'src/lib-win32/thread/thread.h') -Raw)
Assert-Independent $thread
$rejected=$false
try{Assert-Independent ($thread+"`n#include `"common/worker-thread/session.h`"")}catch{$rejected=$true}
if(!$rejected){throw 'Product dependency negative control accepted'}
$bridge=Get-Content (Join-Path $root 'src/common/worker-thread/bridge.c') -Raw
if($bridge -match 'ntvdm-exe/|worker-base/'){throw 'Shared bridge uses private component'}
Write-Output 'PASS VDMREDIR shared thread closure and generic-library negative control'
