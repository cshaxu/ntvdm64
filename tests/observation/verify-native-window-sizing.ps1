[CmdletBinding()]
param([Parameter(Mandatory)][string]$CompilerDriver,
      [Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
$compiler=(Resolve-Path $CompilerDriver).Path
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build directory required'}
New-Item -ItemType Directory $root | Out-Null
$source=Join-Path $PSScriptRoot 'native_window_sizing_probe.c'
& $compiler cl.exe /nologo /W4 /WX /O2 /MT "/Fo$root/probe.obj" "/Fe$root/probe.exe" $source /link user32.lib *> "$root/compile.log"
if($LASTEXITCODE){throw 'Native sizing compilation failed'}
$p=Start-Process "$root/probe.exe" -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/probe.log" -RedirectStandardError "$root/probe.err"
$p.Id|Set-Content "$root/probe.pid"
if(!$p.WaitForExit(10000)){$p.Kill($true);throw 'Native sizing timeout'}
$p.ExitCode|Set-Content "$root/probe.exit"
if($p.ExitCode){throw 'Native sizing observation failed'}
$log=Get-Content "$root/probe.log" -Raw
foreach($name in @('fixed-caption','sizeable-caption','popup-border','minimized-caption')){
 if($log -notmatch ([regex]::Escape($name)+' request=154,240 actual=\d+,\d+ style=')){throw "Missing $name observation"}
}
if($log -notmatch 'minimized-caption after-move154-and-restore=\d+,\d+ iconic=0'){throw 'Minimized window did not restore'}
Get-FileHash $source,"$root/probe.exe"|Select-Object Path,Hash|ConvertTo-Json|Set-Content "$root/inputs.json"
'PASS observation and owned-window cleanup; metric values are evidence, not a universal pixel contract'|Set-Content "$root/result.txt"
