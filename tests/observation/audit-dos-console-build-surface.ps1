[CmdletBinding()]
param(
    [string]$RepositoryRoot = (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)),
    [string]$NinjaGraph = 'build/M0-T423/S1/restart-formal-x86/build.ninja',
    [switch]$Callsites,
    [string]$Dumpbin = ''
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path.Replace('\','/').TrimEnd('/')
$graph = if ([IO.Path]::IsPathRooted($NinjaGraph)) { $NinjaGraph } else { Join-Path $root $NinjaGraph }
$sources = [Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
$objects = [Collections.Generic.List[object]]::new()
foreach ($line in Get-Content -LiteralPath $graph) {
    if ($line -match '^build (?<object>.+): cc(?:_[a-z0-9_]+)? (?<file>.+?\.[cC])(?:\s+\|.*)?$') {
        $object = $Matches.object.Replace('$:', ':').Replace('$ ', ' ')
        $path = $Matches.file.Replace('$:', ':').Replace('$ ', ' ')
        if (-not [IO.Path]::IsPathRooted($path)) { $path = Join-Path (Split-Path $graph) $path }
        $full = [IO.Path]::GetFullPath($path).Replace('\','/')
        if ($full.StartsWith($root + '/src/mvdm/', [StringComparison]::OrdinalIgnoreCase) -or
            $full.StartsWith($root + '/src/ntvdm-exe/', [StringComparison]::OrdinalIgnoreCase)) {
            if (-not (Test-Path -LiteralPath $full -PathType Leaf)) { throw "Missing selected source: $full" }
            [void]$sources.Add($full)
            $objects.Add([pscustomobject]@{Source=$full; Object=$object})
        }
    }
}
if ($sources.Count -eq 0) { throw 'No selected worker C translation units found in graph.' }

# Preserve byte positions/newlines while removing comments and string literals.
# The graph selects translation units, NOT individual #if branches. Results
# intentionally remain lexical candidates until preprocessing/caller review.
$strip = '(?s)/\*.*?\*/|//[^\r\n]*|"(?:\\.|[^"\\])*"|''(?:\\.|[^''\\])*'''
$calls = [Collections.Generic.List[object]]::new()
foreach ($file in ($sources | Sort-Object)) {
    $text = Get-Content -LiteralPath $file -Raw
    if (-not $text.Contains('Console')) { continue }
    $code = [regex]::Replace($text, $strip, [Text.RegularExpressions.MatchEvaluator]{
        param($m)
        [regex]::Replace($m.Value, '[^\r\n]', ' ')
    })
    $number = 0
    foreach ($line in [regex]::Split($code, '\r?\n')) {
        ++$number
        foreach ($m in [regex]::Matches($line, '\b(?<api>[A-Za-z_][A-Za-z_0-9]*Console[A-Za-z_0-9]*)\s*\(')) {
            $calls.Add([pscustomobject]@{
                File = $file.Substring($root.Length + 1)
                Line = $number
                Api = $m.Groups['api'].Value
            })
        }
    }
}
if ($calls.Count -eq 0) { throw 'No Console boundary candidates found.' }
"Selected worker C units: $($sources.Count)"
"Lexical Console occurrences: $($calls.Count)"
"Files with occurrences: $(($calls | Select-Object -ExpandProperty File -Unique).Count)"
"Distinct spellings: $(($calls | Select-Object -ExpandProperty Api -Unique).Count)"
if ($Callsites) {
    $calls | ForEach-Object { '{0}:{1}: {2}' -f $_.File,$_.Line,$_.Api }
} else {
    $calls | Group-Object Api | Sort-Object Name | ForEach-Object { '{0}: {1}' -f $_.Name,$_.Count }
}
'AUDIT ONLY: declarations/definitions and inactive branches may occur; no reachability or runtime pass inferred.'

if ($Dumpbin) {
    if (-not (Test-Path -LiteralPath $Dumpbin -PathType Leaf)) { throw 'Dumpbin not found.' }
    'Compiled Console function references (object selection, not runtime reachability):'
    foreach ($item in $objects) {
        $objectPath = if ([IO.Path]::IsPathRooted($item.Object)) { $item.Object } else { Join-Path (Split-Path $graph) $item.Object }
        if (-not (Test-Path -LiteralPath $objectPath -PathType Leaf)) { throw "Missing compiled object: $objectPath" }
        $symbols = & $Dumpbin /nologo /symbols $objectPath
        if ($LASTEXITCODE -ne 0) { throw "Dumpbin failed: $objectPath" }
        foreach ($symbol in $symbols) {
            # Direct function type or x86 stdcall import decoration. Exclude
            # ordinary globals such as hWndConsole/ConsoleNoUpdates.
            if ($symbol -match 'UNDEF.*\(\).*External\s+\|\s+(?<name>\S*Console\S*)' -or
                $symbol -match 'UNDEF.*External\s+\|\s+(?<name>__imp__\w*Console\w*@\d+)') {
                '{0}: {1}' -f $item.Source.Substring($root.Length + 1), $Matches.name
            }
        }
    }
}
