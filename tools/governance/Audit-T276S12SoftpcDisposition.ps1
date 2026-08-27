[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$operations = Join-Path $root 'docs/etc/operations'
$fileLedgerPath = Join-Path $operations 'mvdm-file-recovery-ledger.tsv'
$buildLedgerPath = Join-Path $operations 'm0-t276-s2-translation-unit-build-ledger.tsv'
foreach ($path in @($fileLedgerPath, $buildLedgerPath)) {
    if (-not (Test-Path -LiteralPath $path)) { throw "Missing T276 input: $path" }
}

$files = @(Import-Csv -LiteralPath $fileLedgerPath -Delimiter "`t" | Where-Object package_root -eq 'softpc.new' | Sort-Object target_path)
$builds = @(Import-Csv -LiteralPath $buildLedgerPath -Delimiter "`t")
if ($files.Count -ne 998) { throw "Expected 998 softpc.new rows, found $($files.Count)." }

function Get-SoftpcDisposition {
    param([string]$Path)
    if ($Path -match '^softpc\.new/host/src/') {
        return @('original-host-control', 'mvdm-host', 'candidate-adapter-bound', 'host control source; requires per-interface SoftPC/Win32/monitor audit')
    }
    if ($Path -match '^softpc\.new/(host/inc|host/link|host/dirs)') {
        return @('host-control-declaration-or-build', 'mvdm-host', 'host-control-input-review', 'supporting original host-control input; no standalone body claim')
    }
    if ($Path -match '^softpc\.new/obj\.vdm/') {
        return @('original-host-product-composition', 'mvdm-host', 'app-composition-review', 'historical NTVDM product composition source; app is the eventual composition consumer, not a source rewrite target')
    }
    if ($Path -match '^softpc\.new/base/(ccpu386|cpu|cvidc|video|disks|keymouse|comms|system|dos)/') {
        return @('historical-machine-execution-or-device', 'mvdm-host', 'bochs-replacement-profile-excluded', 'CPU/memory/device execution ownership belongs to Bochs; retain exact original source as exclusion evidence')
    }
    if ($Path -match '^softpc\.new/base/bios/') {
        return @('historical-machine-firmware', 'mvdm-softpc-firmware', 'adapter-bochs-firmware-review', 'separate firmware mirror input; later evaluate only as an adapter-bochs manifest-selected input, never as a host runtime library or parallel machine implementation')
    }
    if ($Path -match '^softpc\.new/(bios|roms|data)/') {
        return @('firmware-rom-or-machine-data', 'mvdm-softpc-firmware', 'adapter-bochs-firmware-review', 'separate firmware mirror input; no automatic machine or host-runtime enablement')
    }
    if ($Path -match '^softpc\.new/base/support/') {
        return @('historical-machine-support', 'mvdm-host', 'adapter-mvdm-host-out/softpc-review', 'machine-facing helper set; review only through original SoftPC-shaped adapter boundary')
    }
    if ($Path -match '^softpc\.new/base/inc/') {
        return @('historical-machine-declaration', 'mvdm-host', 'adapter-mvdm-host-out/softpc-declaration-review', 'shared original SoftPC machine declaration/input; no standalone body or direct host-control claim')
    }
    if ($Path -match '^softpc\.new/base/unix/') {
        return @('historical-unix-port', 'mvdm-host', 'profile-excluded', 'non-Windows historical port input; retained mirror only')
    }
    if ($Path -match '^softpc\.new/(base/debug|base/dasm386|debugger)/') {
        return @('historical-debugger-or-disassembler', 'mvdm-host', 'adapter-mvdm-host-out/debugger-review', 'debugger/disassembler product input; no default host-runtime enablement')
    }
    if ($Path -match '^softpc\.new/(convert|dat2obj|rename)/') {
        return @('historical-build-tool', 'tools/opennt', 'tool-profile-excluded', 'independent historical build conversion tool, not an MVDM runtime package')
    }
    if ($Path -match '^softpc\.new/(base/dirs|dirs)$') {
        return @('original-build-control', 'mvdm-host', 'build-control-only', 'original build control; no runtime body')
    }
    throw "No S12 disposition rule for $Path"
}

$out = foreach ($file in $files) {
    $build = @($builds | Where-Object file_id -eq $file.file_id)
    $buildMembership = if ($build.Count -eq 0) { 'not-a-translation-unit' } else { $build.original_build_membership -join ';' }
    $rule = Get-SoftpcDisposition -Path $file.target_path
    [pscustomobject][ordered]@{
        file_id = $file.file_id
        target_path = $file.target_path
        file_kind = $file.file_kind
        original_build_membership = $buildMembership
        original_subpackage_role = $rule[0]
        final_component_owner = $rule[1]
        provisional_composition_disposition = $rule[2]
        source_derived_reason = $rule[3]
        state = 'provisional-not-enabled'
        evidence = 'mvdm-file-recovery-ledger.tsv; m0-t276-s2-translation-unit-build-ledger.tsv'
    }
}

$out | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" | Set-Content -LiteralPath (Join-Path $operations 'm0-t276-s12-softpc-file-disposition-ledger.tsv') -Encoding utf8
Write-Output "T276 S12 softpc disposition generated: $($out.Count) paths."
