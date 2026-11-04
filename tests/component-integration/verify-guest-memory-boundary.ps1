[CmdletBinding()]
param([Parameter(Mandatory)][string]$WorkerBuildRoot,
    [Parameter(Mandatory)][string]$WowBuildRoot)
$ErrorActionPreference='Stop'
function Get-Dependencies([string]$root,[string]$target) {
    $output=@(& ninja -C $root -t deps $target 2>&1)
    if($LASTEXITCODE -ne 0 -or $output -match 'deps not found') {
        throw "Missing compiler dependency data for $target"
    }
    return (($output -join "`n").Replace('\','/'))
}
function Assert-ConsumerBoundary([string]$text,[string]$shared,[string[]]$private) {
    if(!$text.Contains($shared)){throw "Missing shared contract: $shared"}
    foreach($path in $private) {
        if($text.Contains($path)){throw "Private guest-memory contract leaked: $path"}
    }
}
$wow=Get-Dependencies $WowBuildRoot 'obj/wow32-dll/wow_callback_frame_lease.obj'
$redir=Get-Dependencies $WorkerBuildRoot 'obj/vdmredir-dll/mvdm_redirector_guest_copy.obj'
$wowPrivate=@('src/ntvdm-exe/session/guest_memory_lease.h',
    'src/ntvdm-exe/softpc/include/mvdm_softpc_guest_memory.h')
$redirPrivate=@('src/ntvdm-exe/redir/include/mvdm_redirector_worker_copy.h',
    'src/common/guest-memory/redirector.h')
Assert-ConsumerBoundary $wow 'src/common/guest-memory/access.h' $wowPrivate
Assert-ConsumerBoundary $redir 'src/common/guest-memory/access.h' $redirPrivate
foreach($case in @(@($wow,'src/common/guest-memory/access.h',$wowPrivate[0]),
                    @($redir,'src/common/guest-memory/access.h',$redirPrivate[0]),
                    @($redir,'src/common/guest-memory/access.h',$redirPrivate[1]))) {
    $rejected=$false
    try { Assert-ConsumerBoundary ($case[0]+"`n"+$case[2]) $case[1] @($case[2]) } catch { $rejected=$true }
    if(!$rejected){throw "Private-header negative control accepted: $($case[2])"}
}
Write-Output 'PASS shared guest-memory compiler dependencies and three forbidden-header negative controls'
