[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$OutputPath
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$root = [IO.Path]::GetFullPath($RepositoryRoot)
$ledgerPath = Join-Path $root 'docs/etc/research/t225-s57-bop-owner-package-profile-ledger-001.json'
if (!(Test-Path -LiteralPath $ledgerPath)) { throw "Missing T225 S57 ledger: $ledgerPath" }

$packageMap = @{
    'DEM direct-host namespace/file/FCB/search/state package' = [ordered]@{
        packageId = 'dem'; targetSubtask = 'M0 T226 S2'
        fixtureId = 'bx_ntvdm_dem_package_family_v1_test'
        fixturePath = 'tests/bx-vdm/bx_ntvdm_dem_package_family_v1_test.c'
        fixtureKind = 'existing-package-baseline'
    }
    'COMMAND bootstrap/launch/console/environment/lifecycle package' = [ordered]@{
        packageId = 'command'; targetSubtask = 'M0 T226 S3'
        fixtureId = 't225_s20_command_package_family_fixture'
        fixturePath = 'tests/bx-vdm/t225_s20_command_package_family_fixture.c'
        fixtureKind = 'existing-package-baseline'
    }
    'XMS/A20/allocator/UMB/INT15 package' = [ordered]@{
        packageId = 'xms'; targetSubtask = 'M0 T226 S4'
        fixtureId = 't226_s4_xms_package_conformance_negative_fixture'
        fixturePath = 'tests/bx-vdm/t226_s4_xms_package_conformance_negative_fixture.c'
        fixtureKind = 'versioned-negative-fixture'
    }
    'DPMI protected-mode/LDT/exception/memory package' = [ordered]@{
        packageId = 'dpmi'; targetSubtask = 'M0 T226 S5'
        fixtureId = 't226_s5_dpmi_package_conformance_negative_fixture'
        fixturePath = 'tests/bx-vdm/t226_s5_dpmi_package_conformance_negative_fixture.c'
        fixtureKind = 'versioned-negative-fixture'
    }
    'Redirector package' = [ordered]@{
        packageId = 'redirector'; targetSubtask = 'M0 T226 S6'
        fixtureId = 't226_s6_redirector_package_conformance_negative_fixture'
        fixturePath = 'tests/bx-vdm/t226_s6_redirector_package_conformance_negative_fixture.c'
        fixtureKind = 'versioned-negative-fixture'
    }
    'Debugger package' = [ordered]@{
        packageId = 'debugger'; targetSubtask = 'M0 T226 S7'
        fixtureId = 't226_s7_debugger_package_conformance_negative_fixture'
        fixturePath = 'tests/bx-vdm/t226_s7_debugger_package_conformance_negative_fixture.c'
        fixtureKind = 'versioned-negative-fixture'
    }
    'Top-level or machine/BIOS owner package' = [ordered]@{
        packageId = 'top-level-machine'; targetSubtask = 'M0 T226 S7'
        fixtureId = 't226_s7_top_level_machine_conformance_fixture'
        fixturePath = 'tests/bx-vdm/t226_s7_top_level_machine_conformance_fixture.c'
        fixtureKind = 'versioned-negative-fixture'
    }
    'WOW host-composition package' = [ordered]@{
        packageId = 'wow'; targetSubtask = 'M0 T226 S7'
        fixtureId = 't226_s7_wow_conformance_negative_fixture'
        fixturePath = 'tests/bx-vdm/t226_s7_wow_conformance_negative_fixture.c'
        fixtureKind = 'versioned-negative-fixture'
    }
}

$expectedCounts = [ordered]@{
    dem = 73; command = 17; xms = 12; dpmi = 25; redirector = 50
    debugger = 16; 'top-level-machine' = 9; wow = 1
}

$ledger = Get-Content -LiteralPath $ledgerPath -Raw | ConvertFrom-Json
$entries = @($ledger.entries)
if ($entries.Count -ne 203) { throw "Expected 203 source ledger entries, got $($entries.Count)" }
$rows = foreach ($entry in $entries) {
    $owner = [string]$entry.ownerPackage
    if (!$packageMap.ContainsKey($owner)) { throw "No conformance package for $($entry.identity): $owner" }
    $binding = $packageMap[$owner]
    [pscustomobject][ordered]@{
        identity = $entry.identity
        selector = $entry.selector
        service = $entry.service
        handler = $entry.handler
        ownerPackage = $owner
        packageId = $binding.packageId
        targetSubtask = $binding.targetSubtask
        sourceRecoveryRung = $entry.sourceRecoveryRung
        directReadonlyContract = 'Direct and Readonly are required; Overlay remains ABI-reserved and is not selected by this fixture.'
        fixtureId = $binding.fixtureId
        fixturePath = $binding.fixturePath
        fixtureKind = $binding.fixtureKind
        localAcceptance = 'Validate the package-local copied ABI, guest-memory effect where applicable, typed resume/stop result, Direct/Readonly enforcement, ordering and source-defined failure disposition; native trace is not an acceptance input.'
    }
}
$duplicates = @($rows | Group-Object identity | Where-Object { $_.Count -ne 1 })
if ($duplicates.Count -ne 0) { throw 'Conformance manifest has non-unique BOP identities' }
$unexpected = @($rows | Where-Object { [string]::IsNullOrWhiteSpace([string]$_.fixtureId) -or [string]::IsNullOrWhiteSpace([string]$_.fixturePath) })
if ($unexpected.Count -ne 0) { throw 'Conformance manifest contains an unbound fixture target' }
foreach ($pair in $expectedCounts.GetEnumerator()) {
    $actual = @($rows | Where-Object { $_.packageId -eq $pair.Key }).Count
    if ($actual -ne $pair.Value) { throw "Package $($pair.Key) expected $($pair.Value) rows, got $actual" }
}

$output = [IO.Path]::GetFullPath($OutputPath)
$parent = Split-Path -Parent $output
if (!(Test-Path -LiteralPath $parent)) { New-Item -ItemType Directory -Path $parent | Out-Null }
$result = [ordered]@{
    schema = 'runner.t226.s1.bop-local-conformance-manifest.v1'
    sourceLedger = 'docs/etc/research/t225-s57-bop-owner-package-profile-ledger-001.json'
    sourceEntryCount = $entries.Count
    scope = 'Shared local conformance contract and test inventory only; this manifest does not claim provider implementation completeness.'
    mutationProfiles = @('direct', 'readonly')
    overlayDisposition = 'ABI reservation only; no T226 implementation acceptance.'
    packageCounts = $expectedCounts
    entries = $rows
}
[IO.File]::WriteAllText($output, ($result | ConvertTo-Json -Depth 6) + [Environment]::NewLine, [Text.UTF8Encoding]::new($false))
Write-Host "Exported $($rows.Count) T226 S1 conformance rows to $output"