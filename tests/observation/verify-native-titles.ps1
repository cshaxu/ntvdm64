param([Parameter(Mandatory)][string]$BuildRoot,[Parameter(Mandatory)][string]$WorkerBuildRoot,
 [Parameter(Mandatory)][string]$Hook64BuildRoot,[Parameter(Mandatory)][string]$StyleBuildRoot,
 [switch]$CompileOnly)
$ErrorActionPreference='Stop'
Import-Module Microsoft.PowerShell.Utility
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)){throw 'Build-owned output required'}
$worker=(Resolve-Path $WorkerBuildRoot).Path;$hook64=(Resolve-Path $Hook64BuildRoot).Path;$style=(Resolve-Path $StyleBuildRoot).Path
$null=New-Item -ItemType Directory -Force $root
try {
 foreach($arch in @('x86','x64')) {
  $dir=Join-Path $root "$arch/system32";$null=New-Item -ItemType Directory -Force $dir
  Copy-Item "$worker/nthook32.dll","$hook64/nthook64.dll","$style/x86/ntw16s32.dll","$style/x64/ntw16s64.dll","$repo/assets/release/run16.exe" $dir
  if($arch -eq 'x86'){$inputs=@("$worker/nthook-installer.lib","$worker/nthook-context.lib","$worker/obj/run16/support.obj","$worker/original-opennt-rtl-x86.lib")}
  else {
   $manifest=Get-Content "$hook64/source-manifest.json" -Raw|ConvertFrom-Json
   $inputs=@($manifest.sources|Where-Object {$_.path -match 'nthook32-dll/(context|installer|detours/.*)\.cpp$|src/common/native_image.c$|opennt_support_rtl.c$|base/ntos/rtl/error.c$'}|ForEach-Object {Join-Path $hook64 $_.object})
  }
  $libraries=($inputs|ForEach-Object {'"'+$_+'"'}) -join ' '
  @"
@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=$arch -host_arch=x64 >nul
if errorlevel 1 exit /b %errorlevel%
cd /d "$dir"
rc /nologo /fo title.res "$repo/tests/component-integration/native_title_hook_test.rc"
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MT /O2 /W4 /WX /D_WIN32_WINNT=0x0A00 /I"$repo/src" "$repo/tests/component-integration/native_title_hook_test.cpp" /Fenative-title-test.exe /link title.res $libraries kernel32.lib user32.lib uxtheme.lib ntdll.lib
if errorlevel 1 exit /b %errorlevel%
link /nologo /subsystem:windows /entry:wmainCRTStartup /out:native-title-gui.exe native_title_hook_test.obj title.res $libraries kernel32.lib user32.lib uxtheme.lib ntdll.lib
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MT /O2 /W4 /WX /D_WIN32_WINNT=0x0A00 /I"$repo/src" "$repo/tests/component-integration/style_loader_failure_test.cpp" /Festyle-loader-failure-test.exe /link $libraries kernel32.lib user32.lib ntdll.lib
"@ | Set-Content "$dir/build.cmd" -Encoding ascii
  & $env:ComSpec /d /c "$dir/build.cmd" *> "$dir/build.txt"
  if($LASTEXITCODE){throw "Native title fixture build failed: $arch"}
 }
 if($CompileOnly){return}
 foreach($arch in @('x86','x64')) {
  $dir=Join-Path $root "$arch/system32"
  $other=if($arch -eq 'x86'){'x64'}else{'x86'}
  foreach($case in @('windows','messages')) {
   $arguments=if($case -eq 'messages'){'--messages'}else{'"'+$root+'\'+$other+'\system32\native-title-test.exe"'}
   $output=if($case -eq 'messages'){'messages'}else{'result'}
   $errors=if($case -eq 'messages'){'messages-error'}else{'error'}
   $p=Start-Process "$dir/native-title-test.exe" -ArgumentList $arguments -WindowStyle Hidden -PassThru -RedirectStandardOutput "$dir/$output.txt" -RedirectStandardError "$dir/$errors.txt"
   $null=$p.Handle
   if(!$p.WaitForExit(300000)){& taskkill /PID $p.Id /T /F;throw "Native title timeout: $arch/$case"}
   $p.Refresh()
   if($null -eq $p.ExitCode -or $p.ExitCode -ne 0){throw "Native title failed: $arch/$case exit=$($p.ExitCode)"}
  }
 }
 '0'|Set-Content "$root/exit-code.txt"
} catch {$_|Out-String|Set-Content "$root/failure.txt";'1'|Set-Content "$root/exit-code.txt";exit 1}
