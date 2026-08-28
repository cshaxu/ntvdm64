[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)] [ValidateSet('x86', 'x64')] [string]$Architecture,
    [string]$RepositoryRoot = ''
)
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
function NinjaPath([string]$Path) { $v = $Path.Replace('\', '/'); if ($v.Length -ge 2 -and $v[1] -eq ':') { return $v.Substring(0, 1) + '$' + ':' + $v.Substring(2) }; return $v }
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot) }
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = Join-Path $root ("build/M0-T309/S1/{0}" -f $Architecture)
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (!(Test-Path $vs) -or !(Get-Command ninja -ErrorAction SilentlyContinue)) { throw 'Missing MSVC Build Tools or Ninja.' }
$source = 'src/mvdm-host-overlay/softpc.new/host/src/nt_bop_dem_command.c'
$fixture = 'tests/mvdm-host/nt_bop_dem_command_subset_fixture.c'
foreach ($path in @($source, $fixture)) { if (!(Test-Path (Join-Path $root $path)) -or $path -match '(^|/)src\.old(/|$)') { throw "Invalid T309 input: $path" } }
New-Item -ItemType Directory -Force $build, (Join-Path $build 'obj'), (Join-Path $build 'bin') | Out-Null
$envFile = Join-Path $build 'msvc-mt.cmd'
@('@echo off', 'set "MVDM_T309_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ready', ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'), 'if errorlevel 1 exit /b %errorlevel%', ':ready', 'cd /d "%MVDM_T309_CALLER_CWD%"', '%*') | Set-Content $envFile -Encoding ascii
$includes = @('src/adapter-mvdm-host-out/win32/include','src/adapter-mvdm-host-out/monitor/include','src/adapter-mvdm-host-out/softpc/include','src/mvdm-support/inc') | ForEach-Object { '/I "' + (NinjaPath (Join-Path $root $_)) + '"' }
$flags = '/nologo /TC /c /std:c11 /MT /W4 /WX /showIncludes /DWIN_32 /Di386 /DNTVDM ' + ($includes -join ' ')
$g = [Collections.Generic.List[string]]::new(); $g.Add('ninja_required_version = 1.10'); $g.Add('build_root = '+(NinjaPath $build)); $g.Add('cflags = '+$flags); $g.Add('')
$g.Add('rule cc'); $g.Add('  command = cmd.exe /d /s /c call '+(NinjaPath $envFile)+' cl.exe $cflags /Fo$out $in'); $g.Add('  deps = msvc'); $g.Add('  msvc_deps_prefix = Note: including file:')
$g.Add('rule link'); $g.Add('  command = cmd.exe /d /s /c cd /d $build_root && call '+(NinjaPath $envFile)+' link.exe /nologo /OUT:$out @$out.rsp')
$g.Add('rule run'); $g.Add('  command = $in')
$g.Add('build obj/nt_bop_subset.obj: cc '+(NinjaPath (Join-Path $root $source))); $g.Add('build obj/fixture.obj: cc '+(NinjaPath (Join-Path $root $fixture)))
$out = 'bin/nt-bop-dem-command-subset-fixture.exe'; [IO.File]::WriteAllLines((Join-Path $build ($out+'.rsp')), @('obj/nt_bop_subset.obj','obj/fixture.obj','kernel32.lib'), [Text.UTF8Encoding]::new($false)); $g.Add('build '+$out+': link obj/nt_bop_subset.obj obj/fixture.obj'); $g.Add('build test: run '+$out); $g.Add('default test')
[IO.File]::WriteAllLines((Join-Path $build 'build.ninja'), $g, [Text.UTF8Encoding]::new($false)); Write-Host "Generated T309 S1 ingress graph: $build"
