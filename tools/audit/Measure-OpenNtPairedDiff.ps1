[CmdletBinding()]
param(
    [string]$CoverageCsv = 'build/M0-T405/S1/source-coverage-002/host-coverage.csv',
    [string]$OutputDirectory = 'build/M0-T405/S1/paired-diff-001'
)
$ErrorActionPreference = 'Stop'
$repo = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$output = [IO.Path]::GetFullPath((Join-Path $repo $OutputDirectory))
$build = [IO.Path]::GetFullPath((Join-Path $repo 'build')) + [IO.Path]::DirectorySeparatorChar
if (-not $output.StartsWith($build, [StringComparison]::OrdinalIgnoreCase)) { throw 'Output must be below build.' }
if (Test-Path -LiteralPath $output) { throw 'Output already exists.' }
$pairs = [Collections.Generic.List[object]]::new()
foreach ($row in Import-Csv -LiteralPath (Join-Path $repo $CoverageCsv)) {
    if ($row.source_verified -ne 'True') { continue }
    if ((Get-FileHash -LiteralPath (Join-Path $repo $row.path)).Hash -ne $row.current_sha256 -or
        (Get-FileHash -LiteralPath $row.source).Hash -ne $row.actual_source_sha256) {
        throw "Coverage hash changed: $($row.path)"
    }
    $pairs.Add([pscustomobject]@{path=$row.path;source=$row.source;group=$row.provenance;equal=($row.identity -eq 'byte-equal')})
}
foreach ($name in @(& git -C $repo ls-files src/opennt-host)) {
    if ($name -eq 'src/opennt-host/README.md') { continue }
    $relative = $name.Substring('src/opennt-host/'.Length)
    # Registered NetLib mirror drops the original ds/ prefix.
    if ($relative.StartsWith('netapi/')) { $relative = 'ds/' + $relative }
    $source = Join-Path 'O:/repos.external/OpenNT' $relative
    if (-not (Test-Path -LiteralPath $source -PathType Leaf)) { throw "Missing original: $source" }
    $equal = (Get-FileHash -LiteralPath $source).Hash -eq (Get-FileHash -LiteralPath (Join-Path $repo $name)).Hash
    $pairs.Add([pscustomobject]@{path=$name;source=$source;group='non-mvdm-opennt-reference';equal=$equal})
}
$rows = foreach ($pair in $pairs) {
    $added=0; $deleted=0; $kind='byte-equal'
    if (-not $pair.equal) {
        $stat = @(& git -C $repo -c core.autocrlf=false diff --no-index --no-ext-diff --no-textconv --ignore-cr-at-eol --numstat -- $pair.source (Join-Path $repo $pair.path))
        if ($LASTEXITCODE -gt 1) { throw "Diff failed: $($pair.path)" }
        $kind='cr-only'
        if ($stat.Count) {
            if ($stat.Count -ne 1) { throw 'Expected exactly one diff record.' }
            $fields=$stat[0] -split "`t"
            if ($fields[0] -eq '-') { $added=$null; $deleted=$null; $kind='binary-different' }
            else { $added=[int]$fields[0]; $deleted=[int]$fields[1]; $kind='text-different' }
        }
    }
    [pscustomobject]@{path=$pair.path;source=$pair.source;group=$pair.group;kind=$kind;
        added=$added;deleted=$deleted;source_sha256=(Get-FileHash -LiteralPath $pair.source).Hash}
}
$null=New-Item -ItemType Directory -Path $output
$rows | Export-Csv -LiteralPath (Join-Path $output 'paired-diff.csv') -NoTypeInformation -Encoding UTF8
$rows | Group-Object group,kind | ForEach-Object {
    [pscustomobject]@{classification=$_.Name;files=$_.Count;
        added=($_.Group | Measure-Object added -Sum).Sum;deleted=($_.Group | Measure-Object deleted -Sum).Sum}
} | ConvertTo-Json
