param([Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
& "$PSScriptRoot/Invoke-Win95ProbeBuild.ps1" -OutputRoot $OutputRoot -Kind loader
