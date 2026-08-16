[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$OutputPath
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$root = [IO.Path]::GetFullPath($RepositoryRoot)
$auditPath = Join-Path $root 'docs/etc/research/t221-s1-dem-current-bound-route-audit-001.json'
if (-not (Test-Path -LiteralPath $auditPath -PathType Leaf)) { throw 'T221 P1 audit is required' }
$audit = ConvertFrom-Json -InputObject (Get-Content -LiteralPath $auditPath -Raw)
$entries = @($audit.entries)
if ($entries.Count -ne 73) { throw "Expected 73 DEM audit rows, got $($entries.Count)" }

# Keep each service set as one property value.  Windows PowerShell otherwise
# enumerates the array while evaluating the PSCustomObject literal.
function Services([string[]]$values) { return ,@($values) }
$groups = @(
    [pscustomobject]@{
        name = 'file-handle-fcb-search'
        services = Services @('00','01','02','03','04','05','06','07','08','09','0A','0B','0C','12','16','17','1E','20','22','27','2C','2D','2E','2F','30','31','44','47','48')
        contract = 'Checked path, token, DTA or SRCHBUF copies; original register/error layouts differ by service. No host HANDLE/pointer crosses the boundary.'
        evidence = @('docs/etc/research/t202-s3-dem-file-handle-fcb-search-source-abi-map-001.md','docs/etc/research/t220-s7-direct-token-jft-sft-representation-map-001.md')
        recovery = 'original CCPU/SAS body excluded; smallest checked seam then source-derived owner rehost; pipes remain Redirector-deferred.'
    },
    [pscustomobject]@{
        name = 'gset-drive-cwd-volume'
        services = Services @('0D','0E','0F','10','13','14','15','18','19','1A','1B','1C','25','41','46')
        contract = 'Copied drive/CWD/DTA/DPB records and source-specific date/time, volume and failure layouts; snapshot is data, not an ambient-host substitute.'
        evidence = @('docs/etc/research/t199-s37-dem-gset-dpb-source-abi-001.md','docs/etc/research/t202-s2-dem-namespace-cwd-volume-source-abi-map-001.md','docs/etc/research/t220-s2-bounded-dem-process-read-design-001.md')
        recovery = 'original CCPU/SoftPC body excluded; retain source-shaped fixed-width capability seam and migrate synthetic answers to the shared profile ABI.'
    },
    [pscustomobject]@{
        name = 'misc-lifecycle-console-wow'
        services = Services @('11','1D','23','34','35','36','37','38','39','3A','3B','3C','3D','3E','45')
        contract = 'DOS-load, VDD/debug/WOW, process and terminal operations have distinct continuation/no-op/controlled-stop contracts; no generic success fallback is valid.'
        evidence = @('docs/etc/research/t199-s37-dem-misc-family-routing-correction-001.md','docs/etc/research/t220-s6-dem-pdb-termination-admission-map-001.md','docs/etc/research/t220-s7-direct-token-jft-sft-representation-map-001.md')
        recovery = 'split by original subfamily; VDD, private engine terminal and WOW dependencies remain unavailable/deferred unless a complete owner package admits them.'
    },
    [pscustomobject]@{
        name = 'error-lock'
        services = Services @('32','33','3F')
        contract = 'Hard-error locator registration, retry and handle-lock semantics use source-specific register/packet failure forms and cannot collapse into a generic ACL refusal.'
        evidence = @('docs/etc/research/t199-s37-dem-working-service-ledger-001.md','docs/etc/research/r20-dem-setharderrorinfo-contract.md')
        recovery = 'original guest packet body excluded; retain checked registration and source-derived unavailable/retry outcomes until the complete error/lock owner is admitted.'
    },
    [pscustomobject]@{
        name = 'dasd-ioctl'
        services = Services @('21','29','2A')
        contract = 'IOCTL metadata and absolute disk I/O preserve subfunction-specific invalid-drive/no-BDS failures; they are not pathname file services.'
        evidence = @('docs/etc/research/t199-s37-dem-dasd-ioctl-family-regression-001.md','docs/etc/research/t96-s3-demioctl-source-contract-001.md')
        recovery = 'raw-device/VDD/SoftPC composition deferred behind an explicit device profile; retain only source-shaped metadata or unavailable results.'
    },
    [pscustomobject]@{
        name = 'original-noop'
        services = Services @('1F','24','26','28','2B','40','43')
        contract = 'demNotYetImplemented clears CF and resumes; no file, device, or host operation occurs.'
        evidence = @('docs/etc/research/t199-s2-dem-source-abi-failure-matrix-001.md','docs/etc/research/t198-s105-dem-fast-io-disposition-map-001.md')
        recovery = 'reuse original visible no-op contract; retain only after current facade/source mapping confirms it.'
    },
    [pscustomobject]@{
        name = 'fastread-exception'
        services = Services @('42')
        contract = 'Original table selects demNotYetImplemented; the retained compatibility branch is separately source-derived and must not be conflated with original no-op.'
        evidence = @('docs/etc/research/t198-s105-dem-fast-io-disposition-map-001.md','docs/etc/research/t198-s109-dem-fastread-contract-reconciliation-001.md')
        recovery = 're-evaluate retained compatibility branch as a complete DEM package decision; delete it if source-shaped original failure is selected.'
    }
)

$assigned = @{}
foreach ($group in $groups) {
    foreach ($service in $group.services) {
        if ($assigned.ContainsKey($service)) { throw "Duplicate recovery-ledger group for 50:$service" }
        $assigned[$service] = $group
    }
}
if ($assigned.Count -ne 73) { throw "Recovery-ledger grouping has $($assigned.Count) services, expected 73" }

$ledger = foreach ($entry in ($entries | Sort-Object { [Convert]::ToInt32($_.identity.Substring(3),16) })) {
    $service = $entry.identity.Substring(3)
    if (-not $assigned.ContainsKey($service)) { throw "No recovery-ledger group for $($entry.identity)" }
    $group = $assigned[$service]
    [pscustomobject]@{
        identity = $entry.identity
        originalHandler = $entry.originalHandler
        originalDispatcher = $entry.originalDispatcher
        currentBoundRoute = $entry.currentBoundRoute
        sourceOwnerGroup = $group.name
        sourceAbiFailureContract = $group.contract
        sourceEvidence = $group.evidence
        recoveryRungDisposition = $group.recovery
        sharedProfileDisposition = $entry.sharedProfileDisposition
        currentShim = $entry.laterShim
        fallbackDisposition = $entry.temporaryFallback
        status = 'audit only: no provider completeness or runtime claim'
    }
}

$output = [IO.Path]::GetFullPath($OutputPath)
$parent = Split-Path -Parent $output
if (-not (Test-Path -LiteralPath $parent)) { New-Item -ItemType Directory -Path $parent | Out-Null }
[ordered]@{
    schema = 'ntdos64.t221.s1.dem-recovery-ledger.v1'
    generatedUtc = [DateTime]::UtcNow.ToString('o')
    serviceCount = $ledger.Count
    staticOnly = $true
    ownerGroups = @($groups | ForEach-Object { [pscustomobject]@{ name=$_.name; serviceCount=$_.services.Count; services=$_.services } })
    entries = $ledger
} | ConvertTo-Json -Depth 7 | Set-Content -LiteralPath $output -Encoding utf8
Write-Host "Exported $($ledger.Count) DEM source/ABI/failure ledger rows to $output"
