[CmdletBinding()]
param([Parameter(Mandatory = $true)][string]$RepositoryRoot)
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$operations = Join-Path $root 'docs/etc/operations'
$inputPath = Join-Path $operations 'm0-t276-s14-xms-file-disposition-ledger.tsv'
if (-not (Test-Path -LiteralPath $inputPath)) { throw "Missing input: $inputPath" }
$rows = @(Import-Csv -LiteralPath $inputPath -Delimiter "`t")
if ($rows.Count -ne 15) { throw "Expected 15 XMS rows, found $($rows.Count)." }
$families = @(
    [pscustomobject][ordered]@{ interface_id='XMS-IF-01'; interface_family='XMS service dispatch ABI'; source_paths='xms.486/xms.h; xms.486/xmsdisp.c'; owner='adapter-bop; adapter-softpc'; contract='Original XMS dispatch table and unsupported-service result remain in source; adapter-bop carries mechanics only.'; disposition='existing adapter boundary prerequisite' },
    [pscustomobject][ordered]@{ interface_id='XMS-IF-02'; interface_family='A20 control/query'; source_paths='xms.486/xmsa20.c'; owner='adapter-softpc; adapter-bochs; session'; contract='Original AX/BL success/failure behavior is retained; only adapter-softpc can request typed Bochs A20 mechanics.'; disposition='machine-seam prerequisite' },
    [pscustomobject][ordered]@{ interface_id='XMS-IF-03'; interface_family='extended-memory allocation and block move'; source_paths='xms.486/xms.c; xms.486/xmsblock.c'; owner='adapter-softpc; adapter-bochs; session; opennt-mvdm-support'; contract='Original suballocator/provider algorithms require bounded guest range transactions, not host-address exposure.'; disposition='machine-seam prerequisite' },
    [pscustomobject][ordered]@{ interface_id='XMS-IF-04'; interface_family='UMB reserve/request/release'; source_paths='xms.486/xms.c; xms.486/xmsdisp.c; xms.486/xmsumb.c'; owner='adapter-softpc; adapter-bochs; session'; contract='Original UMB list and B0/B1/B2 result flow are retained only after a durable bounded guest-byte UMB seam exists.'; disposition='machine-profile-gated' },
    [pscustomobject][ordered]@{ interface_id='XMS-IF-05'; interface_family='INT 15 and guest interrupt update'; source_paths='xms.486/xmsmisc.c'; owner='adapter-softpc; adapter-bochs; session'; contract='Original INT 15 notification/query remains source-owned; IVT/interrupt mechanics are a machine seam, not an XMS-local emulation.'; disposition='machine-seam prerequisite' },
    [pscustomobject][ordered]@{ interface_id='XMS-IF-06'; interface_family='suballocator support'; source_paths='xms.486/xms.h; xms.486/xms.c; xms.486/xmsblock.c; xms.486/i386/xmsmem86.c; xms.486/xmsmemr.c'; owner='opennt-mvdm-support'; contract='Original suballoc API remains a support-package prerequisite; no new allocator is embedded in the XMS mirror.'; disposition='support-package prerequisite' },
    [pscustomobject][ordered]@{ interface_id='XMS-IF-07'; interface_family='historical direct host-address virtual memory'; source_paths='xms.486/i386/xmsmem86.c; xms.486/xmsmemr.c'; owner='adapter-softpc; adapter-bochs; adapter-win32'; contract='Historical ULONG-to-host-pointer and NT virtual-memory paths cannot be exposed in a portable x86/x64 profile; preserve originals as evidence and use only a separately proved same-shaped guest-range seam.'; disposition='architecture/profile exclusion' },
    [pscustomobject][ordered]@{ interface_id='XMS-IF-08'; interface_family='XMS shared state and initialization'; source_paths='xms.486/xms.c; xms.486/xms.h'; owner='session; adapter-softpc'; contract='Session owns instance state while original XMS globals/data layout and initialization sequence remain source-owned.'; disposition='session prerequisite' }
)
foreach ($family in $families) { $family | Add-Member -NotePropertyName state -NotePropertyValue 'provisional-not-enabled' }
$families | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" | Set-Content -LiteralPath (Join-Path $operations 'm0-t276-s14-xms-interface-family-ledger.tsv') -Encoding utf8
Write-Output "T276 S14 XMS interface families generated: $($families.Count) rows."
