[CmdletBinding()]
param(
    [string]$RepositoryRoot = '.'
)

$ErrorActionPreference = 'Stop'
$RepositoryRoot = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$mirrorRoot = Join-Path $RepositoryRoot 'src/bx-vdm/bop/opennt/dpmi'
$formalManifest = Join-Path $RepositoryRoot 'tools/build/t225-s7-full-module-manifest.json'
$ledger = Join-Path $RepositoryRoot 'docs/etc/operations/m0-t257-s2-dpmi-dosx-source-import-ledger-001.md'

foreach ($required in @($formalManifest, $ledger,
        (Join-Path $RepositoryRoot 'src/opennt/base/mvdm/dpmi32/dpmi32.c'),
        (Join-Path $RepositoryRoot 'src/opennt/base/mvdm/dpmi32/i386/dpmi386.c'),
        (Join-Path $RepositoryRoot 'src/opennt/base/mvdm/dpmi/486/dxboot.asm'))) {
    if (-not (Test-Path -LiteralPath $required)) {
        throw "T257 S2 boundary input is missing: $required"
    }
}

if (Test-Path -LiteralPath $mirrorRoot) {
    throw "T257 S2 forbids a DPMI product mirror before S3: $mirrorRoot"
}

$manifestText = Get-Content -Raw -LiteralPath $formalManifest
if ($manifestText -match 'src/bx-vdm/bop/opennt/dpmi/') {
    throw 'T257 S2 forbids DPMI mirror sources in the formal product manifest before S3.'
}

$activeDpmibodies = rg -n --glob '*.{c,cc,h}' 'DpmiDispatch|DpmiSetDescriptorEntry|DpmiInitDosx' (Join-Path $RepositoryRoot 'src/bx-vdm')
if ($LASTEXITCODE -eq 0) {
    throw "T257 S2 found an active DPMI body before S3:`n$activeDpmibodies"
}
if ($LASTEXITCODE -ne 1) {
    throw "T257 S2 source scan failed with rg exit code $LASTEXITCODE"
}

Write-Host 'T257 S2 DPMI import boundary: PASS (no product mirror or active DPMI body).'
