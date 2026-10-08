param(
    [Parameter(Mandatory)][string]$Patchset,
    [Parameter(Mandatory)][string]$RetailRoot,
    [Parameter(Mandatory)][string]$RetailKrnl386,
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$BuildRoot,
    [string]$ShortFixtureRoot = ''
)
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path "$PSScriptRoot/../..").Path
$build = if($ShortFixtureRoot) {[IO.Path]::GetFullPath($ShortFixtureRoot)} else {[IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))}
if((!$ShortFixtureRoot -and !$build.StartsWith((Join-Path $repo 'build') + '\',[StringComparison]::OrdinalIgnoreCase)) -or (Test-Path -LiteralPath $build)) {
    throw 'Use a fresh test fixture path'
}
$retail = (Resolve-Path -LiteralPath $RetailRoot).Path.TrimEnd('\')
$retailKrnl = (Resolve-Path -LiteralPath $RetailKrnl386).Path
$package = (Resolve-Path -LiteralPath $PackageRoot).Path.TrimEnd('\')
foreach($name in @('WIN.COM','SYSTEM.INI','SYSTEM\DOSX.EXE','SYSTEM\KRNL386.EXE','SYSTEM\WIN386.EXE')) {
    if(!(Test-Path -LiteralPath (Join-Path $retail $name) -PathType Leaf)) {throw "Missing retail fixture input: $name"}
}
if((Get-FileHash -LiteralPath $retailKrnl).Hash -ne 'FBEAF672EE9E917318CE8FCD1D560DB805DBFADF4777039ABBAAFF1A9C75B980') {throw 'Retail KRNL386 fixture identity mismatch'}
foreach($name in @('MOUSE31.DRV','KRNL386-ADAPTED.EXE','WIN386-ADAPTED.EXE','KRNL386-RETAIL.EXE','WIN386-RETAIL.EXE')) {
    if(!(Test-Path -LiteralPath (Join-Path $package $name) -PathType Leaf)) {throw "Missing package fixture input: $name"}
}
New-Item -ItemType Directory -Force -Path (Join-Path $build 'installed/SYSTEM'),(Join-Path $build 'package'),(Join-Path $build 'media/PATCH') | Out-Null
Copy-Item -LiteralPath (Join-Path $retail 'WIN.COM'),(Join-Path $retail 'SYSTEM.INI') -Destination (Join-Path $build 'installed')
Copy-Item -LiteralPath (Join-Path $retail 'SYSTEM/DOSX.EXE'),(Join-Path $retail 'SYSTEM/WIN386.EXE') -Destination (Join-Path $build 'installed/SYSTEM')
Copy-Item -LiteralPath $retailKrnl -Destination (Join-Path $build 'installed/SYSTEM/KRNL386.EXE')
Copy-Item -LiteralPath $Patchset -Destination (Join-Path $build 'package/PATCHSET.EXE')
Copy-Item -LiteralPath (Join-Path $package 'MOUSE31.DRV'),(Join-Path $package 'KRNL386-ADAPTED.EXE'),(Join-Path $package 'WIN386-ADAPTED.EXE'),(Join-Path $package 'KRNL386-RETAIL.EXE'),(Join-Path $package 'WIN386-RETAIL.EXE') -Destination (Join-Path $build 'package')
Get-ChildItem -LiteralPath $package -File | ForEach-Object {
    Copy-Item -LiteralPath $_.FullName -Destination (Join-Path $build 'media/PATCH')
}

# PIF templates are embedded in PATCHSET.  They must contain no former
# installation drive/path because the executable is shipped to arbitrary media
# roots; PATCHSET fills all such fields only after the user supplies a target.
$patchsetBytes = [IO.File]::ReadAllBytes((Join-Path $build 'package/PATCHSET.EXE'))
$patchsetText = [Text.Encoding]::ASCII.GetString($patchsetBytes)
if($patchsetText -match '(?i)[a-z]:\\') {throw 'PATCHSET retains a hard-coded drive path in its embedded payload'}

& (Join-Path $build 'package/PATCHSET.EXE') --win31 (Join-Path $build 'installed')
if($LASTEXITCODE -ne 0) {throw "PATCHSET failed: $LASTEXITCODE"}
& (Join-Path $repo 'tests/component-integration/win31_launch_profile_test.ps1') -InstallRoot (Join-Path $build 'installed') -ProfileDirectory (Join-Path $build 'installed/PATCH')
& (Join-Path $repo 'tests/component-integration/win31_setup_package_test.ps1') -PackageRoot (Join-Path $build 'media') -InstallRoot (Join-Path $build 'installed') -RepositoryRoot $repo

$krnlBefore = (Get-FileHash -LiteralPath $retailKrnl).Hash
$krnlAfter = (Get-FileHash -LiteralPath (Join-Path $build 'installed/SYSTEM/KRNL386.EXE')).Hash
if($krnlBefore -ne 'FBEAF672EE9E917318CE8FCD1D560DB805DBFADF4777039ABBAAFF1A9C75B980' -or $krnlAfter -ne '88E095A7C39C6294E8E3BE71CBD2EDC1E4DCECBB7101D1581021EA7BE7B33181') {throw 'PATCHSET did not apply the checked KRNL386 candidate'}
if((Get-FileHash -LiteralPath (Join-Path $build 'installed/PATCH/KRNL386.ORIG')).Hash -ne $krnlBefore) {throw 'PATCHSET did not preserve KRNL386.ORIG'}
if((Get-Content -LiteralPath (Join-Path $build 'installed/SYSTEM.INI') -Raw) -notmatch '(?im)^mouse\.drv=.*\\PATCH\\MOUSE31\.DRV\s*$') {
    throw 'PATCHSET did not select the installed mouse driver'
}
Remove-Item -LiteralPath (Join-Path $build 'installed/PATCH/KRNL386.ORIG') -Force
& (Join-Path $build 'package/PATCHSET.EXE') --win31 (Join-Path $build 'installed')
if($LASTEXITCODE -ne 0) {throw 'PATCHSET did not restore a recovery copy for a prepatched derived-media install'}
if((Get-FileHash -LiteralPath (Join-Path $build 'installed/PATCH/KRNL386.ORIG')).Hash -ne $krnlBefore) {
    throw 'PATCHSET restored an incorrect KRNL386 recovery copy'
}
'PASS PATCHSET produces a self-contained Win3.1 installation patch with checked KRNL386/WIN386 recovery copies, including a derived-media install'
