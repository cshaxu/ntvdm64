param(
    [Parameter(Mandatory = $true)]
    [string]$UnionLedgerPath,
    [Parameter(Mandatory = $true)]
    [string]$TargetRoot,
    [Parameter(Mandatory = $true)]
    [string]$ManifestPath
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Get-SelectedPath($Row) {
    if ($Row.selection -eq 'opennt-4.5-one-sided') {
        return $Row.secondary_path
    }
    return $Row.primary_path
}

function Get-S5Disposition([string]$Path) {
    $name = [IO.Path]::GetFileName($Path).ToLowerInvariant()
    $extension = [IO.Path]::GetExtension($Path).ToLowerInvariant()
    if ($extension -in @('.c', '.cc', '.cpp', '.cxx', '.asm', '.s')) {
        return 'defer-provider-body-s6'
    }
    if ($name -in @('makefile', 'makefil0', 'sources', 'dirs') -or
        $name.StartsWith('makefile.') -or
        $extension -in @('.h', '.inc', '.def', '.rc', '.rom', '.ram', '.ico',
            '.cmd', '.lnk', '.gi', '.mac', '.src', '.prf', '.bar', '.tt',
            '.std', '.skl', '.sed', '.inf', '.ppd')) {
        return 'import-topology-or-resource-s5'
    }
    return 'defer-nontopology-input-s6'
}

$target = (Resolve-Path -LiteralPath $TargetRoot).Path.TrimEnd([char]92)
$ledger = Import-Csv -LiteralPath $UnionLedgerPath -Delimiter "`t"
$rows = @($ledger | Where-Object { $_.owner_root -eq 'mvdm-host' } |
    Sort-Object target_path)
if ($rows.Count -eq 0) {
    throw 'No selected MVDM host rows are present in the union ledger.'
}

$manifest = foreach ($row in $rows) {
    $disposition = Get-S5Disposition $row.target_path
    $selectedPath = Get-SelectedPath $row
    $targetPath = $row.target_path
    $action = 'deferred'
    if ($disposition -eq 'import-topology-or-resource-s5') {
        $source = (Resolve-Path -LiteralPath $selectedPath).Path
        $destination = Join-Path $target $targetPath.Replace([char]'/', [char]92)
        $destinationDirectory = Split-Path -Parent $destination
        if (-not (Test-Path -LiteralPath $destinationDirectory -PathType Container)) {
            New-Item -ItemType Directory -Path $destinationDirectory | Out-Null
        }
        $sourceHash = (Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash.ToLowerInvariant()
        if (Test-Path -LiteralPath $destination -PathType Leaf) {
            $destinationHash = (Get-FileHash -LiteralPath $destination -Algorithm SHA256).Hash.ToLowerInvariant()
            if ($destinationHash -ne $sourceHash) {
                throw "Refusing to overwrite non-identical topology input: $destination"
            }
            $action = 'already-identical'
        } else {
            Copy-Item -LiteralPath $source -Destination $destination
            $action = 'copied-byte-identical'
        }
    }
    [pscustomobject][ordered]@{
        target_path = $targetPath
        package_root = $row.package_root
        s5_disposition = $disposition
        selected_source_path = $selectedPath
        selected_source_sha256 = if ($row.selection -eq 'opennt-4.5-one-sided') { $row.secondary_sha256 } else { $row.primary_sha256 }
        action = $action
        source_selection = $row.selection
    }
}

$directory = Split-Path -Parent $ManifestPath
if (-not (Test-Path -LiteralPath $directory -PathType Container)) {
    throw "Manifest directory does not exist: $directory"
}
$manifest | ConvertTo-Csv -Delimiter "`t" -NoTypeInformation |
    Set-Content -LiteralPath $ManifestPath -Encoding utf8
Write-Output "Imported $(@($manifest | Where-Object { $_.s5_disposition -eq 'import-topology-or-resource-s5' }).Count) topology/resource files; deferred $(@($manifest | Where-Object { $_.s5_disposition -ne 'import-topology-or-resource-s5' }).Count) rows."
