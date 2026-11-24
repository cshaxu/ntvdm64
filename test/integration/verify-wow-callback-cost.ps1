[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,
 [string]$WorkerBuildRoot=(Join-Path $PSScriptRoot '../../build/.ninja/msvc-x86-release/worker'),
 [string]$ProviderBuildRoot=(Join-Path $PSScriptRoot '../../build/.ninja/msvc-x86-release/wow32'),
 [string]$ImageRoot,
 [Parameter(Mandatory)][string]$Observer,
 [string]$CompilerDriver=(Join-Path $PSScriptRoot '../../build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd'))
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'));$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned callback diagnostic required'}
$null=New-Item -ItemType Directory $root;$shell=(Get-Process -Id $PID).Path
function Owned($name,$exe,$line,$seconds,$working=$repo){
 $p=Start-Process $exe -ArgumentList $line -WindowStyle Hidden -WorkingDirectory $working -PassThru -RedirectStandardOutput "$root/$name.stdout.txt" -RedirectStandardError "$root/$name.stderr.txt";$null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');image=$exe;command=$line;working=$working;deadlineSeconds=$seconds}|ConvertTo-Json|Set-Content "$root/$name.process.json"
 if(!$p.WaitForExit($seconds*1000)){taskkill.exe /PID $p.Id /T /F;throw "Owned callback diagnostic deadline $name"}
 $p.WaitForExit();$code=$p.ExitCode;$p.Dispose();$code|Set-Content "$root/$name.exit.txt";if($code){throw "$name failed $code"}
}
try{
 if(!$ImageRoot){$ImageRoot="$repo/assets/release"}
 $ImageRoot=(Resolve-Path $ImageRoot).Path
 if($ImageRoot -ne "$repo\assets\release" -and !$ImageRoot.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)){throw 'Only manifest-coherent build/release image roots are admitted'}
 $source="$repo/src/compat/wow_window_words_binding.c"
 $inputs=@($PSCommandPath,$source,$Observer,$CompilerDriver,"$ProviderBuildRoot/build.ninja","$ProviderBuildRoot/wow32.dll","$ProviderBuildRoot/wow32.dll.map","$ProviderBuildRoot/wow32-x86.def","$WorkerBuildRoot/ntvdm.exe","$WorkerBuildRoot/ntvdm.exe.map","$WorkerBuildRoot/ntvdm.lib","$repo/assets/winnt.zip","$repo/assets/release/release-manifest.json","$PSScriptRoot/../observation/verify-wow-callback-cost-probe.ps1","$PSScriptRoot/../observation/wow_callback_cost_fixture.c","$PSScriptRoot/../observation/wow_callback_cost_data.h","$PSScriptRoot/../observation/wow_callback_cost_instrument.h","$PSScriptRoot/../observation/wow_callback_cost_dispatch.h","$PSScriptRoot/../observation/wow_callback_cost_probe.h","$PSScriptRoot/../observation/wow_observer_cost.h","$PSScriptRoot/verify-wow-mine-latency.ps1","$PSScriptRoot/../observation/wow_mine_latency_probe.c","$PSScriptRoot/../observation/wow_mine_gameplay_probe.c","$PSScriptRoot/../observation/summarize-wow-callback-cost.ps1")
 $published=@(Get-Content "$repo/assets/release/release-manifest.json" -Raw|ConvertFrom-Json)
 $images=@(Get-Content "$ImageRoot/release-manifest.json" -Raw|ConvertFrom-Json)
 if(!$published.Count -or $images.Count -ne $published.Count -or @($images.name|Sort-Object -Unique).Count -ne $images.Count -or @(Compare-Object ($images.name|Sort-Object) ($published.name|Sort-Object)).Count){throw 'Coherent current image identities required'}
 $inputs+="$ImageRoot/release-manifest.json"
 foreach($image in $published){$inputs+="$repo/assets/release/$($image.name)"}
 foreach($image in $images){$path="$ImageRoot/$($image.name)";if((Get-FileHash $path).Hash -ne $image.sha256){throw 'Selected image manifest mismatch'};$inputs+=$path}
 foreach($pair in @(@('ntvdm.exe',$WorkerBuildRoot),@('WOW32.DLL',$ProviderBuildRoot))){if((Get-FileHash "$($pair[1])/$($pair[0])").Hash -ne (@($images|Where-Object name -eq $pair[0])[0].InputSha256)){throw 'Image/MAP baseline mismatch'}}
 Owned build $env:ComSpec ('/d /s /c ""'+$repo+'/tools/build-fixtures.cmd" x86 wow-callback-cost "'+(Resolve-Path $ProviderBuildRoot).Path+'""') 300
 $diagnostic=Join-Path $ProviderBuildRoot 'diagnostics/callback-cost'
 Copy-Item "$diagnostic/callback-profile.c","$diagnostic/WOW32.DLL","$diagnostic/WOW32.DLL.map" $root
 $object=(Get-Content "$diagnostic/callback-object.txt" -Raw).Trim()
 Copy-Item $object "$root/callback-profile.obj"
 $members=@(Get-Content "$diagnostic/objects.txt")
 if(@($members|Where-Object {$_ -match 'obj_compat_wow_window_words_binding_obj'}).Count -or @($members|Where-Object {$_ -eq $object}).Count -ne 1){throw 'Diagnostic object substitution mismatch'}
 $inputs+=@($members|Where-Object {$_ -ne $object})
 $inputs+=@("$repo/tools/build/wow32/callback-cost.cmake","$repo/tools/build/wow32/CMakeLists.txt","$ProviderBuildRoot/CMakeCache.txt")
 $inputs|Sort-Object -Unique|Get-FileHash|Export-Csv -NoTypeInformation "$root/inputs.csv"
 Owned unit $shell ('-NoProfile -File "'+$PSScriptRoot+'/../observation/verify-wow-callback-cost-probe.ps1" -BuildRoot "'+$root+'/unit"') 180
 Owned parser $shell ('-NoProfile -File "'+$PSScriptRoot+'/../observation/summarize-wow-callback-cost.ps1" -BuildRoot "'+$root+'/parser-controls" -SelfTest') 30
 $map=Get-Content "$root/WOW32.DLL.map" -Raw
 if($map -match '\bwow_window_words_binding\.obj\b'){throw 'Original object linked alongside diagnostic copy'}
 $preferred=[Convert]::ToInt64(([regex]::Match($map,'Preferred load address is ([0-9A-Fa-f]+)')).Groups[1].Value,16)
 $symbols=[regex]::Matches($map,'(?m)^\s+[0-9A-Fa-f]{4}:[0-9A-Fa-f]{8}\s+_T447CallbackCosts\s+([0-9A-Fa-f]{8})\s')
 if($symbols.Count -ne 1){throw 'Expected one native diagnostic data symbol'}
 $rva=[Convert]::ToInt64($symbols[0].Groups[1].Value,16)-$preferred;if($rva -le 0 -or $rva -gt [uint32]::MaxValue){throw 'Invalid diagnostic data RVA'}
 @{symbol='_T447CallbackCosts';version=2;rva=$rva;capacity=64;callbacksPerRun=30;flagPairs=12;revealPairs=3;originalStatementsChanged=$false;phaseMarkersReversible=$true;productionOutputsChanged=$false;limits='Inclusive callback wall time; read-only after measured intervals; one QPC tick cross-thread uncertainty; not physical mouse latency or full S4'}|ConvertTo-Json|Set-Content "$root/diagnostic.json"
 Get-FileHash "$root/callback-profile.c","$root/callback-profile.obj","$root/WOW32.DLL","$root/WOW32.DLL.map"|Export-Csv -NoTypeInformation "$root/diagnostic-inputs.csv"
 Owned prepare-baseline $env:ComSpec ('/d /s /c ""'+$repo+'/tools/prepare-wow-diagnostic.cmd" "'+$root+'/media" "'+$ImageRoot+'" "'+$WorkerBuildRoot+'" "'+$ProviderBuildRoot+'""') 120
 $package="$root/media/WINNT"
 Owned baseline $shell ('-NoProfile -File "'+$PSScriptRoot+'/verify-wow-mine-latency.ps1" -PackageRoot "'+$package+'" -Observer "'+$Observer+'" -CompilerDriver "'+$CompilerDriver+'" -BuildRoot "'+$root+'/baseline" -WorkerMap "'+$package+'/system32/ntvdm.exe.map" -WowMap "'+$package+'/system32/WOW32.DLL.map" -ObserverCost -CallbackWorkload') 360
 Owned prepare-measurement $env:ComSpec ('/d /s /c ""'+$repo+'/tools/prepare-wow-diagnostic.cmd" "'+$root+'/instrumented" "'+$ImageRoot+'" "'+$WorkerBuildRoot+'" "'+$ProviderBuildRoot+'" "'+$diagnostic+'/WOW32.DLL""') 120
 $package="$root/instrumented/WINNT"
 Owned measurement $shell ('-NoProfile -File "'+$PSScriptRoot+'/verify-wow-mine-latency.ps1" -PackageRoot "'+$package+'" -Observer "'+$Observer+'" -CompilerDriver "'+$CompilerDriver+'" -BuildRoot "'+$root+'/measurement" -WorkerMap "'+$package+'/system32/ntvdm.exe.map" -WowMap "'+$package+'/system32/WOW32.DLL.map" -ObserverCost -CallbackCostRva '+$rva.ToString('X8')) 360
 Owned summary $shell ('-NoProfile -File "'+$PSScriptRoot+'/../observation/summarize-wow-callback-cost.ps1" -BuildRoot "'+$root+'"') 60
 foreach($input in Import-Csv "$root/inputs.csv"){if((Get-FileHash $input.Path).Hash -ne $input.Hash){throw 'Frozen source/runtime input changed'}}
 foreach($input in Import-Csv "$root/diagnostic-inputs.csv"){if((Get-FileHash $input.Path).Hash -ne $input.Hash){throw 'Diagnostic output changed'}}
 foreach($image in $published){foreach($dir in @("$repo/assets/release",'O:\winnt\system32')){if((Get-FileHash "$dir/$($image.name)").Hash -ne $image.sha256){throw 'Production changed during diagnostic'}}}
 @{sourceStatementsUnchanged=$true;phaseMarkersReversible=$true;productionChanged=$false;guestChanged=$false;callbackPairs=45;interpretationPending=$true}|ConvertTo-Json|Set-Content "$root/result.json"
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
