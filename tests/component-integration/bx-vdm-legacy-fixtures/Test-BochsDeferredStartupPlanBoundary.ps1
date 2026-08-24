$ErrorActionPreference = 'Stop'

$repository = Split-Path -Parent (Split-Path $PSScriptRoot)
$main = Get-Content (Join-Path $repository 'refs\bochs\main.cc') -Raw
$exception = Get-Content (Join-Path $repository 'src\bx-core\cpu\exception.cc') -Raw

$default = '#ifndef BX_NTVDM_ENABLE_DEFERRED_STARTUP_PLAN' + "`n" +
    '#define BX_NTVDM_ENABLE_DEFERRED_STARTUP_PLAN 0' + "`n" + '#endif'
if (-not $exception.Contains($default)) { throw 'Missing default-off deferred plan macro.' }

$consumerStart = $exception.IndexOf('static bx_bool bx_ntvdm_deferred_startup_plan_consumer')
$consumerEnd = $exception.IndexOf('#endif', $consumerStart)
if ($consumerStart -lt 0 -or $consumerEnd -lt $consumerStart) {
    throw 'Unable to isolate deferred plan consumer.'
}
$consumer = $exception.Substring($consumerStart, $consumerEnd - $consumerStart)
$ordered = @(
    'take_deferred_startup_plan', 'bx_ntvdm_startup_plan_v1_preflight',
    'ordinary_ram_readable', 'copy_from_ordinary_ram',
    'copy_to_ordinary_ram', 'payload_write.guest_physical_address',
    'copy_to_ordinary_ram', 'preserved_state_address', 'apply_real_mode_entry',
    'BX_NTVDM_EXCEPTION_RESUME')
$offset = 0
foreach ($token in $ordered) {
    $next = $consumer.IndexOf($token, $offset)
    if ($next -lt $offset) { throw "Missing or misordered consumer token: $token" }
    $offset = $next + $token.Length
}
foreach ($forbidden in @('BOP', 'OpenNT', 'DOS', 'WOW', 'DEM', 'COMMAND',
        'firmware', 'marker', '0xC4', '0xc8', '0x0f', '0x0b', 'selector',
        'device', 'profile')) {
    if ($consumer -match [regex]::Escape($forbidden)) {
        throw "Forbidden semantic term in deferred plan consumer: $forbidden"
    }
}
if ($consumer -notmatch 'return 0;' -or $consumer -notmatch 'return 1;') {
    throw 'Consumer does not preserve explicit refusal/success outcomes.'
}
$call = $exception.IndexOf('bx_ntvdm_deferred_startup_plan_consumer(BX_CPU(0)')
$startupEnd = $exception.IndexOf('#if BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT', $call)
$legacyEnd = $exception.LastIndexOf('#endif', $call)
if ($call -lt 0 -or $legacyEnd -lt 0 -or $startupEnd -lt $call -or
    $exception.Substring($legacyEnd, $call - $legacyEnd) -match 'STARTUP_TRANSACTION') {
    throw 'Deferred consumer is incorrectly nested in the legacy startup transaction path.'
}

$hardware = $main.IndexOf('bx_init_hardware();')
$prepare = $main.IndexOf('bx_prepare_deferred_startup_plan()', $hardware)
$cpuLoop = $main.IndexOf('BX_CPU(0)->cpu_loop()', $hardware)
if ($hardware -lt 0 -or $prepare -lt $hardware -or $cpuLoop -lt $prepare) {
    throw 'Deferred pre-loop preparation is not after hardware initialization and before CPU loop.'
}
Write-Output 'Bochs deferred startup-plan boundary verification passed.'
