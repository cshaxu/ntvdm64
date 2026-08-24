$ErrorActionPreference = 'Stop'
$repository = Split-Path -Parent (Split-Path $PSScriptRoot)
$header = Get-Content (Join-Path $repository 'src\bx-vdm\startup_session.h') -Raw
$source = Get-Content (Join-Path $repository 'src\bx-vdm\startup_session.c') -Raw
$environmentHeader = Get-Content (Join-Path $repository 'src\bx-vdm\startup_session_environment.h') -Raw
$environmentSource = Get-Content (Join-Path $repository 'src\bx-vdm\startup_session_environment.c') -Raw
foreach ($term in @('byob_image.h', 'startup_plan_abi.h')) {
    if (-not $header.Contains($term)) { throw "Missing required startup-session include: $term" }
}
foreach ($term in @('byob_profile_validate_file_select', 'byob_image_load_exact', 'byob_image_release', '0x700u', '0x714u', 'entry.cs = 0x70u')) {
    if (-not $source.Contains($term)) { throw "Missing required startup-session control: $term" }
}
$environmentRequired = @('GetEnvironmentVariableW', 'NTDOS64_ADAPTER_PROFILE', 'NTDOS64_ADAPTER_ROOT', 'bx_ntvdm_startup_session_v1_install', 'bx_ntvdm_startup_session_v1_prepare_ntio_plan')
foreach ($term in $environmentRequired) {
    if (-not $environmentSource.Contains($term)) { throw "Missing required startup-session environment control: $term" }
}
$code = (($header + "`n" + $source + "`n" + $environmentHeader + "`n" + $environmentSource) -replace '(?s)/\*.*?\*/', '' -replace '(?m)//.*$', '')
foreach ($term in @('bx_ntvdm_adapter_runtime', 'bx_ntvdm_host_session', 'host_service', 'dem_', 'bop', 'search', 'namespace', 'drive_policy', 'CreateFile', 'BX_MEM', 'bochs')) {
    if ($code.IndexOf($term, [StringComparison]::OrdinalIgnoreCase) -ge 0) {
        throw "Prohibited startup-session boundary term: $term"
    }
}
Write-Output 'bx-ntvdm-startup-session boundary: PASS'
