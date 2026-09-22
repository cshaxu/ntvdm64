param(
    [Parameter(Mandatory = $true)]
    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$domain = Join-Path $root 'src\ntvdm-exe\softpc\mvdm_softpc_wow_page_domain.c'
$bridge = Join-Path $root 'src\wow32-dll\source\wow_user_registration_bridge.c'
$objects = Join-Path $root 'src\wow32-dll\source\wow_user_object_bindings.c'
$wuman = Join-Path $root 'src\mvdm\wow32\wuman.c'

foreach ($path in @($domain, $bridge, $objects, $wuman)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing selected source: $path" }
}

$domainText = Get-Content -LiteralPath $domain -Raw
$bridgeText = Get-Content -LiteralPath $bridge -Raw
$objectsText = Get-Content -LiteralPath $objects -Raw
$wumanText = Get-Content -LiteralPath $wuman -Raw

foreach ($required in @(
    '#define MVDM_SOFTPC_HANDLE_ENTRY_BYTES 16u',
    '#define MVDM_SOFTPC_HANDLE_ENTRY_COUNT 0x10000u',
    'MVDM_SOFTPC_TEB_DESKTOP_INFO 0x5cu',
    'MVDM_SOFTPC_TEB_CLIENT_DELTA 0x60u',
    'int mvdm_softpc_wow_page_domain_set_client_desktop',
    'int mvdm_softpc_wow_page_domain_clear_client_desktop'
)) {
    if (-not $domainText.Contains($required)) { throw "Missing client-view contract: $required" }
}
if (-not $wumanText.Contains('mvdm_softpc_wow_page_domain_guest_shared_info()')) {
    throw 'WU32NotifyWow no longer publishes the guest-linear SHAREDINFO address.'
}
if (-not $bridgeText.Contains('unsupported_full_handle')) {
    throw 'The explicit incomplete GetFullUserHandle slot is no longer visible for audit.'
}
if (-not $objectsText.Contains('SetLastError(ERROR_INVALID_STATE); return FALSE;')) {
    throw 'The active guest handle publisher no longer explicitly rejects incomplete phead publication.'
}

$productionSetters = @(
    Get-ChildItem -LiteralPath (Join-Path $root 'src') -Recurse -File -Filter '*.c' |
        Where-Object { $_.FullName -ne $domain } |
        Select-String -Pattern 'mvdm_softpc_wow_page_domain_set_client_desktop\s*\(' -List
)
if ($productionSetters.Count -ne 0) {
    throw "Unexpected desktop producer(s) require fresh lifecycle audit: $($productionSetters.Path -join ', ')"
}

Write-Output 'WOW32_CLIENT_VIEW_GAP_CONFIRMED'
Write-Output 'desktop producer calls: 0'
Write-Output 'active incomplete HANDLEENTRY publication: explicit failure'
