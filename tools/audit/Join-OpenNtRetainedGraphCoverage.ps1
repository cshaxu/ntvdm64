[CmdletBinding()]
param(
    [string]$InventoryCsv = 'build/M0-T405/S1/all-roots-001/inventory.csv',
    [string[]]$GraphPaths = @(
        'build/M0-T404/S5/r014-dpmi-vector-observation/build.ninja',
        'build/M0-T404/S5/r015-wow32-provider/build.ninja'
    ),
    [string]$OutputDirectory = 'build/M0-T405/S1/graph-coverage-001'
)
$ErrorActionPreference = 'Stop'
$repo = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$prefix = $repo + [IO.Path]::DirectorySeparatorChar
$output = [IO.Path]::GetFullPath((Join-Path $repo $OutputDirectory))
$build = [IO.Path]::GetFullPath((Join-Path $repo 'build')) + [IO.Path]::DirectorySeparatorChar
if (-not $output.StartsWith($build, [StringComparison]::OrdinalIgnoreCase)) { throw 'Output must be below build.' }
if (Test-Path -LiteralPath $output) { throw 'Output already exists.' }
$inventory = @(Import-Csv -LiteralPath (Join-Path $repo $InventoryCsv))
$indexed = @{}
foreach ($row in $inventory) {
    if ($indexed.ContainsKey($row.path)) { throw "Duplicate inventory path: $($row.path)" }
    if ((Get-FileHash -LiteralPath (Join-Path $repo $row.path)).Hash -ne $row.sha256) {
        throw "Inventory identity changed: $($row.path)"
    }
    $indexed[$row.path] = $row
}
$tracked = @(& git -C $repo ls-files src)
if ($LASTEXITCODE -ne 0) { throw 'Cannot enumerate tracked source.' }
if ($tracked.Count -ne $indexed.Count) { throw 'Tracked source membership changed.' }
foreach ($path in $tracked) {
    if (-not $indexed.ContainsKey($path)) { throw "Tracked path missing from inventory: $path" }
}
$selected = @{}
$edges = [Collections.Generic.List[object]]::new()
$graphs = foreach ($graph in $GraphPaths) {
    $absoluteGraph = [IO.Path]::GetFullPath((Join-Path $repo $graph))
    $lineNumber = 0
    foreach ($line in Get-Content -LiteralPath $absoluteGraph) {
        $lineNumber++
        # Deliberately scoped to these generated cc/cc_* and rc rules, not a
        # general Ninja parser or proof of linked symbol reachability.
        if ($line -notmatch '^build .*: (cc\w*|rc) (.+)$') { continue }
        $rule = $matches[1]
        $inputText = $matches[2]
        # Current retained graphs have one absolute source input. Fail closed
        # on new syntax rather than silently omitting a compile rule.
        if ($inputText -notmatch '^([A-Za-z]\$?:/[^\s]+)(?:\s+[|].*)?$') {
            throw "Unsupported compile input at ${graph}:$lineNumber : $inputText"
        }
        $raw = $matches[1].Replace('$:', ':')
        $absolute = [IO.Path]::GetFullPath($raw)
        if (-not $absolute.StartsWith($prefix, [StringComparison]::OrdinalIgnoreCase)) {
            throw "Compile input outside repository: $raw"
        }
        $path = $absolute.Substring($prefix.Length).Replace('\', '/')
        if (-not $path.StartsWith('src/')) { continue } # Generated build carriers are not source inventory rows.
        if (-not $indexed.ContainsKey($path)) { throw "Selected source absent from inventory: $path" }
        $selected[$path] = $true
        $edges.Add([pscustomobject]@{graph=$graph;line=$lineNumber;rule=$rule;raw_path=$raw;path=$path})
    }
    [pscustomobject]@{graph=$graph;sha256=(Get-FileHash -LiteralPath $absoluteGraph).Hash}
}
if ($edges.Count -eq 0) { throw 'No compile edges found.' }
$rows = foreach ($row in $inventory) {
    $classification = 'not-a-direct-translation-unit'
    if ($row.path -match '\.(c|cpp|cxx|asm|rc)$') { $classification = 'not-selected-in-retained-graphs' }
    if ($selected.ContainsKey($row.path)) { $classification = 'selected-in-retained-graphs' }
    [pscustomobject]@{path=$row.path;root=$row.root;sha256=$row.sha256;
        selection=$classification;review_assignment=$row.review_assignment}
}
$null = New-Item -ItemType Directory -Path $output
$rows | Export-Csv -LiteralPath (Join-Path $output 'coverage.csv') -NoTypeInformation -Encoding UTF8
$edges | Export-Csv -LiteralPath (Join-Path $output 'compile-edges.csv') -NoTypeInformation -Encoding UTF8
$graphs | Export-Csv -LiteralPath (Join-Path $output 'graph-identities.csv') -NoTypeInformation -Encoding UTF8
[pscustomobject]@{inventory_rows=$inventory.Count;selected_paths=$selected.Count;
    compile_edges=$edges.Count;output=$output;limitation='Retained compile membership only; no link, runtime or header-use claim.'} | ConvertTo-Json
