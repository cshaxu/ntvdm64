param(
    [Parameter(Mandatory)][string]$BuildRoot,
    [Parameter(Mandatory)][string]$DriverBuild,
    [Parameter(Mandatory)][string]$GuardBuild,
    [Parameter(Mandatory)][string]$PackageInput,
    [Parameter(Mandatory)][string]$OriginalInstallation,
    [Parameter(Mandatory)][string]$InstallTarget
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$prefix=(Join-Path $repo 'build')+'\'
$build=[IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
if (!$build.StartsWith($prefix,[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $build)) {
    throw 'Fresh build-owned installation required'
}
$driver=(Resolve-Path (Join-Path $repo $DriverBuild)).Path
$guard=(Resolve-Path (Join-Path $repo $GuardBuild)).Path
$package=(Resolve-Path (Join-Path $repo $PackageInput)).Path
foreach($p in @($driver,$guard,$package)) {
    if (!$p.StartsWith($prefix,[StringComparison]::OrdinalIgnoreCase)) { throw 'Build-owned inputs required' }
}
$guardManifest=Get-Content "$guard/build.json" -Raw | ConvertFrom-Json
if ($guardManifest.role -ne 'test-only-restricted-write-relink-not-product' -or
    $guardManifest.baselineSha256 -ne (Get-FileHash "$package/system32/ntvdm.exe").Hash -or
    $guardManifest.executableSha256 -ne (Get-FileHash "$guard/ntvdm.exe").Hash) {
    throw 'Guard/base identity mismatch'
}
New-Item -ItemType Directory -Path $build | Out-Null
$runtime=Join-Path $build 'runtime'
New-Item -ItemType Directory -Path $runtime | Out-Null
Copy-Item -Path "$package/*" -Destination $runtime -Recurse
Copy-Item -LiteralPath "$guard/ntvdm.exe" -Destination "$runtime/system32/ntvdm.exe" -Force
$media=Join-Path $runtime 'MEDIA'
New-Item -ItemType Directory -Path $media | Out-Null
$original=@(Get-ChildItem -LiteralPath $OriginalInstallation -File | ForEach-Object {
    Copy-Item -LiteralPath $_.FullName -Destination $media
    @{name=$_.Name;sha256=(Get-FileHash $_.FullName).Hash;length=$_.Length}
})
# Augmented disposable source, not modification of the original installation.
Copy-Item -LiteralPath "$driver/MOUSE101.DRV" -Destination "$media/MOUSE.DRV" -Force
Add-Content -LiteralPath "$runtime/system32/config.nt" -Encoding ASCII -Value "`r`nREM Original Setup child execution stays inside this DOS machine.`r`ndosonly"
New-Item -ItemType Directory -Path "$runtime/TMP" | Out-Null
[ordered]@{
    role='private-original-setup-installation-not-product'
    root=$build; runtime=$runtime; media=$media
    expectedDestination=$InstallTarget
    originalRoot=(Resolve-Path $OriginalInstallation).Path
    originalInputs=$original
    addonDriverSha256=(Get-FileHash "$media/MOUSE.DRV").Hash
    guardSha256=$guardManifest.executableSha256
    guardMap=Join-Path $guard 'ntvdm.exe.map'
} | ConvertTo-Json -Depth 5 | Set-Content "$build/inputs.json"
'Prepared guarded build-owned install copy; original files unchanged'
