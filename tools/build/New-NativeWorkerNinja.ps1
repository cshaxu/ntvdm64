[CmdletBinding()]
param(
    [Parameter(Mandatory=$true)][ValidateSet('x86','x64')][string]$Architecture,
    [Parameter(Mandatory=$true)][string]$BuildRoot,
    [string]$RepositoryRoot='',
    [string]$NinjaExecutable=''
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
if(!$RepositoryRoot) { $RepositoryRoot=Split-Path -Parent (Split-Path -Parent $PSScriptRoot) }
$root=(Resolve-Path -LiteralPath $RepositoryRoot).Path
$build=[IO.Path]::GetFullPath($BuildRoot)
if(!$build.StartsWith((Join-Path $root 'build')+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)) {
    throw 'Native worker outputs must be inside the repository build directory.'
}
function NinjaPath([string]$value) { return $value.Replace('\','/').Replace(':','$:') }
if(!$NinjaExecutable) {
    $NinjaExecutable=Get-Command ninja.exe -All | Where-Object Source -match 'Ninja-build\.Ninja' |
        Select-Object -First 1 -ExpandProperty Source
}
$vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if(!(Test-Path -LiteralPath $vs) -or !(Test-Path -LiteralPath $NinjaExecutable)) { throw 'MSVC and Ninja required.' }
New-Item -ItemType Directory -Path $build -Force | Out-Null
$font=Join-Path $build 'frontend-font/native_pc_font.h'
& (Join-Path $PSScriptRoot 'Generate-FrontendFont.ps1') -OutputFile $font
$include=@('src','src/ntsrv-exe/opennt/include','src/opennt-abi/host-compat/include',
    'src/opennt-host/public/sdk/inc','src/opennt-abi/source/public/sdk/inc',
    'src/opennt-abi/source/public/internal/base/inc','src/opennt-host/base/win32/inc',
    'src/opennt-host/base/win32/server','src/opennt-abi/source/public/internal/windows/inc',
    'src/opennt-abi/source/public/internal/ds/inc','src/opennt-abi/source/private/inc') |
    ForEach-Object { '/I "'+(NinjaPath (Join-Path $root $_))+'"' }
$flags='/nologo /c /MT /O2 /Gy /W4 /we4013 /showIncludes /D_NO_CRT_STDIO_INLINE '+
    ($include -join ' ')+' /I obj/rpc /I "'+(NinjaPath (Split-Path $font))+'"'
$graph=[Collections.Generic.List[string]]::new()
$graph.Add('cflags = '+$flags)
$graph.Add('rule cc')
$graph.Add('  command = cl.exe $cflags /Fo$out $in')
$graph.Add('  description = CC $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file:')
$graph.Add('rule midl')
$graph.Add('  command = midl.exe /nologo /env '+$(if($Architecture -eq 'x64'){'x64'}else{'win32'})+
    ' /target NT100 /client stub /server none /prefix client Client_ /prefix server Server_ /acf "'+
    (NinjaPath (Join-Path $root 'src/common/protocol/service.acf'))+'" /out obj/rpc /h service.h /cstub service_c.c $in')
$graph.Add('build obj/rpc/service.h obj/rpc/service_c.c: midl '+(NinjaPath (Join-Path $root 'src/common/protocol/service.idl')))
$graph.Add('rule link')
$graph.Add('  command = link.exe /nologo /opt:ref /manifest:embed /manifestuac:"level=''asInvoker'' uiAccess=''false''" /subsystem:console /out:$out /map:$out.map $in kernel32.lib ntdll.lib rpcrt4.lib user32.lib advapi32.lib shell32.lib gdi32.lib legacy_stdio_definitions.lib')
# One native execution source set for both machine profiles; no guest MVDM,
# original CSR/VDM execution body, x86 assembly or opposite-width archive.
$sources=@(
    'src/ntvwm-exe/main.c','src/ntvwm-exe/next_command.c','src/ntvwm-exe/execution.c',
    'src/ntvwm-exe/presentation.c','src/ntvwm-exe/text_frame.c','src/ntvwm-exe/console_state.c',
    'src/worker-base/connection.c','src/worker-base/shutdown_close.c',
    'src/worker-base/publication.c','src/worker-base/input_watch.c',
    'src/common/transport/pipe_transfer.c','src/common/console/client.c','src/common/console/members.c',
    'src/common/rpc/local_binding.c','src/common/rpc/native_command.c','src/common/rpc/frontend_control.c',
    'src/common/rpc/worker_control.c','src/common/rpc/management.c','src/common/codec/native_launch.c',
    'src/common/system_root.c','src/common/guest_environment.c','src/common/image_classification.c',
    'src/run16-exe/native_launch.c','src/ntsrv-exe/opennt/source/base_rpc_client.c',
    'src/ntsrv-exe/opennt/source/base_command.c','src/ntsrv-exe/opennt/source/base_values.c',
    'src/ntsrv-exe/opennt/source/base_payload.c','src/ntsrv-exe/opennt/source/base_startup.c',
    'src/ntsrv-exe/opennt/source/base_client_process.c','src/ntsrv-exe/transport/rpc_security.c',
    'src/ntsrv-exe/transport/vdm_message.c','src/ntsrv-exe/transport/vdm_payload.c',
    'src/opennt-abi/host-compat/opennt_support_rtl.c',
    'src/nthook32-dll/context.cpp','src/nthook32-dll/installer.cpp',
    'src/nthook32-dll/detours/detours.cpp','src/nthook32-dll/detours/modules.cpp',
    'src/nthook32-dll/detours/disasm.cpp','src/nthook32-dll/detours/image.cpp',
    'src/nthook32-dll/detours/creatwth.cpp')
$objects=[Collections.Generic.List[string]]::new()
$manifest=@()
foreach($source in $sources) {
    $object='obj/'+$source.Substring(4).Replace('.cpp','.obj').Replace('.c','.obj')
    $objects.Add($object)
    $graph.Add('build '+$object+': cc '+(NinjaPath (Join-Path $root $source))+' | obj/rpc/service.h')
    if($source.EndsWith('.cpp')) { $graph.Add('  cflags = '+$flags+' /std:c++14 /DWIN32_LEAN_AND_MEAN /D_WIN32_WINNT=0x0601') }
    elseif($source -match 'src/ntsrv-exe/opennt/source/base_') {
        $nativeFlags=$flags+' /DWIN32 /DWINNT /FI "'+
            (NinjaPath (Join-Path $root 'src/opennt-abi/host-compat/include/nt.h'))+'"'
        if($source -match 'base_(command|values|payload)\.c$') {
            $nativeFlags+=' /D_CSRSRV_ /DOPENNT_BASE_VDM_SERVER /FI "'+
                (NinjaPath (Join-Path $root 'src/ntsrv-exe/opennt/include/base_server.h'))+'"'
        }
        $graph.Add('  cflags = '+$nativeFlags)
    }
    $manifest+=@{source=$source;object=$object;sha256=(Get-FileHash -LiteralPath (Join-Path $root $source) -Algorithm SHA256).Hash}
}
$graph.Add('build obj/rpc/service_c.obj: cc obj/rpc/service_c.c | obj/rpc/service.h')
$objects.Add('obj/rpc/service_c.obj')
$target=if($Architecture -eq 'x64'){'ntvwm64.exe'}else{'ntvwm32.exe'}
$graph.Add('build '+$target+': link '+($objects -join ' '))
# Hook and worker share the installer/context objects; classifier selection is
# confined to this original source slice, never the worker's architecture.
$hookSources=@('src/nthook32-dll/entry.cpp','src/nthook32-dll/create_process.cpp',
    'src/run16-exe/hook_classification.c','src/run16-exe/image_classification.c',
    'src/common/application_search.c','src/opennt-host/base/win32/client/vdm.c',
    'src/ntsrv-exe/opennt/source/base_classifier_path.c','src/opennt-host/base/ntos/rtl/error.c')
$hookObjects=@('obj/common/image_classification.obj','obj/opennt-abi/host-compat/opennt_support_rtl.obj',
    'obj/nthook32-dll/context.obj','obj/nthook32-dll/installer.obj')+
    @('detours','modules','disasm','image','creatwth' | ForEach-Object { 'obj/nthook32-dll/detours/'+$_+'.obj' })
foreach($source in $hookSources) {
    $object='obj/'+$source.Substring(4).Replace('.cpp','.obj').Replace('.c','.obj')
    $objects.Add($object);$hookObjects+=$object
    $manifest+=@{source=$source;object=$object;sha256=(Get-FileHash -LiteralPath (Join-Path $root $source) -Algorithm SHA256).Hash}
    $graph.Add('build '+$object+': cc '+(NinjaPath (Join-Path $root $source)))
    if($source.EndsWith('.cpp')) { $graph.Add('  cflags = '+$flags+' /std:c++14 /DWIN32_LEAN_AND_MEAN /D_WIN32_WINNT=0x0601') }
    elseif($source -match '(vdm|hook_classification|base_classifier_path|error)\.c$') {
        $classifierFlags=$flags+' /DWIN32 /DWINNT /DOPENNT_BASE_CLIENT_CLASSIFIER /FI "'+
            (NinjaPath (Join-Path $root 'src/opennt-abi/host-compat/include/nt.h'))+'"'
        if($source.EndsWith('/vdm.c')) { $classifierFlags+=' /D_X86_' }
        if($source.EndsWith('/error.c')) { $classifierFlags+=' /Gz /DNTOS_KERNEL_RUNTIME' }
        $graph.Add('  cflags = '+$classifierFlags)
    }
}
$hookTarget=if($Architecture -eq 'x64'){'nthook64.dll'}else{'nthook32.dll'}
$graph.Add('rule hook_link')
$graph.Add('  command = link.exe /nologo /dll /opt:ref /out:$out /map:$out.map /export:DetourFinishHelperProcess,@1 /export:NthookAnchor /export:NthookContextFlags $in kernel32.lib ntdll.lib advapi32.lib legacy_stdio_definitions.lib')
$graph.Add('build '+$hookTarget+': hook_link '+($hookObjects -join ' '))
$fixture='obj/tests/nthook_install.obj'
$objects.Add($fixture)
$graph.Add('build '+$fixture+': cc '+(NinjaPath (Join-Path $root 'tests/component-integration/nthook_install_test.cpp')))
$graph.Add('  cflags = '+$flags+' /std:c++14 /DWIN32_LEAN_AND_MEAN /D_WIN32_WINNT=0x0601')
$fixtureInputs=$fixture+' obj/common/image_classification.obj obj/nthook32-dll/context.obj obj/nthook32-dll/installer.obj '+
    (@('detours','modules','disasm','image','creatwth' | ForEach-Object { 'obj/nthook32-dll/detours/'+$_+'.obj' }) -join ' ')
$graph.Add('rule gui_fixture_link')
$graph.Add('  command = link.exe /nologo /opt:ref /manifest:embed /manifestuac:"level=''asInvoker'' uiAccess=''false''" /subsystem:windows /entry:wmainCRTStartup /out:$out $in kernel32.lib ntdll.lib legacy_stdio_definitions.lib')
$graph.Add('build nthook-install-test.exe: link '+$fixtureInputs+' || '+$hookTarget+' nthook-gui-test.exe')
$graph.Add('build nthook-gui-test.exe: gui_fixture_link '+$fixtureInputs)
$testObject='obj/tests/native_machine.obj'
$graph.Add('build '+$testObject+': cc '+(NinjaPath (Join-Path $root 'tests/component-integration/native_machine_test.c')))
$graph.Add('build native-machine-test.exe: link '+$testObject+' obj/common/image_classification.obj')
$objects.Add($testObject)
foreach($test in @(
    @('application-search','tests/app/application_search_test.c','obj/common/application_search.obj'),
    @('image-classification','tests/observation/run16_image_classification_test.c','obj/run16-exe/image_classification.obj obj/common/image_classification.obj'))) {
    $object='obj/tests/'+$test[0]+'.obj';$objects.Add($object)
    $graph.Add('build '+$object+': cc '+(NinjaPath (Join-Path $root $test[1])))
    $graph.Add('build '+$test[0]+'-test.exe: link '+$object+' '+$test[2])
}
$graph.Add('default '+$target)
foreach($object in $objects) { New-Item -ItemType Directory -Path (Join-Path $build (Split-Path $object)) -Force | Out-Null }
[IO.File]::WriteAllLines((Join-Path $build 'build.ninja'),$graph,[Text.UTF8Encoding]::new($false))
$runner=@('@echo off',('call "'+$vs+'" -arch='+$Architecture+' -host_arch=x64 -vcvars_ver=14.43 >nul'),
    'if errorlevel 1 exit /b %errorlevel%',('"'+$NinjaExecutable+'" -C "'+$build+'" -j 12 %*'))
[IO.File]::WriteAllLines((Join-Path $build 'run-ninja.cmd'),$runner,[Text.UTF8Encoding]::new($false))
$record=@{architecture=$Architecture;target=$target;hook=$hookTarget;compiler='MSVC14.43';runtime='/MT';sources=$manifest;
    boundary='shared worker/Hook sources; same-width installation and context-only x86 launcher; opposite-width interception remains S4'}
[IO.File]::WriteAllText((Join-Path $build 'source-manifest.json'),($record | ConvertTo-Json -Depth 5),[Text.UTF8Encoding]::new($false))
Write-Output "Generated isolated $Architecture native worker: $build"
