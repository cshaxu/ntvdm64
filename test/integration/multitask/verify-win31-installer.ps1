[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$InstallSource,
 [Parameter(Mandatory)][string]$InstallRoot,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$OutputRoot
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../../..").Path
$driver="$repo/assets/release/ntvdm64.386"
$instance="$repo/assets/release/insignia.386"
$copy=& "$repo/test/integration/ntvdm64-386/New-Win31ProbeCopy.ps1" `
 -DriverImage $driver -InstanceDriver $instance -InstallSource $InstallSource `
 -InstallRoot $InstallRoot -OutputRoot $OutputRoot
$dest=$copy.Destination
try {
 # The helper stages both drivers; remove precisely its copied entries/files
 # to exercise the real installer on a prepared, isolated pre-install state.
 $ini="$dest/SYSTEM.INI";$enc=[Text.Encoding]::GetEncoding(28591)
 $text=$enc.GetString([IO.File]::ReadAllBytes($ini))
 foreach($name in 'INSIGNIA.386','NTVDM64.386'){
  $line='(?im)^device='+[regex]::Escape("$dest\PATCH\$name")+'\r*\n'
  if([regex]::Matches($text,$line).Count -ne 1){throw "Expected one prepared entry: $name"}
  $text=[regex]::Replace($text,$line,'')
  if((Get-FileHash "$dest/PATCH/$name").Hash -ne (Get-FileHash "$repo/assets/release/$name").Hash){throw "Unexpected prepared driver: $name"}
  Remove-Item -LiteralPath "$dest/PATCH/$name"
 }
 [IO.File]::WriteAllBytes($ini,$enc.GetBytes($text))
 & "$repo/tools/win-multitask/APPLY.CMD" $dest
 if($LASTEXITCODE){throw "Multitask installation failed: $LASTEXITCODE"}
 foreach($name in 'INSIGNIA.386','NTVDM64.386'){
  if((Get-FileHash "$dest/PATCH/$name").Hash -ne (Get-FileHash "$repo/assets/release/$name").Hash){throw "Installed driver differs: $name"}
 }
 & "$repo/test/integration/win31_runtime_matrix_test.ps1" -PackageRoot $PackageRoot `
  -InstallRoot $dest -Observer $Observer -OutputRoot "$($copy.Task)/runtime"
} finally {
 if(@(Get-ChildItem $copy.Source -Force -Recurse -File).Count -ne $copy.Original.Count){throw 'Source installation inventory changed'}
 foreach($row in $copy.Original){
  if((Get-FileHash ($copy.Source+$row.path)).Hash -ne $row.sha256){throw "Source installation changed: $($row.path)"}
 }
}
