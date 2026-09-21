param(
    [string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path,
    [string]$GuestUser = 'O:\winnt\system32\USER.EXE'
)
$ErrorActionPreference = 'Stop'

# Read-only provenance/profile check. This is not WOW runtime acceptance.
$expectedHash = '6746088F2A87CD366E4FF5DA424B39B5BD91586D5F7570AFBF3C05BCA94E935C'
$mirror = Join-Path $RepositoryRoot 'src/mvdm/wow16/user/k/daytona/USER.EXE'
foreach ($path in @($GuestUser, $mirror)) {
    if ((Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash -ne $expectedHash) {
        throw "Unrecognized USER.EXE; profile offsets cannot be applied: $path"
    }
}
$bytes = [IO.File]::ReadAllBytes($GuestUser)
$ne = [BitConverter]::ToInt32($bytes, 0x3c)
if ($bytes[$ne] -ne 0x4e -or $bytes[$ne + 1] -ne 0x45) { throw 'Not an NE image' }
$table = $ne + [BitConverter]::ToUInt16($bytes, $ne + 0x22)
$shift = [BitConverter]::ToUInt16($bytes, $ne + 0x32)
$code = [BitConverter]::ToUInt16($bytes, $table) -shl $shift
$map = Get-Content -Raw (Join-Path $RepositoryRoot 'src/mvdm/wow16/user/k/daytona/user.map')
function Get-CodeOffset([string]$symbol) {
    $match = [regex]::Match($map, '(?m)^\s*0001:([0-9A-Fa-f]+)\s+' + [regex]::Escape($symbol) + '\s*$')
    if (!$match.Success) { throw "Missing source map symbol: $symbol" }
    return [Convert]::ToInt32($match.Groups[1].Value, 16)
}
$metrics = $code + (Get-CodeOffset 'IGetSystemMetrics')
$setup = [BitConverter]::ToString($bytes, $metrics, 14)
if ($setup -ne '55-8B-EC-1E-B8-23-00-8E-D8-B8-3B-00-8E-E0') {
    throw 'Missing PMODE32 DS=23h / FS=3Bh setup'
}
$validate = $code + (Get-CodeOffset '@HMValidateHandle@8')
$sharedOffset = Get-CodeOffset '_wow16gpsi'
if ([BitConverter]::ToString($bytes, $validate, 5) -ne '66-56-2E-66-A1' -or
    [BitConverter]::ToUInt16($bytes, $validate + 5) -ne $sharedOffset) {
    throw 'Handle validator does not read the mapped wow16gpsi DWORD'
}
# Pin the actual binary consumer ABI, not sizeof a modern/retail USER header.
# All spans are inside the pinned NE code segment and named by its own map.
$script:consumerChecks = 0
function Assert-ConsumerBytes([string]$symbol, [int]$span,
    [string]$pattern, [int]$count = 1) {
    $start = $code + (Get-CodeOffset $symbol)
    if ($start -lt $code -or $span -le 0 -or $start + $span -gt $bytes.Length) {
        throw "Invalid consumer span: $symbol"
    }
    $hex = [BitConverter]::ToString($bytes, $start, $span)
    if ([regex]::Matches($hex, [regex]::Escape($pattern)).Count -ne $count) {
        throw "Pinned shared ABI changed: $symbol / $pattern"
    }
    $script:consumerChecks++
}
# init.c line 99 (WOWDBG) is mapped to 0001:482c in the retained user.map.
# The actual 16-bit stores publish low WORD 0 and high WORD 8400, not a
# value inferred from the modern host SDK's WINVER or host DEBUG switch.
Assert-ConsumerBytes 'LibMain' 96 'C7-46-EE-00-00-C7-46-F0-00-84'
Assert-ConsumerBytes 'LibMain' 96 'C7-46-E6-A2-5D-8C-4E-E8'
Assert-ConsumerBytes 'LibMain' 96 'C7-46-EA-A6-5D-8C-4E-EC'
Assert-ConsumerBytes '_GetSystemMetrics@4' 140 '67-66-8B-84-8A-70-01-00-00' 3
Assert-ConsumerBytes '_GetSystemMetrics@4' 140 '66-83-F9-4C'
Assert-ConsumerBytes '_GetSystemMetrics@4' 140 '67-66-81-78-50-00-04-00-00'
Assert-ConsumerBytes '_GetSysColor@4' 50 '67-66-8B-84-8A-A0-02-00-00'
Assert-ConsumerBytes '_GetSysColor@4' 50 '66-83-F9-19'
Assert-ConsumerBytes '_GetCursorPos@4' 80 '67-66-8B-81-04-03-00-00'
Assert-ConsumerBytes '_GetCursorPos@4' 80 '67-66-8B-81-08-03-00-00'
Assert-ConsumerBytes '@HMValidateHandle@8' 127 '66-C1-E7-04'
Assert-ConsumerBytes '@HMValidateHandle@8' 127 '67-66-8B-40-04'
Assert-ConsumerBytes '@HMValidateHandle@8' 127 '67-39-48-0A'
Assert-ConsumerBytes '@HMValidateHandle@8' 127 '67-38-50-08'
Assert-ConsumerBytes '@HMValidateHandle@8' 127 '64-66-A1-18-00'
Assert-ConsumerBytes '@HMValidateHandle@8' 127 '67-66-8B-50-5C'
Assert-ConsumerBytes '@HMValidateHandle@8' 127 '67-66-2B-48-1C'
Assert-ConsumerBytes '@ValidateHwnd@4' 40 '66-83-C0-44'
Assert-ConsumerBytes '@ValidateHwnd@4' 40 '67-66-39-48-28'
Assert-ConsumerBytes '@ValidateHwnd@4' 40 '67-66-8B-40-2C'
Assert-ConsumerBytes '__GetDesktopWindow@0' 26 '67-66-8B-40-08'
Assert-ConsumerBytes 'GETTICKCOUNT' 38 '66-BA-00-00-FE-7F'
Assert-ConsumerBytes 'GETTICKCOUNT' 38 '67-66-8B-02-67-66-F7-62-04-66-0F-AC-D0-18'
# Window/menu/class offsets used by the original optimized graph walkers.
# Names come from original user.h and the retained generated assembly; actual
# instruction bytes below establish the selected immutable binary's layout.
Assert-ConsumerBytes '_GetClassNameA@12' 145 '67-66-8B-70-0C'
Assert-ConsumerBytes '_GetClassNameA@12' 145 '67-66-8B-48-4C'
Assert-ConsumerBytes '_GetClassNameA@12' 145 '67-66-8B-4C-01-64'
Assert-ConsumerBytes '__GetDlgItem@8' 80 '67-66-8B-48-1C'
Assert-ConsumerBytes '__GetDlgItem@8' 80 '67-66-39-48-68'
Assert-ConsumerBytes '__GetDlgItem@8' 80 '67-66-8B-50-14'
Assert-ConsumerBytes '__GetParent@4' 94 '67-66-8B-48-18'
Assert-ConsumerBytes '__GetParent@4' 94 '67-66-8B-48-20'
Assert-ConsumerBytes '__GetWindowRect@8' 41 '66-83-C6-28'
Assert-ConsumerBytes '__GetClientRect@8' 133 '67-66-8B-42-40-67-2B-42-38'
Assert-ConsumerBytes '__GetClientRect@8' 133 '67-66-8B-42-44-67-2B-42-3C'
Assert-ConsumerBytes '__GetClientRect@8' 133 '67-F6-82-A0-00-00-00-80'
Assert-ConsumerBytes '__GetWindow@8' 317 '67-F6-83-A4-00-00-00-08'
Assert-ConsumerBytes '__IsWindowVisible@4' 87 '67-66-0F-B7-41-7A-80-E4-3F-66-3D-9D-02-00-00'
Assert-ConsumerBytes '__IsWindowVisible@4' 87 '67-F6-81-AB-00-00-00-10'
Assert-ConsumerBytes '_IsIconic@4' 47 '67-8A-80-AB-00-00-00-24-20'
Assert-ConsumerBytes '_IsWindowEnabled@4' 48 '67-8A-80-AB-00-00-00-24-08'
Assert-ConsumerBytes '_IsZoomed@4' 48 '67-8A-88-AB-00-00-00-66-83-E1-01'
Assert-ConsumerBytes '_IsWindow@4' 72 '67-F6-44-0A-09-01'
Assert-ConsumerBytes '_GetMenuItemID@8' 89 '67-66-39-48-28'
Assert-ConsumerBytes '_GetMenuItemID@8' 89 '66-C1-E1-06-67-66-8B-50-38'
Assert-ConsumerBytes '_GetMenuItemID@8' 89 '67-66-83-78-0C-00'
Assert-ConsumerBytes '_GetMenuItemID@8' 89 '67-66-8B-40-08'
Assert-ConsumerBytes '__GetMenuState@12' 110 '67-66-8B-41-04-67-66-0B-01'
$sharedAbi = [ordered]@{
    sharedInfo = @{ serverInfo = 0; handleTable = 4 }
    serverInfo = @{ handleCount = 4; metrics = 368; metricCount = 76;
        colors = 672; colorCount = 25; cursorX = 772; cursorY = 776 }
    handleEntry = @{ stride = 16; object = 0; type = 8; flags = 9;
        destroyMask = 1; uniqueness = 10 }
    window = @{ self = 12; next = 20; parent = 24; child = 28; owner = 32;
        windowRect = 40; clientRect = 56; class = 76; menuOrControlId = 104;
        functionId = 122; compatibilityByte = 160; extendedStyleByte = 164;
        styleHighByte = 171; desktopFunctionId = 669 }
    windowClass = @{ ansiName = 100 }
    menu = @{ self = 12; itemCount = 40; items = 56 }
    menuItem = @{ stride = 64; type = 0; state = 4; id = 8; submenu = 12 }
    teb = @{ self = 24; clientInfo = 68; expectedWindowsVersion = 80;
        desktopInfo = 92; clientDelta = 96; cachedWindowHandle = 108;
        cachedWindow = 112 }
    desktopInfo = @{ heapBase = 0; heapLimit = 4; desktopWindow = 8 }
    clock = @{ linearAddress = '7FFE0000'; low = 0; multiplier = 4; shift = 24 }
}
$header = Get-Content -Raw (Join-Path $RepositoryRoot 'src/mvdm/wow32/wusercli.h')
$body = Get-Content -Raw (Join-Path $RepositoryRoot 'src/mvdm/wow32/wusercli.c')
$dispatch = Get-Content -Raw (Join-Path $RepositoryRoot 'src/mvdm/wow32/wutbl2.h')
$local = [regex]::Matches($header, '(?m)^#define\s+(WU32\w+)\s+LOCALAPI\s*$')
if ($local.Count -ne 26) { throw "Expected 26 mappings, found $($local.Count)" }
$rows = foreach ($entry in $local) {
    $name = $entry.Groups[1].Value
    $target = [regex]::Match($header, '(?m)^#define\s+' + $name + '\s+(WU32\w+)\s*$')
    if (!$target.Success) { throw "No original thunk for $name" }
    $function = $target.Groups[1].Value
    if ($body -notmatch ('ULONG\s+FASTCALL\s+' + $function + '\s*\(')) {
        throw "Missing original function body: $function"
    }
    if ($dispatch -notmatch ('\b' + $name + '\b')) { throw "No dispatch entry: $name" }
    [pscustomobject]@{ mapping = $name; originalThunk = $function }
}
[pscustomobject]@{
    result = 'WOW_USER_PROFILE_SOURCE_AND_BINARY_OK'
    evidenceKind = 'read-only source and pinned-binary inspection; not runtime acceptance'
    guestHash = $expectedHash
    guestProfile = 'PMODE32'
    consumerChecks = $script:consumerChecks
    userRegistration = @{
        buildInfo = '84000000'
        comparedHighWord = '8400'
        userAbiVersion = '0400'
        debugLayout = $true
    }
    flatDataSelector = '0023'
    tebSelector = '003B'
    binarySharedAbi = $sharedAbi
    mappings = @($rows)
} | ConvertTo-Json -Depth 4
