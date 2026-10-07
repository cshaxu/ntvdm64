param(
    [Parameter(Mandatory)][string]$BuildRoot,
    [Parameter(Mandatory)][string]$OriginalMedia,
    [Parameter(Mandatory)][string]$Destination,
    [Parameter(Mandatory)][string]$NtDos,
    [Parameter(Mandatory)][string]$PifTemplate,
    [Parameter(Mandatory)][string]$OriginalWin386,
    [string]$ReleaseRoot = '',
    [string]$PreviousManifest = ''
)
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path "$PSScriptRoot/../..").Path
$build = [IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
if(!$build.StartsWith((Join-Path $repo 'build') + '\', [StringComparison]::OrdinalIgnoreCase) -or (Test-Path -LiteralPath $build)) {throw 'Use a fresh build-owned packaging output'}
$source = (Resolve-Path -LiteralPath $OriginalMedia).Path.TrimEnd('\')
$destination = [IO.Path]::GetFullPath($Destination).TrimEnd('\')
if($destination -eq [IO.Path]::GetPathRoot($destination).TrimEnd('\') -or $destination -eq $repo -or $repo.StartsWith($destination + '\', [StringComparison]::OrdinalIgnoreCase)) {throw 'Choose a dedicated package directory, not a drive or repository root'}
if(!$ReleaseRoot) {$ReleaseRoot = Join-Path $repo 'assets/release'}
$release = (Resolve-Path -LiteralPath $ReleaseRoot).Path
if($source -eq $destination -or $source.StartsWith($destination + '\', [StringComparison]::OrdinalIgnoreCase)) {throw 'Package destination must not contain source media'}
if((Test-Path -LiteralPath $destination) -and !$PreviousManifest) {throw 'Preserve existing package without a proven previous manifest'}
if(Test-Path -LiteralPath $destination) {
    $previous = Get-Content -LiteralPath $PreviousManifest -Raw | ConvertFrom-Json
    if($previous.destination -ne $destination) {throw 'Previous package target mismatch'}
    foreach($entry in $previous.files) {if((Get-FileHash -LiteralPath (Join-Path $destination $entry.path)).Hash -ne $entry.sha256) {throw 'Existing package changed; preserve user files'}}
}
New-Item -ItemType Directory -Path $build | Out-Null
$stage = Join-Path $build 'package'; New-Item -ItemType Directory -Path $stage | Out-Null
Get-ChildItem -LiteralPath $source -Force | ForEach-Object {Copy-Item -LiteralPath $_.FullName -Destination $stage -Recurse -Force}
$adaptation = Join-Path $build 'retail-adaptation'
& (Join-Path $PSScriptRoot 'adapt-retail-dosmgr.ps1') -OriginalWin386 $OriginalWin386 -NtDos $NtDos -OutputRoot $adaptation
& (Join-Path $PSScriptRoot 'apply-setup.ps1') -MediaRoot $stage -ReleaseRoot $release -PifTemplate $PifTemplate -AdaptedWin386 (Join-Path $adaptation 'WIN386.EXE') -AdaptationManifest (Join-Path $adaptation 'adaptation.json')
$manifest = @(Get-ChildItem -LiteralPath $stage -Recurse -File | ForEach-Object {
    [ordered]@{path=$_.FullName.Substring($stage.Length + 1);sha256=(Get-FileHash -LiteralPath $_.FullName).Hash;length=$_.Length}
})
[ordered]@{originalMedia=$source;destination=$destination;format='unchanged original media plus authored PATCH only';files=$manifest} | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $build 'package-manifest.json') -Encoding UTF8
if(Test-Path -LiteralPath $destination) {Move-Item -LiteralPath $destination -Destination (Join-Path $build 'previous-package')}
try {
    New-Item -ItemType Directory -Path $destination | Out-Null
    Get-ChildItem -LiteralPath $stage -Force | ForEach-Object {
        Copy-Item -LiteralPath $_.FullName -Destination $destination -Recurse -Force
    }
    foreach($entry in $manifest) {if((Get-FileHash -LiteralPath (Join-Path $destination $entry.path)).Hash -ne $entry.sha256) {throw 'Published package identity mismatch'}}
} catch {
    if(Test-Path -LiteralPath $destination) {Move-Item -LiteralPath $destination -Destination (Join-Path $build 'failed-package')}
    if(Test-Path -LiteralPath (Join-Path $build 'previous-package')) {Move-Item -LiteralPath (Join-Path $build 'previous-package') -Destination $destination}
    throw
}
Write-Host "PASS complete original-media/PATCH package at $destination; installation not run"
