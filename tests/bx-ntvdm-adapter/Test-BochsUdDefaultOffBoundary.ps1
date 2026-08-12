[CmdletBinding()]
param(
    [string]$RepositoryRoot = ''
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path))
}

$exceptionPath = Join-Path $RepositoryRoot 'src\bochs\cpu\exception.cc'
if (-not (Test-Path -LiteralPath $exceptionPath -PathType Leaf)) {
    throw "Missing exception source: $exceptionPath"
}

$source = Get-Content -LiteralPath $exceptionPath -Raw
$default = '#ifndef BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT' + "`n" +
           '#define BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT 0' + "`n" +
           '#endif'
if (-not $source.Contains($default)) {
    throw 'Missing BX-ABI-027 local default-off macro invariant'
}

$includeStart = $source.IndexOf('#if BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT', [System.StringComparison]::Ordinal)
$includeEnd = $source.IndexOf('#endif', $includeStart, [System.StringComparison]::Ordinal)
if ($includeStart -lt 0 -or $includeEnd -lt $includeStart) {
    throw 'Missing BX-ABI-027 guarded adapter include block'
}
$logThis = '#define LOG_THIS BX_CPU_THIS_PTR'
$logThisIndex = $source.IndexOf($logThis, [System.StringComparison]::Ordinal)
if ($logThisIndex -lt 0 -or $logThisIndex -ge $includeStart) {
    throw 'LOG_THIS must remain outside the BX-ABI-027 default-off guard'
}
$includeBlock = $source.Substring($includeStart, $includeEnd - $includeStart)
foreach ($token in @(
        '#include "bx_ntvdm_exception_intercept.h"',
        '#include "bx_ntvdm_exception_abi.h"',
        '#include "bx_ntvdm_adapter_runtime.h"',
        '#include "bx_ntvdm_guest_gather_read_action_v1.h"',
        'bx_ntvdm_try_intercept_ud')) {
    if (-not $includeBlock.Contains($token)) {
        throw "Missing guarded adapter seam token: $token"
    }
}

$exceptionStart = $source.IndexOf('void BX_CPU_C::exception(unsigned vector, Bit16u error_code)', [System.StringComparison]::Ordinal)
$instrumentIndex = $source.IndexOf('BX_INSTR_EXCEPTION(BX_CPU_ID, vector, error_code);', $exceptionStart, [System.StringComparison]::Ordinal)
$guardIndex = $source.IndexOf('#if BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT', $instrumentIndex, [System.StringComparison]::Ordinal)
$debuggerIndex = $source.IndexOf('#if BX_DEBUGGER', $guardIndex, [System.StringComparison]::Ordinal)
if ($exceptionStart -lt 0 -or $instrumentIndex -lt $exceptionStart -or $guardIndex -lt $instrumentIndex -or $debuggerIndex -lt $guardIndex) {
    throw 'Missing BX-ABI-027 guarded exception-body seam ordering'
}
$bodyBlock = $source.Substring($guardIndex, $debuggerIndex - $guardIndex)
foreach ($token in @('bx_ntvdm_try_intercept_ud', 'bx_ntvdm_cpu_state_v1', 'bx_ntvdm_instruction_window_v1', 'response.resume_rip')) {
    if (-not $bodyBlock.Contains($token)) {
        throw "Missing guarded exception-body token: $token"
    }
}
$originalLogIndex = $source.IndexOf('BX_DEBUG(("exception(0x%02x): error_code=%04x"', $debuggerIndex, [System.StringComparison]::Ordinal)
if ($originalLogIndex -lt $debuggerIndex) {
    throw 'Original post-BX_INSTR_EXCEPTION logging path is no longer reachable outside the seam guard'
}

$startupIncludeStart = $source.IndexOf('#if BX_NTVDM_ENABLE_STARTUP_TRANSACTION', [System.StringComparison]::Ordinal)
$startupIncludeEnd = $source.IndexOf('#endif', $startupIncludeStart, [System.StringComparison]::Ordinal)
if ($startupIncludeStart -lt 0 -or $startupIncludeEnd -lt $startupIncludeStart) {
    throw 'Missing BX-ABI-036 guarded startup-transaction include block'
}
$outside = $source.Substring(0, $startupIncludeStart) +
           $source.Substring($startupIncludeEnd + '#endif'.Length, $includeStart - ($startupIncludeEnd + '#endif'.Length)) +
           $source.Substring($includeEnd)
foreach ($token in @('bx_ntvdm_exception_intercept.h', 'bx_ntvdm_exception_abi.h', 'bx_ntvdm_adapter_runtime.h', 'bx_ntvdm_guest_gather_read_action_v1.h')) {
    if ($token -in @('bx_ntvdm_exception_intercept.h', 'bx_ntvdm_exception_abi.h')) { continue }
    if ($outside.Contains($token)) {
        throw "Adapter include escapes default-off guard: $token"
    }
}

$cpuResultDefault = '#ifndef BX_NTVDM_ENABLE_CPU_RESULT_BRIDGE' + "`n" +
                    '#define BX_NTVDM_ENABLE_CPU_RESULT_BRIDGE 0' + "`n" + '#endif'
if (-not $source.Contains($cpuResultDefault)) { throw 'Missing BX-ABI-015 default-off macro.' }
$cpuResultStart = $source.IndexOf('#if BX_NTVDM_ENABLE_CPU_RESULT_BRIDGE', [System.StringComparison]::Ordinal)
$cpuResultEnd = $source.IndexOf('#endif', $cpuResultStart, [System.StringComparison]::Ordinal)
if ($cpuResultStart -lt 0 -or $cpuResultEnd -lt $cpuResultStart) { throw 'Missing BX-ABI-015 guarded CPU-result bridge block.' }
$cpuResultBlock = $source.Substring($cpuResultStart, $cpuResultEnd - $cpuResultStart)
foreach ($required in @('bx_ntvdm_cpu_result_bridge_v1_dispatch', 'BX_NTVDM_CPU_RESULT_V2_RESUME', 'response->cpu_result')) {
    if (-not $cpuResultBlock.Contains($required)) { throw "Missing CPU-result bridge token: $required" }
}
foreach ($forbidden in @('BOP', 'BIOS', 'OpenNT', 'DOS', 'WOW', 'DEM', '0x12', '0xC4', 'set_reg16', 'set_CF')) {
    if ($cpuResultBlock.IndexOf($forbidden, [StringComparison]::OrdinalIgnoreCase) -ge 0) { throw "Forbidden CPU-result bridge term: $forbidden" }
}

Write-Host 'Bochs #UD default-off compilation boundary verification passed.'
