[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$operations = Join-Path $root 'docs/etc/operations'
$inputPath = Join-Path $operations 'm0-t276-s13-dos-file-disposition-ledger.tsv'
if (-not (Test-Path -LiteralPath $inputPath)) { throw "Missing input: $inputPath" }
$sources = @(Import-Csv -LiteralPath $inputPath -Delimiter "`t" | Where-Object file_kind -eq 'source')
if ($sources.Count -ne 27) { throw "Expected 27 DOS source rows, found $($sources.Count)." }

function Get-PathsForOwner {
    param([string]$Owner)
    return [string]::Join('; ', @($sources | Where-Object { $_.lexical_external_families -like "*$Owner (*" } | Select-Object -ExpandProperty source_path))
}

$families = @(
    [pscustomobject][ordered]@{ interface_id = 'DOS-IF-01'; interface_family = 'BOP ABI transport'; owner = 'adapter-mvdm-host-in'; source_paths = Get-PathsForOwner 'adapter-mvdm-host-in'; evidence_basis = 'literal cmdsvc.h/dossvc.h/BOP lexical surface'; original_contract = 'OpenNT DEM/COMMAND dispatch remains source-owned; adapter transports fixed-width ingress/completion only'; disposition = 'existing adapter boundary prerequisite' },
    [pscustomobject][ordered]@{ interface_id = 'DOS-IF-02'; interface_family = 'SoftPC/SAS guest-state facade'; owner = 'adapter-softpc'; source_paths = Get-PathsForOwner 'adapter-softpc'; evidence_basis = 'literal softpc.h, Sim32, SAS or register-access lexical surface'; original_contract = 'retain historical CCPU/SAS call shapes and bind only through adapter-softpc to adapter-bochs'; disposition = 'existing adapter boundary prerequisite' },
    [pscustomobject][ordered]@{ interface_id = 'DOS-IF-03'; interface_family = 'public Win32/NTDLL host capability'; owner = 'adapter-win32'; source_paths = Get-PathsForOwner 'adapter-win32'; evidence_basis = 'literal Win32/NTDLL header or API lexical surface'; original_contract = 'retain original provider/error ordering while mapping unavailable historical API through same-shaped public-modern facade'; disposition = 'existing adapter boundary prerequisite' },
    [pscustomobject][ordered]@{ interface_id = 'DOS-IF-04'; interface_family = 'single-session state, environment and opaque identities'; owner = 'session'; source_paths = Get-PathsForOwner 'session'; evidence_basis = 'literal mvdm.h/VDM/re-entry lexical surface'; original_contract = 'session owns mutable instance state, lifecycle and opaque mappings; OpenNT sources retain their original calls and data ordering'; disposition = 'existing internal-owner prerequisite' },
    [pscustomobject][ordered]@{ interface_id = 'DOS-IF-05'; interface_family = 'redirector and standard-stream pipe bridge'; owner = 'adapter-redir'; source_paths = Get-PathsForOwner 'adapter-redir'; evidence_basis = 'literal vrnmpipe/pipe/redirector lexical surface'; original_contract = 'COMMAND/DEM keep original pipe/redirector control flow; specialist adapter owns host bridge mechanics'; disposition = 'specialist adapter prerequisite' },
    [pscustomobject][ordered]@{ interface_id = 'DOS-IF-06'; interface_family = 'VDD product callback boundary'; owner = 'adapter-vdd'; source_paths = Get-PathsForOwner 'adapter-vdd'; evidence_basis = 'literal nt_vdd/VDD lexical surface'; original_contract = 'VDD-specific behavior remains behind adapter-vdd and cannot become generic DEM filesystem behavior'; disposition = 'specialist adapter prerequisite' },
    [pscustomobject][ordered]@{ interface_id = 'DOS-IF-07'; interface_family = 'OEM/NLS support library'; owner = 'mvdm-support'; source_paths = Get-PathsForOwner 'mvdm-support'; evidence_basis = 'literal oemuni/Oem lexical surface'; original_contract = 'preserve original OEM/NLS helper calls; independently audit support package before any binding'; disposition = 'support-package prerequisite' },
    [pscustomobject][ordered]@{ interface_id = 'DOS-IF-08'; interface_family = 'BIOS-style raw media and device IOCTL'; owner = 'adapter-softpc; adapter-bochs; adapter-win32; session'; source_paths = 'dos/dem/demdasd.c; dos/dem/demioctl.c'; evidence_basis = 'original raw DASD/IOCTL role plus host_simulate, BIOS disk and physical-media call sites'; original_contract = 'do not create a second controller: Bochs owns machine/device mechanics, while only proven public host-media endpoints may bind through adapters'; disposition = 'machine-media prerequisite; profile-gated' }
)

foreach ($family in $families) {
    if ([string]::IsNullOrWhiteSpace($family.source_paths)) { throw "No source evidence for $($family.interface_family)." }
    $family | Add-Member -NotePropertyName state -NotePropertyValue 'provisional-not-enabled'
}
$families | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" | Set-Content -LiteralPath (Join-Path $operations 'm0-t276-s13-dos-interface-family-ledger.tsv') -Encoding utf8
Write-Output "T276 S13 DOS interface families generated: $($families.Count) rows."
