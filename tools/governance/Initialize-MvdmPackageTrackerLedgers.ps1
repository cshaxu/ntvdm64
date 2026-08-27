[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$OutputDirectory = 'docs/etc/operations'
)

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}

$ErrorActionPreference = 'Stop'
$repositoryRoot = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$operationsRoot = Join-Path $repositoryRoot $OutputDirectory
$unionPath = Join-Path $operationsRoot 'm0-t274-s1-non-guest-mvdm-union-ledger.tsv'
$debtPath = Join-Path $operationsRoot 'm0-t275-s2-missing-interface-ledger.tsv'

foreach ($path in @($unionPath, $debtPath)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) {
        throw "Missing canonical tracker input: $path"
    }
}

function Write-Tsv([object[]]$Rows, [string]$Name) {
    $path = Join-Path $operationsRoot $Name
    $Rows | Export-Csv -LiteralPath $path -Delimiter "`t" -NoTypeInformation -Encoding utf8
    Write-Host "Wrote $($Rows.Count) rows: $OutputDirectory/$Name"
}

$union = Import-Csv -LiteralPath $unionPath -Delimiter "`t" |
    Where-Object { $_.owner_root -eq 'mvdm-host' } |
    Sort-Object target_path
if ($union.Count -ne 1689) {
    throw "Expected 1,689 selected mvdm-host paths; found $($union.Count)."
}
$index = 0
$fileRows = foreach ($row in $union) {
    $index++
    $extension = [System.IO.Path]::GetExtension($row.target_path).ToLowerInvariant()
    $kind = if ($row.package_root -in @('dirs', 'makefil0')) { 'build-control' }
        elseif ($extension -in @('.c', '.cc', '.cpp', '.asm', '.s')) { 'source' }
        elseif ($extension -in @('.h', '.inc', '.gi')) { 'declaration' }
        else { 'resource-or-other' }
    [pscustomobject]@{
        file_id = ('MVDM-FILE-{0:D4}' -f $index)
        target_path = $row.target_path
        package_root = $row.package_root
        file_kind = $kind
        selection = $row.selection
        selected_source_path = $row.primary_path
        selected_source_sha256 = $row.primary_sha256
        mirror_identity = 'byte-exact-selected-mirror'
        composition_class = 'unreviewed'
        build_profile_state = 'unreviewed'
        divergence_or_exclusion_id = ''
        final_non_recovery_reason = ''
        evidence = 'm0-t274-s1-non-guest-mvdm-union-ledger.tsv'
    }
}

$packageRows = foreach ($group in ($union | Group-Object package_root | Sort-Object Name)) {
    $kind = if ($group.Name -in @('dirs', 'makefil0')) { 'build-control' }
        elseif ($group.Name -in @('inc', 'oemuni', 'suballoc', 'vdmutils')) { 'support' }
        else { 'owner-package' }
    [pscustomobject]@{
        package_id = 'MVDM-PACKAGE-' + $group.Name.ToUpperInvariant().Replace('.', '-')
        package_root = $group.Name
        package_kind = $kind
        selected_path_count = $group.Count
        dependency_state = 'unreviewed'
        topological_wave = ''
        blocked_successor = ''
        evidence = 'm0-t274-s1-non-guest-mvdm-union-ledger.tsv'
    }
}

$debts = Import-Csv -LiteralPath $debtPath -Delimiter "`t"
$successorByDebt = @{
    'MVDM-DEBT-001' = 'Wave 2: host control plane and DEM'
    'MVDM-DEBT-002' = 'Wave 2: host control plane and DEM'
    'MVDM-DEBT-003' = 'Wave 4: XMS'
    'MVDM-DEBT-004' = 'Wave 2: host control plane and DEM'
    'MVDM-DEBT-005' = 'Wave 3: COMMAND local lifecycle / Wave 7 brokered extensions'
    'MVDM-DEBT-006' = 'Wave 2: host control plane and DEM'
    'MVDM-DEBT-007' = 'Wave 5: VDM monitor substrate'
    'MVDM-DEBT-008' = 'Wave 7: Redirector and brokered COMMAND extensions'
    'MVDM-DEBT-009' = 'Wave 8: WOW'
    'MVDM-DEBT-010' = 'Wave 9: VDD and debugger products'
    'MVDM-DEBT-011' = 'Wave 9: VDD and debugger products'
    'MVDM-DEBT-012' = 'Wave 7: Redirector and brokered COMMAND extensions'
}
$interfaceRows = foreach ($debt in $debts) {
    [pscustomobject]@{
        interface_id = $debt.debt_id
        parent_interface_id = ''
        row_kind = 'family-unexpanded'
        caller = $debt.original_package_callers
        declaration_evidence = $debt.original_declaration_evidence
        required_interface = $debt.missing_interface_or_family
        abi_layout = 'expand-before-package-enable'
        owner = $debt.adapter_owner
        availability = 'unreviewed'
        recovery_disposition = 'unreviewed'
        failure_contract = $debt.modern_recovery_or_failure
        successor = $successorByDebt[$debt.debt_id]
        focused_evidence = ''
    }
}

$profileRows = foreach ($package in $packageRows) {
    [pscustomobject]@{
        profile_id = 'MVDM-PROFILE-' + $package.package_root.ToUpperInvariant().Replace('.', '-')
        package_root = $package.package_root
        x86_target = 'unreviewed'
        x64_target = 'unreviewed'
        original_inputs = 'derive-from-file-ledger'
        include_and_library_closure = 'unreviewed'
        profile_state = 'unreviewed'
        compile_evidence = ''
        link_evidence = ''
        local_test_evidence = ''
    }
}

$exclusionRows = @(
    [pscustomobject]@{
        record_id = 'MVDM-EXCLUSION-0001'
        source_scope = 'softpc.new/base/ccpu386/* execution bodies'
        original_purpose = 'historical SoftPC CPU execution engine'
        disposition = 'candidate-bochs-replacement-exclusion'
        reason = 'Bochs is the sole CPU execution owner; tracker must verify each body before final exclusion.'
        retained_contract = 'original headers/call shapes remain adapter-softpc evidence'
        adapter_or_exception_id = 'adapter-softpc / adapter-bochs'
        final_state = 'unreviewed'
        evidence = 'docs/design/ARCHITECTURE.md'
    }
)

Write-Tsv $fileRows 'mvdm-file-recovery-ledger.tsv'
Write-Tsv $interfaceRows 'mvdm-interface-recovery-ledger.tsv'
Write-Tsv $packageRows 'mvdm-package-dependency-ledger.tsv'
Write-Tsv $profileRows 'mvdm-build-profile-ledger.tsv'
Write-Tsv $exclusionRows 'mvdm-divergence-exclusion-ledger.tsv'
