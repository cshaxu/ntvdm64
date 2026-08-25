[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [Parameter(Mandatory = $true)]
    [string]$OutputRoot
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
}
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
$output = [IO.Path]::GetFullPath($OutputRoot)
if (Test-Path -LiteralPath $output) {
    throw "Refusing to overwrite existing BOP inventory root: $output"
}

function Get-Sha256([string]$Path) {
    return (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant()
}

function Convert-BopLiteral([string]$Text) {
    if ($Text -match '^0x([0-9A-Fa-f]+)$') {
        return [Convert]::ToInt32($Matches[1], 16)
    }
    if ($Text -match '^[0-9]+$') {
        return [Convert]::ToInt32($Text, 10)
    }
    throw "Unsupported BOP numeric literal: $Text"
}

function Parse-CDefines([string]$Path, [scriptblock]$Include) {
    $entries = @()
    $lineNumber = 0
    foreach ($line in Get-Content -LiteralPath $Path) {
        $lineNumber++
        if ($line -match '^\s*#define\s+([A-Za-z_][A-Za-z0-9_]*)\s+(0x[0-9A-Fa-f]+|[0-9]+)\b') {
            $name = $Matches[1]
            if (& $Include $name) {
                $entries += [ordered]@{
                    name = $name; value = Convert-BopLiteral $Matches[2]
                    source = $Path.Substring($repository.Length + 1).Replace('\', '/')
                    line = $lineNumber; source_text = $line.Trim()
                }
            }
        }
    }
    return $entries
}

function Parse-DpmiSubfunctions([string]$Path) {
    $entries = @(); $inside = $false; $lineNumber = 0
    foreach ($line in Get-Content -LiteralPath $Path) {
        $lineNumber++
        if ($line -match 'DPMI Bop Sub Functions') { $inside = $true; continue }
        if (-not $inside) { continue }
        if ($line -match '^\s*#define\s+MAX_DPMI_BOP_FUNC\b') { break }
        if ($line -match '^\s*#define\s+([A-Za-z_][A-Za-z0-9_]*)\s+(0x[0-9A-Fa-f]+|[0-9]+)\b') {
            $entries += [ordered]@{
                name = $Matches[1]; value = Convert-BopLiteral $Matches[2]
                source = $Path.Substring($repository.Length + 1).Replace('\', '/')
                line = $lineNumber; source_text = $line.Trim()
            }
        }
    }
    if ($entries.Count -eq 0) { throw "No DPMI BOP subfunctions parsed from $Path" }
    return $entries
}

function Parse-BiosBopDefinitions([string]$Path) {
    $entries = @(); $inside = $false; $lineNumber = 0
    foreach ($line in Get-Content -LiteralPath $Path) {
        $lineNumber++
        if ($line -match 'define the BOP call numbers') { $inside = $true; continue }
        if ($inside -and ($line -match 'defines the structure of the Bios internal storage area')) { break }
        if ($inside -and ($line -match '^\s*#define\s+(BIOS_[A-Z0-9_]+)\s+(0x[0-9A-Fa-f]+|[0-9]+)\b')) {
            $entries += [ordered]@{
                name = $Matches[1]; value = Convert-BopLiteral $Matches[2]
                source = $Path.Substring($repository.Length + 1).Replace('\', '/')
                line = $lineNumber; source_text = $line.Trim()
            }
        }
    }
    if ($entries.Count -lt 20) { throw "Unexpectedly incomplete BIOS BOP definition parse from $Path" }
    return $entries
}

function Parse-BiosTable([string]$Path) {
    $entries = @(); $lineNumber = 0
    foreach ($line in Get-Content -LiteralPath $Path) {
        $lineNumber++
        if ($line -match '^\s*([A-Za-z_][A-Za-z0-9_]*)\s*,?\s*/\*\s*BOP\s+([0-9A-Fa-f]{1,2})\s*\*/') {
            $entries += [ordered]@{
                selector = [Convert]::ToInt32($Matches[2], 16); owner_symbol = $Matches[1]
                source = $Path.Substring($repository.Length + 1).Replace('\', '/')
                line = $lineNumber; source_text = $line.Trim()
            }
        }
    }
    if ($entries.Count -lt 20) { throw "Unexpectedly incomplete BIOS BOP table parse from $Path" }
    return $entries
}

$sourcePaths = [ordered]@{
    selector_c = Join-Path $repository 'src\opennt-bop\inc\bop.h'
    selector_asm = Join-Path $repository 'src\opennt-bop\inc\BOP.INC'
    dos = Join-Path $repository 'src\opennt-bop\inc\dossvc.h'
    command = Join-Path $repository 'src\opennt-bop\inc\cmdsvc.h'
    xms = Join-Path $repository 'src\opennt-bop\inc\xmssvc.h'
    dpmi = Join-Path $repository 'src\opennt-bop\inc\dpmi.h'
    redir = Join-Path $repository 'src\opennt-bop\inc\rdrsvc.h'
    debugger = Join-Path $repository 'src\opennt-bop\inc\dbgsvc.h'
    bios_definitions = Join-Path $repository 'src\opennt-softpc\bios\bios.h'
    bios_table = Join-Path $repository 'src\opennt-softpc\bios\bios.c'
}
foreach ($path in $sourcePaths.Values) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing pinned BOP source: $path" }
}

$selectors = Parse-CDefines $sourcePaths.selector_c { param($name) $name -match '^BOP_' -and $name -ne 'BOP_SIZE' }
if ($selectors.Count -lt 10) { throw 'Unexpectedly incomplete top-level BOP selector set.' }
$asmSelectors = @()
$lineNumber = 0
foreach ($line in Get-Content -LiteralPath $sourcePaths.selector_asm) {
    $lineNumber++
    if ($line -match '^\s*(BOP_[A-Z0-9_]+)\s+EQU\s+([0-9A-Fa-f]+)H\b' -and $Matches[1] -ne 'BOP_SIZE') {
        $asmSelectors += [ordered]@{ name = $Matches[1]; value = [Convert]::ToInt32($Matches[2], 16); line = $lineNumber }
    }
}
foreach ($selector in $selectors) {
    $match = @($asmSelectors | Where-Object { $_.name -eq $selector.name })
    if ($match.Count -ne 1 -or $match[0].value -ne $selector.value) {
        throw "C/ASM selector disagreement for $($selector.name)"
    }
}

$families = @(
    [ordered]@{ selector_name = 'BOP_DOS'; selector_value = 0x50; source_kind = 'service'; entries = (Parse-CDefines $sourcePaths.dos { param($name) $name -match '^SVC_DEM|^SVC_GETDRIVEINFO$|^SVC_OUTPUT_STRING$|^SVC_INPUT_STRING$|^SVC_ISDEBUG$|^SVC_PDBTERMINATE$' }) },
    [ordered]@{ selector_name = 'BOP_CMD'; selector_value = 0x54; source_kind = 'service'; entries = (Parse-CDefines $sourcePaths.command { param($name) $name -match '^SVC_CMD|^SVC_GETSTDHANDLE$|^SVC_EXECCOMSPEC32$|^SVC_RETURNEXITCODE$|^SVC_GETCONFIGSYS$|^SVC_GETAUTOEXECBAT$|^SVC_GETKBDLAYOUT$|^SVC_GETINITENVIRONMENT$|^SVC_GETSTARTINFO$' }) },
    [ordered]@{ selector_name = 'BOP_XMS'; selector_value = 0x52; source_kind = 'service'; entries = (Parse-CDefines $sourcePaths.xms { param($name) $name -match '^XMS_' -and $name -ne 'XMS_LASTSVC' }) },
    [ordered]@{ selector_name = 'BOP_DPMI'; selector_value = 0x53; source_kind = 'service'; entries = (Parse-DpmiSubfunctions $sourcePaths.dpmi) },
    [ordered]@{ selector_name = 'BOP_REDIR'; selector_value = 0x57; source_kind = 'service'; entries = (Parse-CDefines $sourcePaths.redir { param($name) $name -match '^SVC_RDR|^SVC_NETBIOS5C$|^SVC_NETBIOS5CINTERRUPT$|^SVC_NETBIOSCHECK$|^SVC_DLC_5C$|^SVC_VDM_WINDOW_INIT$' }) },
    [ordered]@{ selector_name = 'BOP_DEBUGGER'; selector_value = 0x56; source_kind = 'service'; entries = (Parse-CDefines $sourcePaths.debugger { param($name) $name -match '^DBG_' }) }
)
foreach ($family in $families) {
    if ($family.entries.Count -eq 0) { throw "Empty BOP family inventory: $($family.selector_name)" }
}
$biosDefinitions = Parse-BiosBopDefinitions $sourcePaths.bios_definitions
$biosTable = Parse-BiosTable $sourcePaths.bios_table

New-Item -ItemType Directory -Path $output -Force | Out-Null
$sourceManifest = @($sourcePaths.GetEnumerator() | ForEach-Object {
    [ordered]@{ role = $_.Key; path = $_.Value.Substring($repository.Length + 1).Replace('\', '/'); sha256 = Get-Sha256 $_.Value }
})
$record = [ordered]@{
    schema = 'runner.opennt-bop-inventory.v1'
    source_tree = 'src-owned OpenNT selected inputs'
    source_manifest = $sourceManifest
    selector_instruction = [ordered]@{ bytes = @('c4','c4','selector'); base_bytes = 3; family_service_byte = 4 }
    top_level_selectors = $selectors
    service_families = $families
    softpc_bios_selector_definitions = $biosDefinitions
    softpc_bios_table_rows = $biosTable
    interpretation = 'Definitions only. BIOS table rows preserve their source conditional context through source_text; no row is asserted enabled by the selected Bochs profile. No entry is asserted implemented by the adapter.'
}
$jsonPath = Join-Path $output 'opennt-bop-inventory.json'
[IO.File]::WriteAllText($jsonPath, ($record | ConvertTo-Json -Depth 8), [Text.UTF8Encoding]::new($false))
$jsonHash = Get-Sha256 $jsonPath
$lines = @(
    '# OpenNT MVDM BOP Definition Inventory',
    '',
    'This is a source-definition inventory only. It does not declare a BOP enabled, connected, or implemented.',
    '',
    '## Top-level selectors',
    '',
    '| Name | Hex | Source |',
    '| --- | ---: | --- |'
)
foreach ($entry in $selectors) { $lines += "| ``$($entry.name)`` | ``0x{0:x2}`` | ``$($entry.source):$($entry.line)`` |" -f $entry.value }
$lines += @('', '## Service families', '', '| Selector | Entries | Source |', '| --- | ---: | --- |')
foreach ($family in $families) {
    $source = $family.entries[0].source
    $lines += "| ``$($family.selector_name)`` | $($family.entries.Count) | ``$source`` |"
}
$lines += @('', '## SoftPC BIOS table', '', "The parser retained $($biosTable.Count) explicitly annotated table rows. Conditional compilation remains in each source line and is not evaluated.", '', "JSON SHA-256: ``$jsonHash``")
[IO.File]::WriteAllText((Join-Path $output 'opennt-bop-inventory.md'), ($lines -join "`r`n"), [Text.UTF8Encoding]::new($false))
Write-Output "OpenNT BOP inventory exported: $output"
