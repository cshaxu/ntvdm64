[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [Parameter(Mandatory = $true)]
    [string]$InventoryJson,
    [Parameter(Mandatory = $true)]
    [string]$OutputRoot
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
}
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
$inventoryPath = [IO.Path]::GetFullPath($InventoryJson)
$output = [IO.Path]::GetFullPath($OutputRoot)
if (-not (Test-Path -LiteralPath $inventoryPath -PathType Leaf)) { throw "Missing inventory JSON: $inventoryPath" }
if (Test-Path -LiteralPath $output) { throw "Refusing to overwrite existing provider-map root: $output" }

function Get-Sha256([string]$Path) { (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant() }
function Get-Relative([string]$Path) { $Path.Substring($repository.Length + 1).Replace('\', '/') }

function Read-DispatchTable([string]$RelativePath, [string]$StartPattern) {
    $path = Join-Path $repository $RelativePath
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing dispatcher source: $path" }
    $rows = @(); $inside = $false; $lineNumber = 0
    foreach ($line in Get-Content -LiteralPath $path) {
        $lineNumber++
        if (-not $inside) { if ($line -match $StartPattern) { $inside = $true }; continue }
        if ($line -match '^\s*};') { break }
        if ($line -match '^\s*([A-Za-z_][A-Za-z0-9_]*)\s*,?\s*(?://\s*(.*))?$') {
            $rows += [ordered]@{ index = $rows.Count; handler = $Matches[1]; annotation = $Matches[2]; source = (Get-Relative $path); line = $lineNumber }
        }
    }
    if ($rows.Count -eq 0) { throw "No rows parsed from $RelativePath" }
    return $rows
}

$inventory = Get-Content -Raw -LiteralPath $inventoryPath | ConvertFrom-Json
if ($inventory.schema -ne 'ntdos64.opennt-bop-inventory.v1') { throw "Unexpected inventory schema: $($inventory.schema)" }

$tables = [ordered]@{
    BOP_DOS = Read-DispatchTable 'docs\etc\legacy_code\opennt-bop\original\dos\dem\demdisp.c' '^\s*PFNSVC\s+apfnSVC\s*\[\]\s*='
    BOP_CMD = Read-DispatchTable 'docs\etc\legacy_code\opennt-bop\original\dos\command\cmddisp.c' '^\s*PFNSVC\s+apfnSVCCmd\s*\[\]\s*='
    BOP_XMS = Read-DispatchTable 'docs\etc\legacy_code\opennt-bop\original\xms.486\xmsdisp.c' '^\s*PFNSVC\s+apfnXMSSvc\s*\[\]\s*='
    BOP_DPMI = Read-DispatchTable 'docs\etc\legacy_code\opennt-bop\original\dpmi32\dpmi32.c' '^\s*VOID\s*\(\*DpmiDispatchTable\['
    BOP_REDIR = Read-DispatchTable 'docs\etc\legacy_code\opennt-bop\original\vdmredir\vrdisp.c' '^\s*VOID\s*\(\*VrDispatchTable\[\]\)\(VOID\)\s*='
}

$bindings = @{
    BOP_DOS = 'DemDispatch and handlers include softpc.h and use historical register/SAS contracts.'
    BOP_CMD = 'CmdDispatch and handlers include softpc.h; command, console, process, and environment services need CLI capability review.'
    BOP_XMS = 'XMSDispatch and handlers include softpc.h; A20/extended-memory/UMB semantics require native-machine ownership review.'
    BOP_DPMI = 'DpmiDispatch reads the subfunction via Sim32GetVDMPointer and advances IP under protected-mode state.'
    BOP_REDIR = 'VrDispatch includes NT/Win32, network, named-pipe, mailslot, NetAPI and DLC bindings.'
    BOP_DEBUGGER = 'DBGDispatch reads VDM stack frames via Sim32GetVDMPointer; its source does not expose an equivalent flat function-pointer table.'
}

$families = @()
foreach ($family in $inventory.service_families) {
    $mapRows = @()
    $table = $tables[$family.selector_name]
    foreach ($entry in $family.entries) {
        $handler = $null
        if ($null -ne $table -and $entry.value -ge 0 -and $entry.value -lt $table.Count) { $handler = $table[$entry.value] }
        $isSentinel = $entry.name -match 'LASTSVC$'
        $isDebugger = $family.selector_name -eq 'BOP_DEBUGGER'
        $mapRows += [ordered]@{
            name = $entry.name; value = $entry.value; definition_source = "$($entry.source):$($entry.line)"
            original_handler = if ($isSentinel) { $null } elseif ($isDebugger) { 'DBGDispatch (switch-routed)' } elseif ($null -eq $handler) { $null } else { $handler.handler }
            handler_source = if ($isDebugger) { 'controlled-import:opennt-bop/dbg/dbg.c:1361' } elseif ($null -eq $handler) { $null } else { "$($handler.source):$($handler.line)" }
            handler_annotation = if ($null -eq $handler) { $null } else { $handler.annotation }
            dispatch_kind = if ($isSentinel) { 'dispatcher-bound-sentinel' } elseif ($isDebugger) { 'switch-routed' } else { 'function-table' }
            current_disposition = if ($isSentinel) { 'not-callable-sentinel' } else { 'mapped-not-enabled' }; provider = $null
        }
    }
    $families += [ordered]@{
        selector_name = $family.selector_name; selector_value = $family.selector_value
        original_entry = switch ($family.selector_name) {
            'BOP_DOS' { 'MS_bop_0 -> DemDispatch' }; 'BOP_CMD' { 'MS_bop_4 -> CmdDispatch' }
            'BOP_XMS' { 'MS_bop_2 -> XMSDispatch' }; 'BOP_DPMI' { 'MS_bop_3 -> DpmiDispatch' }
            'BOP_REDIR' { 'MS_bop_5 -> VrDispatch (historically lazy-loaded)' }; 'BOP_DEBUGGER' { 'MS_bop_6 -> DBGDispatch' }
            default { 'inventory-defined family' }
        }
        host_binding = $bindings[$family.selector_name]; services = $mapRows
    }
}

$topLevelRouting = @{
    BOP_DOS = 'MS_bop_0 -> DemDispatch'; BOP_WOW = 'MS_bop_1 -> WOW32 dynamic host composition'
    BOP_XMS = 'MS_bop_2 -> XMSDispatch'; BOP_DPMI = 'MS_bop_3 -> DpmiDispatch'
    BOP_CMD = 'MS_bop_4 -> CmdDispatch'; BOP_DEBUGGER = 'MS_bop_6 -> DBGDispatch'
    BOP_REDIR = 'MS_bop_5 -> VDMREDIR dynamic host composition'; BOP_NOSUPPORT = 'historical host warning UI'
    BOP_WAITIFIDLE = 'MS_bop_A -> host idle scheduling'; BOP_DBGBREAKPOINT = 'MS_bop_B -> host debugger'
    BOP_KBD = 'historical temporary VDD/input hook'; BOP_VIDEO = 'historical temporary VDD/video hook'
    BOP_NOTIFICATION = 'MS_bop_E -> host notification'; BOP_UNIMPINT = 'historical temporary monitor hook'
    BOP_SWITCHTOREALMODE = 'historical monitor CPU-mode path'; BOP_UNSIMULATE = 'MS_bop_F -> VDM execution termination'
}
$topLevel = @()
foreach ($selector in $inventory.top_level_selectors) {
    $disposition = switch ($selector.name) {
        'BOP_DOS' { 'family-mapped-not-enabled' }; 'BOP_CMD' { 'family-mapped-not-enabled' }
        'BOP_XMS' { 'family-mapped-not-enabled' }; 'BOP_DPMI' { 'family-mapped-not-enabled' }
        'BOP_DEBUGGER' { 'family-mapped-not-enabled' }; 'BOP_REDIR' { 'family-mapped-not-enabled' }
        'BOP_UNSIMULATE' { 'existing-fixture-awaits-common-ingress-migration' }
        default { 'mapped-deferred-or-unavailable' }
    }
    $topLevel += [ordered]@{ name = $selector.name; value = $selector.value; definition_source = "$($selector.source):$($selector.line)"; original_route = $topLevelRouting[$selector.name]; current_disposition = $disposition }
}

$record = [ordered]@{
    schema = 'ntdos64.opennt-bop-provider-module-map.v1'; inventory_json_sha256 = Get-Sha256 $inventoryPath
    interpretation = 'Original source mapping only. mapped-not-enabled never implies a linkable provider, adapter support, or guest-visible success.'
    provider_order = @('original-composable', 'original-with-contained-cli-capability', 'source-derived-after-evidenced-blocker', 'original-failure-or-deferred')
    top_level_selectors = $topLevel
    service_families = $families
    debugger = [ordered]@{ selector_name = 'BOP_DEBUGGER'; original_entry = 'MS_bop_6 -> DBGDispatch'; source = 'controlled-import:opennt-bop/dbg/dbg.c:1361'; host_binding = $bindings.BOP_DEBUGGER; current_disposition = 'mapped-not-enabled' }
    softpc_bios = [ordered]@{ selector_definitions = $inventory.softpc_bios_selector_definitions; table_rows = $inventory.softpc_bios_table_rows; current_disposition = 'mapped-not-enabled'; interpretation = 'Historical BIOS[] owner symbols are retained verbatim from the input inventory; no conditional row is asserted active.' }
}

New-Item -ItemType Directory -Path $output -Force | Out-Null
$jsonPath = Join-Path $output 'opennt-bop-provider-module-map.json'
[IO.File]::WriteAllText($jsonPath, ($record | ConvertTo-Json -Depth 8), [Text.UTF8Encoding]::new($false))
$lines = @('# OpenNT BOP Provider and Module Map', '', 'Generated source mapping only; no row declares an enabled provider or a successful guest operation.', '', '| Family | Services | Original entry | Current disposition |', '| --- | ---: | --- | --- |')
foreach ($family in $families) { $lines += "| $($family.selector_name) | $($family.services.Count) | $($family.original_entry) | mapped-not-enabled |" }
$lines += @('', "Top-level selectors: $($topLevel.Count)", "Historical BIOS selector definitions: $(@($record.softpc_bios.selector_definitions).Count)", "Historical BIOS table rows: $(@($record.softpc_bios.table_rows).Count)", '', "Inventory JSON SHA-256: $(Get-Sha256 $inventoryPath)", "Map JSON SHA-256: $(Get-Sha256 $jsonPath)")
[IO.File]::WriteAllText((Join-Path $output 'opennt-bop-provider-module-map.md'), ($lines -join [Environment]::NewLine), [Text.UTF8Encoding]::new($false))
Write-Output "OpenNT BOP provider/module map exported: $output"
