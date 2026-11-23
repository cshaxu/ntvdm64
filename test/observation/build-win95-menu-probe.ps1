param([Parameter(Mandatory)][string]$OutputRoot,
      [ValidateSet('Menu','DosRestart')][string]$Probe='Menu')
$ErrorActionPreference='Stop'
$kind=if($Probe -eq 'DosRestart'){'dos_restart'}else{'menu'}
& "$PSScriptRoot/Invoke-Win95ProbeBuild.ps1" -OutputRoot $OutputRoot -Kind $kind
