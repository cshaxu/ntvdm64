[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,
 [Parameter(Mandatory)][string]$Observer,
 [string]$CompilerDriver='',
 [string]$WorkerBuildRoot='',
 [string]$ProviderBuildRoot='',
 [switch]$BuildOnly)
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
 $start=@{FilePath=$exe;WindowStyle='Hidden';WorkingDirectory=$working;PassThru=$true;RedirectStandardOutput="$root/$name.stdout.txt";RedirectStandardError="$root/$name.stderr.txt"}
 if($line){$start.ArgumentList=$line}
 $p=Start-Process @start;$null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');image=$exe;command=$line;working=$working;deadlineSeconds=$seconds}|ConvertTo-Json|Set-Content "$root/$name.process.json"
 if(!$p.WaitForExit($seconds*1000)){taskkill.exe /PID $p.Id /T /F;throw "Owned graph diagnostic deadline $name"}
 $p.WaitForExit();$code=$p.ExitCode;$p.Dispose();$code|Set-Content "$root/$name.exit.txt";if($code){throw "$name failed $code"}
}
function Mark($value,$old,$new){
 if([regex]::Matches($value,[regex]::Escape($old)).Count -ne 1){throw "One source boundary required: $old"}
 return $value.Replace($old,$new)
}
try{
 $source="$repo/src/compat/wow_user_object_bindings.c"
 $test="$PSScriptRoot/../observation"
 $inputs=@($PSCommandPath,$source,$compiler,$observer,"$repo/assets/winnt.zip","$provider/build.ninja","$provider/wow32.dll","$provider/wow32.dll.map","$provider/wow32-x86.def","$worker/ntvdm.exe","$worker/ntvdm.exe.map","$worker/ntvdm.lib","$repo/assets/release/release-manifest.json","$repo/tools/build/wow32/graph-cost.cmake")
 foreach($name in @('wow_graph_cost_instrument.h','wow_graph_cost_fixture.c','wow_page_cost_data.h','wow_page_cost_probe.h','wow_mine_latency_probe.c','wow_mine_gameplay_probe.c','wow_native_pc_sample.c')){$inputs+="$test/$name"}
 $inputs+="$PSScriptRoot/verify-wow-mine-latency.ps1"
 $images=@(Get-Content "$repo/assets/release/release-manifest.json" -Raw|ConvertFrom-Json)
 if(!$images.Count -or @($images.name|Sort-Object -Unique).Count -ne $images.Count){throw 'Coherent current manifest required'}
 foreach($image in $images){foreach($dir in @("$repo/assets/release",'O:\winnt\system32')){if((Get-FileHash "$dir/$($image.name)").Hash -ne $image.sha256){throw 'Installed manifest mismatch'};$inputs+="$dir/$($image.name)"}}
 foreach($pair in @(@('ntvdm.exe',$worker),@('WOW32.DLL',$provider))){if((Get-FileHash "$($pair[1])/$($pair[0])").Hash -ne (@($images|Where-Object name -eq $pair[0])[0].InputSha256)){throw 'Selected parent/provider input mismatch'}}
 Owned build $env:ComSpec ('/d /s /c ""'+$repo+'/tools/build-fixtures.cmd" x86 wow-graph-cost "'+$provider+'""') 300
 $diagnostic="$provider/diagnostics/graph-cost"
 Copy-Item "$diagnostic/graph-profile.c","$diagnostic/WOW32.DLL","$diagnostic/WOW32.DLL.map","$diagnostic/wow_page_cost_probe.h" $root
 $object=(Get-Content "$diagnostic/graph-object.txt" -Raw).Trim()
 Copy-Item $object "$root/graph-profile.obj"
 $members=@(Get-Content "$diagnostic/objects.txt")
 if(@($members|Where-Object {$_ -match 'obj_compat_wow_user_object_bindings_obj'}).Count -or @($members|Where-Object {$_ -eq $object}).Count -ne 1){throw 'Diagnostic object substitution mismatch'}
 $inputs+=@($members|Where-Object {$_ -ne $object})
 $inputs+=@(Get-ChildItem $provider -File -Filter '*.lib').FullName
 $deps=@(& ninja.exe -C $provider -t deps ([IO.Path]::GetRelativePath($provider,$object).Replace('\','/')))
 if($LASTEXITCODE){throw 'Diagnostic dependencies missing'}
 $deps|Set-Content "$root/original-deps.txt"
 $inputs+=@($deps|ForEach-Object {if($_ -match '^\s+([A-Za-z]:[/\\].*)$' -and (Test-Path $Matches[1])){$Matches[1]}})
 $inputs|Sort-Object -Unique|Get-FileHash|Export-Csv -NoTypeInformation "$root/inputs.csv"
 Owned unit-build $env:ComSpec ('/d /s /c ""'+$repo+'/tools/build-fixtures.cmd" x86 wow-graph-cost-unit"') 180
 Copy-Item "$repo/build/.ninja/fixtures-x86/wow-graph-cost-unit.exe" "$root/unit.exe"
 Owned unit "$root/unit.exe" '' 30
 $map=Get-Content "$root/WOW32.DLL.map" -Raw
 if($map -match '\bwow_user_object_bindings\.obj\b'){throw 'Old graph object linked alongside diagnostic'}
 $preferred=[Convert]::ToInt64(([regex]::Match($map,'Preferred load address is ([0-9A-Fa-f]+)')).Groups[1].Value,16)
 $symbols=[regex]::Matches($map,'(?m)^\s+[0-9A-Fa-f]{4}:[0-9A-Fa-f]{8}\s+_T447ProfileData\s+([0-9A-Fa-f]{8})\s')
 if($symbols.Count -ne 1){throw 'One profile symbol required'}
 $rva=[Convert]::ToInt64($symbols[0].Groups[1].Value,16)-$preferred
 if($rva -le 0 -or $rva -gt [uint32]::MaxValue){throw 'Invalid profile RVA'}
 @{rva=$rva;kinds=@('capture','normalize','project','refresh','commit','unused');inclusive=$true;sourceReversal=$true;productionChanged=$false}|ConvertTo-Json|Set-Content "$root/diagnostic.json"
 if($BuildOnly){
  foreach($input in Import-Csv "$root/inputs.csv"){if((Get-FileHash $input.Path).Hash -ne $input.Hash){throw 'Frozen graph build input changed'}}
  @{nativeBuildPassed=$true;guestRuntimeExecuted=$false;productionChanged=$false;published=$false}|ConvertTo-Json|Set-Content "$root/result.json"
  0|Set-Content "$root/exit.txt"
  return
 }
 # Keep the existing read-only probe ABI, changing only its observed module
 # and rejecting a counter writer-concurrency violation explicitly.
 foreach($name in @('wow_page_cost_data.h','wow_mine_latency_probe.c','wow_mine_gameplay_probe.c','wow_native_pc_sample.c')){Copy-Item "$test/$name" "$root/$name"}
 $runner=[IO.File]::ReadAllText("$PSScriptRoot/verify-wow-mine-latency.ps1")
 $runner=Mark $runner '$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot ''../..''))' ('$repo='''+$repo+'''')
 $runner=Mark $runner '. "$PSScriptRoot/../observation/isolated_package_cleanup.ps1"' ('. "'+$test+'/isolated_package_cleanup.ps1"')
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
