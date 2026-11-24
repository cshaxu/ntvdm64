[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,
 [string]$WorkerBuildRoot=(Join-Path $PSScriptRoot '../../build/.ninja/msvc-x86-release/worker'),
 [string]$ProviderBuildRoot=(Join-Path $PSScriptRoot '../../build/.ninja/msvc-x86-release/wow32'),
 [Parameter(Mandatory)][string]$Observer,
 [string]$CompilerDriver=(Join-Path $PSScriptRoot '../../build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd'),
 [switch]$BuildOnly)
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
try{
 $source="$repo/src/compat/mvdm_softpc_wow_page_domain.c"
 $inputs=@($source,$PSCommandPath,"$PSScriptRoot/../observation/wow_page_cost_instrument.h","$PSScriptRoot/../observation/wow_page_cost_data.h","$PSScriptRoot/../observation/wow_page_cost_probe.h","$PSScriptRoot/../observation/wow_mine_latency_probe.c","$PSScriptRoot/../observation/wow_mine_gameplay_probe.c","$PSScriptRoot/verify-wow-mine-latency.ps1",$Observer,$CompilerDriver,"$repo/assets/winnt.zip","$WorkerBuildRoot/build.ninja","$WorkerBuildRoot/ntvdm.exe","$WorkerBuildRoot/ntvdm.exe.map","$WorkerBuildRoot/ntvdm.lib","$ProviderBuildRoot/wow32.dll","$ProviderBuildRoot/wow32.dll.map")
 $inputs=@($inputs|Where-Object {$_})
 $inputs+=@(Get-ChildItem $WorkerBuildRoot -File -Filter '*.lib').FullName
 $inputs|Sort-Object -Unique|Get-FileHash|Export-Csv -NoTypeInformation "$root/inputs.csv"
 Owned build $env:ComSpec ('/d /s /c ""'+$repo+'/tools/build-fixtures.cmd" x86 wow-page-cost "'+$WorkerBuildRoot+'""') 300
 $diagnostic=Join-Path $WorkerBuildRoot 'diagnostics/page-cost'
 foreach($name in 'page-domain-profile.c','page-domain-profile.obj','ntvdm.exe','ntvdm.exe.map','ntvdm.lib','members.txt','inputs.txt'){
  Copy-Item "$diagnostic/$name" "$root/$name"
 }
 $members=@(Get-Content "$root/members.txt")
 if(@($members|Where-Object {$_ -eq 'obj/compat/mvdm_softpc_wow_page_domain.obj'}).Count -or
    @($members|Where-Object {$_ -eq 'diagnostics/page-cost/page-domain-profile.obj'}).Count -ne 1){throw 'Page diagnostic member substitution mismatch'}
 $directInputs=@(Get-Content "$root/inputs.txt"|Where-Object {$_ -match '\.(obj|res)$'}|ForEach-Object {Join-Path $WorkerBuildRoot $_})
 $directInputs+=@("$WorkerBuildRoot/generated/ntvdm-wow32-provider.def","$WorkerBuildRoot/obj/dpmi/dpmiint.obj","$repo/build/.ninja/build-tools/ntvdm-tib.exe","$repo/tools/build/worker/page-cost.cmake")
 $directInputs|Sort-Object -Unique|Get-FileHash|Export-Csv -NoTypeInformation -Append "$root/inputs.csv"
 $map=Get-Content "$root/ntvdm.exe.map" -Raw
 if($map -match 'softpc-bindings:mvdm_softpc_wow_page_domain\.obj'){throw 'Original page-domain object was unexpectedly pulled alongside the diagnostic copy'}
 $preferred=[Convert]::ToInt64(([regex]::Match($map,'Preferred load address is ([0-9A-Fa-f]+)')).Groups[1].Value,16)
 $symbols=[regex]::Matches($map,'(?m)^\s+[0-9A-Fa-f]{4}:[0-9A-Fa-f]{8}\s+_T447ProfileData\s+([0-9A-Fa-f]{8})\s')
 if($symbols.Count -ne 1){throw 'Exactly one diagnostic data symbol required'}
 $rva=[Convert]::ToInt64($symbols[0].Groups[1].Value,16)-$preferred
 if($rva -le 0 -or $rva -gt [uint32]::MaxValue){throw 'Invalid diagnostic data RVA'}
 @{rva=$rva;symbol='_T447ProfileData';originalBodiesChanged=$false;productionOutputsChanged=$false;durations='Inclusive nested durations; do not sum across kinds';observer='ReadProcessMemory stable even-sequence snapshot; no target writes'}|ConvertTo-Json|Set-Content "$root/diagnostic.json"
 Get-FileHash "$root/page-domain-profile.c","$root/ntvdm.exe","$root/ntvdm.exe.map","$root/page-domain-profile.obj"|Export-Csv -NoTypeInformation "$root/diagnostic-inputs.csv"
 if($BuildOnly){@{nativeBuildPassed=$true;guestRuntimeExecuted=$false;rva=$rva}|ConvertTo-Json|Set-Content "$root/result.json";0|Set-Content "$root/exit.txt";return}
 Owned prepare-baseline $env:ComSpec ('/d /s /c ""'+$repo+'/tools/prepare-wow-diagnostic.cmd" "'+$root+'/media" "'+$repo+'/assets/release" "'+$WorkerBuildRoot+'" "'+$ProviderBuildRoot+'""') 120
 $package="$root/media/WINNT"
 Owned baseline $shell ('-NoProfile -File "'+$PSScriptRoot+'/verify-wow-mine-latency.ps1" -PackageRoot "'+$package+'" -Observer "'+$Observer+'" -CompilerDriver "'+$CompilerDriver+'" -BuildRoot "'+$root+'/baseline" -WorkerMap "'+$package+'/system32/ntvdm.exe.map" -WowMap "'+$package+'/system32/wow32.dll.map"') 300
 Owned prepare-measurement $env:ComSpec ('/d /s /c ""'+$repo+'/tools/prepare-wow-diagnostic.cmd" "'+$root+'/instrumented" "'+$repo+'/assets/release" "'+$WorkerBuildRoot+'" "'+$ProviderBuildRoot+'" "'+$diagnostic+'/ntvdm.exe""') 120
 $package="$root/instrumented/WINNT"
 Owned ems $shell ('-NoProfile -File "'+$PSScriptRoot+'/verify-original-ems-probe.ps1" -PackageRoot "'+$package+'" -Observer "'+$Observer+'" -BuildRoot "'+$root+'/ems"') 180
 Owned measurement $shell ('-NoProfile -File "'+$PSScriptRoot+'/verify-wow-mine-latency.ps1" -PackageRoot "'+$package+'" -Observer "'+$Observer+'" -CompilerDriver "'+$CompilerDriver+'" -BuildRoot "'+$root+'/measurement" -WorkerMap "'+$package+'/system32/ntvdm.exe.map" -WowMap "'+$package+'/system32/wow32.dll.map" -PageCostRva '+$rva.ToString('X8')) 300
 foreach($input in Import-Csv "$root/inputs.csv"){if((Get-FileHash $input.Path).Hash -ne $input.Hash){throw 'Frozen production/diagnostic input changed'}}
 foreach($input in Import-Csv "$root/diagnostic-inputs.csv"){if((Get-FileHash $input.Path).Hash -ne $input.Hash){throw 'Frozen diagnostic image changed'}}
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
