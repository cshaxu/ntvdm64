[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path $RepositoryRoot).Path
$verifiers = @(
    'Verify-T276S25W0W1FinalDisposition.ps1',
    'Verify-T276S25SoftpcObviousFinalDisposition.ps1',
    'Verify-T276S25SoftpcDeclarationFinalDisposition.ps1',
    'Verify-T276S25SoftpcResourceFinalDisposition.ps1',
    'Verify-T276S25SoftpcToolSourceFinalDisposition.ps1',
    'Verify-T276S25SoftpcSupportSourceFinalDisposition.ps1',
    'Verify-T276S25SoftpcDeviceSourceFinalDisposition.ps1',
    'Verify-T276S25SoftpcControlSourceFinalDisposition.ps1',
    'Verify-T276S25XmsFinalDisposition.ps1',
    'Verify-T276S25DpmiFinalDisposition.ps1',
    'Verify-T276S25Dpmi32FinalDisposition.ps1',
    'Verify-T276S25SimFinalDisposition.ps1',
    'Verify-T276S25IeuvddexFinalDisposition.ps1',
    'Verify-T276S25V86FinalDisposition.ps1',
    'Verify-T276S25VdmredirFinalDisposition.ps1',
    'Verify-T276S25DosFinalDisposition.ps1',
    'Verify-T276S25FaxFinalDisposition.ps1',
    'Verify-T276S25DebuggerToolsFinalDisposition.ps1',
    'Verify-T276S25Wow32CarrierFinalDisposition.ps1',
    'Verify-T276S25Wow32ProviderFinalDisposition.ps1',
    'Verify-T276FinalIntegrationLedger.ps1'
)
foreach ($name in $verifiers) {
    & (Join-Path $root "tools/governance/$name") -RepositoryRoot $root
}
Write-Output "T276 S25 closure verification passed: $($verifiers.Count) governed verifier(s)."
