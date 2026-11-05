[CmdletBinding()]
param(
    [Parameter(Mandatory=$true)][string]$BuildRoot,
    [Parameter(Mandatory=$true)][string]$ReferenceGraph,
    [string]$RepositoryRoot=(Get-Location).Path,
    # Explicit developer-only wrapper. The formal graph remains direct MSVC.
    [string]$CompilerCache=''
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$build=if([IO.Path]::IsPathRooted($BuildRoot)){[IO.Path]::GetFullPath($BuildRoot)}else{[IO.Path]::GetFullPath((Join-Path $root $BuildRoot))}
$compilerCommand = & "$PSScriptRoot/Initialize-NinjaTree.ps1" -BuildRoot $build -CompilerCache $CompilerCache
New-Item -ItemType Directory -Path $build -Force | Out-Null
New-Item -ItemType Directory -Path (Join-Path $build 'obj/basesrv') -Force | Out-Null
function NP([string]$p) { $p.Replace('\','/').Replace(':','$:') }
$compilerCachePath=$CompilerCache
$compilerCacheDirectory=if($CompilerCache){Join-Path $root 'build/.ccache'}else{''}
$reference=[IO.File]::ReadAllText([IO.Path]::GetFullPath($ReferenceGraph))
$referenceFlags=[regex]::Match($reference,'(?m)^cflags = (.*)$').Groups[1].Value
if(-not $referenceFlags) { throw 'Missing reference include graph' }
$referenceRun16=Join-Path (Split-Path -Parent ([IO.Path]::GetFullPath($ReferenceGraph))) 'run16.exe'
# Only the installer fixtures stage this executable. DLL-only builds have no
# launcher dependency; Ninja diagnoses a missing input if a fixture needs it.
# Reuse only native include closure, never x86 objects/defines/worker targets.
$includes=@([regex]::Matches($referenceFlags,'/I "[^"]+"') | ForEach-Object { $_.Value })
$includes += '/I "'+(NP (Join-Path $root 'src/ntsrv-exe/opennt/include'))+'"'
$flags='/nologo /c /MT /W4 /Gy /showIncludes /D_WIN32_WINNT=0x0A00 /I obj/basesrv '+($includes -join ' ')
$graph=[Collections.Generic.List[string]]::new()
$graph.Add('ninja_required_version = 1.10')
$graph.Add('release_cflags = /O2')
$graph.Add('cflags = '+$flags)
$graph.Add('rule cc')
$graph.Add('  command = '+$compilerCommand+' $cflags $release_cflags /Fo$out $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file: ')
$graph.Add('rule dll')
$graph.Add('  command = link.exe /nologo /dll /opt:ref /out:$out /map:$out.map /implib:nthook64-import.lib /def:"'+(NP (Join-Path $root 'src/nthook32-dll/nthook64.def'))+'" $in kernel32.lib user32.lib uxtheme.lib ntdll.lib advapi32.lib rpcrt4.lib legacy_stdio_definitions.lib')
$graph.Add('rule stage')
$graph.Add('  command = powershell.exe -NoProfile -ExecutionPolicy Bypass -File "'+(NP (Join-Path $root 'tools/build/Stage-NativeWorkerImage.ps1'))+'" -InputFile $in -OutputFile "$out"')
$graph.Add('build run16.exe: stage '+(NP $referenceRun16))
$graph.Add('rule idl')
$graph.Add('  command = midl.exe /nologo /env x64 /target NT100 /prefix client Client_ /prefix server Server_ /acf "'+(NP (Join-Path $root 'src/common/protocol/service.acf'))+'" /out obj/basesrv /h service.h /cstub service_c.c /sstub service_s.c "'+(NP (Join-Path $root 'src/common/protocol/service.idl'))+'"')
$graph.Add('build obj/basesrv/service.h obj/basesrv/service_c.c obj/basesrv/service_s.c: idl '+(NP (Join-Path $root 'src/common/protocol/service.idl'))+' | '+(NP (Join-Path $root 'src/common/protocol/service.acf')))
$graph.Add('build observation-stub.obj: cc obj/basesrv/service_c.c | obj/basesrv/service.h')
$sources=@('src/nthook32-dll/entry.c','src/common/native-hook/context.c','src/common/native-hook/packet.c',
    'src/common/native-hook/installer.c','src/nthook32-dll/create_process.c',
    'src/lib-win32/process-hook/staging.c','src/lib-win32/process-hook/helper.c','src/lib-win32/process-hook/payload.c',
    'src/lib-win32/process-hook/startup_hooks.c','src/lib-win32/process-hook/minhook/src/buffer.c',
    'src/lib-win32/process-hook/minhook/src/trampoline.c','src/lib-win32/process-hook/minhook/src/hde/hde64.c',
    'src/common/native_image.c','src/lib-win32/process-image/image.c','src/lib-win32/handle/duplicate.c','src/lib-win32/file/identity.c',
    'src/nthook32-dll/hook_classification.c',
    'src/non-mvdm/base/win32/client/vdm.c',
    'src/common/base_srv/base_classifier_path.c',
    'src/compat/opennt_support_rtl.c',
    'src/non-mvdm/base/ntos/rtl/error.c')
$sources+=@('src/lib-win32/rpc/local_binding.c','src/common/rpc/management.c','src/common/base_srv/scope.c')
$objects=@();$manifest=@();$i=0
foreach($source in $sources) {
    $object='unit'+$i+'.obj';++$i;$objects+=$object
    $graph.Add('build '+$object+': cc '+(NP (Join-Path $root $source))+' | obj/basesrv/service.h')
    if($source -in @('src/lib-win32/process-hook/staging.c','src/lib-win32/process-hook/helper.c','src/lib-win32/process-hook/payload.c')) { $graph.Add('  cflags = $cflags /std:c11') }
    if($source -eq 'src/lib-win32/process-hook/startup_hooks.c') { $graph.Add('  cflags = $cflags /std:c11 /FI"'+(NP (Join-Path $root 'src/compat/include/process_hook_msvc.h'))+'"') }
    if($source -eq 'src/non-mvdm/base/win32/client/vdm.c') {
        $graph.Add('  cflags = $cflags /DOPENNT_BASE_CLIENT_CLASSIFIER /FI"'+(NP (Join-Path $root 'src/common/base_srv/legacy_classifier_arch.h'))+'"')
    }
    if($source -eq 'src/non-mvdm/base/ntos/rtl/error.c') {
        $graph.Add('  cflags = $cflags /DNTOS_KERNEL_RUNTIME')
    }
    $manifest += @{path=$source;sha256=(Get-FileHash (Join-Path $root $source) -Algorithm SHA256).Hash;object=$object}
}
$graph.Add('build nthook64.dll: dll '+($objects -join ' ')+' observation-stub.obj | '+(NP (Join-Path $root 'src/nthook32-dll/nthook64.def')))
$graph.Add('rule test')
$graph.Add('  command = link.exe /nologo /subsystem:$subsystem /entry:wmainCRTStartup /opt:ref /out:$out /map:$out.map $in kernel32.lib ntdll.lib advapi32.lib user32.lib rpcrt4.lib legacy_stdio_definitions.lib')
$graph.Add('build process-image-machine-test.obj: cc '+(NP (Join-Path $root 'tests/observation/process_image_machine_test.c')))
$imageObject=$objects[[Array]::IndexOf($sources,'src/lib-win32/process-image/image.c')]
$graph.Add('build process-image-machine-test.exe: test process-image-machine-test.obj '+$imageObject)
$graph.Add('  subsystem = console')
$graph.Add('build fixture.obj: cc '+(NP (Join-Path $root 'tests/component-integration/nthook_install_test.c')))
$graph.Add('  cflags = $cflags /std:c11')
$testObjects=$objects
$graph.Add('build nthook-install-test.exe: test fixture.obj '+($testObjects -join ' ')+' observation-stub.obj || nthook64.dll nthook-gui-test.exe run16.exe')
$graph.Add('  subsystem = console')
$graph.Add('build nthook-gui-test.exe: test fixture.obj '+($testObjects -join ' ')+' observation-stub.obj')
$graph.Add('  subsystem = windows')
$graph.Add('default nthook64.dll')
$utf8=[Text.UTF8Encoding]::new($false)
[IO.File]::WriteAllText((Join-Path $build 'build.ninja'),($graph -join "`r`n")+"`r`n",$utf8)
$ninja=(Get-Command ninja.exe -ErrorAction Stop).Source
$vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$runner=[Collections.Generic.List[string]]::new()
$runner.Add('@echo off')
$runner.Add('call "'+$vs+'" -arch=x64 -host_arch=x64 >nul')
$runner.Add('if errorlevel 1 exit /b %errorlevel%')
$runner.Add('"'+$ninja+'" -C "'+$build+'" -j 8 %*')
[IO.File]::WriteAllText((Join-Path $build 'run-ninja.cmd'),($runner -join "`r`n")+"`r`n",$utf8)
$cacheMetadata=if($compilerCacheDirectory){@{enabled=$true;executable=$compilerCachePath;directory=$compilerCacheDirectory;temporaryDirectory=(Join-Path $compilerCacheDirectory 'tmp');maxSize='5G'}}
[IO.File]::WriteAllText((Join-Path $build 'source-manifest.json'),
    (@{architecture='AMD64';scope='Hook DLL only; no worker';compilerCache=$cacheMetadata;sources=$manifest} | ConvertTo-Json -Depth 5),$utf8)
Write-Output "Generated Hook-only AMD64 graph: $build"
