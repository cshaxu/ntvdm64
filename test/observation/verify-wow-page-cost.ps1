[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,
 [string]$WorkerBuildRoot=(Join-Path $PSScriptRoot '../../build/.ninja/msvc-x86-release/worker'),
 [string]$ProviderBuildRoot=(Join-Path $PSScriptRoot '../../build/.ninja/msvc-x86-release/wow32'),
 [Parameter(Mandatory)][string]$Observer,
 [string]$CompilerDriver=(Join-Path $PSScriptRoot '../../build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd'),
 [Parameter(Mandatory)][string]$LogonHelper)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned diagnostic root required'}
$null=New-Item -ItemType Directory $root
$shell=(Get-Process -Id $PID).Path
function Owned($name,$exe,$line,$seconds,$working=$repo){
 $p=Start-Process $exe -ArgumentList $line -WindowStyle Hidden -WorkingDirectory $working -PassThru -RedirectStandardOutput "$root/$name.stdout.txt" -RedirectStandardError "$root/$name.stderr.txt";$null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');image=$exe;command=$line;working=$working;deadlineSeconds=$seconds}|ConvertTo-Json|Set-Content "$root/$name.process.json"
 if(!$p.WaitForExit($seconds*1000)){taskkill.exe /PID $p.Id /T /F;throw "Owned diagnostic deadline $name"}
 $p.WaitForExit();$code=$p.ExitCode;$p.Dispose();$code|Set-Content "$root/$name.exit.txt";if($code){throw "$name failed $code"}
}
function Replace-One($text,$old,$replacement){
 if([regex]::Matches($text,[regex]::Escape($old)).Count -ne 1){throw "Expected one mechanical source boundary: $old"}
 return $text.Replace($old,$replacement)
}
try{
 $source="$repo/src/ntvdm-exe/softpc/mvdm_softpc_wow_page_domain.c"
 $inputs=@($source,$PSCommandPath,"$PSScriptRoot/wow_page_cost_instrument.h","$PSScriptRoot/wow_page_cost_data.h","$PSScriptRoot/wow_page_cost_probe.h","$PSScriptRoot/wow_mine_latency_probe.c","$PSScriptRoot/wow_mine_gameplay_probe.c","$PSScriptRoot/verify-wow-mine-latency.ps1",$Observer,$CompilerDriver,$LogonHelper,"$repo/assets/winnt.zip","$WorkerBuildRoot/build.ninja","$WorkerBuildRoot/ntvdm.exe","$WorkerBuildRoot/ntvdm.exe.map","$WorkerBuildRoot/ntvdm.lib","$ProviderBuildRoot/wow32.dll","$ProviderBuildRoot/wow32.dll.map")
 $inputs+=@(Get-ChildItem $WorkerBuildRoot -File -Filter '*.lib').FullName
 $inputs|Sort-Object -Unique|Get-FileHash|Export-Csv -NoTypeInformation "$root/inputs.csv"
 $text=[IO.File]::ReadAllText($source)
 $bodies=$text
 foreach($name in @('collect_observed_classes','collect_retired_windows','copy_window_view','project_window_graph')){
  $full='mvdm_softpc_wow_page_domain_'+$name
  $bodies=Replace-One $bodies ('int '+$full+'(') ('static int page_cost_body_'+$name+'(')
 }
 $restored=$bodies
 foreach($name in @('collect_observed_classes','collect_retired_windows','copy_window_view','project_window_graph')){
  $restored=Replace-One $restored ('static int page_cost_body_'+$name+'(') ('int mvdm_softpc_wow_page_domain_'+$name+'(')
 }
 if($restored -cne $text){throw 'Instrumented bodies did not reverse exactly to source'}
 $wrappers=@'
#undef qsort
int mvdm_softpc_wow_page_domain_collect_observed_classes(void) {
 LONGLONG started=page_cost_begin(); int result=page_cost_body_collect_observed_classes();
 page_cost_end(PAGE_COST_CLASS,started,0);return result;
}
int mvdm_softpc_wow_page_domain_collect_retired_windows(void) {
 LONGLONG started=page_cost_begin(); int result=page_cost_body_collect_retired_windows();
 page_cost_end(PAGE_COST_RETIRED,started,0);return result;
}
int mvdm_softpc_wow_page_domain_copy_window_view(ULONG capacity,mvdm_softpc_wow_window_observation *rows,ULONG *count) {
 LONGLONG started=page_cost_begin(); int result=page_cost_body_copy_window_view(capacity,rows,count);
 page_cost_end(PAGE_COST_COPY,started,result?*count:0);return result;
}
int mvdm_softpc_wow_page_domain_project_window_graph(ULONG count,const mvdm_softpc_wow_window_observation *rows) {
 LONGLONG started=page_cost_begin(); int result=page_cost_body_project_window_graph(count,rows);
 page_cost_end(PAGE_COST_GRAPH,started,count);return result;
}
'@
 # Generated instrumentation is a mechanical copy. No original algorithm,
 # branch, loop, ownership transition or guest source is edited.
 $overlay='#include "'+($PSScriptRoot.Replace('\','/'))+'/wow_page_cost_instrument.h"'+"`r`n#define qsort page_cost_qsort`r`n"+$bodies+"`r`n"+$wrappers
 [IO.File]::WriteAllText("$root/page-domain-profile.c",$overlay,[Text.UTF8Encoding]::new($false))
 $cc=@(& ninja.exe -C $WorkerBuildRoot -t commands obj/adapter-softpc/mvdm_softpc_wow_page_domain.obj)[-1]
 if($LASTEXITCODE){throw 'Cannot read actual worker compile command'}
 $cc=Replace-One $cc '/Foobj/adapter-softpc/mvdm_softpc_wow_page_domain.obj' ('/Fo"'+$root+'/page-domain-profile.obj"')
 $cc=Replace-One $cc ($source.Replace('\','/')) ('"'+$root+'/page-domain-profile.c"')
 $link=@(& ninja.exe -C $WorkerBuildRoot -t commands ntvdm.exe)[-1]
 if($LASTEXITCODE -or $link -notmatch '^cmd.exe /d /s /c "(.*)"$'){throw 'Cannot read actual worker link command'}
 $link=$Matches[1]
 $directInputs=@([regex]::Matches($link,'(?:^|\s)(obj/[^\s"]+\.(?:obj|res))(?=\s|$)')|ForEach-Object {Join-Path $WorkerBuildRoot $_.Groups[1].Value})
 $directInputs+=@("$WorkerBuildRoot/generated/ntvdm-wow32-provider.def","$WorkerBuildRoot/obj/adapter-monitor/mvdm_vdm_tib.obj","$repo/tools/audit/Verify-VdmTibStorage.mjs")
 $directInputs|Sort-Object -Unique|Get-FileHash|Export-Csv -NoTypeInformation -Append "$root/inputs.csv"
 $link=Replace-One $link 'link.exe ' ('link.exe "'+$root+'/page-domain-profile.obj" /implib:"'+$root+'/ntvdm.lib" ')
 $link=Replace-One $link '/out:ntvdm.exe ' ('/out:"'+$root+'/ntvdm.exe" ')
 if([regex]::Matches($link,'ntvdm\.exe\.map').Count -ne 2){throw 'Expected link MAP plus VdmTib audit MAP'}
 $link=$link.Replace('ntvdm.exe.map',($root+'/ntvdm.exe.map'))
 $vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
 Owned compile $env:ComSpec ('/d /s /c "call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && '+$cc+'"') 180 $WorkerBuildRoot
 Owned link $env:ComSpec ('/d /s /c "call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && '+$link+'"') 180 $WorkerBuildRoot
 $map=Get-Content "$root/ntvdm.exe.map" -Raw
 if($map -match 'softpc-bindings:mvdm_softpc_wow_page_domain\.obj'){throw 'Original page-domain object was unexpectedly pulled alongside the diagnostic copy'}
 $preferred=[Convert]::ToInt64(([regex]::Match($map,'Preferred load address is ([0-9A-Fa-f]+)')).Groups[1].Value,16)
 $symbols=[regex]::Matches($map,'(?m)^\s+[0-9A-Fa-f]{4}:[0-9A-Fa-f]{8}\s+_T447ProfileData\s+([0-9A-Fa-f]{8})\s')
 if($symbols.Count -ne 1){throw 'Exactly one diagnostic data symbol required'}
 $rva=[Convert]::ToInt64($symbols[0].Groups[1].Value,16)-$preferred
 if($rva -le 0 -or $rva -gt [uint32]::MaxValue){throw 'Invalid diagnostic data RVA'}
 @{rva=$rva;symbol='_T447ProfileData';originalBodiesChanged=$false;productionOutputsChanged=$false;durations='Inclusive nested durations; do not sum across kinds';observer='ReadProcessMemory stable even-sequence snapshot; no target writes'}|ConvertTo-Json|Set-Content "$root/diagnostic.json"
 Get-FileHash "$root/page-domain-profile.c","$root/ntvdm.exe","$root/ntvdm.exe.map","$root/page-domain-profile.obj"|Export-Csv -NoTypeInformation "$root/diagnostic-inputs.csv"
 Expand-Archive "$repo/assets/winnt.zip" "$root/media"
 $package="$root/media/WINNT"
 foreach($image in Get-Content "$repo/assets/release/release-manifest.json" -Raw|ConvertFrom-Json){
  if((Get-FileHash "$repo/assets/release/$($image.name)").Hash -ne $image.sha256){throw 'Release baseline mismatch'}
  Copy-Item "$repo/assets/release/$($image.name)" "$package/system32/$($image.name)" -Force
 }
 Owned baseline $shell ('-NoProfile -File "'+$PSScriptRoot+'/verify-wow-mine-latency.ps1" -PackageRoot "'+$package+'" -Observer "'+$Observer+'" -CompilerDriver "'+$CompilerDriver+'" -BuildRoot "'+$root+'/baseline" -WorkerMap "'+$WorkerBuildRoot+'/ntvdm.exe.map" -WowMap "'+$ProviderBuildRoot+'/wow32.dll.map"') 300
 Copy-Item "$root/ntvdm.exe" "$package/system32/ntvdm.exe" -Force
 Owned ems $shell ('-NoProfile -File "'+$PSScriptRoot+'/verify-original-ems-probe.ps1" -PackageRoot "'+$package+'" -Observer "'+$Observer+'" -LogonHelper "'+$LogonHelper+'" -CompilerDriver "'+$CompilerDriver+'" -BuildRoot "'+$root+'/ems"') 180
 Owned measurement $shell ('-NoProfile -File "'+$PSScriptRoot+'/verify-wow-mine-latency.ps1" -PackageRoot "'+$package+'" -Observer "'+$Observer+'" -CompilerDriver "'+$CompilerDriver+'" -BuildRoot "'+$root+'/measurement" -WorkerMap "'+$root+'/ntvdm.exe.map" -WowMap "'+$ProviderBuildRoot+'/wow32.dll.map" -PageCostRva '+$rva.ToString('X8')) 300
 foreach($input in Import-Csv "$root/inputs.csv"){if((Get-FileHash $input.Path).Hash -ne $input.Hash){throw 'Frozen production/diagnostic input changed'}}
 foreach($input in Import-Csv "$root/diagnostic-inputs.csv"){if((Get-FileHash $input.Path).Hash -ne $input.Hash){throw 'Frozen diagnostic image changed'}}
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
