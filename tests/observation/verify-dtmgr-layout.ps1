[CmdletBinding()]
param([string]$BuildRoot='build/M0-T423/S1/dtmgr-edit-style')
$ErrorActionPreference='Stop'
$repo=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$build=[IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
if (!$build.StartsWith((Join-Path $repo 'build')+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)) {
    throw 'Fixture output must stay under repository build/.'
}
New-Item -ItemType Directory -Force -Path $build | Out-Null
$commands=@"
@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul
if errorlevel 1 exit /b 1
midl.exe /nologo /env win32 /target NT100 /prefix client Client_ /prefix server Server_ /out . /h service.h /cstub service_c.c /sstub service_s.c "$repo\src\basesrv-exe\transport\service.idl"
if errorlevel 1 exit /b 1
cl.exe /nologo /c /MT /W4 /we4013 /I . /I "$repo\src" "$repo\src\dtmgr-exe\main.c" service_c.c "$repo\src\basesrv-exe\transport\rpc_security.c" "$repo\tests\observation\dtmgr_layout_test.c"
if errorlevel 1 exit /b 1
link.exe /nologo /subsystem:console /out:dtmgr.exe main.obj service_c.obj rpc_security.obj rpcrt4.lib kernel32.lib user32.lib advapi32.lib
if errorlevel 1 exit /b 1
link.exe /nologo /subsystem:console /out:dtmgr_layout_test.exe dtmgr_layout_test.obj service_c.obj rpc_security.obj rpcrt4.lib kernel32.lib user32.lib advapi32.lib
exit /b %errorlevel%
"@
[IO.File]::WriteAllText((Join-Path $build 'build.cmd'),$commands,[Text.Encoding]::ASCII)
Push-Location $build
try {
    & cmd.exe /c build.cmd
    if ($LASTEXITCODE) { throw 'x86 build failed' }
    $test=Start-Process -FilePath (Join-Path $build 'dtmgr_layout_test.exe') -WorkingDirectory $build -WindowStyle Hidden -PassThru -Wait -RedirectStandardOutput (Join-Path $build 'fixture.stdout.txt') -RedirectStandardError (Join-Path $build 'fixture.stderr.txt')
    Get-Content (Join-Path $build 'fixture.stdout.txt')
    Get-Content (Join-Path $build 'fixture.stderr.txt')
    if ($test.ExitCode) { throw "Layout fixture failed: $($test.ExitCode)" }
} finally { Pop-Location }
