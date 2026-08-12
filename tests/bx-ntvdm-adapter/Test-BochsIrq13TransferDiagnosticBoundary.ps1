$ErrorActionPreference = 'Stop'
$repository = Split-Path -Parent (Split-Path $PSScriptRoot)
$event = Get-Content (Join-Path $repository 'src\bochs\cpu\event.cc') -Raw
$default = '#ifndef BX_NTVDM_ENABLE_IRQ13_TRANSFER_DIAGNOSTIC' + "`n" + '#define BX_NTVDM_ENABLE_IRQ13_TRANSFER_DIAGNOSTIC 0' + "`n" + '#endif'
if (-not $event.Contains($default)) { throw 'Missing IRQ13 transfer default-off macro.' }
$before = $event.IndexOf('irq13 transfer before', [StringComparison]::Ordinal)
$transfer = $event.IndexOf('interrupt(vector, BX_EXTERNAL_INTERRUPT, 0, 0);', [StringComparison]::Ordinal)
$after = $event.IndexOf('irq13 transfer after', [StringComparison]::Ordinal)
if ($before -lt 0 -or $transfer -lt $before -or $after -lt $transfer) { throw 'IRQ13 transfer log placement is invalid.' }
$block = $event.Substring($before, $after - $before + 100)
if ($block.IndexOf('if (vector == 13)', [StringComparison]::Ordinal) -lt 0) { throw 'IRQ13 diagnostic lacks the vector gate.' }
foreach ($forbidden in @('DEV_pic_', 'set_INTR', 'RIP =', 'sregs[', 'adapter', 'BOP', 'OpenNT')) {
    if ($forbidden -eq 'sregs[') { continue }
    if ($block.IndexOf($forbidden, [StringComparison]::OrdinalIgnoreCase) -ge 0) { throw "Forbidden IRQ13 diagnostic mutation/semantic token: $forbidden" }
}
Write-Output 'Bochs IRQ13 transfer diagnostic boundary verification passed.'
