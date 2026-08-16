[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$OutputPath
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$root = [IO.Path]::GetFullPath($RepositoryRoot)
$ledgerPath = Join-Path $root 'docs/etc/research/t221-s1-dem-recovery-ledger-001.json'
if (-not (Test-Path -LiteralPath $ledgerPath -PathType Leaf)) { throw 'T221 P4 recovery ledger is required' }
$ledger = ConvertFrom-Json -InputObject (Get-Content -LiteralPath $ledgerPath -Raw)
$entries = @($ledger.entries)
if ($entries.Count -ne 73) { throw "Expected 73 recovery-ledger rows, got $($entries.Count)" }

function Get-Disposition($entry) {
    $service = $entry.identity.Substring(3)
    switch ($entry.sourceOwnerGroup) {
        'original-noop' { return 'retain: original CF-clear no-op contract is already the owner result' }
        'fastread-exception' { return 'migrate: absorb or replace the standalone compatibility branch in the complete DEM token-I/O package' }
        'file-handle-fcb-search' {
            if ($service -in @('47','48')) { return 'retain: explicit Redirector-deferred pipe unavailable fence' }
            if ($service -eq '30') { return 'replace: generic fallback with the original local-clock FCB contract in the FCB package' }
            return 'migrate: retain seam only as a shared-profile backend of the complete file/handle/FCB/search package'
        }
        'gset-drive-cwd-volume' {
            if ($service -in @('0E','10')) { return 'replace: generic fallback with source-shaped shared drive/volume capability result' }
            return 'migrate: retain snapshot/context seam under the shared profile ABI and GSET package'
        }
        'misc-lifecycle-console-wow' {
            if ($service -eq '3C') { return 'replace: finite boot cleanup cannot stand for complete PDB/VDD/DOS_ABORT lifecycle ownership' }
            return 'migrate: retain only documented no-VDD/no-debug/no-WOW or terminal result inside the formal MISC/lifecycle package'
        }
        'error-lock' { return 'replace: generic cli-unavailable with source-shaped error/lock owner disposition' }
        'dasd-ioctl' {
            if ($service -eq '21') { return 'migrate: retain metadata subfunctions under explicit raw-device profile policy; keep unsupported subfunctions unavailable' }
            return 'retain: source-shaped no-BDS/raw-device unavailable result until a device profile exists' }
        default { throw "Unknown source-owner group: $($entry.sourceOwnerGroup)" }
    }
}

$rows = foreach ($entry in ($entries | Sort-Object { [Convert]::ToInt32($_.identity.Substring(3),16) })) {
    $disposition = Get-Disposition $entry
    [pscustomobject]@{
        identity = $entry.identity
        sourceOwnerGroup = $entry.sourceOwnerGroup
        currentBoundRoute = $entry.currentBoundRoute
        currentShim = $entry.currentShim
        currentFallback = $entry.fallbackDisposition
        disposition = $disposition
    }
}
if ($rows.Count -ne 73 -or @($rows | Where-Object { [string]::IsNullOrWhiteSpace($_.disposition) }).Count -ne 0) {
    throw 'Workaround disposition coverage is incomplete'
}
$output = [IO.Path]::GetFullPath($OutputPath)
$parent = Split-Path -Parent $output
if (-not (Test-Path -LiteralPath $parent)) { New-Item -ItemType Directory -Path $parent | Out-Null }
[ordered]@{
    schema = 'ntdos64.t221.s1.dem-workaround-disposition.v1'
    generatedUtc = [DateTime]::UtcNow.ToString('o')
    serviceCount = $rows.Count
    staticOnly = $true
    entries = $rows
} | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath $output -Encoding utf8
Write-Host "Exported $($rows.Count) DEM workaround dispositions to $output"
