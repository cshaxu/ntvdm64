[CmdletBinding()]
param([Parameter(Mandatory)][string]$WorkerBuildRoot)
$ErrorActionPreference='Stop'
$root=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$graph=Get-Content (Join-Path $WorkerBuildRoot 'build.ninja') -Raw
$line=[regex]::Match($graph,'(?m)^build VDMREDIR\.dll[^\r\n]*')
if(!$line.Success -or $line.Value -notmatch 'ntvdm\.lib' -or
    $line.Value -match '(?:common-worker-thread|compat-worker-thread|lib-thread)\.lib' -or
    $graph -match 'src/common/worker-thread/bridge\.c|src/ntvdm-exe/win32/thread_start_compat\.c') {
    throw 'VDMREDIR must use the worker-owned thread entry'
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
try{Assert-Independent ($thread+"`n#include `"ntvdm-exe/session/session.h`"")}catch{$rejected=$true}
if(!$rejected){throw 'Product dependency negative control accepted'}
$compat=(Get-Content (Join-Path $root 'src/compat/thread_start.c') -Raw) +
    (Get-Content (Join-Path $root 'src/compat/include/thread_lifecycle.h') -Raw)
if($compat -match 'common/|worker-base/|ntvdm-exe/|mvdm-host/|session_thread_') {
    throw 'Compatibility thread bridge depends on an outer owner'
}
if((Get-Content (Join-Path $root 'src/ntvdm-exe/softpc/mvdm_standalone_worker.c') -Raw) -notmatch 'opennt_thread_initialize\(\)' -or
   (Get-Content (Join-Path $root 'src/vdmredir-dll/source/mvdm_redirector_dll_entry.c') -Raw) -match 'thread_initialize') {
    throw 'Thread compatibility must be initialized once in the worker'
}
Write-Output 'PASS VDMREDIR uses the worker-owned thread closure; generic-library negative control passed'
