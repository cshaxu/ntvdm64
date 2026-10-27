[CmdletBinding()]
param([Parameter(Mandatory)][string]$PackageRoot,
      [Parameter(Mandatory)][string]$RpcProbe,
      [Parameter(Mandatory)][string]$Observer,
      [Parameter(Mandatory)][string]$OutputRoot,
      [Parameter(Mandatory)][string]$TempRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
. "$repo/tests/observation/isolated_package_cleanup.ps1"
$output=[IO.Path]::GetFullPath($OutputRoot)
$build=[IO.Path]::GetFullPath((Join-Path $repo 'build'))+'\'
if(!$output.StartsWith($build,[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $output)) {
    throw 'Use a fresh repository-build evidence directory'
}
New-Item -ItemType Directory -Path $output | Out-Null
$stages=@();$scopes=@()
try {
    foreach($i in 0..1) {
        $stage=New-PhysicalRuntimeStage $PackageRoot -FreshPackageIdentity
        $stages+=,$stage
        $scopes+=,(New-IsolatedPackageScope $stage)
        $rows=@((Get-Content (Join-Path $stage 'product-system32-manifest.json') -Raw | ConvertFrom-Json))
        $identity=[guid]$rows[0].PackageId
        $probe=Join-Path $stage 'system32/rpc-probe.exe'
        Copy-Item -LiteralPath $RpcProbe -Destination $probe
        & "$repo/tools/build/Set-PackageIdentity.ps1" -Image $probe -PackageId $identity
    }
    & "$PSScriptRoot/verify-package-isolation.ps1" -PackageA $stages[0] -PackageB $stages[1] -OutputRoot (Join-Path $output 'rpc')
    & "$PSScriptRoot/verify-package-session-isolation.ps1" -PackageA $stages[0] -PackageB $stages[1] `
        -Observer $Observer -OutputRoot (Join-Path $output 'dos') -TempRoot $TempRoot -Kind dos
    'PASS independently identified test stages from one baseline'
} finally {
    foreach($scope in $scopes){Stop-IsolatedPackageScope $scope}
    foreach($stage in $stages){Remove-PhysicalRuntimeStage $stage}
}
