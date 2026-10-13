param([Parameter(Mandatory)][string]$WorkerBuildRoot,[Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$worker=(Resolve-Path $WorkerBuildRoot).Path
$out=[IO.Path]::GetFullPath($BuildRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)){throw 'Build-owned output required'}
$null=New-Item -ItemType Directory -Force "$out/system32"
@"
@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul
if errorlevel 1 exit /b %errorlevel%
cd /d "$out/system32"
cl /nologo /MT /O2 /W4 /I"$repo/src" "$repo/tests/component-integration/dos_style_relay_test.c" /Ferun16.exe /link "$worker/softpc-win32-bindings.lib" "$worker/nthook-installer.lib" "$worker/nthook-context.lib" "$worker/common-root.lib" "$worker/original-opennt-rtl-x86.lib" ntdll.lib kernel32.lib user32.lib advapi32.lib
if errorlevel 1 exit /b %errorlevel%
run16.exe
"@ | Set-Content "$out/run.cmd" -Encoding ascii
& $env:ComSpec /d /c "$out/run.cmd" *> "$out/result.txt"
if($LASTEXITCODE){throw 'DOS style relay fixture failed'}
