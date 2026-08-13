param(
    [string]$RepositoryRoot = (Split-Path -Parent (Split-Path -Parent $PSScriptRoot))
)

$ErrorActionPreference = 'Stop'
$root = Join-Path $RepositoryRoot 'src\bx-vdm'
$frozen = @(
    'bx_ntvdm_dem_readonly_file_service.c','bx_ntvdm_dem_boot_drive_service.c',
    'bx_ntvdm_dem_drive_service.c','bx_ntvdm_dem_debug_service.c',
    'bx_ntvdm_dem_hard_error_service.c',
    'bx_ntvdm_dem_dta_service.c','bx_ntvdm_dem_dpb_service.c',
    'bx_ntvdm_dem_path_search_service_v1.c',
    'bx_ntvdm_vdd_create_user_notify_service.c','bx_ntvdm_cmd_get_next_service.c',
    'bx_ntvdm_cmd_comspec_bootstrap_service.c','bx_ntvdm_cmd_set_info_service.c',
    'bx_ntvdm_cmd_boot_file_service.c','bx_ntvdm_cmd_current_dir_service.c',
    'bx_ntvdm_cmd_keyboard_layout_service.c'
)
$core = @('bx_ntvdm_bop_ingress_v1.c','bx_ntvdm_bop_catalog_v1.c',
    'bx_ntvdm_dem_plane_v1.c','bx_ntvdm_command_plane_v1.c',
    'bx_ntvdm_xms_dpmi_plane_v1.c','bx_ntvdm_system_plane_v1.c')
$found = @()
Get-ChildItem -LiteralPath $root -Filter '*.c' | ForEach-Object {
    $text = Get-Content -LiteralPath $_.FullName -Raw
    if ($text -match 'bytes\[2\]' -and $text -match 'bytes\[3\]') { $found += $_.Name }
}
foreach ($name in $found) {
    if ($name -notin $frozen -and $name -notin $core) { throw "Unregistered direct BOP recognizer: $name" }
}
foreach ($name in $frozen) {
    if ($name -notin $found) { throw "Frozen recognizer missing or no longer direct: $name" }
}
Write-Output "Legacy BOP endpoint register verified: frozen=$($frozen.Count) core=$($core.Count) direct=$($found.Count)"
