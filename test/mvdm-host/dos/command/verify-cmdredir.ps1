[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,[Parameter(Mandatory)][string]$ReportRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../../../..").Path
$build=(Resolve-Path $BuildRoot).Path
$report=[IO.Path]::GetFullPath($ReportRoot)
$null=New-Item -ItemType Directory -Force $report
$graph=Get-Content "$build/build.ninja" -Raw
$flags=[regex]::Match($graph,'(?m)^cflags = (.+)$').Groups[1].Value.Trim().Replace('$:',':')
if(!$flags){throw 'Missing production compile flags'}
$vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$commands=@('@echo off',('call "'+$vs+'" -arch=x86 -host_arch=x64 >nul'),
 'if errorlevel 1 exit /b %errorlevel%',
 ('cl '+$flags+' /I "'+$repo+'/src/mvdm-host/dos/command" "'+$PSScriptRoot+'/t290_s3_cmdredir_fixture.c" /Fo"'+$report+'/fixture.obj"'),
 'if errorlevel 1 exit /b %errorlevel%',
 ('link /nologo /out:"'+$report+'/pipe-fixture.exe" "'+$report+'/fixture.obj" "'+$build+'/obj/command/cmdredir.obj" kernel32.lib user32.lib'),
 'exit /b %errorlevel%')
$commands|Set-Content "$report/build.cmd" -Encoding ascii
& $env:ComSpec /d /c "$report/build.cmd"
if($LASTEXITCODE){throw 'Pipe fixture build failed'}
$p=Start-Process "$report/pipe-fixture.exe" -WindowStyle Hidden -PassThru
$row=[ordered]@{PID=$p.Id;Started=$p.StartTime.ToUniversalTime().ToString('o');ObjectSHA256=(Get-FileHash "$build/obj/command/cmdredir.obj").Hash}
$done=$p.WaitForExit(15000) # Test deadline only.
if(!$done){$p.Kill();$p.WaitForExit()}
$row.Exit=$p.ExitCode;$row.TimedOut=!$done
$row|ConvertTo-Json|Set-Content "$report/result.json" -Encoding utf8
if(!$done -or $p.ExitCode -ne 0){throw 'Pipe byte/exit assertions failed'}
'PASS original COMMAND pipe input/output bytes and thread exit'
