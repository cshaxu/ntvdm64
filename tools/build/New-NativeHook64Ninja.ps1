[CmdletBinding()]
param(
    [Parameter(Mandatory=$true)][string]$BuildRoot,
    [Parameter(Mandatory=$true)][string]$ReferenceGraph,
    [string]$RepositoryRoot=(Get-Location).Path
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$build=[IO.Path]::GetFullPath((Join-Path $root $BuildRoot))
if(-not $build.StartsWith((Join-Path $root 'build')+'\',[StringComparison]::OrdinalIgnoreCase)) {
    throw 'Hook build must remain below repository build/'
}
New-Item -ItemType Directory -Path $build -Force | Out-Null
New-Item -ItemType Directory -Path (Join-Path $build 'obj/basesrv') -Force | Out-Null
function NP([string]$p) { $p.Replace('\','/').Replace(':','$:') }
$reference=[IO.File]::ReadAllText([IO.Path]::GetFullPath($ReferenceGraph))
$referenceFlags=[regex]::Match($reference,'(?m)^cflags = (.*)$').Groups[1].Value
if(-not $referenceFlags) { throw 'Missing reference include graph' }
# Reuse only native include closure, never x86 objects/defines/worker targets.
$includes=@([regex]::Matches($referenceFlags,'/I "[^"]+"') | ForEach-Object { $_.Value })
$includes += '/I "'+(NP (Join-Path $root 'src/ntsrv-exe/opennt/include'))+'"'
$flags='/nologo /c /MT /W4 /Gy /showIncludes /DWIN32 /D_WIN32_WINNT=0x0A00 /I obj/basesrv '+($includes -join ' ')
$graph=[Collections.Generic.List[string]]::new()
$graph.Add('ninja_required_version = 1.10')
$graph.Add('cflags = '+$flags)
$graph.Add('rule cc')
$graph.Add('  command = cl.exe $cflags /Fo$out $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file: ')
$graph.Add('rule dll')
$graph.Add('  command = link.exe /nologo /dll /opt:ref /out:$out /map:$out.map /implib:nthook64-import.lib /def:"'+(NP (Join-Path $root 'src/nthook32-dll/nthook64.def'))+'" $in kernel32.lib ntdll.lib advapi32.lib rpcrt4.lib legacy_stdio_definitions.lib')
$graph.Add('rule idl')
$graph.Add('  command = midl.exe /nologo /env x64 /target NT100 /prefix client Client_ /prefix server Server_ /acf "'+(NP (Join-Path $root 'src/common/protocol/service.acf'))+'" /out obj/basesrv /h service.h /cstub service_c.c /sstub service_s.c "'+(NP (Join-Path $root 'src/common/protocol/service.idl'))+'"')
$graph.Add('build obj/basesrv/service.h obj/basesrv/service_c.c obj/basesrv/service_s.c: idl '+(NP (Join-Path $root 'src/common/protocol/service.idl'))+' | '+(NP (Join-Path $root 'src/common/protocol/service.acf')))
$graph.Add('build observation-stub.obj: cc obj/basesrv/service_c.c | obj/basesrv/service.h')
$sources=@('src/nthook32-dll/entry.cpp','src/nthook32-dll/context.cpp',
    'src/nthook32-dll/installer.cpp','src/nthook32-dll/create_process.cpp',
    'src/common/application_search.c','src/common/native_image.c',
    'src/run16-exe/hook_classification.c',
    'src/opennt-host/base/win32/client/vdm.c',
    'src/ntsrv-exe/opennt/source/base_classifier_path.c',
    'src/opennt-abi/host-compat/opennt_support_rtl.c',
    'src/opennt-host/base/ntos/rtl/error.c')
foreach($name in @('detours','modules','disasm','image','creatwth')) {
    $sources += 'src/nthook32-dll/detours/'+$name+'.cpp'
}
$sources+=@('src/common/rpc/local_binding.c','src/common/rpc/management.c','src/ntsrv-exe/transport/rpc_security.c')
$objects=@();$manifest=@();$i=0
foreach($source in $sources) {
    $object='unit'+$i+'.obj';++$i;$objects+=$object
    $graph.Add('build '+$object+': cc '+(NP (Join-Path $root $source))+' | obj/basesrv/service.h')
    if($source -eq 'src/opennt-host/base/win32/client/vdm.c') {
        $graph.Add('  cflags = $cflags /DOPENNT_BASE_CLIENT_CLASSIFIER /FI"'+(NP (Join-Path $root 'src/nthook32-dll/legacy_classifier_arch.h'))+'"')
    }
    if($source -eq 'src/opennt-host/base/ntos/rtl/error.c') {
        $graph.Add('  cflags = $cflags /DNTOS_KERNEL_RUNTIME')
    }
    $manifest += @{path=$source;sha256=(Get-FileHash (Join-Path $root $source) -Algorithm SHA256).Hash;object=$object}
}
$graph.Add('build nthook64.dll: dll '+($objects -join ' ')+' observation-stub.obj | '+(NP (Join-Path $root 'src/nthook32-dll/nthook64.def')))
$graph.Add('rule test')
$graph.Add('  command = link.exe /nologo /subsystem:$subsystem /entry:wmainCRTStartup /opt:ref /out:$out /map:$out.map $in kernel32.lib ntdll.lib advapi32.lib user32.lib legacy_stdio_definitions.lib')
$graph.Add('build fixture.obj: cc '+(NP (Join-Path $root 'tests/component-integration/nthook_install_test.cpp')))
$testObjects=@($objects | Where-Object { $_ -notin @('unit0.obj','unit3.obj','unit6.obj','unit7.obj','unit8.obj') -and [int]($_ -replace '\D','') -lt 16 })
$graph.Add('build nthook-install-test.exe: test fixture.obj '+($testObjects -join ' ')+' || nthook64.dll nthook-gui-test.exe')
$graph.Add('  subsystem = console')
$graph.Add('build nthook-gui-test.exe: test fixture.obj '+($testObjects -join ' '))
$graph.Add('  subsystem = windows')
$graph.Add('default nthook64.dll')
$utf8=[Text.UTF8Encoding]::new($false)
[IO.File]::WriteAllText((Join-Path $build 'build.ninja'),($graph -join "`r`n")+"`r`n",$utf8)
$ninja=(Get-Command ninja.exe -ErrorAction Stop).Source
$vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
[IO.File]::WriteAllText((Join-Path $build 'run-ninja.cmd'),
    '@echo off'+"`r`n"+'call "'+$vs+'" -arch=x64 -host_arch=x64 >nul'+"`r`n"+
    'if errorlevel 1 exit /b %errorlevel%'+"`r`n"+'"'+$ninja+'" -C "'+$build+'" -j 8 %*'+"`r`n",$utf8)
[IO.File]::WriteAllText((Join-Path $build 'source-manifest.json'),
    (@{architecture='AMD64';scope='Hook DLL only; no worker';sources=$manifest} | ConvertTo-Json -Depth 5),$utf8)
Write-Output "Generated Hook-only AMD64 graph: $build"
