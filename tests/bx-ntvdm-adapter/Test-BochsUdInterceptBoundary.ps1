[CmdletBinding()]
param(
    [string]$RepositoryRoot = ''
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path))
}

$exception = Join-Path $RepositoryRoot 'src\bochs\cpu\exception.cc'
$header = Join-Path $RepositoryRoot 'src\bochs\cpu\bx_ntvdm_exception_intercept.h'
foreach ($path in @($exception, $header)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing seam file: $path" }
}

$exceptionText = Get-Content -LiteralPath $exception -Raw
# The separately tested, default-off catalogue listener is allowed to name its
# observation subject.  This test protects the generic transport around it,
# not that listener's own intentionally narrow logging block.
$listenerStart = $exceptionText.IndexOf('static void bx_ntvdm_bop_catalog_listener')
if ($listenerStart -lt 0) { throw 'Missing separately governed BOP listener block.' }
$listenerEnd = $exceptionText.IndexOf('#endif', $listenerStart)
if ($listenerEnd -lt $listenerStart) { throw 'Unable to isolate BOP listener block.' }
$genericExceptionText = $exceptionText.Remove($listenerStart,
    ($listenerEnd + '#endif'.Length) - $listenerStart)
$text = $genericExceptionText + "`n" + (Get-Content -LiteralPath $header -Raw)
foreach ($required in @(
    'BX_NTVDM_EXCEPTION_INTERCEPT_ABI_VERSION',
    'vector != BX_UD_EXCEPTION',
    'BX_NTVDM_EXCEPTION_PASS_THROUGH',
    'BX_NTVDM_EXCEPTION_RESUME',
    'response.resume_rip',
    'longjmp(BX_CPU_THIS_PTR jmp_buf_env, 1)',
    'take_pending_startup_snapshot',
    'ordinary_ram_readable',
    'copy_from_ordinary_ram',
    'complete_profile_startup_snapshot'
)) {
    if (-not $text.Contains($required)) { throw "Missing required neutral exception-seam token: $required" }
}

foreach ($forbidden in @('BOP', 'OpenNT', 'DOS', 'WOW', 'DEM', 'COMMAND')) {
    if ($text -match ("(?i)(?<![A-Za-z0-9_])" + [regex]::Escape($forbidden) + "(?![A-Za-z0-9_])")) {
        throw "Forbidden semantic token found in Bochs #UD seam: $forbidden"
    }
}
if ($text -match '0xC4') { throw 'Forbidden marker byte found in Bochs #UD seam: 0xC4' }

Write-Host 'Bochs #UD interception boundary verification passed.'
