[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$OpenNtRoot = 'O:\repos.external\opennt',
    [string]$OpenNt45Root = 'O:\repos.external\opennt-4.5'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Get-Sha256([string]$Path) {
    return (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant()
}

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$opennt = (Resolve-Path -LiteralPath $OpenNtRoot).Path
$opennt45 = (Resolve-Path -LiteralPath $OpenNt45Root).Path
$destination = Join-Path $root 'docs/etc/operations/m0-t288-s1-monitor-source-identity-ledger.tsv'

$packages = @(
    [ordered]@{ id = 'SIM16'; component = 'mvdm-host'; target = 'mvdm-host'; relative = 'sim16'; source = 'base/mvdm/sim16'; source45 = 'nt/private/mvdm/sim16' },
    [ordered]@{ id = 'SIM32'; component = 'mvdm-host'; target = 'mvdm-host'; relative = 'sim32'; source = 'base/mvdm/sim32'; source45 = 'nt/private/mvdm/sim32' },
    [ordered]@{ id = 'V86'; component = 'mvdm-host'; target = 'mvdm-host'; relative = 'v86'; source = 'base/mvdm/v86'; source45 = 'nt/private/mvdm/v86' },
    [ordered]@{ id = 'IEUVDDEx'; component = 'mvdm-host'; target = 'mvdm-host'; relative = 'ieuvddex'; source = 'base/mvdm/ieuvddex'; source45 = 'nt/private/mvdm/ieuvddex' },
    [ordered]@{ id = 'KEYMOUSE'; component = 'mvdm-softpc-firmware'; target = 'mvdm-softpc-firmware'; relative = 'softpc.new/base/keymouse'; source = 'base/mvdm/softpc.new/base/keymouse'; source45 = 'nt/private/mvdm/softpc.new/base/keymouse' }
)

$rows = [System.Collections.Generic.List[object]]::new()
foreach ($package in $packages) {
    $currentRoot = Join-Path $root ('src/{0}/{1}' -f $package.component, $package.relative)
    if (!(Test-Path -LiteralPath $currentRoot -PathType Container)) {
        throw "Missing current package root: $currentRoot"
    }
    foreach ($item in Get-ChildItem -LiteralPath $currentRoot -Recurse -File | Sort-Object FullName) {
        $relative = $item.FullName.Substring($currentRoot.Length + 1).Replace('\', '/')
        $sourcePath = Join-Path $opennt (($package.source + '/' + $relative).Replace('/', '\'))
        $source45Path = Join-Path $opennt45 (($package.source45 + '/' + $relative).Replace('/', '\'))
        $currentHash = Get-Sha256 $item.FullName
        $sourceHash = if (Test-Path -LiteralPath $sourcePath -PathType Leaf) { Get-Sha256 $sourcePath } else { '' }
        $source45Hash = if (Test-Path -LiteralPath $source45Path -PathType Leaf) { Get-Sha256 $source45Path } else { '' }
        $identity = if ($currentHash -eq $sourceHash -and $currentHash -eq $source45Hash) {
            'exact both'
        } elseif ($currentHash -eq $sourceHash) {
            'exact OpenNT selected'
        } elseif ($currentHash -eq $source45Hash) {
            'exact OpenNT-4.5 selected'
        } else {
            'requires divergence/source-selection audit'
        }
        $rows.Add([pscustomobject][ordered]@{
            file_id = ('T288-{0}-{1:d4}' -f $package.id, ($rows.Count + 1))
            package = $package.id
            current_path = ('src/{0}/{1}/{2}' -f $package.component, $package.relative, $relative)
            target_component = $package.target
            opennt_path = ($package.source + '/' + $relative)
            opennt45_path = ($package.source45 + '/' + $relative)
            current_sha256 = $currentHash
            opennt_sha256 = $sourceHash
            opennt45_sha256 = $source45Hash
            identity = $identity
        })
    }
}

$rows | ConvertTo-Csv -Delimiter "`t" -NoTypeInformation | Set-Content -LiteralPath $destination -Encoding utf8
Write-Output ("Wrote {0} T288 source identity rows: {1}" -f $rows.Count, $destination)
