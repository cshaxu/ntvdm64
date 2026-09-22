param(
    [string]$BuildRoot = 'build/M0-T420/S40/provider-stdcall-r1',
    [switch]$RequireLocalRegistration
)
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$build = (Resolve-Path (Join-Path $root $BuildRoot)).Path
if (!$build.StartsWith(($root + '\build\'), [StringComparison]::OrdinalIgnoreCase)) {
    throw 'Select a repository build root.'
}
$source = 'O:/repos.external/OpenNT/windows/core/ntuser/client/client.c'
if ((Get-FileHash $source).Hash -ne '787EDA33785A87CE9E71202C97B5913E2857F7106FED5BB97103049B1D5E3472') {
    throw 'Original USER registration source changed.'
}
$text = [IO.File]::ReadAllText($source)
$start = $text.IndexOf('DWORD UserRegisterWowHandlers(')
$end = $text.IndexOf('return (DWORD)&gSharedInfo;', $start)
if ($start -lt 0 -or $end -le $start) { throw 'Original registration boundary changed.' }
$assignments = [regex]::Matches($text.Substring($start, $end - $start),
    'apfnWowOut->(pfn\w+)\s*=\s*(\w+)\s*;')
$header = [IO.File]::ReadAllText((Join-Path $root 'src/opennt-abi/source/public/internal/windows/inc/wowuserp.h'))
$headerStart = $header.IndexOf('pfnCsCreateWindowEx;')
$headerEnd = $header.IndexOf('pfnFillWindow;', $headerStart)
if ($headerStart -lt 0 -or $headerEnd -lt $headerStart) { throw 'Output ABI boundary changed.' }
$fields = @([regex]::Matches($header.Substring($headerStart, $headerEnd + 'pfnFillWindow;'.Length - $headerStart),
    '\b(pfn\w+)\s*;') | ForEach-Object { $_.Groups[1].Value })
$names = @($assignments | ForEach-Object { $_.Groups[1].Value })
if ($assignments.Count -ne 20 -or $fields.Count -ne 20 -or
    @(Compare-Object $names $fields).Count -ne 0) { throw 'Output ABI/source slots do not match.' }
$bridgePath = Join-Path $root 'src/wow32-dll/source/wow_user_registration_bridge.c'
$bridge = [IO.File]::ReadAllText($bridgePath)
$profile = (& (Join-Path $PSScriptRoot 'verify-wow-user-profile.ps1')) | ConvertFrom-Json
$buildInfo = [regex]::Match($bridge, 'output->dwBldInfo\s*=\s*0x([0-9a-fA-F]{8})u;')
if (!$buildInfo.Success -or $buildInfo.Groups[1].Value -ine $profile.userRegistration.buildInfo) {
    throw 'Registration build/layout identity differs from the pinned immutable USER consumer.'
}
$bridgeStart = $bridge.IndexOf('DWORD WINAPI UserRegisterWowHandlers(')
if ($bridgeStart -lt 0) { throw 'Local registration bridge is missing.' }
$bridgeEnd = $bridge.IndexOf('return shared;', $bridgeStart)
if ($bridgeEnd -le $bridgeStart) { throw 'Local registration bridge return boundary changed.' }
$bridgeAssignments = @([regex]::Matches($bridge.Substring($bridgeStart, $bridgeEnd - $bridgeStart),
    'output->(pfn\w+)\s*=\s*([^;]+);') | ForEach-Object {
        [pscustomobject]@{ slot = $_.Groups[1].Value; target = $_.Groups[2].Value.Trim() }
    })
if ($bridgeAssignments.Count -ne 20 -or
    @(Compare-Object @($bridgeAssignments.slot) $fields).Count -ne 0 -or
    @($bridgeAssignments | Where-Object { [string]::IsNullOrWhiteSpace($_.target) }).Count -ne 0) {
    throw 'Local registration bridge does not populate exactly every output slot.'
}
$mapPath = Join-Path $build 'wow32.dll.map'
$map = [IO.File]::ReadAllText($mapPath)
$bindings = @{
    pfnServerLoadCreateCursorIcon = 'wow_private_user_load_create_cursor_icon'
    pfnServerLoadCreateMenu = 'wow_private_user_load_create_menu'
    pfnWOWGetIdFromDirectory = 'wow_private_user_get_id_from_directory'
    pfnGetMenuIndex = 'wow_private_user_get_menu_index'
    pfnWowGetDefWindowProcBits = 'wow_private_user_get_def_window_proc_bits'
    pfnFillWindow = 'wow_private_user_fill_window'
}
# A recovered core has a different ABI from the output-table callback. Its
# presence is useful recovery evidence, never permission to assign the slot.
$cores = @{
    pfnDirectedYield = 'xxxDirectedYield'
    pfnYieldTask = 'xxxUserYield'
    pfnWowWaitForMsgAndEvent = 'xxxSleepTask'
    pfnRegisterClassWOWA = 'wow_original_register_class'
    pfnRegisterUserHungAppHandlers = 'xxxRegisterUserHungAppHandlers'
    pfnInitTask = 'xxxInitTask'
}
function Symbol-Line([string]$symbol) {
    # Exact stdcall symbol, not a similarly named WOW caller or import pointer.
    $pattern = '(?m)^\s*\d+:\w+\s+_' + [regex]::Escape($symbol) + '@\d+\s+[^\r\n]+\r?$'
    $match = [regex]::Match($map, $pattern)
    if ($match.Success) { $match.Value.Trim() } else { $null }
}
if (!(Symbol-Line 'W32Init')) { throw 'Map does not expose the selected WOW32 ABI symbols.' }
$registration = Symbol-Line 'UserRegisterWowHandlers'
$registrationKind = if (!$registration) { 'absent' }
    elseif ($registration -match '(?i):[^\s]+\.dll\s*$') { 'external-dll-import' }
    else { 'local-symbol-runtime-unverified' }
$rows = @($assignments | ForEach-Object {
    $field = $_.Groups[1].Value
    $owner = $_.Groups[2].Value
    $binding = $bindings[$field]
    [ordered]@{
        slot = $field
        originalOwner = $owner
        originalSymbol = Symbol-Line $owner
        bindingCandidate = $binding
        bindingSymbol = if ($binding) { Symbol-Line $binding } else { $null }
        recoveredCore = $cores[$field]
        recoveredCoreSymbol = if ($cores[$field]) { Symbol-Line $cores[$field] } else { $null }
        acceptance = 'not-established-by-symbol-presence'
    }
})
[ordered]@{
    kind = 'composition-observation-not-runtime-acceptance'
    originalRegistrationSHA256 = (Get-FileHash $source).Hash
    localRegistrationBridgeSHA256 = (Get-FileHash $bridgePath).Hash
    providerSHA256 = (Get-FileHash (Join-Path $build 'wow32.dll')).Hash
    mapSHA256 = (Get-FileHash $mapPath).Hash
    registrationSymbol = $registration
    registrationKind = $registrationKind
    localRegistrationGate = ($registrationKind -eq 'local-symbol-runtime-unverified')
    localOutputSlotCount = $bridgeAssignments.Count
    slots = $rows
} | ConvertTo-Json -Depth 5
if ($RequireLocalRegistration -and $registrationKind -ne 'local-symbol-runtime-unverified') {
    throw "WOW output registration is $registrationKind; linked recovery cores do not populate the output table."
}
