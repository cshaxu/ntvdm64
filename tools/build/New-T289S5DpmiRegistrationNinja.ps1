[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)] [ValidateSet('x86', 'x64')] [string]$Architecture,
    [string]$RepositoryRoot = ''
)
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
function NinjaPath([string]$Path) { $v = $Path.Replace('\', '/'); if ($v.Length -ge 2 -and $v[1] -eq ':') { return $v.Substring(0, 1) + '$' + ':' + $v.Substring(2) }; return $v }
function ObjectName([string]$Path) { return (($Path -replace '[^A-Za-z0-9_]', '_') + '.obj') }
function Hash([string]$Path) { return (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant() }
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot) }
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = Join-Path $root ("build/M0-T289/S5/registration-{0}" -f $Architecture)
$bochs = Join-Path $root ("build/M0-T289/S3/{0}/lib/bochs-core.lib" -f $Architecture)
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (!(Test-Path $bochs) -or !(Test-Path $vs) -or !(Get-Command ninja -ErrorAction SilentlyContinue)) { throw 'Missing S3 Bochs prerequisite, MSVC or Ninja.' }
$c = @('src/session/mapping_manager.c', 'src/session/guest_memory_lease.c', 'src/session/session.c',
    'src/adapter-mvdm-host-out/win32/source/opennt_support_rtl.c',
    'src/adapter-mvdm-host-out/monitor/mvdm_vdm_tib.c', 'src/adapter-mvdm-host-out/monitor/source/vdm_control.c',
    'src/adapter-mvdm-host-out/softpc/mvdm_command_registers.c', 'src/adapter-mvdm-host-out/softpc/mvdm_protected_frame_transaction.c', 'src/adapter-mvdm-host-out/softpc/mvdm_protected_span.c',
    'src/mvdm-host/dpmi32/data.c', 'src/mvdm-host/dpmi32/register.c', 'src/mvdm-host/dpmi32/dpmiint.c',
    'src/mvdm-host-overlay/dpmi32/dpmi_interrupt_registration.c')
$cxx = @('src/adapter-bochs/headless_8042.cc', 'src/adapter-bochs/machine_facade.cc', 'src/adapter-bochs/minimal_machine.cc', 'src/adapter-bochs/minimal_pic.cc', 'src/adapter-bochs/minimal_sim.cc')
$fixture = 'tests/adapter-bochs/t289_s5_dpmi_registration_fixture.cc'
foreach ($path in $c + $cxx + $fixture) { if (!(Test-Path (Join-Path $root $path)) -or $path -match '(^|/)src\.old(/|$)') { throw "Invalid S5 input: $path" } }
New-Item -ItemType Directory -Force $build, (Join-Path $build 'obj'), (Join-Path $build 'lib'), (Join-Path $build 'bin') | Out-Null
$envFile = Join-Path $build 'msvc-mt.cmd'
@('@echo off', 'set "MVDM_T289_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ready', ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'), 'if errorlevel 1 exit /b %errorlevel%', ':ready', 'cd /d "%MVDM_T289_CALLER_CWD%"', '%*') | Set-Content $envFile -Encoding ascii
$manifest = [ordered]@{schema='m0.t289.s5.dpmi-registration.v1';architecture=$Architecture;runtimeLibrary='/MT';sources=@($c+$cxx+$fixture | ForEach-Object {[ordered]@{path=$_;sha256=Hash (Join-Path $root $_)}});prerequisites=@([ordered]@{path=$bochs.Substring($root.Length+1).Replace('\','/');sha256=Hash $bochs});forbiddenInputs=@('src.old','DOSX host translation unit','host LDT implementation','BOP ingress','prebuilt product archive')}
$manifest | ConvertTo-Json -Depth 5 | Set-Content (Join-Path $build 'source-manifest.json') -Encoding utf8
$ci = @('src','src/session','src/adapter-mvdm-host-out/softpc/include','src/adapter-mvdm-host-out/win32/include','src/adapter-mvdm-host-out/monitor/include','src/mvdm-host/dpmi32','src/mvdm-host-overlay/dpmi32','src/mvdm-support/inc','src/mvdm-platform-abi/source/public/sdk/inc','src/mvdm-platform-abi/source/public/internal/base/inc','src/mvdm-platform-abi/source/public/ddk/inc','src/mvdm-host/softpc.new/host/inc','src/mvdm-host/softpc.new/base/inc') | ForEach-Object {'/I "' + (NinjaPath (Join-Path $root $_)) + '"'}
$cxxi = @('src','src/session','src/bochs-core','src/bochs-core/cpu','src/bochs-core/iodev','src/bochs-core/instrument/stubs','src/adapter-bochs','src/adapter-mvdm-host-in','src/adapter-mvdm-host-out/softpc/include','src/adapter-mvdm-host-out/win32/include','src/adapter-mvdm-host-out/monitor/include') | ForEach-Object {'/I "' + (NinjaPath (Join-Path $root $_)) + '"'}
$cf = '/nologo /TC /c /std:c11 /MT /W4 /showIncludes /DWIN_32 /Di386 /DDEVL /FI "' + (NinjaPath (Join-Path $root 'src/adapter-mvdm-host-out/win32/include/nt.h')) + '" /FI "' + (NinjaPath (Join-Path $root 'src/adapter-mvdm-host-out/softpc/include/error_abi.h')) + '" /FI "' + (NinjaPath (Join-Path $root 'src/adapter-mvdm-host-out/monitor/include/vdm.h')) + '" ' + ($ci -join ' ')
$cxxf = '/nologo /TP /c /std:c++14 /EHsc /MT /W4 /showIncludes /DWIN32 /DRUNTIME_ENABLE_MACHINE_UD_BRIDGE=1 ' + ($cxxi -join ' ')
$g = [Collections.Generic.List[string]]::new(); $g.Add('ninja_required_version = 1.10'); $g.Add('build_root = '+(NinjaPath $build)); $g.Add('cflags = '+$cf); $g.Add('cxxflags = '+$cxxf); $g.Add('')
$g.Add('rule cc'); $g.Add('  command = cmd.exe /d /s /c call '+(NinjaPath $envFile)+' cl.exe $cflags /Fo$out $in'); $g.Add('  deps = msvc'); $g.Add('  msvc_deps_prefix = Note: including file:')
$g.Add('rule cxx'); $g.Add('  command = cmd.exe /d /s /c call '+(NinjaPath $envFile)+' cl.exe $cxxflags /Fo$out $in'); $g.Add('  deps = msvc'); $g.Add('  msvc_deps_prefix = Note: including file:')
$g.Add('rule lib'); $g.Add('  command = cmd.exe /d /s /c cd /d $build_root && call '+(NinjaPath $envFile)+' lib.exe /nologo /OUT:$out @$out.rsp')
$g.Add('rule link'); $g.Add('  command = cmd.exe /d /s /c cd /d $build_root && call '+(NinjaPath $envFile)+' link.exe /nologo /OUT:$out @$out.rsp')
$objects=[Collections.Generic.List[string]]::new(); foreach($s in $c){$o='obj/'+(ObjectName $s);$g.Add('build '+$o+': cc '+(NinjaPath (Join-Path $root $s)));$objects.Add($o)}
$lib='lib/original-dpmi-s5-registration.lib';[IO.File]::WriteAllLines((Join-Path $build ($lib+'.rsp')),$objects,[Text.UTF8Encoding]::new($false));$g.Add('build '+$lib+': lib '+($objects -join ' '))
$fo='obj/'+(ObjectName $fixture);$g.Add('build '+$fo+': cxx '+(NinjaPath (Join-Path $root $fixture)));$bo=[Collections.Generic.List[string]]::new();foreach($s in $cxx){$o='obj/'+(ObjectName $s);$g.Add('build '+$o+': cxx '+(NinjaPath (Join-Path $root $s)));$bo.Add($o)}
$out='bin/dpmi-registration-fixture.exe';[IO.File]::WriteAllLines((Join-Path $build ($out+'.rsp')), @($fo,$lib)+@($bo)+@($bochs,'kernel32.lib','user32.lib'), [Text.UTF8Encoding]::new($false));$g.Add('build '+$out+': link '+$fo+' '+$lib+' '+($bo -join ' ')+' '+(NinjaPath $bochs));$g.Add('default '+$out)
[IO.File]::WriteAllLines((Join-Path $build 'build.ninja'),$g,[Text.UTF8Encoding]::new($false));Write-Host "Generated T289 S5 DPMI registration graph: $build"
