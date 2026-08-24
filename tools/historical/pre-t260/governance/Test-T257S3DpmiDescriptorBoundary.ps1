[CmdletBinding()]
param([string]$RepositoryRoot = '.')

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$RepositoryRoot = (Resolve-Path -LiteralPath $RepositoryRoot).Path

function Require([bool]$Condition, [string]$Message) {
    if (!$Condition) { throw $Message }
}

$manifestPath = Join-Path $RepositoryRoot 'tools/build/t225-s7-full-module-manifest.json'
$sourcePath = Join-Path $RepositoryRoot 'src/bx-vdm/bop/opennt/dpmi/dpmiselr.c'
$shimPath = Join-Path $RepositoryRoot 'src/bx-vdm/bop/shim/dpmi_descriptor_source_shim.c'
$fixturePath = Join-Path $RepositoryRoot 'tests/bx-vdm/t257_s3_dpmi_descriptor_source_fixture.c'
foreach ($path in @($manifestPath, $sourcePath, $shimPath, $fixturePath)) {
    Require (Test-Path -LiteralPath $path -PathType Leaf) "T257 S3 descriptor boundary input is missing: $path"
}

$manifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
$vdm = @($manifest.modules | Where-Object { $_.name -eq 'bx-vdm' })
Require ($vdm.Count -eq 1) 'Formal manifest has no unique bx-vdm module.'
foreach ($required in @(
    'src/bx-vdm/bop/opennt/dpmi/dpmi32.c',
    'src/bx-vdm/bop/opennt/dpmi/dpmiselr.c',
    'src/bx-vdm/bop/shim/dpmi_startup_session_shim.c',
    'src/bx-vdm/bop/shim/dpmi_startup_source_shim.c',
    'src/bx-vdm/bop/shim/dpmi_descriptor_source_shim.c')) {
    Require (@($vdm[0].sources) -contains $required) "T257 S3 formal source membership is incomplete: $required"
}
Require (@($manifest.fixtures | Where-Object { $_.name -eq 't257-s3-dpmi-descriptor-source-fixture' }).Count -eq 1) 'T257 S3 descriptor source has no formal fixture membership.'

$source = Get-Content -LiteralPath $sourcePath -Raw
$shim = Get-Content -LiteralPath $shimPath -Raw
$sourceCode = [regex]::Replace($source, '(?s)/\*.*?\*/|//[^\r\n]*', '')
$shimCode = [regex]::Replace($shim, '(?s)/\*.*?\*/|//[^\r\n]*', '')
Require ($source -match '(?m)^#define i386 1$') 'Selected 486 source branch is not explicit.'
Require ($source -match '#include "\.\./\.\./shim/dpmi_descriptor_source_shim\.h"') 'dpmiselr does not use its named source boundary.'
Require ($source -notmatch '#include "softpc\.h"') 'dpmiselr revives the old SoftPC/NT4 header closure.'
Require ($sourceCode -notmatch 'Sim32GetVDMPointer') 'dpmiselr retains a raw historical guest-pointer call.'
Require ($sourceCode -notmatch 'NtSetLdtEntries|NtSetInformationProcess|ProcessLdtInformation') 'dpmiselr retains a process-LDT import.'
Require ($source -match '(?s)#ifndef i386\s+            FlatAddress\[') 'The historical non-486 FlatAddress cache is not excluded from the selected branch.'
Require ($shim -match 'bx_ntvdm_mantle_execute_protected_range_action_v1') 'Descriptor input does not use the selector-blind protected-range action.'
Require ($shim -match 'bx_ntvdm_mantle_checked_ram_write_v1') 'Descriptor publication does not use checked guest RAM.'
Require ($shimCode -notmatch 'NtSetLdtEntries|NtSetInformationProcess|ProcessLdtInformation|Sim32GetVDMPointer|FlatAddress|IntelBase') 'Descriptor seam contains a refused NT4 descriptor import or cache.'

Write-Host 'T257 S3 DPMI descriptor boundary: PASS (source order, bounded guest table seam, no process LDT).'
