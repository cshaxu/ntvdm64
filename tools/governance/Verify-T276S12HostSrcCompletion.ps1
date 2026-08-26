[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$operations = Join-Path $root 'docs/etc/operations'
$checks = @(
    'Verify-T276S12HostSrcDisposition.ps1',
    'Verify-T276S12HostSrcInterfaces.ps1',
    'Verify-T276S12MonitorSessionInterfaces.ps1',
    'Verify-T276S12SoftpcFacadeInterfaces.ps1',
    'Verify-T276S12Win32CapabilityInterfaces.ps1',
    'Verify-T276S12DeviceBridgeInterfaces.ps1',
    'Verify-T276S12BopVddDebuggerInterfaces.ps1',
    'Verify-T276S12BuildUnobservedInterfaces.ps1'
)
foreach ($check in $checks) { & (Join-Path $root "tools/governance/$check") -RepositoryRoot $root }

$sets = @(
    'm0-t276-s12-monitor-session-interface-ledger.tsv',
    'm0-t276-s12-softpc-facade-interface-ledger.tsv',
    'm0-t276-s12-win32-capability-interface-ledger.tsv',
    'm0-t276-s12-device-bridge-interface-ledger.tsv',
    'm0-t276-s12-bop-vdd-debugger-interface-ledger.tsv',
    'm0-t276-s12-build-unobserved-interface-ledger.tsv'
)
$sources = @()
foreach ($file in $sets) { $sources += @(Import-Csv -LiteralPath (Join-Path $operations $file) -Delimiter "`t" | Select-Object -ExpandProperty source_path -Unique) }
if ($sources.Count -ne 56) { throw "Expected 56 source-family memberships, found $($sources.Count)." }
if (($sources | Sort-Object -Unique).Count -ne 56) { throw 'A host/src source is duplicated across detailed owner ledgers.' }
$disposition = @(Import-Csv -LiteralPath (Join-Path $operations 'm0-t276-s12-host-src-disposition-ledger.tsv') -Delimiter "`t")
$expected = @($disposition | Select-Object -ExpandProperty source_path | Sort-Object)
if ((Compare-Object -ReferenceObject $expected -DifferenceObject ($sources | Sort-Object)).Count -ne 0) { throw 'Detailed owner ledgers do not exactly cover the 56-file host/src disposition ledger.' }
Write-Output 'T276 S12 host/src completion verification passed: 56 source rows across six detailed owner ledgers.'
