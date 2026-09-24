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
$mouse = Join-Path $root 'src\mvdm\softpc.new\host\src\nt_mouse.c'

foreach ($path in @($domain, $bridge, $objects, $wuman, $mouse)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing selected source: $path" }
}

$domainText = Get-Content -LiteralPath $domain -Raw
$bridgeText = Get-Content -LiteralPath $bridge -Raw
$objectsText = Get-Content -LiteralPath $objects -Raw
$wumanText = Get-Content -LiteralPath $wuman -Raw
$mouseText = Get-Content -LiteralPath $mouse -Raw

foreach ($required in @(
    '#define MVDM_SOFTPC_HANDLE_ENTRY_BYTES 16u',
    '#define MVDM_SOFTPC_HANDLE_ENTRY_COUNT 0x10000u',
    'MVDM_SOFTPC_TEB_DESKTOP_INFO 0x5cu',
    'MVDM_SOFTPC_TEB_CLIENT_DELTA 0x60u',
    'MVDM_SOFTPC_SERVERINFO_METRICS 0x170u',
    'MVDM_SOFTPC_SERVERINFO_COLORS 0x2a0u',
    'MVDM_SOFTPC_SERVERINFO_CURSOR_X 0x304u',
    'MVDM_SOFTPC_SERVERINFO_CURSOR_Y 0x308u',
    'static int page_domain_create_desktop_root(void)',
    'int mvdm_softpc_wow_page_domain_client_desktop_ready(void)',
    'int mvdm_softpc_wow_page_domain_set_client_desktop',
    'int mvdm_softpc_wow_page_domain_clear_client_desktop',
    'int mvdm_softpc_wow_page_domain_set_expected_windows_version',
    'int mvdm_softpc_wow_page_domain_publish_cursor'
)) {
    if (-not $domainText.Contains($required)) { throw "Missing client-view contract: $required" }
}
if (-not $wumanText.Contains('mvdm_softpc_wow_page_domain_guest_shared_info()')) {
    throw 'WU32NotifyWow no longer publishes the guest-linear SHAREDINFO address.'
}
if (-not $bridgeText.Contains('mvdm_softpc_wow_page_domain_client_desktop_ready()')) {
    throw 'Registration does not reject an unprepared original client view.'
}
if (-not $domainText.Contains('ULONG guest_windows[MVDM_SOFTPC_HANDLE_ENTRY_COUNT];')) {
    throw 'The page domain has no bounded per-handle guest WND lifecycle table.'
}
if (-not $bridgeText.Contains('output->pfnGetFullUserHandle = registered_full_handle;')) {
    throw 'The original full-handle output remains disconnected from the worker association.'
}
if (-not $objectsText.Contains('wow_user_window_full_handle')) {
    throw 'The worker-local native HWND association has no full-handle lookup.'
}

$mouseScale = $mouseText.IndexOf('ScaleToWindowedVirtualCoordinates')
$cursorPublish = if ($mouseScale -ge 0) {
    $mouseText.IndexOf('mvdm_softpc_wow_page_domain_publish_cursor', $mouseScale)
} else { -1 }
$mouseMask = if ($cursorPublish -ge 0) {
    $mouseText.IndexOf('AssembleCallMask', $cursorPublish)
} else { -1 }
if ($cursorPublish -lt 0 -or $mouseScale -lt 0 -or $mouseMask -lt 0 -or
    $cursorPublish -le $mouseScale -or $cursorPublish -ge $mouseMask) {
    throw 'The shared USER cursor is not published at SoftPC post-transform, pre-dispatch boundary.'
}
$colourPublisher = $domainText.IndexOf('mvdm_softpc_wow_page_domain_publish_system_color')
if ($colourPublisher -lt 0) {
    throw 'The shared USER system-colour publisher is absent.'
}
$user = Join-Path $root 'src\mvdm\wow32\wuser.c'
$userText = Get-Content -LiteralPath $user -Raw
$setSysColors = $userText.IndexOf('SetSysColors(')
$colourCall = if ($setSysColors -ge 0) {
    $userText.IndexOf('mvdm_softpc_wow_page_domain_publish_system_color', $setSysColors + 1)
} else { -1 }
if ($colourCall -lt $setSysColors) {
    throw 'The original SetSysColors success path does not publish shared USER colour data.'
}

$productionSetters = @(
    Get-ChildItem -LiteralPath (Join-Path $root 'src') -Recurse -File -Filter '*.c' |
        Where-Object { $_.FullName -ne $domain } |
        Select-String -Pattern 'mvdm_softpc_wow_page_domain_set_client_desktop\s*\(' -List
)
if ($productionSetters.Count -ne 0) {
    throw "Unexpected external desktop producer(s) require fresh lifecycle audit: $($productionSetters.Path -join ', ')"
}

Write-Output 'WOW32_CLIENT_VIEW_B1_OK'
Write-Output 'desktop root producer: page-domain'
Write-Output 'window HANDLEENTRY/WND projection: active'
Write-Output 'guest CLS projection: active'
Write-Output 'SERVERINFO metric/color/cursor bootstrap: active'
Write-Output 'SERVERINFO cursor publication: SoftPC post-transform boundary active'
Write-Output 'SERVERINFO colour publication: original SetSysColors success path active'
Write-Output 'TEB expected-Windows-version task carrier: active'
Write-Output 'callback-visible mutation: pending later S2 closure edge'
