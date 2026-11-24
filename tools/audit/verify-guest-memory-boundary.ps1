[CmdletBinding()]
param([Parameter(Mandatory)][string]$WorkerBuildRoot,
    [Parameter(Mandatory)][string]$WowBuildRoot)
$ErrorActionPreference='Stop'
. "$PSScriptRoot/native-source-graph.ps1"
function Get-Dependencies([string]$root,[string]$product,[string]$source) {
    $graph=Read-Graph $root
    if(!$graph.ContainsKey($product)){throw "Missing product target: $product"}
    $selected=[Collections.Generic.HashSet[string]]::new()
    $pending=[Collections.Generic.Stack[string]]::new()
    $pending.Push($product)
    while($pending.Count){
        $node=$pending.Pop()
        if($selected.Add($node) -and $graph.ContainsKey($node)){
            foreach($input in $graph[$node]){$pending.Push($input)}
        }
    }
    $objects=@($selected | Where-Object {
        $_ -match '\.obj$' -and @($graph[$_] | Where-Object {$_ -like "*/$source"}).Count
    })
    if($objects.Count -ne 1){throw "Expected one production compiler for $source; found $($objects.Count)"}
    $target=$objects[0]
    $output=@(& ninja -C $root -t deps $target 2>&1)
    if($LASTEXITCODE -ne 0 -or ($output -join "`n") -notmatch '#deps \d+.*\(VALID\)') {
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
$wow=Get-Dependencies $WowBuildRoot 'wow32.dll' 'src/compat/wow_callback_frame_lease.c'
$redir=Get-Dependencies $WorkerBuildRoot 'VDMREDIR.dll' 'src/compat/mvdm_redirector_guest_copy.c'
$wowPrivate=@('src/compat/include/guest_memory_lease.h',
    'src/ntvdm-exe/softpc/include/mvdm_softpc_guest_memory.h')
$redirPrivate=@('src/ntvdm-exe/redir/include/mvdm_redirector_worker_copy.h',
    'src/common/guest-memory/redirector.h')
Assert-ConsumerBoundary $wow 'src/compat/include/guest_memory_access.h' $wowPrivate
Assert-ConsumerBoundary $redir 'src/compat/include/guest_memory_access.h' $redirPrivate
foreach($case in @(@($wow,'src/compat/include/guest_memory_access.h',$wowPrivate[0]),
                    @($redir,'src/compat/include/guest_memory_access.h',$redirPrivate[0]),
                    @($redir,'src/compat/include/guest_memory_access.h',$redirPrivate[1]))) {
    $rejected=$false
    try { Assert-ConsumerBoundary ($case[0]+"`n"+$case[2]) $case[1] @($case[2]) } catch { $rejected=$true }
    if(!$rejected){throw "Private-header negative control accepted: $($case[2])"}
}
Write-Output 'PASS shared guest-memory compiler dependencies and three forbidden-header negative controls'
