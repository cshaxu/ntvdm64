$ErrorActionPreference = 'Stop'

$repository = Split-Path -Parent $PSScriptRoot
$source = Get-Content -LiteralPath (Join-Path $repository 'src\bochs\cpu\exception.cc') -Raw

if ($source -notmatch '(?s)#ifndef BX_NTVDM_ENABLE_UNMATCHED_UD_DIAGNOSTIC\s*#define BX_NTVDM_ENABLE_UNMATCHED_UD_DIAGNOSTIC 0\s*#endif') {
    throw 'The unmatched-UD diagnostic must be default-off.'
}
if ($source -notmatch 'ntdos64 unmatched-ud cpu=%u cs=%04x rip=%08x') {
    throw 'Missing fixed unmatched-UD position record.'
}
$startup = $source.IndexOf('if (bx_ntvdm_startup_transaction_interceptor(&request, &response))')
$diagnostic = $source.IndexOf('ntdos64 unmatched-ud')
$broad = $source.IndexOf('#if BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT', $startup)
if ($startup -lt 0 -or $diagnostic -lt $startup -or ($broad -ge 0 -and $diagnostic -gt $broad)) {
    throw 'The diagnostic must occur only after the narrow interceptor declines and before the broad path.'
}
if ($source -notmatch '(?s)#if BX_NTVDM_ENABLE_UNMATCHED_UD_DIAGNOSTIC\s*BX_INFO\(\(.*?prev_rip.*?\)\);\s*#endif') {
    throw 'The diagnostic must be locally macro-gated and use only the existing fault position.'
}
if ($source -match 'unmatched-ud.*(instruction_window|bx_ntvdm_|copy_from_ordinary_ram|copy_to_ordinary_ram)') {
    throw 'The diagnostic must not read guest memory or call the adapter.'
}

Write-Host 'Unmatched-UD diagnostic boundary checks passed.'
