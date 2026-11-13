[CmdletBinding()]
param([Parameter(Mandatory)][string]$WowBuildRoot)
$ErrorActionPreference='Stop'
$graph=Get-Content (Join-Path $WowBuildRoot 'build.ninja') -Raw
$targets=@([regex]::Matches($graph,
    '(?m)^build (obj/[^ :]+\.obj): cc [^\r\n]*src/(?:mvdm-host/wow32|wow32-dll)/[^\r\n]*\.c\r?$') |
    ForEach-Object {$_.Groups[1].Value})
if($targets.Count -lt 70){throw "Too few selected WOW objects: $($targets.Count)"}
$raw=@(& ninja -C $WowBuildRoot -t deps 2>&1)
if($LASTEXITCODE){throw 'Cannot read compiler dependencies'}
$blocks=@{}
$current=''
foreach($line in $raw){
    if($line -match '^(obj/[^:]+\.obj): #deps \d+.*\(VALID\)'){
        $current=$Matches[1]; $blocks[$current]=''; continue
    }
    if($current -and $line -match '^\s+'){ $blocks[$current]+=$line.Replace('\','/')+"`n" }
}
$forbidden=@('ntvdm-exe/session/session.h','ntvdm-exe/session/guest_memory_lease.h',
    'ntvdm-exe/softpc/include/mvdm_softpc_firmware.h',
    'ntvdm-exe/softpc/include/mvdm_softpc_guest_memory.h',
    'ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h',
    'ntvdm-exe/wow/include/wow_gdi_alias.h',
    'ntvdm-exe/wow/include/wow_user_runtime.h',
    'ntvdm-exe/wow/include/wow_user_session_binding.h',
    'ntvdm-exe/softpc/include/mvdm_shadow_registry.h')
function Assert-Boundary([string]$deps) {
    foreach($path in $forbidden){if($deps.Contains($path)){throw "Private WOW dependency: $path"}}
    if($deps -match 'src/ntvdm-exe/'){throw 'Unexpected NTVDM private dependency'}
}
foreach($target in $targets){
    if(!$blocks.ContainsKey($target)){throw "Missing valid compiler dependencies: $target"}
    Assert-Boundary $blocks[$target]
}
foreach($path in $forbidden){
    $rejected=$false
    try{Assert-Boundary $path}catch{$rejected=$true}
    if(!$rejected){throw "Negative control accepted: $path"}
}
Write-Output "PASS $($targets.Count) selected WOW objects; $($forbidden.Count) private-header negative controls; no NTVDM private dependency"
