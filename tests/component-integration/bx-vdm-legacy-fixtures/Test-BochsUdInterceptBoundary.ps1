[CmdletBinding()]
param(
    [string]$RepositoryRoot = ''
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path))
}

$exception = Join-Path $RepositoryRoot 'src\bx-core\cpu\exception.cc'
foreach ($path in @($exception)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing seam file: $path" }
}

$text = Get-Content -LiteralPath $exception -Raw
foreach ($required in @(
    'vector != BX_UD_EXCEPTION',
    'BX_NTVDM_GENERIC_UD_RESUME',
    'mantle_context_outcome.resume_rip',
    'longjmp(BX_CPU_THIS_PTR jmp_buf_env, 1)'
)) {
    if (-not $text.Contains($required)) { throw "Missing required neutral exception-seam token: $required" }
}

foreach ($forbidden in @('OpenNT', 'DOS', 'WOW', 'DEM', 'COMMAND',
        'NTDOS64_ADAPTER_TRACE', 'bx_ntvdm_exception_observer_v1')) {
    if ($text -match ("(?i)(?<![A-Za-z0-9_])" + [regex]::Escape($forbidden) + "(?![A-Za-z0-9_])")) {
        throw "Forbidden semantic token found in Bochs #UD seam: $forbidden"
    }
}
if ($text -match '0xC4') { throw 'Forbidden marker byte found in Bochs #UD seam: 0xC4' }

Write-Host 'Bochs #UD interception boundary verification passed.'
