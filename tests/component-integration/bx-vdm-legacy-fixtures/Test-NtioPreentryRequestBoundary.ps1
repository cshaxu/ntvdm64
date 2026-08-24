$ErrorActionPreference = 'Stop'

$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$header = Get-Content (Join-Path $root 'src/bx-vdm/ntio_preentry.h') -Raw
$source = Get-Content (Join-Path $root 'src/bx-vdm/ntio_preentry.cc') -Raw
foreach ($pattern in @('bx_ntvdm_ntio_preentry_v1_prepare',
        'BX_NTVDM_FINITE_RUN_REQUEST_VERSION', '0x8400u', '0x700u', '0x714u',
        '0x0070u', 'memcpy\(request->entry_bytes')) {
    if ($header -notmatch $pattern -and $source -notmatch $pattern) {
        throw "Missing NTIO pre-entry contract invariant: $pattern"
    }
}
foreach ($pattern in @('BOP', 'DemDispatch', 'LoadLibrary', 'CreateFile',
        'OpenNT', 'DOS', 'SIM->', 'bx_cpu')) {
    if ($header -match $pattern -or $source -match $pattern) {
        throw "Forbidden NTIO pre-entry dependency: $pattern"
    }
}
Write-Output 'ntio-preentry-request-boundary: bx-vdm owns source-locked request conversion only'
