[CmdletBinding()]
param(
    [string]$RepositoryRoot = ''
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent (Split-Path -Parent $PSScriptRoot))
}
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$demFile = Join-Path $root 'src/mvdm-host/dos/dem/demfile.c'
$wowFile = Join-Path $root 'src/mvdm-host/wow32/wkfileio.c'
$graphFile = Join-Path $root 'tools/build/New-T310OriginalSoftpcNinja.ps1'
$dem = Get-Content -LiteralPath $demFile -Raw
$wow = Get-Content -LiteralPath $wowFile -Raw
$graph = Get-Content -LiteralPath $graphFile -Raw

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
if ($dem -notmatch 'static PDOSSF demSftHeadPointer\(VOID\)' -or
    $dem -notmatch 'Sim32GetVDMPointer\(sftHead, 0, 0\)') {
    throw 'The only retained SFT-head adapter is not the checked synchronous Sim32 binding.'
}
if ($wow -match 'mvdm_vdd_sft_shadow_(commit|discard)' -or
    $graph -match 'mvdm_vdd_sft_shadow') {
    throw 'A selected caller or formal build member still names the retired shadow provider.'
}

Write-Output 'T409 original VDD lifecycle source boundary passed.'
