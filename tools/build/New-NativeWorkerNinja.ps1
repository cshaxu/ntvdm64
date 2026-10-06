[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$BuildRoot,
 [Parameter(Mandatory)][string]$ReferenceGraph,
 [ValidateSet('Worker','Frontend','Monitor','Launcher','Service')][string]$Component='Worker',
 [string]$RepositoryRoot=(Get-Location).Path
)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($RepositoryRoot)
$build=[IO.Path]::GetFullPath((Join-Path $root $BuildRoot))
if(!$build.StartsWith((Join-Path $root 'build')+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Native outputs must remain below build/'}
$reference=(Resolve-Path $ReferenceGraph).Path
$text=Get-Content $reference -Raw
function NP([string]$path){$path.Replace('\','/').Replace(':','$:')}
$baseFlags=[regex]::Match($text,'(?m)^cflags = (.*)$').Groups[1].Value
if(!$baseFlags){throw 'Missing audited reference flags'}
$includes=@([regex]::Matches($baseFlags,'/I "[^"]+"')|ForEach-Object {$_.Value})
$nativeFlags='/nologo /c /MT /W4 /we4013 /we4311 /we4302 /Gy /showIncludes /DWIN32 /DWINNT /DOPENNT_ADAPTER_NT_ALERT_THREAD /D_NO_CRT_STDIO_INLINE /D_WIN32_WINNT=0x0A00 '+($includes -join ' ')+' /I "'+(NP (Join-Path $root 'src/ntsrv-exe/opennt/include'))+'" /I obj/basesrv'
if($Component -eq 'Service'){
 $nativeFlags+=' /we4312 /FI "'+(NP (Join-Path $root 'src/opennt-abi/host-compat/include/nt.h'))+'" /I "'+(NP (Join-Path $root 'src/opennt-host/base/win32/inc'))+'" /I "'+(NP (Join-Path $root 'src/opennt-host/base/win32/server'))+'"'
}
$edges=@{}
foreach($match in [regex]::Matches($text,'(?m)^build ([^\r\n]+): ([^\r\n]+)\r?\n((?:[ \t]+[^\r\n]*\r?\n)*)')) {
 $left=$match.Groups[1].Value -split '\s+'
 $right=$match.Groups[2].Value -split '\s+'
 $rule=$right[0];$inputs=@();$implicit=$false
 foreach($part in $right[1..($right.Count-1)]) {
  if($part -eq '||'){break}
  if($part -eq '|'){$implicit=$true;continue}
  $inputs+=,$part
 }
 $edge=[pscustomobject]@{Outputs=@($left|Where-Object {$_ -ne '|'});Rule=$rule;Inputs=$inputs;Bindings=$match.Groups[3].Value}
 foreach($output in $edge.Outputs){$edges[$output]=$edge}
}
$null=New-Item -ItemType Directory -Path $build -Force
if($edges.ContainsKey('ntvwm-source-closure')){$edges['ntvwm.exe']=$edges['ntvwm-source-closure']}
if($edges.ContainsKey('ntcon-source-closure')){$edges['ntcon.exe']=$edges['ntcon-source-closure']}
if($edges.ContainsKey('ntmon-source-closure')){$edges['ntmon.exe']=$edges['ntmon-source-closure']}
if($edges.ContainsKey('run16-source-closure')){$edges['run16.exe']=$edges['run16-source-closure']}
if($edges.ContainsKey('ntsrv-source-closure')){$edges['ntsrv.exe']=$edges['ntsrv-source-closure']}
$null=New-Item -ItemType Directory -Path (Join-Path $build 'obj/basesrv') -Force
if($Component -in @('Worker','Frontend')){
 $null=New-Item -ItemType Directory -Path (Join-Path $build 'frontend-font') -Force
 & (Join-Path $root 'tools/build/Generate-FrontendFont.ps1') -OutputFile (Join-Path $build 'frontend-font/native_pc_font.h')
}
$graph=[Collections.Generic.List[string]]::new()
$graph.Add('ninja_required_version = 1.10')
$graph.Add('cflags = '+$nativeFlags)
$graph.Add('entry = wmainCRTStartup')
$graph.Add('subsystem = console')
$graph.Add('rule cc')
$graph.Add('  command = cl.exe $cflags /Fo$out $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file: ')
$graph.Add('rule lib')
$graph.Add('  command = lib.exe /nologo /out:$out $in')
$graph.Add('rule link')
$graph.Add('  command = link.exe /nologo /machine:x64 /subsystem:$subsystem /entry:$entry /opt:ref /out:$out /map:$out.map $in rpcrt4.lib ntdll.lib kernel32.lib shell32.lib user32.lib gdi32.lib advapi32.lib legacy_stdio_definitions.lib')
$graph.Add('rule test_link')
$graph.Add('  command = link.exe /nologo /machine:x64 /subsystem:console /opt:ref /out:$out /map:$out.map $in rpcrt4.lib ntdll.lib kernel32.lib shell32.lib user32.lib gdi32.lib advapi32.lib legacy_stdio_definitions.lib')
$graph.Add('rule idl')
$graph.Add('  command = midl.exe /nologo /env x64 /target NT100 /prefix client Client_ /prefix server Server_ /acf "'+(NP (Join-Path $root 'src/common/protocol/service.acf'))+'" /out obj/basesrv /h service.h /cstub service_c.c /sstub service_s.c "'+(NP (Join-Path $root 'src/common/protocol/service.idl'))+'"')
$graph.Add('build obj/basesrv/service_c.c obj/basesrv/service_s.c obj/basesrv/service.h: idl '+(NP (Join-Path $root 'src/common/protocol/service.idl'))+' | '+(NP (Join-Path $root 'src/common/protocol/service.acf')))
$done=@{};$manifest=[Collections.Generic.List[object]]::new()
# Finite client-side closure proved by the image-matched S1 ledger. Native
# /Gy can discard the whole legacy dispatch branch; do not make regeneration
# depend on whether a later optimized map happens to pull these members.
$bindingMembers=@('command.obj','payload.obj','process.obj','startup.obj','values.obj')
if($Component -eq 'Launcher'){$bindingMembers+=@('classifier-path.obj','config.obj')}
if($Component -eq 'Service'){
 # Image-matched service closure, not the client classifier/capture island.
 $bindingMembers=@('service.obj','service-core.obj','worker-registry.obj','frontend-registry.obj','native-commands.obj','service-lifecycle.obj','service-management.obj','task-observation.obj','worker-spawn.obj','command.obj','values.obj','payload.obj','startup.obj','dispatch.obj','resources.obj','streams.obj','waits.obj','registry.obj','reservation.obj','request.obj','config.obj','interactive.obj','config-command.obj','native_image.obj')
}
function Add-Edge([string]$target) {
 if($Component -ne 'Launcher' -and $target -eq 'opennt-base-client.lib'){return}
 if($Component -notin @('Worker','Launcher','Service') -and $target -eq 'original-opennt-rtl-x86.lib'){return}
 if($target -match '^obj/basesrv/service_[cs]\.c$|^obj/basesrv/service\.h$'){return}
 if($done.ContainsKey($target)){return}
 if(!$edges.ContainsKey($target)){throw "Unresolved selected build edge: $target"}
 $edge=$edges[$target];$done[$target]=$true
 $output=$target.Replace('original-opennt-rtl-x86.lib','original-opennt-rtl-native.lib')
 $inputs=@($edge.Inputs)
 if($Component -ne 'Launcher'){$inputs=@($inputs|Where-Object {$_ -ne 'opennt-base-client.lib'})}
 if($Component -notin @('Worker','Launcher','Service')){$inputs=@($inputs|Where-Object {$_ -ne 'original-opennt-rtl-x86.lib'})}
 if($target -eq 'opennt-base-bindings.lib'){
  # This archive also houses the service provider. Select only the worker's
  # actually pulled client-side members, not unused server/config bodies.
  $inputs=@($inputs|Where-Object {(Split-Path $_ -Leaf) -in $bindingMembers})
 }
 if($target -eq 'original-opennt-rtl-x86.lib'){
  # Current matched NTVWM map proves error.obj only; no original Base VDM,
  # CSR capture or RTL environment may enter this native-worker closure.
  $inputs=if($Component -eq 'Launcher'){@('obj/opennt-rtl/error.obj','obj/opennt-rtl/environ.obj')}else{@('obj/opennt-rtl/error.obj')}
 }
 if($edge.Rule -in @('lib','frontend_link','base_rpc_test_link','console_test_link','pointer_test_link','monitor_link','run16_link','broker_test_link','rtl_fixture_link','basesrv_link','basesrv_service_test_link') -or ($target -in @('ntvwm.exe','ntcon.exe','ntmon.exe','run16.exe','ntsrv.exe') -and $edge.Rule -eq 'phony')) {
  foreach($input in $inputs){Add-Edge $input}
  $rule=if($edge.Rule -eq 'lib'){'lib'}elseif($edge.Rule -in @('console_test_link','pointer_test_link','monitor_link','broker_test_link','rtl_fixture_link','basesrv_service_test_link') -or $Component -eq 'Monitor'){'test_link'}else{'link'}
  $graph.Add('build '+$output+': '+$rule+' '+(($inputs|ForEach-Object {$_.Replace('original-opennt-rtl-x86.lib','original-opennt-rtl-native.lib')}) -join ' '))
  if($target -eq 'common-worker-control-test.exe' -or $edge.Rule -in @('console_test_link','pointer_test_link')){$graph.Add('  entry = mainCRTStartup')}
  if($target -eq 'run16.exe'){$graph.Add('  entry = wWinMainCRTStartup')}
  if($target -eq 'ntsrv.exe'){$graph.Add('  entry = mainCRTStartup');$graph.Add('  subsystem = windows')}
  if($target -eq 'native-capture-test.exe'){$graph.Add('  entry = mainCRTStartup')}
  $entryOverride=[regex]::Match($edge.Bindings,'(?m)^\s+entry = (mainCRTStartup|wmainCRTStartup)\s*$').Groups[1].Value
  if($entryOverride){$graph.Add('  entry = '+$entryOverride)}
  return
 }
 if($edge.Rule -notin @('cc','cc_rtl')){throw "Unadmitted native rule: $target/$($edge.Rule)"}
 $source=$inputs[0]
 if($Component -eq 'Service' -and $target -eq 'obj/opennt-base-bindings/config-command.obj'){
  # Same verbatim fragment recipe as the formal graph; architecture-local output.
  $configSource=Join-Path $root 'src/opennt-host/base/win32/client/vdm.c'
  $configText=[IO.File]::ReadAllText($configSource)
  $helper=[regex]::Matches($configText,'(?ms)^static VOID BaseSetLastNTError\(.*?^}')
  $function=[regex]::Matches($configText,'(?ms)^BOOL\r?\nBaseGetVdmConfigInfo\(.*?^}')
  if($helper.Count -ne 1 -or $function.Count -ne 1){throw 'Original VDM configuration closure is missing or ambiguous'}
  $null=New-Item -ItemType Directory -Path (Join-Path $build 'generated') -Force
  $configCarrier=Join-Path $build 'generated/base_vdm_config.c'
  $preamble="#include <nt.h>`r`n#include <ntrtl.h>`r`n#include <wchar.h>`r`n#include <basevdm.h>`r`n#include <base_config.h>`r`n#define VDM_TAG 0`r`n#define MAKE_TAG(Tag) 0`r`n"
  [IO.File]::WriteAllText($configCarrier,$preamble+$helper[0].Value+"`r`n"+$function[0].Value+"`r`n",[Text.UTF8Encoding]::new($false))
  $source=NP $configCarrier
 }
 if($source -notmatch '\.(c|cpp)$'){throw "Not a source: $source"}
 if($source -notmatch '^O\$:/'){
  if($source -notin @('obj/basesrv/service_c.c','obj/basesrv/service_s.c')){throw "Unknown generated source: $source"}
 }
 foreach($input in $inputs|Select-Object -Skip 1){
  if($input -match '^O\$:/.*\.(c|h)$'){
   $dependency=$input.Replace('$:',':')
   if(!(Test-Path -LiteralPath $dependency -PathType Leaf)){throw "Missing source dependency: $dependency"}
   $manifest.Add(@{path=$dependency;sha256=(Get-FileHash $dependency).Hash;dependencyOf=$target})
  }else{Add-Edge $input}
 }
 $null=New-Item -ItemType Directory -Path (Join-Path $build (Split-Path $target -Parent)) -Force
 $graph.Add('build '+$target+': cc '+$source+' | obj/basesrv/service.h')
 $flags=$nativeFlags
 if($target -eq 'obj/opennt-base-server/srvvdm.obj'){
  # OPENNT-HOST-068: integer-zero binding is local to this original TU.
  $flags+=' /DNULL=0 /FI "'+(NP (Join-Path $root 'src/ntsrv-exe/opennt/include/base_server.h'))+'"'
 }
 $binding=[regex]::Match($edge.Bindings,'(?m)^\s+\w*cflags = (.*)$').Groups[1].Value
 # Carry source-specific semantic defines, not old global machine/ABI flags.
 if($binding){
  foreach($option in [regex]::Matches($binding,'/std:c(?:11|17)(?=\s|$)')){$flags+=' '+$option.Value}
  foreach($option in [regex]::Matches($binding,'/(?:O[0-9dis]|Gy|we4013)(?=\s|$)')){$flags+=' '+$option.Value}
  foreach($include in [regex]::Matches($binding,'/I (?:"[^"]+"|[^\s]+)')){
   $flags+=' '+$include.Value.Replace((NP (Split-Path $reference -Parent)),(NP $build))
  }
  foreach($define in [regex]::Matches($binding,'/D[^\s]+')){
   if($define.Value -notmatch '^/D(_X86_|CPU_[A-Z0-9_]+|NEW_CPU|CCPU|C_VID|SPC386|SIM32|V7VGA|NTVDM|ANSI|PROD|WIN32|WINNT|OPENNT_ADAPTER_NT_ALERT_THREAD|MVDM_SOFTPC_NO_HOST_BOOT_FILE_MUTATION|_NO_CRT_STDIO_INLINE|_WIN32_WINNT)($|=)'){$flags+=' '+$define.Value}
  }
 }
 if($target -in @('obj/opennt-rtl/error.obj','obj/opennt-rtl/environ.obj')){$flags+=' /DNTOS_KERNEL_RUNTIME'}
 if($target -eq 'obj/tests/rtl_x86_fixture.obj'){$flags+=' /DOPENNT_ENVIRONMENT_ONLY'}
 if($target -eq 'obj/opennt-base-client/classifier.obj'){$flags+=' /FI "'+(NP (Join-Path $root 'src/nthook32-dll/legacy_classifier_arch.h'))+'"'}
 if($target -eq 'obj/run16/rpc_client.obj'){
  # This existing client imports historical local Base message declarations.
  # Keep its source-facing ABI header, without injecting it into UI/worker TUs.
  $flags+=' /FI "'+(NP (Join-Path $root 'src/opennt-abi/host-compat/include/nt.h'))+'"'
 }
 if($source -match '\.cpp$'){
  $flags=$flags.Replace('/FI "'+(NP (Join-Path $root 'src/opennt-abi/host-compat/include/nt.h'))+'"','')
  $flags+=' /TP /std:c++14'
 }
 if($target -like 'obj/ntvwm/*'){$flags+=' /I frontend-font'}
 $graph.Add('  cflags = '+$flags)
 if($source -match '^O\$:/'){
  $path=$source.Replace('$:',':').Replace('/','\')
  $normalized=$path.Replace('\','/')
  $originalAllowed=@('/base/ntos/rtl/error.c')
  if($Component -eq 'Launcher'){$originalAllowed+=@('/base/ntos/rtl/environ.c','/base/win32/client/vdm.c','/base/ntdll/csrutil.c')}
  if($Component -eq 'Service'){$originalAllowed+=@('/base/win32/server/srvvdm.c','/windows/core/ntuser/server/exports.c')}
  if($normalized.Contains('/src/mvdm/') -or ($normalized.Contains('/src/opennt-host/') -and !@($originalAllowed|Where-Object {$normalized.EndsWith($_)}).Count)){throw "Unadmitted original source: $path"}
  $manifest.Add(@{path=$path;sha256=(Get-FileHash $path).Hash;object=$target})
 }
}
$targets=if($Component -eq 'Service'){@('ntsrv.exe','basesrv-service-reservation-test.exe','basesrv-idle-policy-test.exe','basesrv-reservation-test.exe','native-service-layout-test.exe','native-observation-fixture.exe')}elseif($Component -eq 'Worker'){@('ntvwm.exe','ntvwm-execution-lifetime-test.exe','common-worker-control-test.exe')}elseif($Component -eq 'Monitor'){@('ntmon.exe','monitor-rpc-test.exe','monitor-layout-test.exe','monitor-session-test.exe','common-management-test.exe')}elseif($Component -eq 'Launcher'){@('run16.exe','run16-image-classification-test.exe','application-search-test.exe','native-capture-test.exe','frontend-scope-lifetime-test.exe','rtl-x86-fixture.exe')}else{@('ntcon.exe','frontend-session-arguments-test.exe','frontend-window-library-test.exe','frontend-window-controller-test.exe','frontend-window-keyboard-test.exe','frontend-window-mouse-test.exe','console-frontend-test.exe','console-video-test.exe','frontend-text-handoff-test.exe','console-channel-lifetime-test.exe','console-frame-failure-test.exe','console-pointer-contract-test.exe')}
foreach($target in $targets){Add-Edge $target}
$graph.Add('default '+$targets[0])
$utf8=[Text.UTF8Encoding]::new($false)
[IO.File]::WriteAllText((Join-Path $build 'build.ninja'),($graph -join "`r`n")+"`r`n",$utf8)
$ninja=(Get-Command ninja.exe -ErrorAction Stop).Source
$vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
[IO.File]::WriteAllText((Join-Path $build 'run-ninja.cmd'),'@echo off'+"`r`n"+'call "'+$vs+'" -arch=x64 -host_arch=x64 >nul'+"`r`n"+'if errorlevel 1 exit /b %errorlevel%'+"`r`n"+'"'+$ninja+'" -C "'+$build+'" -j 8 %*'+"`r`n",$utf8)
@{architecture='AMD64';scope=$Component;reference=$reference;referenceHash=(Get-FileHash $reference).Hash;sources=@($manifest)}|ConvertTo-Json -Depth 6|Set-Content (Join-Path $build 'source-manifest.json') -Encoding UTF8
"Generated native $Component graph: $build ($($manifest.Count) source edges)"
