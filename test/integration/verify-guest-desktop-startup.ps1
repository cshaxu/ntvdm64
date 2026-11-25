[CmdletBinding()]
param(
    [Parameter(Mandatory)][ValidateSet('win31-standard','win31-enhanced','win95')][string]$Case,
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$InstallSource,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$OutputRoot
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$is95=$Case -eq 'win95'
$version=if($is95){'95'}else{'31'}
$mode=if($Case -eq 'win31-standard'){'std'}else{'386'}
$prepare="$PSScriptRoot/ntvdm64-386/New-Win${version}ProbeCopy.ps1"
$copy=& $prepare -InstallSource $InstallSource -InstallRoot "O:/tmp/q$version" -OutputRoot $OutputRoot -DriverImage "$repo/assets/release/ntvdm64.386" -InstanceDriver "$repo/assets/release/insignia.386"
try {
    $options=if($is95){@()}elseif($mode -eq 'std'){@('1')}else{@('2')}
    & "$repo/tools/select-guest-launch.cmd" $copy.Destination $copy.Task $version 0 0 install @options *> "$OutputRoot/select.log"
    if($LASTEXITCODE){throw "Could not select the installed $Case profile"}
    & "$PSScriptRoot/win31_runtime_matrix_test.ps1" -PreparedProfile -PackageRoot $PackageRoot -InstallRoot $copy.Destination -Observer $Observer -OutputRoot "$OutputRoot/runtime" -Cases "$mode-dosonly0" -Windows95:$is95 -Win95ExitBoundary:$is95 -TimeoutMs 90000
} finally {
    . "$repo/test/observation/isolated_package_cleanup.ps1"
    Remove-OwnedGuestProbeCopy $copy.Destination $copy.Task $copy.Source
}
