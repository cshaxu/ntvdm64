[CmdletBinding()]
param([string]$RepositoryRoot = '')

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$operations = Join-Path $root 'docs/etc/operations'
$source = Join-Path $operations 'm0-t276-final-integration-disposition-ledger.tsv'
$target = Join-Path $operations 'm0-t280-s1-host-control-dem-workset-ledger.tsv'

function Get-Group([string]$Path, [string]$Disposition) {
    if ($Disposition -eq 'overlay-required') { return '05-machine-transport-overlay' }
    if ($Disposition -eq 'binding-only') { return '00-declaration-carrier' }
    if ($Disposition -eq 'not-host-runtime') { return '99-excluded-historical-product' }
    if ($Path -like 'dos/dem/*') {
        if ($Path -match 'dem(\.c|data\.c|disp\.c|msg\.c)$') { return '03-dem-dispatch-data' }
        if ($Path -match 'dem(dir|file|fcb|hndl|srch)\.c$') { return '03-dem-namespace-file' }
        return '03-dem-device-state'
    }
    if ($Path -match 'nt_bop\.c$') { return '01-bop-ingress-control' }
    if ($Path -match '(nt_aorc|nt_sas|sim32)\.c$') { return '02-softpc-machine-facade' }
    if ($Path -match 'nt_(event|msscs|reset|term|thred)\.c$') { return '04-session-event-stop' }
    return '04-host-capability-control'
}

$rows = @(Import-Csv -LiteralPath $source -Delimiter "`t" |
    Where-Object { $_.source_path -like 'dos/dem/*' -or $_.source_path -like 'softpc.new/host/src/*' } |
    Sort-Object source_path | ForEach-Object {
        [pscustomobject]@{
            work_item_id = 'T280-' + $_.file_id
            source_path = $_.source_path
            source_owner = if ($_.source_path -like 'dos/dem/*') { 'original DEM provider package' } else { 'original SoftPC host-control package' }
            final_disposition = $_.final_disposition
            implementation_group = Get-Group $_.source_path $_.final_disposition
            final_owner_or_link_boundary = $_.final_owner_or_link_boundary
            named_adapter = $_.named_adapter
            mapping_implication = $_.mapping_implication
            final_evidence = $_.final_evidence
            admission_state = 'S1-inherited-disposition; implementation-not-enabled'
        }
    })

if ($rows.Count -ne 81) { throw "Expected 81 T280 workset paths; found $($rows.Count)." }
$expected = @{ 'adapter-backed' = 49; 'overlay-required' = 6; 'binding-only' = 5; 'not-host-runtime' = 21 }
foreach ($key in $expected.Keys) {
    $actual = @($rows | Where-Object final_disposition -eq $key).Count
    if ($actual -ne $expected[$key]) { throw "Expected $($expected[$key]) $key paths; found $actual." }
}
$rows | Export-Csv -LiteralPath $target -Delimiter "`t" -NoTypeInformation -Encoding utf8
Write-Host 'Wrote T280 S1 workset: 81 paths; adapter-backed=49; overlay-required=6; binding-only=5; not-host-runtime=21.'
