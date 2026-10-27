$ErrorActionPreference='Stop'
$repo='O:\repos.hobby\ntvdm64'
$phase=Join-Path $repo 'build/t473-s10/candidate-validation'
$diagnostic=Join-Path $phase 'creation-diagnostic'
$package=Join-Path $phase 'creation-diagnostic-package'
if(Test-Path -LiteralPath $package){throw 'Fresh diagnostic package required'}
$null=New-Item -ItemType Directory $package
Get-ChildItem -LiteralPath (Join-Path $phase 'package') -Force|ForEach-Object{Copy-Item -LiteralPath $_.FullName -Destination $package -Recurse}
$manifest=Get-Content "$package/product-system32-manifest.json" -Raw|ConvertFrom-Json
$wow=@($manifest|Where-Object Name -eq 'WOW32.DLL')
if($wow.Count -ne 1){throw 'Expected exactly one provider entry'}
Copy-Item -LiteralPath "$diagnostic/WOW32.DLL" -Destination "$package/system32/WOW32.DLL"
& "$repo/tools/build/Set-PackageIdentity.ps1" -Image "$package/system32/WOW32.DLL" -PackageId ([guid]$wow[0].PackageId)
$wow[0].InputSha256=(Get-FileHash "$diagnostic/WOW32.DLL").Hash
$wow[0].Sha256=(Get-FileHash "$package/system32/WOW32.DLL").Hash
$manifest|ConvertTo-Json|Set-Content "$package/product-system32-manifest.json"
@{kind='diagnostic-only';publishable=$false;guestChanged=$false;packageId=$wow[0].PackageId;providerSha256=$wow[0].Sha256;normalCandidateProviderSha256=(Get-FileHash "$phase/package/system32/WOW32.DLL").Hash}|ConvertTo-Json|Set-Content "$package/diagnostic-manifest.json"
$keep=@('SystemRoot','WINDIR','COMSPEC','PATHEXT','SYSTEMDRIVE','NUMBER_OF_PROCESSORS','PROCESSOR_ARCHITECTURE','PROCESSOR_IDENTIFIER','OS')
Get-ChildItem Env:|Where-Object{$_.Name -notin $keep}|ForEach-Object{Remove-Item -LiteralPath ('Env:'+$_.Name)}
$env:PATH='C:\Windows\System32;C:\Windows';$env:TEMP='O:\tmp';$env:TMP='O:\tmp'
& "$repo/tests/observation/verify-wow-sol-options.ps1" -PackageRoot $package -CompilerDriver "$repo/build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd" -BuildRoot "$phase/diagnostic-options-r1"
