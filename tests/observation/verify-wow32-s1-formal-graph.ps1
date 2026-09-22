param(
    [string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path,
    [string]$BuildRoot = 'build/M0-T421/S4/wow32-ntvdd-r1'
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = (Resolve-Path -LiteralPath (Join-Path $root $BuildRoot)).Path
if (!$build.StartsWith(($root + '\build\'), [StringComparison]::OrdinalIgnoreCase)) {
    throw 'BuildRoot must be below the repository build directory.'
}

function Get-ActiveWowSources([string]$path) {
    $text = [IO.File]::ReadAllText($path)
    $start = $text.IndexOf('SOURCES=')
    $end = $text.IndexOf('!IF 0', $start)
    if ($start -lt 0 -or $end -le $start) { throw 'Missing active WOW32 SOURCES block.' }
    @([regex]::Matches($text.Substring($start, $end - $start),
        '(?m)^\s*([A-Za-z0-9_]+)\.c\s*\\?\s*$') |
        ForEach-Object { $_.Groups[1].Value })
}

$wowRoot = Join-Path $root 'src/mvdm/wow32'
$sources = @(Get-ActiveWowSources (Join-Path $wowRoot 'sources'))
if ($sources.Count -ne 77) { throw "Expected 77 selected WOW32 sources, found $($sources.Count)." }

$ninja = [IO.File]::ReadAllText((Join-Path $build 'build.ninja'))
$link = [regex]::Match($ninja, '(?m)^build wow32\.dll \|.*$').Value
if (!$link) { throw 'Formal build graph has no WOW32 link edge.' }
$missing = @($sources | Where-Object { $link -notmatch ('\bobj/' + [regex]::Escape($_) + '\.obj\b') })
if ($missing.Count) { throw ('Formal WOW32 link omits: ' + ($missing -join ', ')) }

$expectedUser = @(
    'hdata', 'clres', 'cldib', 'client', 'rtlinit', 'ntstubs', 'clmenu',
    'handtabl', 'cleanup', 'probe', 'class-remove', 'class-register',
    'taskman', 'input', 'profile', 'queue', 'exitwin', 'class-client', 'class'
)
$actualUser = @([regex]::Matches($link, '\bobj/opennt-user/([^\s/]+)\.obj') |
    ForEach-Object { $_.Groups[1].Value })
if (@(Compare-Object $expectedUser $actualUser).Count) {
    throw 'The original OpenNT USER-owner object set differs from the admitted formal graph.'
}

$targetSources = @([regex]::Matches([IO.File]::ReadAllText((Join-Path $root 'src/wow32-dll/sources')),
    '(?m)^\s*([A-Za-z0-9_]+)\.c\s*\\?\s*$') |
    ForEach-Object { $_.Groups[1].Value })
if ($targetSources.Count -ne 21) { throw "Expected 21 WOW32 DLL bindings, found $($targetSources.Count)." }
$missingBindings = @($targetSources | Where-Object {
    $link -notmatch ('\bobj/wow32-dll/' + [regex]::Escape($_) + '\.obj\b')
})
if ($missingBindings.Count) { throw ('Formal WOW32 link omits bindings: ' + ($missingBindings -join ', ')) }

$expectedTables = [ordered]@{
    'wktbl2.h' = @(310, 96, 136, 78)
    'wutbl2.h' = @(539, 72, 170, 297)
    'wgtbl2.h' = @(491, 4, 277, 210)
    'wkbdtbl2.h' = @(138, 9, 118, 11)
    'wstbl2.h' = @(19, 0, 18, 1)
    'wshtbl2.h' = @(104, 1, 88, 15)
    'wwstbl2.h' = @(152, 0, 105, 47)
    'wthtbl2.h' = @(3, 0, 3, 0)
    'wmmtbl2.h' = @(3, 0, 2, 1)
    'wcmdgtbl.h' = @(27, 0, 19, 8)
}
$tables = foreach ($entry in $expectedTables.GetEnumerator()) {
    $lines = @(Get-Content -LiteralPath (Join-Path $wowRoot $entry.Key) |
        Where-Object { $_ -match '^\s*\{W32FUN\(' })
    $local = (@($lines | Where-Object { $_ -match 'W32FUN\(LOCALAPI' })).Count
    $unimplemented = (@($lines | Where-Object { $_ -match 'W32FUN\(UNIMPLEMENTEDAPI' })).Count
    $entryCount = [int]$lines.Count
    $hostThunk = $entryCount - [int]$local - [int]$unimplemented
    $actual = @($entryCount, [int]$local, [int]$unimplemented, $hostThunk)
    if (@(Compare-Object $entry.Value $actual).Count) {
        throw "Original table disposition changed: $($entry.Key)."
    }
    [ordered]@{ table = $entry.Key; entries = $actual[0]; local = $actual[1]; unimplemented = $actual[2]; hostThunk = $actual[3] }
}

$dynamic = [ordered]@{
    'VDMREDIR' = 'wkfileio.c'
    'WINSPOOL.DRV' = 'wgprnset.c,wgdi.c,wow32fax.c,wspool.c'
    'WINMM.DLL' = 'wmmedia.c'
    'WSOCK32.DLL' = 'wsext.c'
    'OLETHK32.DLL' = 'wuclip.c'
}
foreach ($module in $dynamic.Keys) {
    if (!(Select-String -LiteralPath (Get-ChildItem $wowRoot -Filter '*.c' | Select-Object -ExpandProperty FullName) -SimpleMatch $module -Quiet)) {
        throw "Missing admitted dynamic dependency observation: $module"
    }
}

$forms = 'GETARGPTR','GETVDMPTR','ALLOCVDMPTR','GETOPTPTR','GETMISCPTR',
    'GETPSZPTR','GETPSZIDPTR','GETSTRPTR','GETVARSTRPTR','GETFRAMEPTR',
    'CBFRAMEPTR','SEGPTR','GetPModeVDMPointer','GetRModeVDMPointer','VDMPTR',
    'FLUSHVDMPTR','FREEVDMPTR'
$pointerUse = foreach ($form in $forms) {
    $uses = 0
    foreach ($source in $sources) {
        $uses += [regex]::Matches([IO.File]::ReadAllText((Join-Path $wowRoot ($source + '.c'))),
            '\b' + [regex]::Escape($form) + '\s*\(').Count
    }
    [ordered]@{ form = $form; occurrences = $uses }
}

[ordered]@{
    result = 'WOW32_S1_FORMAL_GRAPH_OK'
    evidenceKind = 'source/build composition inventory; not runtime acceptance'
    activeOriginalSources = $sources.Count
    originalOwnerObjects = $actualUser.Count
    targetBindings = $targetSources.Count
    tables = @($tables)
    tableTotals = [ordered]@{ entries = 1786; local = 182; unimplemented = 936; hostThunk = 668 }
    dynamicDependencies = $dynamic
    guestPointerUse = @($pointerUse)
} | ConvertTo-Json -Depth 4
