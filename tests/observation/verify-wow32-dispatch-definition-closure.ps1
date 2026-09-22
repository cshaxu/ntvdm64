param(
    [Parameter(Mandatory = $true)]
    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$wowRoot = Join-Path $root 'src\mvdm\wow32'
$tableNames = @('wktbl2.h', 'wutbl2.h', 'wgtbl2.h', 'wkbdtbl2.h',
    'wstbl2.h', 'wshtbl2.h', 'wwstbl2.h', 'wthtbl2.h', 'wmmtbl2.h',
    'wcmdgtbl.h')
$expected = [ordered]@{
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

$entries = foreach ($tableName in $tableNames) {
    $table = Join-Path $wowRoot $tableName
    if (-not (Test-Path -LiteralPath $table -PathType Leaf)) { throw "Missing table: $table" }
    Get-Content -LiteralPath $table | Where-Object { $_ -match '^\s*\{W32FUN\(' } |
        ForEach-Object {
            if ($_ -notmatch 'W32FUN\(\s*([^,]+),\s*"([^"]*)"') {
                throw "Cannot parse W32FUN row in ${tableName}: $_"
            }
            $function = $matches[1].Trim()
            [pscustomobject]@{
                table = $tableName
                function = $function
                api = $matches[2]
                kind = if ($function -eq 'LOCALAPI') { 'local' }
                    elseif ($function -eq 'UNIMPLEMENTEDAPI') { 'unimplemented' }
                    else { 'hostThunk' }
            }
        }
}

$sourceTexts = @{}
Get-ChildItem -LiteralPath $wowRoot -Filter '*.c' | ForEach-Object {
    $sourceTexts[$_.Name] = Get-Content -LiteralPath $_.FullName -Raw
}
$unresolved = @()
foreach ($entry in @($entries | Where-Object { $_.kind -eq 'hostThunk' })) {
    $function = ($entry.function -replace '^\(LPFNW32\)\s*', '').Trim()
    if ($function -eq 'NOPAPI') { continue }
    # Original declarations commonly read "ULONG FASTCALL Function(...)".
    # Require a body in a selected C source, not merely a header prototype.
    $pattern = '(?ms)\b' + [regex]::Escape($function) +
        '\b(?:\s+[A-Za-z_][A-Za-z0-9_]*)?\s*\([^;{}]*\)\s*' +
        '(?:(?://[^\r\n]*|/\*.*?\*/)\s*)*\{'
    if (-not [bool]($sourceTexts.Values | Where-Object { $_ -match $pattern })) {
        $unresolved += $entry
    }
}
if ($unresolved.Count -ne 0) {
    throw "Selected original thunk body missing: $($unresolved.function -join ', ')"
}

$tableResult = foreach ($tableName in $tableNames) {
    $rows = @($entries | Where-Object { $_.table -eq $tableName })
    $local = @($rows | Where-Object { $_.kind -eq 'local' }).Count
    $unimplemented = @($rows | Where-Object { $_.kind -eq 'unimplemented' }).Count
    $hostThunk = $rows.Count - $local - $unimplemented
    $actual = @($rows.Count, $local, $unimplemented, $hostThunk)
    if (@(Compare-Object $expected[$tableName] $actual).Count -ne 0) {
        throw "Unexpected original dispatch disposition in $tableName."
    }
    [ordered]@{ table = $tableName; entries = $rows.Count; local = $local;
        unimplemented = $unimplemented; hostThunk = $hostThunk }
}

[ordered]@{
    result = 'WOW32_DISPATCH_DEFINITION_CLOSURE_OK'
    evidenceKind = 'selected original source/table definition audit; not runtime acceptance'
    originalEntries = $entries.Count
    localApiEntries = @($entries | Where-Object { $_.kind -eq 'local' }).Count
    originalUnimplementedEntries = @($entries | Where-Object { $_.kind -eq 'unimplemented' }).Count
    selectedHostThunks = @($entries | Where-Object { $_.kind -eq 'hostThunk' }).Count
    selectedHostThunkBodies = @($entries | Where-Object { $_.kind -eq 'hostThunk' }).Count
    tables = @($tableResult)
} | ConvertTo-Json -Depth 4
