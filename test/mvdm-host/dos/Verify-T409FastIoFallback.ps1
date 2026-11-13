[CmdletBinding()]
param(
    [string]$RepositoryRoot = ''
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent (Split-Path -Parent $PSScriptRoot))
}
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$dispatch = Get-Content -LiteralPath (Join-Path $root 'src/mvdm-host/dos/dem/demdisp.c') -Raw
$header = Get-Content -LiteralPath (Join-Path $root 'src/mvdm-host/dos/dem/dem.h') -Raw
$guestRead = Get-Content -LiteralPath (Join-Path $root 'src/mvdm-guest/dos/v86/doskrnl/dos/handle.asm') -Raw
$graph = Get-Content -LiteralPath (Join-Path $root 'tools/build/New-T310OriginalSoftpcNinja.ps1') -Raw

$startup = Get-Content -LiteralPath (Join-Path $root 'src/mvdm-host/softpc.new/host/src/nt_msscs.c') -Raw
if ($dispatch -notmatch 'demNotYetImplemented,\s*//SVC_DEMFASTREAD' -or
    $dispatch -notmatch 'demNotYetImplemented,\s*//SVC_DEMFASTWRITE' -or
    $dispatch -match 'demFastIoUnavailable') {
    throw 'Original unused fast-I/O table slots were replaced.'
}
if ($startup -notmatch 'fVirtualInt\s*\|=\s*MIPS_BIT_MASK' -or
    $guestRead -notmatch '(?s)Procedure FastOrSlow,near.*?test\s+word ptr ds:FIXED_NTVDMSTATE_REL40, MIPS_BIT_MASK.*?jz\s+fos_fast.*?fos_slow:.*?stc.*?fos_fast:.*?ret') {
    throw 'Original software-CPU slow-I/O gate is absent.'
}
if ($dispatch -match '\bdemFast(Read|Write)\s*(?:\(|,)' -or
    $header -match 'demFastRead|demFastWrite' -or
    $graph -match '\bdemfastio\.c\b') {
    throw 'A selected autonomous fast-I/O implementation remains.'
}
if ($guestRead -notmatch '(?s)SVC\s+SVC_DEMFASTREAD\s*\r?\n\s*jnc.*?HRDSVC\s+SVC_DEMREAD' -or
    $guestRead -notmatch '(?s)SVC\s+SVC_DEMFASTWRITE\s*\r?\n\s*jnc.*?HRDSVC\s+SVC_DEMWRITE') {
    throw 'The original DOS CF fallback sequence is not present.'
}

Write-Output 'PASS source contract: original CPU40 slow-I/O selection and original dispatch slots; runtime coverage belongs to integration tests.'
