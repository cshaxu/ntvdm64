[CmdletBinding()]
param([Parameter(Mandatory = $true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$operations = Join-Path $root 'docs/etc/operations'
$fileLedgerPath = Join-Path $operations 'mvdm-file-recovery-ledger.tsv'
if (-not (Test-Path -LiteralPath $fileLedgerPath)) { throw "Missing input: $fileLedgerPath" }
$files = @(Import-Csv -LiteralPath $fileLedgerPath -Delimiter "`t" | Where-Object { $_.target_path -like 'sim16/*' } | Sort-Object target_path)
if ($files.Count -ne 8) { throw "Expected eight sim16 paths, found $($files.Count)." }

$contracts = @{
    'sim16/sim16.asm' = @('wow16-simulator-packet-and-serial-transport', 'adapter-mvdm-host-out/wow; adapter-mvdm-host-out/monitor; adapter-bochs; session', 'historical 16-bit WOW simulator DLL body: packet transport, VDM memory requests, serial port/PIC interrupt and stack switching; retain as source evidence, never reinterpret as a Bochs guest image', 'WOW-profile-gated host-product body')
    'sim16/siminit.asm' = @('wow16-simulator-initialization', 'adapter-mvdm-host-out/wow; adapter-bochs; session', 'historical 16-bit DLL entry, serial/PIC initialization and buffer setup; no standalone machine or device ownership', 'WOW-profile-gated host-product body')
    'sim16/incs.inc' = @('wow16-simulator-declaration', 'adapter-mvdm-host-out/wow; adapter-mvdm-host-out/monitor; adapter-bochs; session', 'original simulator packet, serial and interrupt declaration surface', 'declaration-only')
    'sim16/segdefs.inc' = @('wow16-simulator-segment-declaration', 'adapter-mvdm-host-out/wow', 'original 16-bit segment layout declaration', 'declaration-only')
    'sim16/sim16.inc' = @('wow16-simulator-declaration', 'adapter-mvdm-host-out/wow; adapter-mvdm-host-out/monitor; adapter-bochs; session', 'original simulator public declaration surface', 'declaration-only')
    'sim16/sim16.def' = @('wow16-simulator-dll-export-description', 'adapter-mvdm-host-out/wow; build governance', 'original 16-bit DLL export description', 'build-description-only')
    'sim16/sim16.mac' = @('wow16-simulator-macro-support', 'adapter-mvdm-host-out/wow; build governance', 'original 16-bit assembler macro support', 'build-description-only')
    'sim16/makefile' = @('wow16-simulator-build-description', 'tools/opennt; build governance', 'original cl16/masm/link16 DLL build and WOW16 library composition evidence; no modern build edge', 'build-description-only')
}

$result = foreach ($file in $files) {
    if (-not $contracts.ContainsKey($file.target_path)) { throw "Missing SIM16 contract: $($file.target_path)" }
    if (-not (Test-Path -LiteralPath $file.selected_source_path)) { throw "Missing selected source: $($file.selected_source_path)" }
    $contract = $contracts[$file.target_path]
    $owner = if ($file.target_path -eq 'sim16/makefile') { 'mvdm-tools' } else { 'mvdm-host' }
    $lexical = if ($file.target_path -in @('sim16/sim16.asm','sim16/siminit.asm')) {
        'adapter-mvdm-host-out/wow (16-bit WOW DLL/packet transport); adapter-mvdm-host-out/monitor (VDM memory transport); adapter-bochs (serial/PIC device mechanics); session (instance coordination)'
    } elseif ($file.file_kind -eq 'declaration') {
        'adapter-mvdm-host-out/wow (16-bit simulator declaration surface)'
    } else { 'none (build/macro metadata)' }
    [pscustomobject][ordered]@{
        file_id = $file.file_id
        source_path = $file.target_path
        file_kind = $file.file_kind
        original_role_group = $contract[0]
        provisional_component_owner = $owner
        required_boundary_owner = $contract[1]
        lexical_external_families = $lexical
        original_contract_summary = $contract[2]
        provisional_disposition = $contract[3]
        state = 'provisional-not-enabled'
        evidence = "$($file.target_path); sim16/makefile; original 16-bit DLL build description"
    }
}
$result | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" | Set-Content -LiteralPath (Join-Path $operations 'm0-t276-s15-sim16-file-disposition-ledger.tsv') -Encoding utf8
Write-Output "T276 S15 SIM16 disposition generated: $($result.Count) rows."
