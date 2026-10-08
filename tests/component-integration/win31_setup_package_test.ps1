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
foreach($name in @('SETUP.CMD','PATCHSET.EXE','MOUSE31.DRV','KRNL386-ADAPTED.EXE','WIN386-ADAPTED.EXE','KRNL386-RETAIL.EXE','WIN386-RETAIL.EXE','README.TXT')) {
    if(!(Test-Path -LiteralPath (Join-Path $payload $name) -PathType Leaf)) {throw "Missing package payload: $name"}
}
foreach($forbidden in @('WORK','TEMP','configure-launch.ps1','run-setup.ps1','WIN31-TEMPLATE.PIF','WIN386-ADAPTATION.JSON','addon-files.json')) {
    if(Test-Path -LiteralPath (Join-Path $payload $forbidden)) {throw "Package retains non-delivery payload: $forbidden"}
}
$runner=Get-Content -LiteralPath (Join-Path $payload 'SETUP.CMD') -Raw
if(!$runner.Contains('call run16 command /c SETUP.EXE')) {throw 'Setup runner does not use the synchronous DOS COMMAND entry'}
if(!$runner.Contains('PATCHSET.EXE" --win31')) {throw 'Setup runner does not invoke the package finalizer'}
& (Join-Path $repo 'tests\component-integration\win31_launch_profile_test.ps1') -InstallRoot $install -ProfileDirectory (Join-Path $install 'PATCH')
$forbiddenRoots = @($package)
# A build-owned installation fixture may deliberately live under the
# repository's build tree.  Its self-contained profiles must name the
# installation root, not be rejected merely because that root shares the
# repository prefix.  For an external installation, retain the repository
# dependency check.
if(!$install.StartsWith($repo + '\',[StringComparison]::OrdinalIgnoreCase)) {
    $forbiddenRoots += $repo
}
foreach($file in @(Get-ChildItem -LiteralPath (Join-Path $install 'PATCH') -File | Where-Object {$_.Extension -in @('.CMD','.NT','.PIF')})) {
    $bytes = [IO.File]::ReadAllBytes($file.FullName)
    $text = [Text.Encoding]::ASCII.GetString($bytes)
    if(@($forbiddenRoots | Where-Object {$text.Contains($_)}).Count -ne 0 -or $text -match '(?i)\\(?:WORK|TEMP)(?:\\|\z)') {throw "Installed payload depends on package/repository/scratch: $($file.Name)"}
}
'PASS clean package payload and self-contained installed profiles'
