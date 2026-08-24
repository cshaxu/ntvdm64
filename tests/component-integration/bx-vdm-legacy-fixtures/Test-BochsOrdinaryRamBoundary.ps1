[CmdletBinding()]
param(
    [string]$RepositoryRoot = ''
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path))
}

$path = Join-Path $RepositoryRoot 'src\bochs-core\memory\misc_mem.cc'
if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing Bochs memory source: $path" }
$text = Get-Content -LiteralPath $path -Raw
$preflightStart = $text.IndexOf('bx_bool BX_MEM_C::ordinary_ram_readable')
if ($preflightStart -lt 0) { throw 'Missing ordinary-RAM readable preflight' }
$copyStart = $text.IndexOf('bx_bool BX_MEM_C::copy_from_ordinary_ram')
if ($copyStart -lt 0) { throw 'Missing ordinary-RAM observation primitive' }
$end = $text.IndexOf("`n/*", $copyStart)
if ($end -lt 0) { throw 'Could not bound ordinary-RAM observation primitive' }
$preflight = $text.Substring($preflightStart, $copyStart - $preflightStart)
$body = $text.Substring($copyStart, $end - $copyStart)

foreach ($required in @(
    'len == 0',
    'A20ADDR(current) != current',
    'memory_handlers[current >> 20]',
    'is_monitor(',
    'getHostMemAddr(NULL, current, BX_READ)'
)) {
    if (-not $preflight.Contains($required)) { throw "Missing observation preflight safety token: $required" }
}
foreach ($required in @(
    'data == NULL || !BX_MEM_THIS ordinary_ram_readable(addr, len)',
    'memcpy(data, memptr, chunk)'
)) {
    if (-not $body.Contains($required)) { throw "Missing observation copy token: $required" }
}
foreach ($forbidden in @(
    'readPhysicalPage', 'dmaReadPhysicalPage', 'BOP', 'OpenNT', 'DOS',
    'WOW', 'DEM', 'COMMAND', '0x714', '0x496', 'adapter'
)) {
    if ($preflight -match ("(?i)(?<![A-Za-z0-9_])" + [regex]::Escape($forbidden) + '(?![A-Za-z0-9_])') -or
        $body -match ("(?i)(?<![A-Za-z0-9_])" + [regex]::Escape($forbidden) + '(?![A-Za-z0-9_])')) {
        throw "Forbidden semantic or fallback token found in ordinary-RAM observation: $forbidden"
    }
}

Write-Host 'Bochs ordinary-RAM observation boundary verification passed.'
