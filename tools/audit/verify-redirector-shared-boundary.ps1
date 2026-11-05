[CmdletBinding()]
param([Parameter(Mandatory)][string]$WorkerBuildRoot)
$ErrorActionPreference='Stop'
$graph=Get-Content (Join-Path $WorkerBuildRoot 'build.ninja') -Raw
$link=[regex]::Match($graph,'(?m)^build VDMREDIR\.dll[^\r\n]*')
if(!$link.Success -or $link.Value -notmatch 'common-opennt-compat\.lib' -or
    $link.Value -match 'worker-base\.lib|softpc-bindings\.lib|softpc-win32-bindings\.lib') {
    throw 'Private worker static archive listed on VDMREDIR link'
}
if($graph -notmatch '(?m)^build common-opennt-compat\.lib: lib obj/opennt-abi-host-compat/opennt_support_rtl\.obj\r?$' -or
   $graph -notmatch '(?m)^build obj/opennt-abi-host-compat/opennt_support_rtl\.obj: cc [^\r\n]*src/compat/opennt_support_rtl\.c\r?$') {
    throw 'Shared OpenNT compatibility provider does not use the established source'
}
$targets=@([regex]::Matches($graph,'(?m)^build (obj/(?:redir|vdmredir-dll)/[^ :]+\.obj): cc ') |
    ForEach-Object {$_.Groups[1].Value})
if($targets.Count -lt 18){throw "Too few Redir objects: $($targets.Count)"}
function Assert-Dependencies([string]$deps) {
    if($deps -match 'src/(?:ntvdm-exe|worker-base)/'){
        throw 'Private worker header reached by VDMREDIR'
    }
}
foreach($target in $targets){
    $deps=@(& ninja -C $WorkerBuildRoot -t deps $target 2>&1)
    if($LASTEXITCODE -or ($deps -join "`n") -notmatch '#deps \d+.*\(VALID\)'){
        throw "Missing compiler dependencies: $target"
    }
    Assert-Dependencies (($deps -join "`n").Replace('\','/'))
}
$rejected=$false
try{Assert-Dependencies 'src/ntvdm-exe/redir/include/mvdm_redirector_async.h'}catch{$rejected=$true}
if(!$rejected){throw 'Private-header negative control accepted'}
Write-Output "PASS $($targets.Count) VDMREDIR objects have no worker-private headers; negative control rejected"
