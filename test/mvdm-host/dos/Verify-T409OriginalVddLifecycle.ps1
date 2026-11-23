[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$BuildCache = 'build/.ninja/msvc-x86-release/worker'
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent (Split-Path -Parent $PSScriptRoot))
}
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$demFile = Join-Path $root 'src/mvdm-host/dos/dem/demfile.c'
$wowFile = Join-Path $root 'src/mvdm-host/wow32/wkfileio.c'
$graphFile = Join-Path $root "$BuildCache/source-manifest.json"
$dem = Get-Content -LiteralPath $demFile -Raw
$demData = Get-Content -LiteralPath (Join-Path $root 'src/mvdm-host/dos/dem/demdata.c') -Raw
$demSetup = Get-Content -LiteralPath (Join-Path $root 'src/mvdm-host/dos/dem/demgset.c') -Raw
$wow = Get-Content -LiteralPath $wowFile -Raw
$graph = Get-Content -LiteralPath $graphFile -Raw
$selected = $graph | ConvertFrom-Json
if ($selected.architecture -ne 'x86' -or
    !@($selected.sources | Where-Object object -eq 'obj/dem/demfile.obj').Count) {
    throw 'Current configured worker DEM source inventory is missing.'
}

foreach ($name in @('GetFreeSftEntry', 'VDDAllocateDosHandle',
    'VDDAssociateNtHandle', 'VDDReleaseDosHandle', 'VDDRetrieveNtHandle')) {
    if ($dem -notmatch ('(?m)^.*\b' + [regex]::Escape($name) + '\b')) {
        throw "Original VDD lifecycle owner is missing $name."
    }
}
if ($dem -match 'mvdm_vdd_sft_shadow|#if 0\s*\r?\n\s*PDOSSFT GetFreeSftEntry') {
    throw 'DEM still selects the retired shadow lifecycle instead of its original body.'
}
if ($dem -notmatch 'Sim32GetVDMPointer\s*\(pPDB' -or
    $dem -notmatch 'GetFreeSftEntry\(pSFTHead, &usSFN\)' -or
    $dem -notmatch 'pJFT\[hFile\] = 0xFF' -or
    $dem -notmatch 'pSFT->SFT_Ref_Count--') {
    throw 'The original VDD allocation/retrieval/release ordering is incomplete.'
}
# S5 P10 restored the original shared SFT head and removed the extra resolver.
# Check initialization and its two consumers, not the retired adapter shape.
if ($demData -notmatch 'PDOSSF\s+pSFTHead\s*=\s*NULL' -or
    $demSetup -notmatch 'pSFTHead\s*=\s*\(PDOSSF\)\s*GetVDMAddr\(getDS\(\),\(WORD\)pDosWowData->lpSftAddr\)' -or
    $dem -notmatch 'extern\s+PDOSSF\s+pSFTHead' -or
    $dem -notmatch 'pSfFlat\s*=\s*pSFTHead' -or
    $dem -match 'demSftHeadPointer|sft_head_location') {
    throw 'The restored original shared SFT-head initialization/use chain is incomplete.'
}
if ($wow -match 'mvdm_vdd_sft_shadow_(commit|discard)' -or
    $graph -match 'mvdm_vdd_sft_shadow') {
    throw 'A selected caller or formal build member still names the retired shadow provider.'
}

Write-Output 'T409 original VDD lifecycle source boundary passed.'
