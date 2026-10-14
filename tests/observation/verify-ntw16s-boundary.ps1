param([Parameter(Mandatory)][string]$BuildRoot,[Parameter(Mandatory)][string]$CandidateRoot,
    [string]$System32,[string]$VsDevCmd='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat')
$ErrorActionPreference='Stop'
Import-Module Microsoft.PowerShell.Utility
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot);$candidate=(Resolve-Path $CandidateRoot).Path
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned evidence root required'}
$null=New-Item -ItemType Directory $root
try{
 foreach($arch in @('x86','x64')){
  $dir=Join-Path $root $arch;$null=New-Item -ItemType Directory $dir
  @"
@echo off
call "$VsDevCmd" -arch=$arch -host_arch=x64 >nul
if errorlevel 1 exit /b %errorlevel%
cd /d "$dir"
cl.exe /nologo /MT /O2 /W4 /WX /utf-8 /D_WIN32_WINNT=0x0A00 /DUNICODE /D_UNICODE /I"$repo/src" /Fo"$dir/test.obj" /Fe"$dir/test.exe" "$repo/tests/app/ntw16s_dll.c" /link user32.lib gdi32.lib
"@|Set-Content "$dir/build.cmd" -Encoding ascii
  & $env:ComSpec /d /c "$dir/build.cmd" *> "$dir/build.txt"
  if($LASTEXITCODE){throw "Fixture build failed $arch"}
  $same=if($arch -eq 'x86'){'ntw16s32.dll'}else{'ntw16s64.dll'}
  $other=if($arch -eq 'x86'){'x64/ntw16s64.dll'}else{'x86/ntw16s32.dll'}
  $dll=if($System32){Join-Path $System32 $same}else{"$candidate/$arch/$same"}
  $wrong=if($System32){Join-Path $System32 (Split-Path $other -Leaf)}else{"$candidate/$other"}
  $demo=if($System32){Join-Path $System32 W16Style.exe}else{"$candidate/$arch/W16Style.exe"}
  Get-FileHash $dll,$wrong,$demo,"$repo/tests/app/ntw16s_dll.c"|ConvertTo-Json|Set-Content "$dir/inputs.json"
  $p=Start-Process "$dir/test.exe" -ArgumentList @($dll,$wrong,$demo,"$dir/showcase.bmp") -WindowStyle Hidden -PassThru -RedirectStandardOutput "$dir/test.txt" -RedirectStandardError "$dir/test.err"
  $null=$p.Handle
  @{pid=$p.Id;start=$p.StartTime;deadline=$p.StartTime.AddSeconds(90)}|ConvertTo-Json|Set-Content "$dir/owner.json"
  if(!$p.WaitForExit(90000)){$p.Kill();$null=$p.WaitForExit(5000);throw "Boundary timeout $arch"}
  $p.Refresh()
  if($null -eq $p.ExitCode -or $p.ExitCode -ne 0){throw "Boundary failed $arch"}
 }
 '0'|Set-Content "$root/exit-code.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";'1'|Set-Content "$root/exit-code.txt";exit 1}
