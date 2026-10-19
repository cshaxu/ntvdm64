[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,
 [Parameter(Mandatory)][string]$Observer,
 [string]$CompilerDriver='',
 [string]$WorkerBuildRoot='',
 [string]$ProviderBuildRoot='')
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'));$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned graph diagnostic required'}
$null=New-Item -ItemType Directory $root;$shell=(Get-Process -Id $PID).Path
if(!$WorkerBuildRoot){$WorkerBuildRoot="$repo/build/.ninja/msvc-x86-release/worker"}
if(!$ProviderBuildRoot){$ProviderBuildRoot="$repo/build/.ninja/msvc-x86-release/wow32"}
$provider=[IO.Path]::GetFullPath($ProviderBuildRoot);$worker=[IO.Path]::GetFullPath($WorkerBuildRoot)
if(!$CompilerDriver){$CompilerDriver=Join-Path $provider 'msvc-x86.cmd'}
$compiler=(Resolve-Path $CompilerDriver).Path;$observer=(Resolve-Path $Observer).Path
function Owned($name,$exe,$line,$seconds,$working=$repo){
 $p=Start-Process $exe -ArgumentList $line -WindowStyle Hidden -WorkingDirectory $working -PassThru -RedirectStandardOutput "$root/$name.stdout.txt" -RedirectStandardError "$root/$name.stderr.txt";$null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');image=$exe;command=$line;working=$working;deadlineSeconds=$seconds}|ConvertTo-Json|Set-Content "$root/$name.process.json"
 if(!$p.WaitForExit($seconds*1000)){taskkill.exe /PID $p.Id /T /F;throw "Owned graph diagnostic deadline $name"}
 $p.WaitForExit();$code=$p.ExitCode;$p.Dispose();$code|Set-Content "$root/$name.exit.txt";if($code){throw "$name failed $code"}
}
$script:replacements=@()
function Mark($value,$old,$new){
 if([regex]::Matches($value,[regex]::Escape($old)).Count -ne 1){throw "One source boundary required: $old"}
 $script:replacements+=,@($old,$new);return $value.Replace($old,$new)
}
try{
 $source="$repo/src/wow32-dll/source/wow_user_object_bindings.c"
 $original=[IO.File]::ReadAllText($source).Replace("`r`n","`n");$body=$original
 $specs=@(
  @('refresh_graph','HWND callback_window, const HWND *payload, ULONG payload_count','callback_window, payload, payload_count',3,'payload_count',"BOOL WINAPI wow_user_window_refresh_graph"),
  @('capture_window','HWND window, window_observation *row','window, row',0,'1',"/* This index/array is transaction scratch"),
  @('normalize_observation_chains',"window_observation *rows, ULONG count,`n    const ULONG *index",'rows, count, index',1,'count',"static BOOL project_graph(const HWND *roots"),
  @('project_graph',"const HWND *roots, ULONG root_count,`n    const HWND *payload, ULONG payload_count",'roots, root_count, payload, payload_count',2,'root_count',"BOOL WINAPI wow_user_window_project_graph")
 )
 foreach($spec in $specs){
  $name=$spec[0];$argsText=$spec[1];$call=$spec[2];$kind=$spec[3];$items=$spec[4];$boundary=$spec[5]
  $body=Mark $body ("static BOOL $name($argsText)") ("static BOOL graph_cost_body_$name($argsText)")
  $wrapper="static BOOL $name($argsText)`n{`n    BOOL result=FALSE; LONGLONG started=graph_cost_begin();`n    __try { result=graph_cost_body_$name($call); }`n    __finally { graph_cost_end($kind,started,$items); }`n    return result;`n}`n`n"
  $body=Mark $body $boundary ($wrapper+$boundary)
 }
 $commit='    result = wow_window_words_project_observed(count, rows);'
 $body=Mark $body $commit ("    { LONGLONG started=graph_cost_begin();`n      __try { "+$commit.Trim()+" }`n      __finally { graph_cost_end(4,started,count); }`n    }")
 $restored=$body
 for($i=$script:replacements.Count-1;$i -ge 0;$i--){$pair=$script:replacements[$i];$restored=$restored.Replace($pair[1],$pair[0])}
 if($restored -cne $original){throw 'Original graph body reversal failed'}
 [IO.File]::WriteAllText("$root/graph-profile.c",('#include "'+$PSScriptRoot.Replace('\','/')+'/wow_graph_cost_instrument.h"'+"`n"+$body),[Text.UTF8Encoding]::new($false))
 $cc=@(& ninja.exe -C $provider -t commands obj/wow32-dll/wow_user_object_bindings.obj)[-1]
 if($LASTEXITCODE){throw 'Original graph compile rule missing'}
 $link=@(& ninja.exe -C $provider -t commands wow32.dll)[-1]
 if($LASTEXITCODE -or $link -notmatch '^link '){throw 'Original provider link rule missing'}
 $inputs=@($PSCommandPath,$source,$compiler,$observer,"$repo/assets/winnt.zip","$provider/build.ninja","$provider/wow32.dll","$provider/wow32.dll.map","$provider/wow32-x86.def","$worker/ntvdm.exe","$worker/ntvdm.exe.map","$worker/ntvdm.lib")
 $inputs+=@(Get-ChildItem $PSScriptRoot -File |Where-Object {$_.Name -in @('wow_graph_cost_instrument.h','wow_graph_cost_fixture.c','wow_page_cost_data.h','wow_page_cost_probe.h','wow_mine_latency_probe.c','wow_mine_gameplay_probe.c','wow_native_pc_sample.c','verify-wow-mine-latency.ps1')}).FullName
 $inputs+=@([regex]::Matches($link,'(?:^|\s)(obj/[^\s"]+\.(?:obj|res))(?=\s|$)')|ForEach-Object {Join-Path $provider $_.Groups[1].Value})
 $inputs+=@(Get-ChildItem $provider -File -Filter '*.lib').FullName
 $deps=@(& ninja.exe -C $provider -t deps obj/wow32-dll/wow_user_object_bindings.obj)
 if($LASTEXITCODE){throw 'Original dependencies missing'}
 $deps|Set-Content "$root/original-deps.txt"
 $inputs+=@($deps|ForEach-Object {if($_ -match '^\s+([A-Za-z]:[/\\].*)$' -and (Test-Path $Matches[1])){$Matches[1]}})
 $images=@(Get-Content "$repo/assets/release/release-manifest.json" -Raw|ConvertFrom-Json)
 if($images.Count -ne 13){throw 'Coherent13 required'}
 foreach($image in $images){foreach($dir in @("$repo/assets/release",'O:\winnt\system32')){if((Get-FileHash "$dir/$($image.name)").Hash -ne $image.sha256){throw 'Installed manifest mismatch'};$inputs+="$dir/$($image.name)"}}
 foreach($pair in @(@('ntvdm.exe',$worker),@('WOW32.DLL',$provider))){if((Get-FileHash "$($pair[1])/$($pair[0])").Hash -ne (Get-FileHash "$repo/assets/release/$($pair[0])").Hash){throw 'Selected parent/provider mismatch'}}
 $inputs|Sort-Object -Unique|Get-FileHash|Export-Csv -NoTypeInformation "$root/inputs.csv"
 $cc=Mark $cc '/Foobj/wow32-dll/wow_user_object_bindings.obj' ('/Fo"'+$root+'/graph-profile.obj"')
 $cc=Mark $cc $source.Replace('\','/') ('"'+$root+'/graph-profile.c"')
 $link=Mark $link ' obj/wow32-dll/wow_user_object_bindings.obj ' (' "'+$root+'/graph-profile.obj" ')
 $parents=[regex]::Matches($link,'(?<!\S)(?:"([^"]*[\\/]ntvdm\.lib)"|([^\s"]*[\\/]ntvdm\.lib))(?=\s|$)')
 if($parents.Count -ne 1){throw 'One parent import required'}
 $link=Mark $link $parents[0].Value ('"'+$worker+'/ntvdm.lib"')
 $link=Mark $link '/map:wow32.dll.map ' ('/map:"'+$root+'/WOW32.DLL.map" ')
 $link=Mark $link '/out:wow32.dll ' ('/out:"'+$root+'/WOW32.DLL" /implib:"'+$root+'/WOW32.lib" ')
 @{compile=$cc;link=$link;originalBodiesReverseExactly=$true}|ConvertTo-Json|Set-Content "$root/commands.json"
 Owned compile $env:ComSpec ('/d /s /c "call "'+$compiler+'" '+$cc+'"') 180 $provider
 Owned link $env:ComSpec ('/d /s /c "call "'+$compiler+'" '+$link+'"') 180 $provider
 Owned unit-build $env:ComSpec ('/d /s /c "call "'+$compiler+'" cl.exe /nologo /MT /O2 /W4 /WX "'+$PSScriptRoot+'/wow_graph_cost_fixture.c" /Fo"'+$root+'/unit.obj" /Fe"'+$root+'/unit.exe""') 120
 Owned unit "$root/unit.exe" '' 30
 $map=Get-Content "$root/WOW32.DLL.map" -Raw
 if($map -match '\bwow_user_object_bindings\.obj\b'){throw 'Old graph object linked alongside diagnostic'}
 $preferred=[Convert]::ToInt64(([regex]::Match($map,'Preferred load address is ([0-9A-Fa-f]+)')).Groups[1].Value,16)
 $symbols=[regex]::Matches($map,'(?m)^\s+[0-9A-Fa-f]{4}:[0-9A-Fa-f]{8}\s+_T447ProfileData\s+([0-9A-Fa-f]{8})\s')
 if($symbols.Count -ne 1){throw 'One profile symbol required'}
 $rva=[Convert]::ToInt64($symbols[0].Groups[1].Value,16)-$preferred
 if($rva -le 0 -or $rva -gt [uint32]::MaxValue){throw 'Invalid profile RVA'}
 @{rva=$rva;kinds=@('capture','normalize','project','refresh','commit','unused');inclusive=$true;sourceReversal=$true;productionChanged=$false}|ConvertTo-Json|Set-Content "$root/diagnostic.json"
 # Keep the existing read-only probe ABI, changing only its observed module
 # and rejecting a counter writer-concurrency violation explicitly.
 $probe=[IO.File]::ReadAllText("$PSScriptRoot/wow_page_cost_probe.h")
 $probe=Mark $probe '"ntvdm.exe"' '"WOW32.DLL"'
 $probe=Mark $probe 'data.version != 1 ||' 'data.version != 1 || data.reserved != 0 ||'
 [IO.File]::WriteAllText("$root/wow_page_cost_probe.h",$probe,[Text.UTF8Encoding]::new($false))
 foreach($name in @('wow_page_cost_data.h','wow_mine_latency_probe.c','wow_mine_gameplay_probe.c','wow_native_pc_sample.c')){Copy-Item "$PSScriptRoot/$name" "$root/$name"}
 $runner=[IO.File]::ReadAllText("$PSScriptRoot/verify-wow-mine-latency.ps1")
 $runner=Mark $runner '$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot ''../..''))' ('$repo='''+$repo+'''')
 $runner=Mark $runner '. "$PSScriptRoot/isolated_package_cleanup.ps1"' ('. "'+$PSScriptRoot+'/isolated_package_cleanup.ps1"')
 [IO.File]::WriteAllText("$root/latency.ps1",$runner,[Text.UTF8Encoding]::new($false))
 Get-FileHash "$root/graph-profile.c","$root/graph-profile.obj","$root/WOW32.DLL","$root/WOW32.DLL.map","$root/latency.ps1","$root/wow_page_cost_probe.h"|Export-Csv -NoTypeInformation "$root/diagnostic-inputs.csv"
 Expand-Archive "$repo/assets/winnt.zip" "$root/media";$package="$root/media/WINNT"
 foreach($image in $images){Copy-Item "$repo/assets/release/$($image.name)" "$package/system32/$($image.name)" -Force}
 Owned baseline $shell ('-NoProfile -File "'+$root+'/latency.ps1" -PackageRoot "'+$package+'" -Observer "'+$observer+'" -CompilerDriver "'+$compiler+'" -BuildRoot "'+$root+'/baseline" -WorkerMap "'+$worker+'/ntvdm.exe.map" -WowMap "'+$provider+'/wow32.dll.map"') 360
 Copy-Item "$root/WOW32.DLL" "$package/system32/WOW32.DLL" -Force
 Owned measurement $shell ('-NoProfile -File "'+$root+'/latency.ps1" -PackageRoot "'+$package+'" -Observer "'+$observer+'" -CompilerDriver "'+$compiler+'" -BuildRoot "'+$root+'/measurement" -WorkerMap "'+$worker+'/ntvdm.exe.map" -WowMap "'+$root+'/WOW32.DLL.map" -PageCostRva '+$rva.ToString('X8')) 360
 foreach($list in @('inputs.csv','diagnostic-inputs.csv')){foreach($input in Import-Csv "$root/$list"){if((Get-FileHash $input.Path).Hash -ne $input.Hash){throw 'Frozen graph diagnostic input changed'}}}
 @{namedGroupPassed=$true;productionChanged=$false;published=$false;interpretationPending=$true}|ConvertTo-Json|Set-Content "$root/result.json"
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
