[CmdletBinding()]
param([Parameter(Mandatory = $true)][string]$RepositoryRoot)
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$operations = Join-Path $root 'docs/etc/operations'
$input = Join-Path $operations 'mvdm-file-recovery-ledger.tsv'
if (-not (Test-Path -LiteralPath $input)) { throw "Missing input: $input" }
$files = @(Import-Csv -LiteralPath $input -Delimiter "`t" | Where-Object { $_.target_path -like 'sim32/*' } | Sort-Object target_path)
if ($files.Count -ne 5) { throw "Expected five SIM32 paths, found $($files.Count)." }
$contracts = @{
    'sim32/sim32.c' = @('historical-wow-serial-monitor-transport', 'adapter-wow; adapter-vdm-monitor; session guest-memory mapping manager; adapter-softpc; adapter-bochs', 'original packet transport provides Sim32 read/write/PSZ/free and wakeup forms through a private NT serial device and 16-bit SIM16 peer; preserve source/API evidence, but replace only its SIM32 public contract through the unique mapping manager when a source-proven caller is admitted', 'historical-product-shell exclusion; API-form recovery only')
    'sim32/sim32.h' = @('sim32-public-monitor-api', 'adapter-vdm-monitor; session guest-memory mapping manager; adapter-softpc; adapter-bochs; adapter-wow', 'original Sim32Get/Set/PSZ/Free/Send declarations and convenience macros; same API form may use checked bounce leases, never raw guest pointers', 'declaration and API-form recovery prerequisite')
    'sim32/sources' = @('historical-build-description', 'build governance', 'original SOURCES explicitly lists no translation unit; it is build/profile evidence only', 'build-description-only')
    'sim32/makefile' = @('historical-build-description', 'build governance', 'original SIM32 library/test build evidence only', 'build-description-only')
    'sim32/b.cmd' = @('historical-build-command', 'tools/opennt; build governance', 'original build command evidence only', 'build-description-only')
}
$result = foreach ($file in $files) {
    if (-not $contracts.ContainsKey($file.target_path)) { throw "Missing SIM32 contract: $($file.target_path)" }
    $c=$contracts[$file.target_path]
    $owner=if ($file.target_path -eq 'sim32/b.cmd') {'mvdm-tools'} else {'mvdm-host'}
    $family=if ($file.target_path -eq 'sim32/sim32.c') {'adapter-wow (private serial/WOW peer); adapter-vdm-monitor (SIM32 public API form); session guest-memory mapping manager (checked 32-bit guest address resolution); adapter-bochs (only machine memory/device mechanics)'} elseif ($file.target_path -eq 'sim32/sim32.h') {'adapter-vdm-monitor; session guest-memory mapping manager; adapter-softpc'} else {'none (build metadata)'}
    [pscustomobject][ordered]@{ file_id=$file.file_id; source_path=$file.target_path; file_kind=$file.file_kind; original_role_group=$c[0]; provisional_component_owner=$owner; required_boundary_owner=$c[1]; lexical_external_families=$family; original_contract_summary=$c[2]; provisional_disposition=$c[3]; state='provisional-not-enabled'; evidence="$($file.target_path); sim32/sources; sim32/makefile" }
}
$result | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" | Set-Content -LiteralPath (Join-Path $operations 'm0-t276-s16-sim32-file-disposition-ledger.tsv') -Encoding utf8
Write-Output "T276 S16 SIM32 disposition generated: $($result.Count) rows."
