param([string]$BuildRoot='build/t474-s6/native-hook-packet')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path (Join-Path $PSScriptRoot '../../..')).Path
$root=[IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
if(!$root.StartsWith((Join-Path $repo 'build')+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){throw 'BuildRoot must be under build'}
New-Item -ItemType Directory -Force $root | Out-Null
$vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
foreach($arch in @('x86','x64')) {
  $dir=Join-Path $root $arch;New-Item -ItemType Directory -Force $dir | Out-Null
  $cmd=Join-Path $dir 'build.cmd'
  @('@echo off',('call "'+$vs+'" -arch='+$arch+' -host_arch=x64 >nul'),'if errorlevel 1 exit /b %errorlevel%',
    ('cl /nologo /std:c11 /MT /DUNICODE /D_UNICODE /D_WIN32_WINNT=0x0A00 /I "'+$repo+'/src" "'+$repo+'/test/common/native-hook/packet_test.c" "'+$repo+'/src/common/native-hook/packet.c" /Fe:packet-test.exe /link kernel32.lib'),
    'if errorlevel 1 exit /b %errorlevel%','packet-test.exe','exit /b %errorlevel%') | Set-Content -Encoding ascii $cmd
  Push-Location $dir
  try { & $env:ComSpec /d /c $cmd; if($LASTEXITCODE){throw "packet test failed on $arch ($LASTEXITCODE)"} }
  finally { Pop-Location }
}
'PASS native-hook packet unit on x86 and x64'
