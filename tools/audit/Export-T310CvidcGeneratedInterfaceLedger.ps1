[CmdletBinding()]
param(
    [string]$RepositoryRoot = (Get-Location).Path
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$cvidc = Join-Path $root 'src/mvdm-host/softpc.new/base/cvidc'
$cpuVid = Join-Path $root 'src/mvdm-host/softpc.new/base/inc/cpu_vid.h'
$gaMark = Join-Path $root 'src/mvdm-host/softpc.new/base/inc/ga_mark.h'
$output = Join-Path $root 'docs/etc/operations/m0-t310-s8-p4-cvidc-generated-interface-ledger.tsv'

function Get-FunctionFields {
    param([string]$Text, [string]$StructName)
    $match = [regex]::Match($Text, '(?s)typedef\s+struct\s*\{([^{}]*)\}\s*' + [regex]::Escape($StructName) + '\s*;')
    if (!$match.Success) {
        $match = [regex]::Match($Text, '(?s)struct\s+' + [regex]::Escape($StructName) + '\s*\{(.*?)\};')
    }
    if (!$match.Success) { throw "Unable to find $StructName" }
    $fields = @()
    foreach ($item in [regex]::Matches($match.Groups[1].Value, '(?m)^\s*([^;]+?)\s*\(\*([A-Za-z0-9_]+)\)\s*IPT([0-9])\(([^;]*)\);')) {
        $fields += [pscustomobject]@{
            ReturnType = ($item.Groups[1].Value.Trim() -replace '\s+', ' ')
            Name = $item.Groups[2].Value
            Arity = [int]$item.Groups[3].Value
            Parameters = ($item.Groups[4].Value.Trim() -replace '\s+', ' ')
        }
    }
    if ($fields.Count -eq 0) { throw "No function-pointer fields in $StructName" }
    return $fields
}

function Get-BalancedBlock {
    param([string]$Text, [int]$OpenBrace)
    $depth = 0
    for ($index = $OpenBrace; $index -lt $Text.Length; $index += 1) {
        if ($Text[$index] -eq '{') { $depth += 1 }
        elseif ($Text[$index] -eq '}') {
            $depth -= 1
            if ($depth -eq 0) { return $Text.Substring($OpenBrace + 1, $index - $OpenBrace - 1) }
        }
    }
    throw 'Unbalanced initializer brace'
}

function Quote-Tsv {
    param([object]$Value)
    return '"' + ([string]$Value).Replace('"', '""') + '"'
}

$cpuVidText = Get-Content -Raw -LiteralPath $cpuVid
$gaMarkText = Get-Content -Raw -LiteralPath $gaMark
$cvidcHeader = Get-Content -Raw -LiteralPath (Join-Path $cvidc 'evidgen.h')
$vglfunc = Get-Content -Raw -LiteralPath (Join-Path $cvidc 'vglfunc.c')
$evidfunc = Get-Content -Raw -LiteralPath (Join-Path $cvidc 'evidfunc.c')

$tableTypes = @{
    'EVID_WRT_POINTERS' = Get-FunctionFields $cpuVidText 'EVID_WRT_POINTERS'
    'EVID_READ_POINTERS' = Get-FunctionFields $cpuVidText 'EVID_READ_POINTERS'
    'EVID_MARK_POINTERS' = Get-FunctionFields $gaMarkText 'EVID_MARK_POINTERS'
    'VideoVector' = Get-FunctionFields $cvidcHeader 'VideoVector'
}

$definitions = @{}
Get-ChildItem -LiteralPath $cvidc -Filter '*.c' | ForEach-Object {
    $sourceText = [string](Get-Content -Raw -LiteralPath $_.FullName)
    foreach ($definition in [regex]::Matches($sourceText, '(?m)GLOBAL\s+([A-Za-z0-9_ *]+?)\s+(S_[A-Za-z0-9_]+)\s+IFN([0-9])\(([^)]*)\)')) {
        $definitions[$definition.Groups[2].Value] = [pscustomobject]@{
            File = $_.Name
            ReturnType = ($definition.Groups[1].Value.Trim() -replace '\s+', ' ')
            Arity = [int]$definition.Groups[3].Value
            Parameters = ($definition.Groups[4].Value.Trim() -replace '\s+', ' ')
        }
    }
}

$rows = [System.Collections.Generic.List[object]]::new()
function Add-Row {
    param(
        [string]$Table,
        [string]$Instance,
        [int]$Slot,
        [object]$Field,
        [string]$Symbol,
        [string]$SourceFile,
        [string]$Disposition
    )
    $definition = $definitions[$Symbol]
    $rows.Add([pscustomobject]@{
        table = $Table
        instance = $Instance
        slot = $Slot
        typed_return = $Field.ReturnType
        typed_arity = $Field.Arity
        typed_parameters = $Field.Parameters
        source_symbol = $Symbol
        generated_definition = if ($definition) { "$($definition.File): $($definition.ReturnType) IFN$($definition.Arity)($($definition.Parameters))" } else { 'not-found' }
        disposition = $Disposition
    })
}

# The original C_Video initializer intentionally leaves the final three table
# slots zero. Preserve that fact instead of inventing a missing provider.
$videoMatch = [regex]::Match($vglfunc, '(?s)struct\s+VideoVector\s+C_Video\s*=\s*\{')
if (!$videoMatch.Success) { throw 'Unable to find C_Video initializer' }
$videoBody = Get-BalancedBlock $vglfunc ($vglfunc.IndexOf('{', $videoMatch.Index))
$videoSymbols = @([regex]::Matches($videoBody, '\b(S_[A-Za-z0-9_]+)\b') | ForEach-Object { $_.Groups[1].Value })
$videoFields = $tableTypes['VideoVector']
for ($index = 0; $index -lt $videoFields.Count; $index += 1) {
    if ($index -lt $videoSymbols.Count) {
        Add-Row 'VideoVector' 'C_Video' $index $videoFields[$index] $videoSymbols[$index] 'vglfunc.c' 'generated-typed-thunk-required'
    } else {
        Add-Row 'VideoVector' 'C_Video' $index $videoFields[$index] '' 'vglfunc.c' 'original-zero-initializer'
    }
}

# Every direct S_* entry in the original EVID pointer tables gets an exact
# typed slot. Arrays repeat their source structure; modulo is therefore the
# original field order, not a project-created convention.
foreach ($kind in @('EVID_WRT_POINTERS', 'EVID_READ_POINTERS', 'EVID_MARK_POINTERS')) {
    $declarationPattern = '(?s)\b' + [regex]::Escape($kind) + '\s+([A-Za-z0-9_]+)\s*(?:\[\])?\s*=\s*\{'
    foreach ($declaration in [regex]::Matches($evidfunc, $declarationPattern)) {
        $body = Get-BalancedBlock $evidfunc ($evidfunc.IndexOf('{', $declaration.Index))
        $symbols = @([regex]::Matches($body, '\b(S_[A-Za-z0-9_]+)\b') | ForEach-Object { $_.Groups[1].Value })
        $fields = $tableTypes[$kind]
        for ($index = 0; $index -lt $symbols.Count; $index += 1) {
            Add-Row $kind $declaration.Groups[1].Value ($index % $fields.Count) $fields[$index % $fields.Count] $symbols[$index] 'evidfunc.c' 'generated-typed-thunk-required'
        }
    }
}

$header = 'table', 'instance', 'slot', 'typed_return', 'typed_arity', 'typed_parameters', 'source_symbol', 'generated_definition', 'disposition' -join "`t"
$lines = [System.Collections.Generic.List[string]]::new()
$lines.Add($header)
foreach ($row in $rows) {
    $lines.Add((@($row.table, $row.instance, $row.slot, $row.typed_return, $row.typed_arity, $row.typed_parameters, $row.source_symbol, $row.generated_definition, $row.disposition) | ForEach-Object { Quote-Tsv $_ }) -join "`t")
}
[IO.Directory]::CreateDirectory((Split-Path -Parent $output)) | Out-Null
[IO.File]::WriteAllLines($output, $lines, [Text.UTF8Encoding]::new($false))

$summary = [pscustomobject]@{
    output = $output
    rows = $rows.Count
    video_vector_rows = @($rows | Where-Object table -eq 'VideoVector').Count
    evid_rows = @($rows | Where-Object table -ne 'VideoVector').Count
    original_zero_slots = @($rows | Where-Object disposition -eq 'original-zero-initializer').Count
    missing_generated_definitions = @($rows | Where-Object { $_.source_symbol -and $_.generated_definition -eq 'not-found' }).Count
}
$summary | ConvertTo-Json -Compress
