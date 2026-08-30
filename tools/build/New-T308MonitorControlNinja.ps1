[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)] [ValidateSet('x86')] [string]$Architecture,
    [string]$RepositoryRoot = ''
)
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
function NinjaPath([string]$Path) { $v = $Path.Replace('\', '/'); if ($v.Length -ge 2 -and $v[1] -eq ':') { return $v.Substring(0, 1) + '$' + ':' + $v.Substring(2) }; return $v }
function ObjectName([string]$Path) { return (($Path -replace '[^A-Za-z0-9_]', '_') + '.obj') }
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot) }
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = Join-Path $root ("build/M0-T308/S1/{0}" -f $Architecture)
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (!(Test-Path $vs) -or !(Get-Command ninja -ErrorAction SilentlyContinue)) { throw 'Missing MSVC Build Tools or Ninja.' }
$sources = @(
    'src/session/mapping_manager.c', 'src/session/guest_memory_lease.c', 'src/session/session.c',
    'src/adapter-mvdm-host-out/win32/source/opennt_support_rtl.c',
    'src/adapter-mvdm-host-out/monitor/source/vdm_control.c',
    'src/adapter-mvdm-host-out/monitor/mvdm_vdm_tib.c')
$fixtures = @(
    'tests/adapter-mvdm-host-out/monitor/vdm_control_fixture.c',
    'tests/adapter-mvdm-host-out/monitor/vdm_tib_fixture.c')
foreach ($path in $sources + $fixtures) { if (!(Test-Path (Join-Path $root $path)) -or $path -match '(^|/)src\.old(/|$)') { throw "Invalid T308 input: $path" } }
New-Item -ItemType Directory -Force $build, (Join-Path $build 'obj'), (Join-Path $build 'bin') | Out-Null
$envFile = Join-Path $build 'msvc-mt.cmd'
@('@echo off', 'set "MVDM_T308_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ready', ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'), 'if errorlevel 1 exit /b %errorlevel%', ':ready', 'cd /d "%MVDM_T308_CALLER_CWD%"', '%*') | Set-Content $envFile -Encoding ascii
$includes = @('src/session','src/adapter-mvdm-host-out/win32/include','src/opennt-host/public/sdk/inc','src/opennt-abi/source/public/sdk/inc','src/opennt-abi/source/public/internal/base/inc','src/adapter-mvdm-host-out/monitor/include') | ForEach-Object { '/I "' + (NinjaPath (Join-Path $root $_)) + '"' }
# Do not define the historical i386 product marker globally.  The monitor
# context selects only the original _X86_ declaration gate for this x86 build.
$flags = '/nologo /TC /c /std:c11 /MT /W4 /WX /showIncludes /DWIN_32 ' + ($includes -join ' ')
$g = [Collections.Generic.List[string]]::new(); $g.Add('ninja_required_version = 1.10'); $g.Add('build_root = '+(NinjaPath $build)); $g.Add('cflags = '+$flags); $g.Add('')
$g.Add('rule cc'); $g.Add('  command = cmd.exe /d /s /c call '+(NinjaPath $envFile)+' cl.exe $cflags /Fo$out $in'); $g.Add('  deps = msvc'); $g.Add('  msvc_deps_prefix = Note: including file:')
$g.Add('rule link'); $g.Add('  command = cmd.exe /d /s /c cd /d $build_root && call '+(NinjaPath $envFile)+' link.exe /nologo /OUT:$out @$out.rsp')
$g.Add('rule run'); $g.Add('  command = $in'); $g.Add('  description = RUN $in')
$objects = [Collections.Generic.List[string]]::new(); foreach ($source in $sources) { $object = 'obj/'+(ObjectName $source); $g.Add('build '+$object+': cc '+(NinjaPath (Join-Path $root $source))); $objects.Add($object) }
$outputs = [Collections.Generic.List[string]]::new(); foreach ($fixture in $fixtures) { $object = 'obj/'+(ObjectName $fixture); $output = 'bin/'+([IO.Path]::GetFileNameWithoutExtension($fixture))+'.exe'; $g.Add('build '+$object+': cc '+(NinjaPath (Join-Path $root $fixture))); [IO.File]::WriteAllLines((Join-Path $build ($output+'.rsp')), @($object)+@($objects)+@('kernel32.lib'), [Text.UTF8Encoding]::new($false)); $g.Add('build '+$output+': link '+$object+' '+($objects -join ' ')); $g.Add('build test-'+([IO.Path]::GetFileNameWithoutExtension($fixture))+': run '+$output); $outputs.Add('test-'+([IO.Path]::GetFileNameWithoutExtension($fixture)))}
$g.Add('build test: phony '+($outputs -join ' ')); $g.Add('default test')
[IO.File]::WriteAllLines((Join-Path $build 'build.ninja'), $g, [Text.UTF8Encoding]::new($false))
Write-Host "Generated T308 S1 monitor control graph: $build"
