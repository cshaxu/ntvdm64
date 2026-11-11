param(
 [Parameter(Mandatory)][string]$BuildRoot,
 [string]$BuildCache='build/.ninja/msvc-x86-release/worker'
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($BuildRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Use a fresh repository-build evidence directory'}
$cache=(Resolve-Path $BuildCache).Path
$null=New-Item -ItemType Directory $out
$graph=Get-Content "$cache/build.ninja" -Raw
$match=[regex]::Match($graph,'(?m)^build obj/opennt-xactsrv/apisrv.obj:.*\r?\n\s+cflags = (.+)$')
if(!$match.Success){throw 'Original apisrv.c is not in the production graph'}
$flags=$match.Groups[1].Value.Replace('$:',':')
if($flags -notmatch '/DCPU_40_STYLE\b' -or $flags -match '/DCPU_30_STYLE\b'){throw 'Unexpected CPU profile'}
($flags+' /Gy /FI"'+$repo+'/test/mvdm-host/vdmredir/server_enum_fixture_binding.h" /Fo"'+$out+'/apisrv-test.obj" "'+$repo+'/src/non-mvdm/ds/netapi/xactsrv/apisrv.c"') | Set-Content "$out/original.rsp"
($flags+' /Gy /Fo"'+$out+'/enum-test.obj" "'+$repo+'/test/mvdm-host/vdmredir/server_enum_test.c"') | Set-Content "$out/fixture.rsp"
$args=@('/nologo','/OPT:REF',"/OUT:$out/enum-test.exe","/MAP:$out/enum-test.map","$out/enum-test.obj","$out/apisrv-test.obj","$cache/obj/vdmredir-dll/mvdm_redirector_remote_unavailable.obj")
foreach($lib in @('original-opennt-xactsrv.lib','original-opennt-netlib.lib','original-opennt-rtl-x86.lib','original-opennt-netapi-api.lib','lib-crt-compat.lib')){$args+=Join-Path $cache $lib}
$args+=@('netapi32.lib','kernel32.lib','ntdll.lib','advapi32.lib','legacy_stdio_definitions.lib')
$args | ForEach-Object {'"'+$_+'"'} | Set-Content "$out/link.rsp"
@('@echo off','call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul',
 'if errorlevel 1 exit /b %errorlevel%',
 ('cl @"'+$out+'\original.rsp"'),'if errorlevel 1 exit /b %errorlevel%',
 ('cl @"'+$out+'\fixture.rsp"'),'if errorlevel 1 exit /b %errorlevel%',
 ('link @"'+$out+'\link.rsp"')) | Set-Content "$out/build.cmd"
& "$out/build.cmd" *> "$out/build.log"
if($LASTEXITCODE){throw 'Original server enumeration fixture build failed'}
$map=Get-Content "$out/enum-test.map" -Raw
if($map -notmatch '_XsNetServerEnum2\s+.*apisrv-test.obj' -or $map -notmatch '_XsFillEnumBuffer\s+.*xssubs.obj'){throw 'Original handler/conversion owners missing'}
& "$out/enum-test.exe" | Tee-Object -FilePath "$out/result.txt"
if($LASTEXITCODE){throw 'Original server enumeration assertions failed'}
