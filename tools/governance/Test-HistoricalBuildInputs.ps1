param(
    [Parameter(Mandatory = $true)][string]$MinNtArchive,
    [Parameter(Mandatory = $true)][string]$OldSourceArchive,
    [Parameter(Mandatory = $true)][string]$IsolatedBuildRoot
)

$projectRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot "../.."))
$buildRoot = [System.IO.Path]::GetFullPath($IsolatedBuildRoot)

function Get-InputRecord([string]$Name, [string]$Path) {
    $fullPath = [System.IO.Path]::GetFullPath($Path)
    if (-not [System.IO.File]::Exists($fullPath)) {
        return [ordered]@{ name = $Name; path = $fullPath; present = $false; sha256 = $null }
    }
    return [ordered]@{
        name = $Name
        path = $fullPath
        present = $true
        sha256 = (Get-FileHash -LiteralPath $fullPath -Algorithm SHA256).Hash.ToLowerInvariant()
    }
}

$result = [ordered]@{
    contract = "runner-historical-build-inputs-v1"
    projectRoot = $projectRoot
    isolatedBuildRoot = $buildRoot
    buildRootOutsideProject = -not $buildRoot.StartsWith($projectRoot, [System.StringComparison]::OrdinalIgnoreCase)
    inputs = @(
        (Get-InputRecord "MinNT archive" $MinNtArchive),
        (Get-InputRecord "old NT source archive" $OldSourceArchive)
    )
}
$result | ConvertTo-Json -Depth 3

if (-not $result.buildRootOutsideProject -or @($result.inputs | Where-Object { -not $_.present }).Count -ne 0) {
    exit 3
}
