[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,
 [string]$WorkerBuildRoot='O:\repos.hobby\ntvdm64\build\M0-T445\S6\r003',
 [string]$ProviderBuildRoot='O:\repos.hobby\ntvdm64\build\M0-T447\S3\production-cohort-x86',
 [string]$ImageRoot,
 [string]$Observer='O:\repos.hobby\ntvdm64\build\M0-T447\S2\frozen-print-observer-r1\observer.exe',
 [string]$CompilerDriver='O:\repos.hobby\ntvdm64\build\M0-T445\S6\provider\msvc-x86.cmd')
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
function Replace-One($text,$old,$replacement){if([regex]::Matches($text,[regex]::Escape($old)).Count -ne 1){throw "Expected one mechanical boundary $old"};return $text.Replace($old,$replacement)}
try{
 if(!$ImageRoot){$ImageRoot="$repo/assets/release"}
 $ImageRoot=(Resolve-Path $ImageRoot).Path
 if($ImageRoot -ne "$repo\assets\release" -and !$ImageRoot.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)){throw 'Only manifest-coherent build/release image roots are admitted'}
 $source="$repo/src/wow32-dll/source/wow_window_words_binding.c"
 $inputs=@($PSCommandPath,$source,$Observer,$CompilerDriver,"$ProviderBuildRoot/build.ninja","$ProviderBuildRoot/wow32.dll","$ProviderBuildRoot/wow32.dll.map","$ProviderBuildRoot/wow32-x86.def","$WorkerBuildRoot/ntvdm.exe","$WorkerBuildRoot/ntvdm.exe.map","$WorkerBuildRoot/ntvdm.lib","$repo/assets/winnt.zip","$repo/assets/release/release-manifest.json","$PSScriptRoot/verify-wow-callback-cost-probe.ps1","$PSScriptRoot/wow_callback_cost_fixture.c","$PSScriptRoot/wow_callback_cost_data.h","$PSScriptRoot/wow_callback_cost_instrument.h","$PSScriptRoot/wow_callback_cost_dispatch.h","$PSScriptRoot/wow_callback_cost_probe.h","$PSScriptRoot/wow_observer_cost.h","$PSScriptRoot/verify-wow-mine-latency.ps1","$PSScriptRoot/wow_mine_latency_probe.c","$PSScriptRoot/wow_mine_gameplay_probe.c","$PSScriptRoot/summarize-wow-callback-cost.ps1")
 $published=@(Get-Content "$repo/assets/release/release-manifest.json" -Raw|ConvertFrom-Json)
 $images=@(Get-Content "$ImageRoot/release-manifest.json" -Raw|ConvertFrom-Json)
 if($images.Count -ne 13 -or $published.Count -ne 13 -or @(Compare-Object ($images.name|Sort-Object) ($published.name|Sort-Object)).Count){throw 'Coherent13 image identities required'}
 $inputs+="$ImageRoot/release-manifest.json"
 foreach($image in $published){$inputs+="$repo/assets/release/$($image.name)"}
 $null=New-Item -ItemType Directory "$root/sealed-images"
 foreach($image in $images){$path="$ImageRoot/$($image.name)";if((Get-FileHash $path).Hash -ne $image.sha256){throw 'Selected image manifest mismatch'};$inputs+=$path;Copy-Item $path "$root/sealed-images/$($image.name)"}
 foreach($pair in @(@('ntvdm.exe',$WorkerBuildRoot),@('WOW32.DLL',$ProviderBuildRoot))){if((Get-FileHash "$($pair[1])/$($pair[0])").Hash -ne (Get-FileHash "$root/sealed-images/$($pair[0])").Hash){throw 'Image/MAP baseline mismatch'};Copy-Item "$($pair[1])/$($pair[0]).map" "$root/sealed-images/$($pair[0]).map"}
 $deps=@(& ninja.exe -C $ProviderBuildRoot -t deps obj/wow32-dll/wow_window_words_binding.obj);if($LASTEXITCODE){throw 'Cannot read original compile dependencies'}
 $deps|Set-Content "$root/original-deps.txt"
 $inputs+=@($deps|ForEach-Object {if($_ -match '^\s+([A-Za-z]:[/\\].*)$' -and (Test-Path $Matches[1])){$Matches[1]}})
 $cc=@(& ninja.exe -C $ProviderBuildRoot -t commands obj/wow32-dll/wow_window_words_binding.obj)[-1];if($LASTEXITCODE){throw 'Cannot read original compile rule'}
 $link=@(& ninja.exe -C $ProviderBuildRoot -t commands wow32.dll)[-1];if($LASTEXITCODE -or $link -notmatch '^link '){throw 'Cannot read original provider link rule'}
 $parentReferences=[regex]::Matches($link,'(?<!\S)(?:"([^"]*[\\/]ntvdm\.lib)"|([^\s"]*[\\/]ntvdm\.lib))(?=\s|$)')
 if($parentReferences.Count -ne 1){throw 'Expected one explicit parent import library'}
 $link=Replace-One $link $parentReferences[0].Value ('"'+(Join-Path $WorkerBuildRoot 'ntvdm.lib').Replace('\','/')+'"')
 $inputs+=@([regex]::Matches($link,'(?:^|\s)(obj/[^\s"]+\.(?:obj|res))(?=\s|$)')|ForEach-Object {Join-Path $ProviderBuildRoot $_.Groups[1].Value})
 $inputs+=@(Get-ChildItem $ProviderBuildRoot -File -Filter '*.lib').FullName
 $inputs|Sort-Object -Unique|Get-FileHash|Export-Csv -NoTypeInformation "$root/inputs.csv"
 Owned unit $shell ('-NoProfile -File "'+$PSScriptRoot+'/verify-wow-callback-cost-probe.ps1" -BuildRoot "'+$root+'/unit" -CompilerDriver "'+$CompilerDriver+'"') 180
 Owned parser $shell ('-NoProfile -File "'+$PSScriptRoot+'/summarize-wow-callback-cost.ps1" -BuildRoot "'+$root+'/parser-controls" -SelfTest') 30
 $original=[IO.File]::ReadAllText($source)
 $body=Replace-One $original 'BOOL WINAPI wow_window_dispatch_immutable(' 'static BOOL WINAPI callback_cost_body('
 $needle="        *result = DispatchClientMessage(&view, message, wp, lp, procedure);`n    } __finally {"
 $normalized=$body.Replace("`r`n","`n")
 $marked="        callback_cost_phase_enter();`n        *result = DispatchClientMessage(&view, message, wp, lp, procedure);`n        callback_cost_phase_leave();`n    } __finally {"
 $body=Replace-One $normalized $needle $marked
 $restored=Replace-One $body $marked $needle
 $restored=Replace-One $restored 'static BOOL WINAPI callback_cost_body(' 'BOOL WINAPI wow_window_dispatch_immutable('
 if($restored -cne $original.Replace("`r`n","`n")){throw 'Body/phase reversal failed'}
 $overlay='#include "'+$PSScriptRoot.Replace('\','/')+'/wow_callback_cost_instrument.h"'+"`r`n"+$body+"`r`n"+'#include "'+$PSScriptRoot.Replace('\','/')+'/wow_callback_cost_dispatch.h"'+"`r`n"
 [IO.File]::WriteAllText("$root/callback-profile.c",$overlay,[Text.UTF8Encoding]::new($false))
 $cc=Replace-One $cc '/Foobj/wow32-dll/wow_window_words_binding.obj' ('/Fo"'+$root+'/callback-profile.obj"')
 $cc=Replace-One $cc $source.Replace('\','/') ('"'+$root+'/callback-profile.c"')
 $link=Replace-One $link ' obj/wow32-dll/wow_window_words_binding.obj ' (' "'+$root+'/callback-profile.obj" ')
 $link=Replace-One $link '/map:wow32.dll.map ' ('/map:"'+$root+'/WOW32.DLL.map" ')
 $link=Replace-One $link '/out:wow32.dll ' ('/out:"'+$root+'/WOW32.DLL" /implib:"'+$root+'/WOW32.lib" ')
 @{compile=$cc;link=$link;bodyReversal=$true;sourceModified=$false}|ConvertTo-Json|Set-Content "$root/commands.json"
 Owned compile $env:ComSpec ('/d /s /c "call "'+$CompilerDriver+'" '+$cc+'"') 180 $ProviderBuildRoot
 Owned link $env:ComSpec ('/d /s /c "call "'+$CompilerDriver+'" '+$link+'"') 180 $ProviderBuildRoot
 $map=Get-Content "$root/WOW32.DLL.map" -Raw
 if($map -match '\bwow_window_words_binding\.obj\b'){throw 'Original object linked alongside diagnostic copy'}
 $preferred=[Convert]::ToInt64(([regex]::Match($map,'Preferred load address is ([0-9A-Fa-f]+)')).Groups[1].Value,16)
 $symbols=[regex]::Matches($map,'(?m)^\s+[0-9A-Fa-f]{4}:[0-9A-Fa-f]{8}\s+_T447CallbackCosts\s+([0-9A-Fa-f]{8})\s')
 if($symbols.Count -ne 1){throw 'Expected one native diagnostic data symbol'}
 $rva=[Convert]::ToInt64($symbols[0].Groups[1].Value,16)-$preferred;if($rva -le 0 -or $rva -gt [uint32]::MaxValue){throw 'Invalid diagnostic data RVA'}
 @{symbol='_T447CallbackCosts';version=2;rva=$rva;capacity=64;callbacksPerRun=30;flagPairs=12;revealPairs=3;originalStatementsChanged=$false;phaseMarkersReversible=$true;productionOutputsChanged=$false;limits='Inclusive callback wall time; read-only after measured intervals; one QPC tick cross-thread uncertainty; not physical mouse latency or full S4'}|ConvertTo-Json|Set-Content "$root/diagnostic.json"
 Get-FileHash "$root/callback-profile.c","$root/callback-profile.obj","$root/WOW32.DLL","$root/WOW32.DLL.map"|Export-Csv -NoTypeInformation "$root/diagnostic-inputs.csv"
 Expand-Archive "$repo/assets/winnt.zip" "$root/media";$package="$root/media/WINNT"
 foreach($image in $images){Copy-Item "$root/sealed-images/$($image.name)" "$package/system32/$($image.name)" -Force}
 Owned baseline $shell ('-NoProfile -File "'+$PSScriptRoot+'/verify-wow-mine-latency.ps1" -PackageRoot "'+$package+'" -Observer "'+$Observer+'" -CompilerDriver "'+$CompilerDriver+'" -BuildRoot "'+$root+'/baseline" -WorkerMap "'+$root+'/sealed-images/ntvdm.exe.map" -WowMap "'+$root+'/sealed-images/WOW32.DLL.map" -ObserverCost -CallbackWorkload') 360
 Copy-Item "$root/WOW32.DLL" "$package/system32/WOW32.DLL" -Force
 Owned measurement $shell ('-NoProfile -File "'+$PSScriptRoot+'/verify-wow-mine-latency.ps1" -PackageRoot "'+$package+'" -Observer "'+$Observer+'" -CompilerDriver "'+$CompilerDriver+'" -BuildRoot "'+$root+'/measurement" -WorkerMap "'+$root+'/sealed-images/ntvdm.exe.map" -WowMap "'+$root+'/WOW32.DLL.map" -ObserverCost -CallbackCostRva '+$rva.ToString('X8')) 360
 Owned summary $shell ('-NoProfile -File "'+$PSScriptRoot+'/summarize-wow-callback-cost.ps1" -BuildRoot "'+$root+'"') 60
 foreach($input in Import-Csv "$root/inputs.csv"){if((Get-FileHash $input.Path).Hash -ne $input.Hash){throw 'Frozen source/runtime input changed'}}
 foreach($input in Import-Csv "$root/diagnostic-inputs.csv"){if((Get-FileHash $input.Path).Hash -ne $input.Hash){throw 'Diagnostic output changed'}}
 foreach($image in $published){foreach($dir in @("$repo/assets/release",'O:\winnt\system32')){if((Get-FileHash "$dir/$($image.name)").Hash -ne $image.sha256){throw 'Production changed during diagnostic'}}}
 @{sourceStatementsUnchanged=$true;phaseMarkersReversible=$true;productionChanged=$false;guestChanged=$false;callbackPairs=45;interpretationPending=$true}|ConvertTo-Json|Set-Content "$root/result.json"
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
