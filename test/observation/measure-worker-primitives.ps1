param([Parameter(Mandatory)][string]$BuildRoot,
 [ValidateSet('x86','x64')][string]$Architecture='x86',[switch]$HighResolutionPublisher)
$ErrorActionPreference='Stop'
$target=if($HighResolutionPublisher){'worker-primitives-high-resolution'}else{'worker-primitives'}
& "$PSScriptRoot/Invoke-MeasurementBuild.ps1" -BuildRoot $BuildRoot -Architecture $Architecture -Target $target
