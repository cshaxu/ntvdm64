[CmdletBinding()]
param([Parameter(Mandatory)][string]$Probe,[Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][uint32]$WorkerId,[Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
$root=(Resolve-Path $OutputRoot).Path
$build=(Resolve-Path "$PSScriptRoot/../../build").Path+'\'
if(!$root.StartsWith($build,[StringComparison]::OrdinalIgnoreCase)){throw 'Capture output must be build-owned'}
$image=(Resolve-Path $Probe).Path;$observerPath=(Resolve-Path $Observer).Path
$prefix=Join-Path $root 'ntmon';$report=Join-Path $root 'ntmon-render.txt'
if(Test-Path $report){throw 'Capture evidence already exists'}
$old=$env:MVDM_OBSERVER_PRIVATE_DESKTOP
try {
 $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
 & $observerPath $image ([IO.Path]::GetDirectoryName($image)) $report ([string]$WorkerId) $prefix --observation-timeout-ms 10000
 if($LASTEXITCODE -or (Get-Content $report -Raw) -notmatch '(?m)^exit=0x00000000\r?$' -or
  !(Get-Content ($report+'.console.txt') -Raw).Contains('PASS live NTSRV')){throw 'Live NTMON rendering failed'}
 Add-Type -AssemblyName System.Drawing
 foreach($view in @('main','modal')) {
  $bitmap=[Drawing.Bitmap]::new("$prefix-$view.bmp")
  try {$bitmap.Save("$prefix-$view.png",[Drawing.Imaging.ImageFormat]::Png)}finally{$bitmap.Dispose()}
 }
 'PASS actual NTMON Console cell captures; PNG is faithful cell/color projection, not desktop pixels'
} finally {$env:MVDM_OBSERVER_PRIVATE_DESKTOP=$old}
