[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$OutputDirectory = 'build/M0-T416/S1/footprint-baseline-001'
)

$ErrorActionPreference = 'Stop'
if (-not $RepositoryRoot) { $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot) }
$repo = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$output = [IO.Path]::GetFullPath((Join-Path $repo $OutputDirectory))
$buildRoot = [IO.Path]::GetFullPath((Join-Path $repo 'build')) + [IO.Path]::DirectorySeparatorChar
if (-not $output.StartsWith($buildRoot, [StringComparison]::OrdinalIgnoreCase)) { throw 'Output must be below build/.' }
if (Test-Path -LiteralPath $output) { throw "Refusing to overwrite evidence: $output" }

function Get-PhysicalLines([string]$Path) {
    $text = [IO.File]::ReadAllText($Path)
    if ($text.Length -eq 0) { return 0 }
    return ([regex]::Matches($text, "`n")).Count + 1
}

function Get-TextDiff([string]$Original, [string]$Current) {
    $stat = @(& git -C $repo -c core.autocrlf=false diff --no-index --no-ext-diff --no-textconv --ignore-cr-at-eol --numstat -- $Original $Current)
    if ($LASTEXITCODE -gt 1) { throw "git diff failed: $Current" }
    if ($stat.Count -eq 0) { return [pscustomobject]@{ kind = 'cr-only'; added = 0; deleted = 0 } }
    if ($stat.Count -ne 1) { throw "Expected exactly one numstat record: $Current" }
    $fields = $stat[0] -split "`t"
    if ($fields[0] -eq '-') { return [pscustomobject]@{ kind = 'binary-different'; added = $null; deleted = $null } }
    return [pscustomobject]@{ kind = 'text-different'; added = [int]$fields[0]; deleted = [int]$fields[1] }
}

$unionLedger = Join-Path $repo 'artifacts/documentation-archive/20260910/etc/operations/ledgers/m0-t274-s1-non-guest-mvdm-union-ledger.tsv'
if (-not (Test-Path -LiteralPath $unionLedger)) { throw "Missing canonical MVDM union ledger: $unionLedger" }
$mvdmSource = @{}
foreach ($row in Import-Csv -LiteralPath $unionLedger -Delimiter "`t") {
    if ($mvdmSource.ContainsKey($row.target_path)) { throw "Duplicate canonical MVDM target: $($row.target_path)" }
    $mvdmSource[$row.target_path] = $row.primary_path
}

$tracked = @(& git -C $repo ls-files src/mvdm src/opennt-host src/mvdm-overlay src/opennt-host-overlay src/adapter-mvdm-host-in src/adapter-mvdm-host-out src/adapter-opennt-host)
if ($LASTEXITCODE -ne 0) { throw 'git ls-files failed' }
$mirrorRows = [Collections.Generic.List[object]]::new()
foreach ($path in $tracked | Where-Object { $_ -like 'src/mvdm/*' -or $_ -like 'src/opennt-host/*' }) {
    if ($path -match '/README\.md$') { continue }
    $relative = if ($path.StartsWith('src/mvdm/')) { $path.Substring('src/mvdm/'.Length) } else { $path.Substring('src/opennt-host/'.Length) }
    $owner = if ($path.StartsWith('src/mvdm/')) { 'mvdm' } else { 'opennt-host' }
    $original = ''
    if ($owner -eq 'mvdm' -and $mvdmSource.ContainsKey($relative)) { $original = $mvdmSource[$relative] }
    if ($owner -eq 'opennt-host') {
        $upstreamRelative = if ($relative.StartsWith('netapi/')) { 'ds/' + $relative } else { $relative }
        $candidate = Join-Path 'O:/repos.external/OpenNT' $upstreamRelative
        if (Test-Path -LiteralPath $candidate -PathType Leaf) { $original = $candidate }
    }
    $current = Join-Path $repo $path
    if (-not $original -or -not (Test-Path -LiteralPath $original -PathType Leaf)) {
        $mirrorRows.Add([pscustomobject]@{ owner=$owner; path=$path; original_path=$original; classification='unpaired'; added=$null; deleted=$null; distance=$null; current_sha256=(Get-FileHash $current).Hash; original_sha256='' })
        continue
    }
    $currentHash = (Get-FileHash $current).Hash
    $originalHash = (Get-FileHash $original).Hash
    if ($currentHash -eq $originalHash) {
        $mirrorRows.Add([pscustomobject]@{ owner=$owner; path=$path; original_path=$original; classification='byte-equal'; added=0; deleted=0; distance=0; current_sha256=$currentHash; original_sha256=$originalHash })
        continue
    }
    $diff = Get-TextDiff $original $current
    $distance = if ($null -eq $diff.added) { $null } else { $diff.added + $diff.deleted }
    $mirrorRows.Add([pscustomobject]@{ owner=$owner; path=$path; original_path=$original; classification=$diff.kind; added=$diff.added; deleted=$diff.deleted; distance=$distance; current_sha256=$currentHash; original_sha256=$originalHash })
}

$privateRows = [Collections.Generic.List[object]]::new()
foreach ($path in $tracked | Where-Object { $_ -like 'src/mvdm-overlay/*' -or $_ -like 'src/opennt-host-overlay/*' }) {
    if ($path -notmatch '\.(c|h|inc)$') { continue }
    $privateRows.Add([pscustomobject]@{ owner=if ($path.StartsWith('src/mvdm-overlay/')) {'mvdm-overlay'} else {'opennt-host-overlay'}; path=$path; physical_lines=(Get-PhysicalLines (Join-Path $repo $path)); kind='private-overlay' })
}

$adapterRows = [Collections.Generic.List[object]]::new()
foreach ($path in $tracked | Where-Object { $_ -like 'src/adapter-mvdm-host-in/*' -or $_ -like 'src/adapter-mvdm-host-out/*' -or $_ -like 'src/adapter-opennt-host/*' }) {
    if ($path -notmatch '\.(c|h)$') { continue }
    $owner = if ($path.StartsWith('src/adapter-mvdm-host-in/')) {'adapter-mvdm-host-in'} elseif ($path.StartsWith('src/adapter-mvdm-host-out/')) {'adapter-mvdm-host-out'} else {'adapter-opennt-host'}
    $adapterRows.Add([pscustomobject]@{ owner=$owner; path=$path; physical_lines=(Get-PhysicalLines (Join-Path $repo $path)); classification='unclassified-in-S1'; functions=$null })
}

$summary = [ordered]@{
    head = (& git -C $repo rev-parse HEAD).Trim()
    union_ledger = $unionLedger
    union_ledger_sha256 = (Get-FileHash $unionLedger).Hash
    mirror = @($mirrorRows | Group-Object owner,classification | ForEach-Object { [pscustomobject]@{ group=$_.Name; files=$_.Count; added=($_.Group | Measure-Object added -Sum).Sum; deleted=($_.Group | Measure-Object deleted -Sum).Sum; distance=($_.Group | Measure-Object distance -Sum).Sum } })
    overlay = @($privateRows | Group-Object owner | ForEach-Object { [pscustomobject]@{ owner=$_.Name; files=$_.Count; physical_lines=($_.Group | Measure-Object physical_lines -Sum).Sum } })
    adapter_raw_inventory = @($adapterRows | Group-Object owner | ForEach-Object { [pscustomobject]@{ owner=$_.Name; files=$_.Count; physical_lines=($_.Group | Measure-Object physical_lines -Sum).Sum } })
    limitation = 'Mirror metric includes only canonical-ledger/directly paired files. Unpaired paths, binary differences, metadata and raw adapter inventory are explicit but are not line-diff savings. Adapter autonomy needs row-level source-owner classification before it can be counted.'
}
$null = New-Item -ItemType Directory -Path $output
$mirrorRows | Sort-Object owner,path | Export-Csv -NoTypeInformation -Encoding UTF8 -LiteralPath (Join-Path $output 'mirror.csv')
$privateRows | Sort-Object owner,path | Export-Csv -NoTypeInformation -Encoding UTF8 -LiteralPath (Join-Path $output 'overlay.csv')
$adapterRows | Sort-Object owner,path | Export-Csv -NoTypeInformation -Encoding UTF8 -LiteralPath (Join-Path $output 'adapter-raw.csv')
$summary | ConvertTo-Json -Depth 5 | Set-Content -Encoding UTF8 -LiteralPath (Join-Path $output 'summary.json')
$summary | ConvertTo-Json -Depth 5
