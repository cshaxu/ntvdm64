[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,
      [string]$VerificationRoot='',
      [ValidateSet('candidate','verified')][string]$Acceptance='candidate')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$build=(Resolve-Path -LiteralPath $BuildRoot).Path
if(!$build.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)){throw 'Build-owned add-on required'}
$metadata=Get-Content "$build/build.inputs.json" -Raw|ConvertFrom-Json
if($metadata.schema -ne 1 -or $metadata.target -ne 'compat95' -or $metadata.file -ne 'compat95.sys' -or @($metadata.sources).Count -ne 3){throw 'Expected complete compat95 producer manifest'}
$source=Join-Path $repo 'src/addon/compat95'
foreach($name in @('compat95.asm','bootlogo.inc','root.inc')) {
 $row=@($metadata.sources|Where-Object path -eq "src/addon/compat95/$name")
 if($row.Count -ne 1 -or (Get-FileHash (Join-Path $source $name)).Hash -ne $row[0].sha256){throw 'Add-on source changed after build'}
}
$hash=(Get-FileHash "$build/compat95.sys").Hash
if($hash -ne (Get-Content "$build/build.sha256" -Raw).Trim()){throw 'Add-on binary identity mismatch'}
if($Acceptance -eq 'verified'){
 if(!$VerificationRoot){throw 'Verified publication requires the four-case evidence root'}
 $evidence=(Resolve-Path -LiteralPath $VerificationRoot).Path
 foreach($case in @('Enabled','Missing','Invalid','Disabled')){
  $result=Get-Content (Join-Path $evidence ("release-"+$case.ToLowerInvariant()+"/result.json")) -Raw|ConvertFrom-Json
  if(!$result.passed -or $result.logoCase -ne $case -or $result.driverSha256 -ne $hash){throw 'Verified release case/identity mismatch'}
  if($case -eq 'Enabled' -and (!$result.animationRetired -or @($result.animationPhases).Count -lt 2 -or $result.postHandoffSamples -lt 3)){throw 'Verified animation requires actual DAC phases and retired display/tick hooks'}
 }
}
$manifestPath=Join-Path $repo 'assets/release/addon-manifest.json'
$prior=Get-Content $manifestPath -Raw|ConvertFrom-Json
if($prior.schema -ne 1){throw 'Unsupported add-on manifest'}
$sources=@($metadata.sources|ForEach-Object {@{name=[IO.Path]::GetFileName($_.path);sha256=$_.sha256}})
$entry=[ordered]@{id='win95-compat';file='compat95.sys';kind='dos-character-driver';architecture='i386-real-mode';sha256=$hash;source='src/addon/compat95/compat95.asm';sourceSha256=($sources|Where-Object name -eq 'compat95.asm').sha256;sources=$sources;acceptance=$Acceptance}
Copy-Item "$build/compat95.sys" (Join-Path $repo 'assets/release/compat95.sys') -Force
[ordered]@{schema=1;drivers=@($prior.drivers|Where-Object id -ne 'win95-compat')+@($entry);utilities=@($prior.utilities)}|ConvertTo-Json -Depth 6|Set-Content $manifestPath -Encoding UTF8
'PASS published compat95.sys add-on; mouse/utility entries and ten-host-image manifest retained'
