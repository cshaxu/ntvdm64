param(
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$InstallRoot,
    [Parameter(Mandatory)][string]$RepositoryRoot
)
$ErrorActionPreference = 'Stop'
$package = (Resolve-Path -LiteralPath $PackageRoot).Path.TrimEnd('\')
$install = (Resolve-Path -LiteralPath $InstallRoot).Path.TrimEnd('\')
$repo = (Resolve-Path -LiteralPath $RepositoryRoot).Path.TrimEnd('\')
$payload = Join-Path $package 'PATCH'
foreach($name in @('SETUP.CMD','configure-launch.ps1','run-setup.ps1','WIN31-TEMPLATE.PIF','MOUSE31.DRV','WIN386-ADAPTED.EXE','WIN386-ADAPTATION.JSON')) {
    if(!(Test-Path -LiteralPath (Join-Path $payload $name) -PathType Leaf)) {throw "Missing package payload: $name"}
}
if(Test-Path -LiteralPath (Join-Path $payload 'WORK')) {throw 'Package contains temporary WORK'}
& (Join-Path $repo 'tests\component-integration\win31_launch_profile_test.ps1') -InstallRoot $install -ProfileDirectory (Join-Path $install 'PATCH')
foreach($file in @(Get-ChildItem -LiteralPath (Join-Path $install 'PATCH') -File | Where-Object {$_.Extension -in @('.CMD','.NT','.PIF')})) {
    $bytes = [IO.File]::ReadAllBytes($file.FullName)
    $text = [Text.Encoding]::ASCII.GetString($bytes)
    if($text.Contains($package) -or $text.Contains($repo) -or $text -match '(?i)\\WORK(?:\\|\z)') {throw "Installed payload depends on package/repository/WORK: $($file.Name)"}
}
'PASS clean package payload and self-contained installed profiles'
