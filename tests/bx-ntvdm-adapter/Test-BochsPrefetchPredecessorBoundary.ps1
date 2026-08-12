[CmdletBinding()]
param(
    [string]$RepositoryRoot = ''
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path))
}

$cpu = Join-Path $RepositoryRoot 'src\bochs\cpu\cpu.cc'
if (-not (Test-Path -LiteralPath $cpu -PathType Leaf)) {
    throw "Missing CPU source: $cpu"
}
$text = Get-Content -LiteralPath $cpu -Raw
if ($text -notmatch '#ifndef BX_NTVDM_ENABLE_PREFETCH_PREDECESSOR_DIAGNOSTIC\s*\r?\n#define BX_NTVDM_ENABLE_PREFETCH_PREDECESSOR_DIAGNOSTIC 0') {
    throw 'Prefetch-predecessor diagnostic is not default-off.'
}
$start = $text.IndexOf('#if BX_NTVDM_ENABLE_PREFETCH_PREDECESSOR_DIAGNOSTIC')
if ($start -lt 0) { throw 'Missing prefetch-predecessor diagnostic block.' }
$end = $text.IndexOf('#endif', $start)
if ($end -lt $start) { throw 'Unable to isolate prefetch-predecessor diagnostic block.' }
$block = $text.Substring($start, ($end + '#endif'.Length) - $start)
foreach ($required in @('real_mode()', 'PREV_RIP', 'EIP', 'limit', 'ntdos64 real-mode prefetch-limit')) {
    if (-not $block.Contains($required)) { throw "Missing required diagnostic token: $required" }
}
foreach ($forbidden in @('BOP', 'OpenNT', 'SoftPC', 'adapter',
        'read_', 'system_read', 'write_', 'copy_', 'exception(', 'EIP =', 'RIP =')) {
    if ($block -match ('(?i)' + [regex]::Escape($forbidden))) {
        throw "Forbidden semantic/state token in prefetch diagnostic: $forbidden"
    }
}
Write-Host 'Bochs prefetch-predecessor boundary verification passed.'
