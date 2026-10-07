param(
    [Parameter(Mandatory)][string]$BuildRoot,
    [Parameter(Mandatory)][string]$PatchedImageRoot,
    [string]$PackageInput='build/M0-T434/S5/r037-runtime',
    [string]$OriginalInstallation='O:\Windows',
    [string]$Setver='O:\winnt\system32\setver.exe'
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$prefix=(Join-Path $repo 'build')+'\'
$root=[IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
if(!$root.StartsWith($prefix,[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned test required'}
$patch=(Resolve-Path (Join-Path $repo $PatchedImageRoot)).Path
$package=(Resolve-Path (Join-Path $repo $PackageInput)).Path
$proof=Get-Content "$patch/installation.json" -Raw | ConvertFrom-Json
if(!$proof.allOtherBytesUnchanged -or $proof.originalSha256 -ne (Get-FileHash "$OriginalInstallation/WIN100.BIN").Hash -or
   $proof.outputSha256 -ne (Get-FileHash "$patch/WIN100.BIN").Hash){throw 'Image installation proof mismatch'}
New-Item -ItemType Directory -Path $root | Out-Null
$runtime=Join-Path $root 'runtime'
New-Item -ItemType Directory -Path $runtime | Out-Null
Copy-Item -Path "$package/*" -Destination $runtime -Recurse
$windows=Join-Path $runtime 'WIN101'
New-Item -ItemType Directory -Path $windows | Out-Null
$original=@(Get-ChildItem -LiteralPath $OriginalInstallation -File | ForEach-Object {
    Copy-Item -LiteralPath $_.FullName -Destination $windows
    @{name=$_.Name;sha256=(Get-FileHash $_.FullName).Hash}
})
Copy-Item -LiteralPath "$patch/WIN100.BIN" -Destination "$windows/WIN100.BIN" -Force
# Normal original SETVER device, using the existing configured user copy.
# No guest instruction patch or fake DOS-version host provider.
Copy-Item -LiteralPath $Setver -Destination "$runtime/system32/SETVER.EXE"
Add-Content -LiteralPath "$runtime/system32/config.nt" -Value "`r`nREM Private Win1.01 compatibility device.`r`ndevice=Z:\system32\SETVER.EXE"
New-Item -ItemType Directory -Path "$runtime/TMP" | Out-Null
Copy-Item -LiteralPath "$PSScriptRoot/win101_return.bat" -Destination "$windows/RETWIN.BAT"
Copy-Item -LiteralPath "$PSScriptRoot/win101_repeat.bat" -Destination "$windows/REPEAT.BAT"
[ordered]@{
    role='mouse-module-image-test'
    runtime=$runtime;windows=$windows; originalInputs=$original
    imageProof=$proof
    setverSha256=(Get-FileHash $Setver).Hash
    workerSha256=(Get-FileHash "$runtime/system32/ntvdm.exe").Hash
} | ConvertTo-Json -Depth 8 | Set-Content "$root/inputs.json"
'Prepared build-owned Windows image test; selected worker hash recorded'
