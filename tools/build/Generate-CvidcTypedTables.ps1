[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$OutputDirectory
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$cvidc = Join-Path $root 'src/mvdm-host/softpc.new/base/cvidc'
$inc = Join-Path $root 'src/mvdm-host/softpc.new/base/inc'
$output = [IO.Path]::GetFullPath($OutputDirectory)

function Get-FunctionFields {
    param([string]$Text, [string]$StructName)
    # `VideoVector` is an original named `struct`, while the EVID carriers
    # are original typedef structs.  Preserve both source forms rather than
    # normalising either mirror header.
    $match = [regex]::Match($Text, '(?s)struct\s+' + [regex]::Escape($StructName) + '\s*\{([^{}]*)\}\s*;')
    if (!$match.Success) {
        $match = [regex]::Match($Text, '(?s)typedef\s+struct\s*\{([^{}]*)\}\s*' + [regex]::Escape($StructName) + '\s*;')
    }
    if (!$match.Success) { throw "Unable to find $StructName" }
    $fields = @()
    foreach ($item in [regex]::Matches($match.Groups[1].Value, '(?m)^\s*([^;]+?)\s*\(\*([A-Za-z0-9_]+)\)\s*IPT([0-9])\(([^;]*)\);')) {
        $tokens = @($item.Groups[4].Value.Trim() -split '\s*,\s*')
        $arity = [int]$item.Groups[3].Value
        if (($arity -eq 0 -and $tokens.Count -ne 1 -and $tokens[0] -ne '') -or ($arity -gt 0 -and $tokens.Count -ne ($arity * 2))) {
            throw "Unexpected parameter shape for $StructName.$($item.Groups[2].Value)"
        }
        $parameters = @()
        for ($index = 0; $index -lt $arity; $index += 1) {
            $parameters += [pscustomobject]@{ Type = $tokens[$index * 2].Trim(); Name = $tokens[$index * 2 + 1].Trim() }
        }
        $fields += [pscustomobject]@{
            ReturnType = ($item.Groups[1].Value.Trim() -replace '\s+', ' ')
            Name = $item.Groups[2].Value
            Parameters = $parameters
        }
    }
    if ($fields.Count -eq 0) { throw "No function-pointer fields in $StructName" }
    return $fields
}

function Get-BalancedEnd {
    param([string]$Text, [int]$OpenBrace)
    $depth = 0
    for ($index = $OpenBrace; $index -lt $Text.Length; $index += 1) {
        if ($Text[$index] -eq '{') { $depth += 1 }
        elseif ($Text[$index] -eq '}') {
            $depth -= 1
            if ($depth -eq 0) { return $index }
        }
    }
    throw 'Unbalanced initializer brace'
}

function New-Wrapper {
    param([string]$Name, [object]$Field, [string]$Symbol)
    $parameters = @($Field.Parameters)
    $declaration = if ($parameters.Count -eq 0) { 'void' } else { ($parameters | ForEach-Object { "$($_.Type) $($_.Name)" }) -join ', ' }
    $words = @($parameters | ForEach-Object {
        if ($_.Type -match '\*' -or $_.Type -eq 'IHP') { '(UINT_PTR)(uintptr_t)' + $_.Name }
        else { '(UINT_PTR)' + $_.Name }
    })
    while ($words.Count -lt 4) { $words += '0' }
    # The generated source is deliberately K&R/generic at this boundary.
    # Call it through the one private, native-word carrier rather than
    # redeclaring the named generated function with a conflicting prototype.
    # The exported table entry itself remains the exact original typed shape.
    $call = "((CVIDC_GENERIC_FUNCTION)$Symbol)($($words -join ', '))"
    if ($Field.ReturnType -eq 'void') {
        return "static void $Name($declaration) { $call; }"
    }
    $return = if ($Field.ReturnType -match '\*' -or $Field.ReturnType -eq 'IHP') {
        "($($Field.ReturnType))(uintptr_t)$call"
    } else {
        "($($Field.ReturnType))$call"
    }
    return "static $($Field.ReturnType) $Name($declaration) { return $return; }"
}

$cpuVid = Get-Content -Raw -LiteralPath (Join-Path $inc 'cpu_vid.h')
$gaMark = Get-Content -Raw -LiteralPath (Join-Path $inc 'ga_mark.h')
$evidgen = Get-Content -Raw -LiteralPath (Join-Path $cvidc 'evidgen.h')
$evidHeader = Get-Content -Raw -LiteralPath (Join-Path $cvidc 'evidfunc.h')
$vglfunc = Get-Content -Raw -LiteralPath (Join-Path $cvidc 'vglfunc.c')
$evidfunc = Get-Content -Raw -LiteralPath (Join-Path $cvidc 'evidfunc.c')

$fields = @{
    VideoVector = Get-FunctionFields $evidgen 'VideoVector'
    EVID_WRT_POINTERS = Get-FunctionFields $cpuVid 'EVID_WRT_POINTERS'
    EVID_READ_POINTERS = Get-FunctionFields $cpuVid 'EVID_READ_POINTERS'
    EVID_MARK_POINTERS = Get-FunctionFields $gaMark 'EVID_MARK_POINTERS'
}

$wrappers = [System.Collections.Generic.List[string]]::new()
$symbols = [System.Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
$wrapperCounter = 0
function Add-Wrapper {
    param([object]$Field, [string]$Symbol)
    $script:wrapperCounter += 1
    $name = ('cvidc_typed_{0:D4}' -f $script:wrapperCounter)
    $script:wrappers.Add((New-Wrapper $name $Field $Symbol))
    $script:symbols.Add($Symbol) | Out-Null
    return $name
}

# Build a replacement C_Video definition from the original initializer order.
$videoMatch = [regex]::Match($vglfunc, '(?s)struct\s+VideoVector\s+C_Video\s*=\s*\{')
if (!$videoMatch.Success) { throw 'Unable to find original C_Video initializer' }
$videoOpen = $vglfunc.IndexOf('{', $videoMatch.Index)
$videoEnd = Get-BalancedEnd $vglfunc $videoOpen
$videoSymbols = @([regex]::Matches($vglfunc.Substring($videoOpen + 1, $videoEnd - $videoOpen - 1), '\b(S_[A-Za-z0-9_]+)\b') | ForEach-Object { $_.Groups[1].Value })
$videoEntries = @()
for ($index = 0; $index -lt $fields.VideoVector.Count; $index += 1) {
    if ($index -lt $videoSymbols.Count) { $videoEntries += Add-Wrapper $fields.VideoVector[$index] $videoSymbols[$index] }
    else { $videoEntries += '0' }
}

# Transform the original EVID table-definition source one initializer at a
# time.  Only generated S_* table values change; variable names, table order,
# dimensions and all source data remain from the original file.
$replacements = [System.Collections.Generic.List[object]]::new()
foreach ($kind in @('EVID_WRT_POINTERS', 'EVID_READ_POINTERS', 'EVID_MARK_POINTERS')) {
    $pattern = '(?s)\b' + [regex]::Escape($kind) + '\s+([A-Za-z0-9_]+)\s*(?:\[\])?\s*=\s*\{'
    foreach ($declaration in [regex]::Matches($evidfunc, $pattern)) {
        $open = $evidfunc.IndexOf('{', $declaration.Index)
        $end = Get-BalancedEnd $evidfunc $open
        $block = $evidfunc.Substring($open + 1, $end - $open - 1)
        $fieldList = $fields[$kind]
        $position = 0
        foreach ($symbolMatch in [regex]::Matches($block, '\b(S_[A-Za-z0-9_]+)\b')) {
            $field = $fieldList[$position % $fieldList.Count]
            $wrapper = Add-Wrapper $field $symbolMatch.Groups[1].Value
            $replacements.Add([pscustomobject]@{
                Index = $open + 1 + $symbolMatch.Index
                Length = $symbolMatch.Length
                Value = $wrapper
            })
            $position += 1
        }
    }
}

$evidBody = $evidfunc -replace '(?m)^#include[^\r\n]*(\r?\n)?', ''
# Include stripping changes offsets. Apply replacements before stripping.
$rewritten = $evidfunc
foreach ($replacement in @($replacements | Sort-Object Index -Descending)) {
    $rewritten = $rewritten.Remove($replacement.Index, $replacement.Length).Insert($replacement.Index, $replacement.Value)
}
$evidBody = $rewritten -replace '(?m)^#include[^\r\n]*(\r?\n)?', ''

$prefix = @(
    '/* Generated from exact original cvidc table declarations and initializers.',
    ' * Do not edit: tools/build/Generate-CvidcTypedTables.ps1 owns this private',
    ' * mvdm-host-overlay carrier. */',
    '#include "insignia.h"',
    '#include "host_def.h"',
    '#include "ga_mark.h"',
    '#include "cpu_vid.h"',
    '#include "evidfunc.h"',
    '#include "evidgen.h"',
    '#include <stdint.h>',
    ''
)
$prefix += 'typedef UINT_PTR (*CVIDC_GENERIC_FUNCTION)(UINT_PTR, UINT_PTR, UINT_PTR, UINT_PTR);'
# `evidfunc.h` declares the table-facing generated forms.  A second group of
# generated setters is intentionally only declared in vglfunc.c in the
# original build.  Give those undeclared forms an old-style native-word
# declaration solely so this generated carrier can take their address; do not
# replace the typed declarations that the original header already owns.
$headerSymbols = [System.Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
foreach ($match in [regex]::Matches($evidHeader, '\b(S_[A-Za-z0-9_]+)\b')) {
    $headerSymbols.Add($match.Groups[1].Value) | Out-Null
}
foreach ($symbol in @($symbols | Where-Object { !$headerSymbols.Contains($_) } | Sort-Object)) {
    $prefix += "extern UINT_PTR $symbol();"
}
$prefix += ''
$prefix += $wrappers
$prefix += ''

[IO.Directory]::CreateDirectory($output) | Out-Null
$videoFile = Join-Path $output 'cvidc_typed_video_vector.c'
$evidFile = Join-Path $output 'cvidc_typed_evid_tables.c'
[IO.File]::WriteAllLines($videoFile, @($prefix + 'struct VideoVector C_Video = {' + (($videoEntries | ForEach-Object { '    ' + $_ + ',' }) -join [Environment]::NewLine) + '};'), [Text.UTF8Encoding]::new($false))
[IO.File]::WriteAllText($evidFile, (($prefix -join [Environment]::NewLine) + [Environment]::NewLine + $evidBody), [Text.UTF8Encoding]::new($false))

[pscustomobject]@{
    video = $videoFile
    evid = $evidFile
    wrappers = $wrapperCounter
    source_symbols = $symbols.Count
    original_video_slots = $videoEntries.Count
    original_evid_entries = $replacements.Count
} | ConvertTo-Json -Compress
