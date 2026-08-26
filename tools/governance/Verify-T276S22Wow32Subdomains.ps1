[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path $RepositoryRoot).Path
$path = Join-Path $root 'docs/etc/operations/m0-t276-s22-wow32-active-subdomain-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t")
if ($rows.Count -ne 77) { throw "Expected 77 active WOW32 C-body rows, found $($rows.Count)." }
if (@($rows | Group-Object source_path | Where-Object Count -ne 1).Count) { throw 'Every active WOW32 source needs exactly one subdomain row.' }
if (@($rows | Where-Object { $_.original_selection -ne 'active original wow32.dll source' }).Count) { throw 'S22 subdomain ledger must contain active original sources only.' }
if (@($rows | Where-Object { [string]::IsNullOrWhiteSpace($_.original_subdomain) -or [string]::IsNullOrWhiteSpace($_.boundary_owners) -or [string]::IsNullOrWhiteSpace($_.mapping_requirement) }).Count) { throw 'Every WOW32 source needs subdomain, boundary and mapping evidence.' }
if (@($rows | Group-Object original_subdomain).Count -ne 8) { throw 'Expected eight original WOW32 subdomains.' }
if (@($rows | Where-Object { $_.provisional_disposition -ne 'whole WOW owner package prerequisite; no body enabled' }).Count) { throw 'S22 must not enable a WOW32 source body.' }
Write-Output 'T276 S22 WOW32 active-subdomain verification passed: 77 active C bodies across 8 original subdomains, none enabled.'
