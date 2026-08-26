[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path $RepositoryRoot).Path
$ops = Join-Path $root 'docs/etc/operations'
$files = @(Import-Csv (Join-Path $ops 'm0-t276-s23-fax-file-disposition-ledger.tsv') -Delimiter "`t")
if ($files.Count -ne 21) { throw "Expected 21 fax file dispositions, found $($files.Count)." }
if (@($files | Group-Object source_path | Where-Object Count -ne 1).Count) { throw 'Every fax path needs exactly one disposition.' }
if (@($files | Where-Object {$_.original_role_group -eq 'historical-gdi-driver-provider-body'}).Count -ne 1) { throw 'Expected exactly one original fax GDI driver body.' }
if (@($files | Where-Object {$_.original_role_group -eq 'historical-fax-ui-dll-provider-body'}).Count -ne 4) { throw 'Expected four original fax UI DLL bodies.' }
if (@($files | Where-Object {$_.state -ne 'provisional-not-enabled'}).Count) { throw 'S23 may not enable a fax body.' }
$interfaces = @(Import-Csv (Join-Path $ops 'm0-t276-s23-fax-interface-family-ledger.tsv') -Delimiter "`t")
if ($interfaces.Count -ne 6) { throw "Expected 6 fax interface families, found $($interfaces.Count)." }
if (@($interfaces | Where-Object {$_.mapping_requirement -notmatch 'mapping.manager'}).Count) { throw 'Every fax interface family must state its mapping-manager disposition.' }
Write-Output 'T276 S23 fax disposition verification passed: 21 paths, 1 GDI driver body, 4 UI bodies, 6 interface families and none enabled.'
