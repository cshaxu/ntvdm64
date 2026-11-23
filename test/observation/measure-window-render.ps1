param([Parameter(Mandatory)][string]$BuildRoot,
 [ValidateSet('x86','x64')][string]$Architecture='x86')
$ErrorActionPreference='Stop'
& "$PSScriptRoot/Invoke-MeasurementBuild.ps1" -BuildRoot $BuildRoot -Architecture $Architecture -Target window-render
