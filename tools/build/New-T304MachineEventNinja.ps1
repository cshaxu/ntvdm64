[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)] [ValidateSet('x86', 'x64')] [string]$Architecture,
    [string]$RepositoryRoot = ''
)
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
function NinjaPath([string]$Path) { $v = $Path.Replace('\', '/'); if ($v.Length -ge 2 -and $v[1] -eq ':') { return $v.Substring(0, 1) + '$:' + $v.Substring(2) }; return $v }
function Hash([string]$Path) { return (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant() }
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot) }
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = Join-Path $root ("build/M0-T304/S2/machine-event-{0}" -f $Architecture)
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (!(Test-Path -LiteralPath $vs -PathType Leaf) -or !(Get-Command ninja -ErrorAction SilentlyContinue)) { throw 'Missing MSVC or Ninja.' }
$facade = 'src/adapter-bochs/machine_facade.cc'
$transportCxx = 'src/adapter-mvdm-host-in/generic_ud_bridge.cc'
$transportC = 'src/adapter-mvdm-host-in/bop_ingress.c'
$fixture = 'tests/adapter-mvdm-host-in/m0_t304_s2_machine_event_abi_fixture.c'
foreach ($path in @($facade, $transportCxx, $transportC, $fixture)) { if (!(Test-Path -LiteralPath (Join-Path $root $path) -PathType Leaf) -or $path -match '(^|/)src\.old(/|$)') { throw "Invalid T304 input: $path" } }
New-Item -ItemType Directory -Force $build, (Join-Path $build 'obj'), (Join-Path $build 'bin') | Out-Null
$envFile = Join-Path $build 'msvc-mt.cmd'
@('@echo off', 'set "MVDM_T304_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ready', ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'), 'if errorlevel 1 exit /b %errorlevel%', ':ready', 'cd /d "%MVDM_T304_CALLER_CWD%"', '%*') | Set-Content -LiteralPath $envFile -Encoding ascii
$manifest = [ordered]@{schema='m0.t304.s2.machine-event.v2';architecture=$Architecture;runtimeLibrary='/MT';inputs=@($facade,$transportCxx,$transportC|ForEach-Object{[ordered]@{path=$_;sha256=Hash (Join-Path $root $_)}});fixture=[ordered]@{path=$fixture;sha256=Hash (Join-Path $root $fixture)};forbiddenInputs=@('src.old','SoftPC/CCPU executor','BOP provider','prebuilt Bochs archive')}
$manifest | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $build 'source-manifest.json') -Encoding utf8
$cflags = '/nologo /TC /c /std:c11 /MT /W4 /WX /showIncludes /I "' + (NinjaPath (Join-Path $root 'src')) + '" /I "' + (NinjaPath (Join-Path $root 'src/adapter-mvdm-host-in')) + '"'
$cxxflags = '/nologo /TP /c /std:c++14 /EHsc /MT /W4 /showIncludes /DWIN32 /I "' + (NinjaPath (Join-Path $root 'src')) + '" /I "' + (NinjaPath (Join-Path $root 'src/bochs-core')) + '" /I "' + (NinjaPath (Join-Path $root 'src/bochs-core/cpu')) + '" /I "' + (NinjaPath (Join-Path $root 'src/bochs-core/iodev')) + '" /I "' + (NinjaPath (Join-Path $root 'src/bochs-core/instrument/stubs')) + '" /I "' + (NinjaPath (Join-Path $root 'src/adapter-bochs')) + '" /I "' + (NinjaPath (Join-Path $root 'src/adapter-mvdm-host-in')) + '"'
$g = [Collections.Generic.List[string]]::new(); $g.Add('ninja_required_version = 1.10'); $g.Add('build_root = '+(NinjaPath $build)); $g.Add('cflags = '+$cflags); $g.Add('cxxflags = '+$cxxflags); $g.Add('')
$g.Add('rule cc'); $g.Add('  command = cmd.exe /d /s /c call '+(NinjaPath $envFile)+' cl.exe $cflags /Fo$out $in'); $g.Add('  deps = msvc'); $g.Add('  msvc_deps_prefix = Note: including file:')
$g.Add('rule cxx'); $g.Add('  command = cmd.exe /d /s /c call '+(NinjaPath $envFile)+' cl.exe $cxxflags /Fo$out $in'); $g.Add('  deps = msvc'); $g.Add('  msvc_deps_prefix = Note: including file:')
$g.Add('rule link'); $g.Add('  command = cmd.exe /d /s /c cd /d $build_root && call '+(NinjaPath $envFile)+' link.exe /nologo /OUT:$out @$out.rsp')
$g.Add('rule run'); $g.Add('  command = $in')
$facadeObject = 'obj/machine_facade.obj'; $transportCxxObject = 'obj/generic_ud_bridge.obj'; $transportCObject = 'obj/bop_ingress.obj'; $fixtureObject = 'obj/machine_event_abi_fixture.obj'
$facadeInput = NinjaPath (Join-Path $root $facade)
$transportCxxInput = NinjaPath (Join-Path $root $transportCxx)
$transportCInput = NinjaPath (Join-Path $root $transportC)
$fixtureInput = NinjaPath (Join-Path $root $fixture)
$g.Add('build '+$facadeObject+': cxx '+$facadeInput)
$g.Add('build '+$transportCxxObject+': cxx '+$transportCxxInput)
$g.Add('build '+$transportCObject+': cc '+$transportCInput)
$g.Add('build '+$fixtureObject+': cc '+$fixtureInput)
$out = 'bin/m0-t304-s2-machine-event-abi-fixture.exe'; [IO.File]::WriteAllLines((Join-Path $build ($out+'.rsp')),@($fixtureObject,$transportCObject,$transportCxxObject,'kernel32.lib'),[Text.UTF8Encoding]::new($false)); $g.Add('build '+$out+': link '+$fixtureObject+' '+$transportCObject+' '+$transportCxxObject); $g.Add('build test: run '+$out+' | '+$facadeObject); $g.Add('default test')
[IO.File]::WriteAllLines((Join-Path $build 'build.ninja'),$g,[Text.UTF8Encoding]::new($false)); Write-Output "Generated T304 S2 minimal machine-event graph: $build"
