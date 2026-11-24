[CmdletBinding()]
param([Parameter(Mandatory)][string]$WowBuildRoot)
$ErrorActionPreference='Stop'
. "$PSScriptRoot/native-source-graph.ps1"
$graph=Read-Graph $WowBuildRoot
$sources=@(Get-GraphSources 'wow32.dll' $graph | Where-Object {
    $_ -match '/src/(?:mvdm-host/wow32|wow32-dll)/.+\.c$'
})
if($sources.Count -lt 70){throw "Too few selected WOW sources: $($sources.Count)"}
$forbidden=@('ntvdm-exe/session/session.h','compat/include/guest_memory_lease.h',
    'ntvdm-exe/softpc/include/mvdm_softpc_firmware.h',
    'ntvdm-exe/softpc/include/mvdm_softpc_guest_memory.h',
    'compat/include/mvdm_softpc_wow_page_domain.h',
    'ntvdm-exe/wow/include/wow_gdi_alias.h',
    'ntvdm-exe/wow/include/wow_user_runtime.h',
    'compat/include/wow_user_session_binding.h',
    'ntvdm-exe/softpc/include/mvdm_shadow_registry.h')
function Assert-Boundary([string]$deps) {
    foreach($path in $forbidden){if($deps.Contains($path)){throw "Private WOW dependency: $path"}}
    if($deps -match 'src/ntvdm-exe/'){throw 'Unexpected NTVDM private dependency'}
}
foreach($source in $sources){
    $relative=$source.Substring($source.IndexOf('/src/')+1)
    Assert-Boundary (Get-ProductCompilerDependencies $WowBuildRoot 'wow32.dll' $relative)
}
foreach($path in $forbidden){
    $rejected=$false
    try{Assert-Boundary $path}catch{$rejected=$true}
    if(!$rejected){throw "Negative control accepted: $path"}
}
Write-Output "PASS $($sources.Count) selected WOW compiler sources; $($forbidden.Count) private-header negative controls; no NTVDM private dependency"
