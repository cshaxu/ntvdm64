[CmdletBinding()]
param([string]$RepositoryRoot = '')

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$ops = Join-Path $root 'docs/etc/operations'

function Read-Tsv([string]$Name) {
    $path = Join-Path $ops $Name
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing input: $path" }
    @(Import-Csv -LiteralPath $path -Delimiter "`t")
}
function Write-Tsv([object[]]$Rows, [string]$Name) {
    $path = Join-Path $ops $Name
    @($Rows) | Export-Csv -LiteralPath $path -Delimiter "`t" -NoTypeInformation -Encoding utf8
    Write-Host "Wrote $($Rows.Count) rows: docs/etc/operations/$Name"
}
function Short-Text([string]$Text, [int]$Limit = 220) {
    if ([string]::IsNullOrWhiteSpace($Text)) { return '' }
    if ($Text.Length -le $Limit) { return $Text }
    return $Text.Substring(0, $Limit - 3) + '...'
}

$files = Read-Tsv 'm0-t276-final-integration-disposition-ledger.tsv'
if ($files.Count -ne 1689) { throw "Expected 1,689 final file rows; found $($files.Count)." }
$declarations = Read-Tsv 'm0-t276-s3-textual-declaration-ledger.tsv'
$calls = Read-Tsv 'm0-t276-s5-unique-call-disposition-ledger.tsv'
$includes = Read-Tsv 'm0-t276-s2-translation-unit-include-ledger.tsv'
$builds = Read-Tsv 'm0-t276-s2-original-build-description-ledger.tsv'
$waves = Read-Tsv 'm0-t276-s10-package-profile-wave-ledger.tsv'

$fileByPath = @{}
foreach ($file in $files) { $fileByPath[$file.source_path] = $file }
$waveByPackage = @{}
foreach ($wave in $waves) { $waveByPackage[$wave.package_root] = $wave }

$fileRows = foreach ($file in $files | Sort-Object file_id) {
    [pscustomobject]@{
        file_id = $file.file_id
        source_path = $file.source_path
        package_root = $file.package_root
        file_kind = $file.file_kind
        final_disposition = $file.final_disposition
        final_owner_or_link_boundary = $file.final_owner_or_link_boundary
        final_change_class = $file.final_change_class
        named_adapter = $file.named_adapter
        tracker_state = 'final-file-disposition-imported; symbol-closure-not-yet-proven'
        evidence = 'm0-t276-final-integration-disposition-ledger.tsv:' + $file.file_id
    }
}

$interfaceRows = New-Object System.Collections.Generic.List[object]
foreach ($decl in $declarations | Sort-Object declaration_id) {
    $interfaceRows.Add([pscustomobject]@{
        interface_id = $decl.declaration_id
        row_kind = 'lexical-declaration-candidate'
        caller_file_id = ''
        caller_path = ''
        caller_package = ''
        source_line = $decl.source_line
        symbol = $decl.lexical_name
        declaration_path = $decl.declaration_path
        declaration_package = $decl.package_root
        correlation = 'declaration-only'
        reachability_state = 'lexical-candidate-not-source-reached'
        owner_state = 'unassigned-pending-source-reachability'
        abi_state = 'unreviewed'
        failure_state = 'unreviewed'
        evidence = 'm0-t276-s3-textual-declaration-ledger.tsv:' + $decl.declaration_id
    })
}
foreach ($call in $calls | Sort-Object disposition_id) {
    $caller = $fileByPath[$call.caller_path]
    $reachability = switch ($call.disposition) {
        'selected-definition-name-observed' { 'source-name-correlation-not-abi-proven' }
        'external-boundary-candidate' { 'source-external-boundary-candidate' }
        default { 'source-correlation-ambiguous' }
    }
    $interfaceRows.Add([pscustomobject]@{
        interface_id = $call.disposition_id
        row_kind = 'call-declaration-correlation'
        caller_file_id = if ($null -ne $caller) { $caller.file_id } else { '' }
        caller_path = $call.caller_path
        caller_package = $call.caller_package
        source_line = $call.caller_line
        symbol = $call.lexical_callee
        declaration_path = $call.declaration_path
        declaration_package = $call.declaration_package
        correlation = $call.disposition
        reachability_state = $reachability
        owner_state = 'unassigned-pending-source-form-and-abi-review'
        abi_state = 'unreviewed'
        failure_state = 'unreviewed'
        evidence = 'm0-t276-s5-unique-call-disposition-ledger.tsv:' + $call.disposition_id
    })
}

$packageRows = New-Object System.Collections.Generic.List[object]
foreach ($group in $files | Group-Object package_root | Sort-Object Name) {
    $wave = $waveByPackage[$group.Name]
    $packageRows.Add([pscustomobject]@{
        edge_id = 'MVDM-T277-PACKAGE-' + $group.Name.ToUpperInvariant().Replace('.', '-')
        edge_kind = 'package-node'
        from_package = $group.Name
        to_package_or_boundary = ''
        observed_edge_count = 0
        evidence_kind = 'final-file-inventory'
        topological_wave = if ($null -ne $wave) { $wave.candidate_wave } else { 'unassigned' }
        dependency_state = 'package-inventory-only'
        evidence = 'm0-t276-final-integration-disposition-ledger.tsv'
    })
}
$includeGroups = $includes | Where-Object {
    -not [string]::IsNullOrWhiteSpace($_.resolved_selected_package) -and
    $_.caller_package -ne $_.resolved_selected_package
} | Group-Object { $_.caller_package + '|' + $_.resolved_selected_package }
$index = 0
foreach ($group in $includeGroups | Sort-Object Name) {
    $index++; $parts = $group.Name -split '\|', 2
    $packageRows.Add([pscustomobject]@{
        edge_id = ('MVDM-T277-INCLUDE-{0:D4}' -f $index)
        edge_kind = 'observed-textual-include'
        from_package = $parts[0]
        to_package_or_boundary = $parts[1]
        observed_edge_count = $group.Count
        evidence_kind = 'literal-include-not-link-proof'
        topological_wave = 'derive-after-source-form-review'
        dependency_state = 'provisional-textual-edge'
        evidence = (($group.Group | Select-Object -First 3 | ForEach-Object { $_.evidence }) -join '; ')
    })
}
$callGroups = $calls | Where-Object {
    -not [string]::IsNullOrWhiteSpace($_.declaration_package) -and
    $_.caller_package -ne $_.declaration_package
} | Group-Object { $_.caller_package + '|' + $_.declaration_package + '|' + $_.disposition }
$index = 0
foreach ($group in $callGroups | Sort-Object Name) {
    $index++; $parts = $group.Name -split '\|', 3
    $packageRows.Add([pscustomobject]@{
        edge_id = ('MVDM-T277-CORRELATION-{0:D4}' -f $index)
        edge_kind = 'observed-name-correlation'
        from_package = $parts[0]
        to_package_or_boundary = $parts[1]
        observed_edge_count = $group.Count
        evidence_kind = $parts[2] + '-not-abi-proof'
        topological_wave = 'derive-after-source-form-review'
        dependency_state = 'provisional-name-correlation'
        evidence = (($group.Group | Select-Object -First 3 | ForEach-Object { $_.evidence }) -join '; ')
    })
}

$buildRows = foreach ($group in $files | Group-Object package_root | Sort-Object Name) {
    $sourceCount = @($group.Group | Where-Object { $_.file_kind -eq 'source' }).Count
    $buildInputs = @($builds | Where-Object { $_.package_root -eq $group.Name })
    [pscustomobject]@{
        profile_id = 'MVDM-T277-PROFILE-' + $group.Name.ToUpperInvariant().Replace('.', '-')
        package_root = $group.Name
        selected_path_count = $group.Count
        selected_source_count = $sourceCount
        original_build_description_count = $buildInputs.Count
        x86_target_state = 'not-enabled; derive-after-interface-closure'
        x64_target_state = 'not-enabled; derive-after-interface-closure'
        include_library_state = 'literal-input-evidence-only'
        final_disposition_summary = (($group.Group | Group-Object final_disposition | Sort-Object Name | ForEach-Object { $_.Name + '=' + $_.Count }) -join '; ')
        evidence = 'm0-t276-final-integration-disposition-ledger.tsv; m0-t276-s2-original-build-description-ledger.tsv'
    }
}

$divergenceRows = foreach ($file in $files | Sort-Object file_id) {
    [pscustomobject]@{
        record_id = 'MVDM-T277-DISPOSITION-' + $file.file_id.Substring($file.file_id.Length - 4)
        file_id = $file.file_id
        source_path = $file.source_path
        package_root = $file.package_root
        disposition_or_exception = $file.final_disposition
        original_purpose_or_boundary = Short-Text $file.final_owner_or_link_boundary
        change_or_exclusion_rule = Short-Text $file.final_change_class
        named_adapter_or_exception = $file.named_adapter
        final_state = 'file-level-final; symbol-level-not-yet-proven'
        evidence = 'm0-t276-final-integration-disposition-ledger.tsv:' + $file.file_id
    }
}

Write-Tsv $fileRows 'm0-t277-file-recovery-ledger.tsv'
Write-Tsv $interfaceRows 'm0-t277-interface-recovery-ledger.tsv'
Write-Tsv $packageRows 'm0-t277-package-dependency-ledger.tsv'
Write-Tsv $buildRows 'm0-t277-build-profile-ledger.tsv'
Write-Tsv $divergenceRows 'm0-t277-divergence-exclusion-ledger.tsv'
