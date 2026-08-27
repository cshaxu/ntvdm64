[CmdletBinding()]
param([string]$RepositoryRoot = '', [string]$OutputDirectory = 'docs/etc/operations')
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot) }
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$ops = Join-Path $root $OutputDirectory
$boundaryPath = Join-Path $ops 'm0-t276-s5-unique-call-disposition-ledger.tsv'
$declarationPath = Join-Path $ops 'm0-t276-s3-textual-declaration-ledger.tsv'
$worklistPath = Join-Path $ops 'm0-t276-s6-boundary-family-worklist.tsv'
$evidencePath = Join-Path $ops 'm0-t276-s6-boundary-family-worklist-001.md'
foreach ($p in @($boundaryPath,$declarationPath)) { if (-not (Test-Path -LiteralPath $p)) { throw "Missing T276 input: $p" } }
function Get-ReviewFamily([string]$Path) {
    if ($Path -match '^(?i:inc/(softpc|memapi)\.h|softpc\.new/(base|host)/inc/)') { return 'adapter-softpc-review' }
    if ($Path -match '^(?i:inc/(vdmredir|rdrsvc|rdrexp)\.h|vdmredir/)') { return 'adapter-redir-review' }
    if ($Path -match '^(?i:inc/(vdmtib|v86def|vint)\.h|v86/|sim16/|sim32/|ieuvddex/)') { return 'adapter-vdm-monitor-review' }
    if ($Path -match '^(?i:inc/wow|wow32/|fax/)') { return 'adapter-wow-review' }
    if ($Path -match '^(?i:inc/(dbg|dbgs|dbgexp)|bde/|dbg/|vdmdbg/|vdmexts/)') { return 'adapter-debugger-review' }
    if ($Path -match '^(?i:vdd/|inc/.*vdd)') { return 'adapter-vdd-review' }
    if ($Path -match '^(?i:inc/(oemuni|cmdsvc|dossvc|curdir)|dos/)') { return 'mvdm-host-or-adapter-mvdm-host-out/win32-review' }
    if ($Path -match '^(?i:xms\.486/)') { return 'adapter-softpc-xms-review' }
    return 'original-package-semantic-review'
}
$boundaries = @(Import-Csv $boundaryPath -Delimiter "`t" | Where-Object { $_.disposition -eq 'external-boundary-candidate' })
if ($boundaries.Count -eq 0) { throw 'S5 produced no external boundary candidates.' }
$declarations = @{}; foreach ($d in (Import-Csv $declarationPath -Delimiter "`t")) { $declarations[$d.declaration_id] = $d }
$i=0; $worklist = foreach ($group in ($boundaries | Group-Object declaration_id | Sort-Object Name)) {
    $i++; $d = $declarations[$group.Name]; if ($null -eq $d) { throw "Missing declaration: $($group.Name)" }
    [pscustomobject]@{ worklist_id=('MVDM-WORK-{0:D5}' -f $i); declaration_id=$d.declaration_id; lexical_name=$d.lexical_name; declaration_path=$d.declaration_path; declaration_package=$d.package_root; review_family=(Get-ReviewFamily $d.declaration_path); caller_count=$group.Count; caller_packages=(($group.Group.caller_package | Sort-Object -Unique) -join ';'); caller_evidence=(($group.Group.evidence | Select-Object -First 8) -join '; '); final_owner='unreviewed'; recovery_state='unreviewed'; evidence=$d.evidence }
}
$worklist | Export-Csv $worklistPath -Delimiter "`t" -NoTypeInformation -Encoding utf8
$counts=@{}; foreach($row in $worklist){if(-not $counts.ContainsKey($row.review_family)){$counts[$row.review_family]=0};$counts[$row.review_family]++}
$lines=@('# M0 T276 S6 original declaration-family worklist','','## Result','',('- External call candidates grouped: '+$boundaries.Count),('- Unique original declaration worklist rows: '+$worklist.Count),'','## Non-final review families','')
foreach($name in ($counts.Keys|Sort-Object)){$lines += '- `'+$name+'`: '+$counts[$name]}
$lines += '','## Limit','','Path-family labels only choose the required review specialist. They do not assign a final owner, prove ABI compatibility, authorize an adapter or classify a source as buildable.'
[System.IO.File]::WriteAllLines($evidencePath,$lines)
Write-Host "Grouped $($boundaries.Count) boundary calls into $($worklist.Count) declaration worklist rows."
