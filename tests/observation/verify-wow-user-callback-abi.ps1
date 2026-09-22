param(
    [string]$RepositoryRoot = (Resolve-Path "$PSScriptRoot/../..").Path,
    [Parameter(Mandatory=$true)][string]$BuildRoot,
    [string]$Dumpbin = 'C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/VC/Tools/MSVC/14.43.34808/bin/Hostx86/x86/dumpbin.exe'
)
$ErrorActionPreference = 'Stop'
# Complete PFNWOWHANDLERSIN x86 ABI, not a real guest/provider acceptance test.
$bytes = [ordered]@{
    pfnLocalAlloc=12; pfnLocalReAlloc=20; pfnLocalLock=8; pfnLocalUnlock=8
    pfnLocalSize=8; pfnLocalFree=8; pfnGetExpWinVer=4; pfnInitDlgCb=8
    pfn16GlobalAlloc=8; pfn16GlobalFree=4; pfnEmptyCB=0; pfnFindResourceEx=16
    pfnLoadResource=8; pfnFreeResource=8; pfnLockResource=8; pfnUnlockResource=8
    pfnSizeofResource=8; pfnWowWndProcEx=24; pfnWowEditNextWord=20
    pfnWowSetFakeDialogClass=4; pfnWowCBStoreHandle=8
}
$source = Join-Path $RepositoryRoot 'src/mvdm/wow32/wow32.c'
$header = Join-Path $RepositoryRoot 'src/opennt-abi/source/public/internal/windows/inc/wowuserp.h'
$code = Get-Content -Raw -LiteralPath $source
$declarations = Get-Content -Raw -LiteralPath $header
$table = [regex]::Match($declarations, '(?s)typedef struct tagAPFNWOWHANDLERSIN\s*\{(.*?)\}\s*PFNWOWHANDLERSIN')
if (!$table.Success) { throw 'Missing original input table' }
$fields = [regex]::Matches($table.Groups[1].Value, '(PFN\w+)\s+(pfn\w+)\s*;')
if ($fields.Count -ne $bytes.Count) { throw 'Callback inventory changed' }
$assignments = [regex]::Matches($code, 'pfnIn\.(\w+)\s*=\s*([^;]+);')
if ($assignments.Count -ne $bytes.Count) { throw 'Missing or duplicate input assignment' }
$assigned = @{}
foreach ($assignment in $assignments) {
    $slot = $assignment.Groups[1].Value
    $target = $assignment.Groups[2].Value.Trim()
    # Original LONG-to-DWORD return cast; the object must still prove @24.
    if ($slot -eq 'pfnWowWndProcEx' -and
        $target -eq '(PFNWOWWNDPROCEX)W32Win16WndProcEx') {
        $target = 'W32Win16WndProcEx'
    }
    if (!$bytes.Contains($slot) -or $assigned.ContainsKey($slot) -or $target -notmatch '^\w+$') {
        throw "Unknown, duplicate or cast callback assignment: $slot = $target"
    }
    $assigned[$slot] = $target
}
$objects = @('obj/wow32.obj','obj/wcall32.obj','obj/wres32.obj','obj/wuclip.obj','obj/wmdisp32.obj',
    'obj/wudlg.obj','obj/wumsg.obj','obj/wuwind.obj','obj/wgfont.obj','obj/wuser.obj','obj/wdde.obj')
$symbolSets = @{}
$identities = foreach ($relative in $objects) {
    $path = Join-Path $BuildRoot $relative
    $lines = @(& $Dumpbin /symbols $path)
    if ($LASTEXITCODE) { throw "Unable to inspect $path" }
    $symbolSets[$relative] = $lines
    [pscustomobject]@{path=$relative; sha256=(Get-FileHash -LiteralPath $path).Hash}
}
$rows = foreach ($field in $fields) {
    $type = $field.Groups[1].Value
    $slot = $field.Groups[2].Value
    if (!$assigned.ContainsKey($slot)) { throw "Unassigned $slot" }
    if ($declarations -notmatch ('\(WINAPI\s*\*'+[regex]::Escape($type)+'\)')) {
        throw "Calling convention changed for $type"
    }
    $symbol = '_'+$assigned[$slot]+'@'+$bytes[$slot]
    $suffix = '\|\s*'+[regex]::Escape($symbol)+'\s*$'
    if (!@($symbolSets['obj/wow32.obj'] | Where-Object { $_ -match 'UNDEF' -and $_ -match $suffix }).Count) {
        throw "Formal caller does not reference $symbol"
    }
    $owners = @($objects | Where-Object {
        @($symbolSets[$_] | Where-Object { $_ -match 'SECT\w+' -and $_ -match $suffix }).Count -gt 0
    })
    if ($owners.Count -ne 1) { throw "Expected one definition of $symbol, got $($owners.Count)" }
    [pscustomobject]@{slot=$slot; type=$type; symbol=$symbol; object=$owners[0]; argumentBytes=$bytes[$slot]}
}
$dialogSymbol = '\|\s*_W32DialogFunc@16\s*$'
if (!@($symbolSets['obj/wudlg.obj'] | Where-Object { $_ -match 'SECT\w+' -and $_ -match $dialogSymbol }).Count) {
    throw 'Original DLGPROC definition is not WINAPI/16-byte'
}
foreach ($consumer in @('obj/wumsg.obj','obj/wuwind.obj')) {
    if (!@($symbolSets[$consumer] | Where-Object { $_ -match 'UNDEF' -and $_ -match $dialogSymbol }).Count) {
        throw "DWL_DLGPROC consumer ABI mismatch: $consumer"
    }
}
$nativeCallbacks = [ordered]@{
    'W32EnumFontFunc@16'='obj/wgfont.obj'
    'W32GrayStringProc@12'='obj/wuser.obj'
    'W32EnumWindowFunc@8'='obj/wuwind.obj'
    'W32DDEFreeGlobalMem32@4'='obj/wdde.obj'
}
foreach ($callback in $nativeCallbacks.Keys) {
    $pattern = '\|\s*_' + [regex]::Escape($callback) + '\s*$'
    if (!@($symbolSets[$nativeCallbacks[$callback]] | Where-Object {
        $_ -match 'SECT\w+' -and $_ -match $pattern
    }).Count) { throw "Original native callback ABI mismatch: $callback" }
}
if ($code -notmatch 'RegisterWowBaseHandlers\(W32DDEFreeGlobalMem32\)') {
    throw 'Global-free hook no longer binds the original body'
}
[pscustomobject]@{
    marker='WOW_USER_INPUT_CALLBACK_ABI_21_OK'
    sourceSha256=(Get-FileHash -LiteralPath $source).Hash
    headerSha256=(Get-FileHash -LiteralPath $header).Hash
    callbacks=$rows
    dialogCallback='W32DialogFunc@16: original definition and both DWL_DLGPROC consumers match'
    nativeCallbacks=$nativeCallbacks
    objects=$identities
    limitation='Static x86 input-table ABI only; not real callback reachability, argument meaning, output-table or guest acceptance.'
} | ConvertTo-Json -Depth 5
