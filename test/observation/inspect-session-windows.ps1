param([Parameter(Mandatory)][int[]]$ProcessIds,[Parameter(Mandatory)][string]$BuildRoot,
      [string]$Desktop='', [switch]$IgnoreSerialWarnings, [switch]$IgnoreDeniedDiskWarning,
      [switch]$IgnoreIllegalOpcodeWarning)
$ErrorActionPreference='Stop'
$root=(Resolve-Path $BuildRoot).Path
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
if(!$root.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Build-owned output required'}
& "$repo/tools/build-fixtures.cmd" x64 window-inspector | Out-Host
if($LASTEXITCODE){throw 'Window inspector build failed'}
$assembly=Join-Path $repo 'build/.ninja/fixtures-x64/window-inspector.dll'
[void][Reflection.Assembly]::LoadFrom($assembly)
if(($IgnoreSerialWarnings -or $IgnoreDeniedDiskWarning -or $IgnoreIllegalOpcodeWarning) -and !$Desktop.StartsWith('NTVDMConsoleTest-')){throw 'Dialog action is restricted to the explicit private test desktop'}
$text=[WindowInspector]::Read($ProcessIds,$Desktop,$IgnoreSerialWarnings,$IgnoreDeniedDiskWarning,$IgnoreIllegalOpcodeWarning)
[IO.File]::AppendAllText("$root/windows.txt",[DateTime]::UtcNow.ToString('o')+"`r`n"+$text)
$text
