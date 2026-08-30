[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)] [ValidateSet('x86', 'x64')] [string]$Architecture,
    [string]$RepositoryRoot = ''
)
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
function NinjaPath([string]$Path) { $v = $Path.Replace('\', '/'); if ($v.Length -ge 2 -and $v[1] -eq ':') { return $v.Substring(0, 1) + '$:' + $v.Substring(2) }; return $v }
function ObjectName([string]$Path) { return (($Path -replace '[^A-Za-z0-9_]', '_') + '.obj') }
function Hash([string]$Path) { return (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant() }
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot) }
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = Join-Path $root ("build/M0-T303/S2/typed-resume-{0}" -f $Architecture)
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (!(Test-Path -LiteralPath $vs -PathType Leaf) -or !(Get-Command ninja -ErrorAction SilentlyContinue)) { throw 'Missing MSVC or Ninja.' }
$core = @(Get-ChildItem -LiteralPath (Join-Path $root 'src/bochs-core') -Recurse -File | Where-Object { $_.Extension -in @('.c', '.cc') } | ForEach-Object { $_.FullName.Substring($root.Length + 1).Replace('\', '/') } | Sort-Object)
$overlay = @(Get-ChildItem -LiteralPath (Join-Path $root 'src/bochs-core-overlay') -Recurse -File | Where-Object { $_.Extension -in @('.c', '.cc') } | ForEach-Object { $_.FullName.Substring($root.Length + 1).Replace('\', '/') } | Sort-Object)
$adapter = @('src/adapter-bochs/headless_8042.cc', 'src/adapter-bochs/machine_facade.cc', 'src/adapter-bochs/machine_lifecycle.cc', 'src/adapter-bochs/minimal_machine.cc', 'src/adapter-bochs/minimal_pic.cc', 'src/adapter-bochs/minimal_sim.cc')
$c = @('src/session/mapping_manager.c', 'src/session/guest_memory_lease.c', 'src/session/session.c', 'src/adapter-mvdm-host-out/softpc/mvdm_command_registers.c', 'src/adapter-mvdm-host-out/softpc/mvdm_guest_location.c', 'src/adapter-mvdm-host-out/softpc/mvdm_protected_frame_transaction.c', 'src/adapter-mvdm-host-out/softpc/mvdm_vdm_stack.c')
$fixture = 'tests/adapter-bochs/m0_t303_s2_typed_resume_fixture.cc'
foreach ($path in @($core + $overlay + $adapter + $c + $fixture)) { if (!(Test-Path -LiteralPath (Join-Path $root $path) -PathType Leaf) -or $path -match '(^|/)src\.old(/|$)') { throw "Invalid T303 input: $path" } }
New-Item -ItemType Directory -Force $build, (Join-Path $build 'obj'), (Join-Path $build 'lib'), (Join-Path $build 'bin') | Out-Null
$envFile = Join-Path $build 'msvc-mt.cmd'
@('@echo off', 'set "MVDM_T303_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ready', ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'), 'if errorlevel 1 exit /b %errorlevel%', ':ready', 'cd /d "%MVDM_T303_CALLER_CWD%"', '%*') | Set-Content -LiteralPath $envFile -Encoding ascii
$manifest = [ordered]@{schema='m0.t303.s2.typed-resume.v1';architecture=$Architecture;runtimeLibrary='/MT';core=@($core+$overlay|ForEach-Object{[ordered]@{path=$_;sha256=Hash (Join-Path $root $_)}});adapter=@($adapter+$c|ForEach-Object{[ordered]@{path=$_;sha256=Hash (Join-Path $root $_)}});fixture=[ordered]@{path=$fixture;sha256=Hash (Join-Path $root $fixture)};forbiddenInputs=@('src.old','SoftPC/CCPU executor','BOP ingress','prebuilt Bochs archive')}
$manifest | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath (Join-Path $build 'source-manifest.json') -Encoding utf8
$ci = @('src','src/session','src/adapter-mvdm-host-out/softpc/include','src/adapter-bochs') | ForEach-Object { '/I "' + (NinjaPath (Join-Path $root $_)) + '"' }
$cxxi = @('src','src/session','src/bochs-core','src/bochs-core/cpu','src/bochs-core/iodev','src/bochs-core/instrument/stubs','src/adapter-bochs','src/adapter-mvdm-host-out/softpc/include') | ForEach-Object { '/I "' + (NinjaPath (Join-Path $root $_)) + '"' }
$cf = '/nologo /TC /c /std:c11 /MT /W4 /WX /showIncludes ' + ($ci -join ' ')
$cxxf = '/nologo /TP /c /std:c++14 /EHsc /MT /W4 /showIncludes /DWIN32 /DRUNTIME_ENABLE_MACHINE_UD_BRIDGE=1 ' + ($cxxi -join ' ')
$g = [Collections.Generic.List[string]]::new(); $g.Add('ninja_required_version = 1.10'); $g.Add('build_root = '+(NinjaPath $build)); $g.Add('cflags = '+$cf); $g.Add('cxxflags = '+$cxxf); $g.Add('')
$g.Add('rule cc'); $g.Add('  command = cmd.exe /d /s /c call '+(NinjaPath $envFile)+' cl.exe $cflags /Fo$out $in'); $g.Add('  deps = msvc'); $g.Add('  msvc_deps_prefix = Note: including file:')
$g.Add('rule cxx'); $g.Add('  command = cmd.exe /d /s /c call '+(NinjaPath $envFile)+' cl.exe $cxxflags /Fo$out $in'); $g.Add('  deps = msvc'); $g.Add('  msvc_deps_prefix = Note: including file:')
$g.Add('rule lib'); $g.Add('  command = cmd.exe /d /s /c cd /d $build_root && call '+(NinjaPath $envFile)+' lib.exe /nologo /OUT:$out @$out.rsp')
$g.Add('rule link'); $g.Add('  command = cmd.exe /d /s /c cd /d $build_root && call '+(NinjaPath $envFile)+' link.exe /nologo /OUT:$out @$out.rsp')
$g.Add('rule run'); $g.Add('  command = $in')
function AddLibrary([string]$name, [string[]]$sources, [string]$rule) { $objects=[Collections.Generic.List[string]]::new(); foreach($source in $sources){$object='obj/'+$name+'/'+(ObjectName $source);$g.Add('build '+$object+': '+$rule+' '+(NinjaPath (Join-Path $root $source)));$objects.Add($object)};$library='lib/'+$name+'.lib';[IO.File]::WriteAllLines((Join-Path $build ($library+'.rsp')),$objects,[Text.UTF8Encoding]::new($false));$g.Add('build '+$library+': lib '+($objects -join ' '));return $library }
$corelib=AddLibrary 'bochs-core' @($core+$overlay) 'cxx'; $adapterlib=AddLibrary 'adapter-bochs' $adapter 'cxx'; $softpclib=AddLibrary 'adapter-softpc' $c 'cc'
$fixtureObject='obj/fixture/'+(ObjectName $fixture);$g.Add('build '+$fixtureObject+': cxx '+(NinjaPath (Join-Path $root $fixture)));$out='bin/m0-t303-s2-typed-resume-fixture.exe';[IO.File]::WriteAllLines((Join-Path $build ($out+'.rsp')),@($fixtureObject,$softpclib,$adapterlib,$corelib,'kernel32.lib','user32.lib'),[Text.UTF8Encoding]::new($false));$g.Add('build '+$out+': link '+$fixtureObject+' '+$softpclib+' '+$adapterlib+' '+$corelib);$g.Add('build test: run '+$out);$g.Add('default '+$out)
[IO.File]::WriteAllLines((Join-Path $build 'build.ninja'),$g,[Text.UTF8Encoding]::new($false));Write-Output "Generated T303 S2 typed-resume graph: $build"
