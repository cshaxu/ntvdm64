[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,
 [Parameter(Mandatory)][string]$BaselineRoot,
 [Parameter(Mandatory)][string]$CandidateRoot,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$CompilerDriver,
 [Parameter(Mandatory)][string]$LogonHelper)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'));$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned paired evidence required'}
$null=New-Item -ItemType Directory $root
$shell=(Get-Process -Id $PID).Path
function Owned($name,$line,$seconds){
 $p=Start-Process $shell -ArgumentList $line -WindowStyle Hidden -WorkingDirectory $repo -PassThru -RedirectStandardOutput "$root/$name.stdout.txt" -RedirectStandardError "$root/$name.stderr.txt";$null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$line;deadlineSeconds=$seconds}|ConvertTo-Json|Set-Content "$root/$name.process.json"
 if(!$p.WaitForExit($seconds*1000)){taskkill.exe /PID $p.Id /T /F;throw "Owned paired deadline $name"}
 $p.WaitForExit();$code=$p.ExitCode;$p.Dispose();$code|Set-Content "$root/$name.exit.txt";if($code){throw "$name failed $code"}
}
try{
 $baseline=(Resolve-Path $BaselineRoot).Path;$candidate=(Resolve-Path $CandidateRoot).Path
 $inputs=@($PSCommandPath,$Observer,$CompilerDriver,$LogonHelper,"$PSScriptRoot/verify-wow-mine-latency.ps1","$PSScriptRoot/wow_mine_latency_probe.c","$PSScriptRoot/wow_mine_gameplay_probe.c","$PSScriptRoot/verify-original-ems-probe.ps1","$repo/assets/winnt.zip")
 foreach($dir in @($baseline,$candidate)){foreach($name in @('ntvdm.exe','WOW32.DLL','ntvdm.exe.map','WOW32.DLL.map')){$inputs+=Join-Path $dir $name}}
 Get-FileHash $inputs|Export-Csv -NoTypeInformation "$root/inputs.csv"
 $sequence=@('baseline','composed','composed-repeat','baseline-repeat')
 @{sequence=$sequence;operationsPerEnvironment=12;environments=@('normal','normal-sampled','private');policy='Fixed ABBA, no retry-until-green; all runs retained';limits='Posted input plus drawing and20ms pixel polling, not physical mouse latency; completion does not assert speedup'}|ConvertTo-Json|Set-Content "$root/plan.json"
 Expand-Archive "$repo/assets/winnt.zip" "$root/media";$package="$root/media/WINNT"
 foreach($row in Get-Content "$repo/assets/release/release-manifest.json" -Raw|ConvertFrom-Json){if((Get-FileHash "$repo/assets/release/$($row.name)").Hash -ne $row.sha256){throw 'Release manifest mismatch'};Copy-Item "$repo/assets/release/$($row.name)" "$package/system32/$($row.name)" -Force}
 $summary=foreach($case in $sequence){
  $dir=if($case -like 'baseline*'){$baseline}else{$candidate}
  foreach($name in @('ntvdm.exe','WOW32.DLL')){Copy-Item (Join-Path $dir $name) "$package/system32/$name" -Force}
  $null=New-Item -ItemType Directory "$root/$case-inputs"
  Get-ChildItem "$package/system32" -File|Where-Object Name -in @('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe','ntmon.exe','WOW32.DLL','VDMREDIR.DLL','nthook32.dll','nthook64.dll','ntw16s32.dll','ntw16s64.dll','W16Style.exe')|Get-FileHash|Export-Csv -NoTypeInformation "$root/$case-inputs/images.csv"
  Owned ($case+'-ems') ('-NoProfile -File "'+$PSScriptRoot+'/verify-original-ems-probe.ps1" -PackageRoot "'+$package+'" -Observer "'+$Observer+'" -LogonHelper "'+$LogonHelper+'" -CompilerDriver "'+$CompilerDriver+'" -BuildRoot "'+$root+'/'+$case+'-ems"') 180
  Owned $case ('-NoProfile -File "'+$PSScriptRoot+'/verify-wow-mine-latency.ps1" -PackageRoot "'+$package+'" -Observer "'+$Observer+'" -CompilerDriver "'+$CompilerDriver+'" -BuildRoot "'+$root+'/'+$case+'" -WorkerMap "'+$dir+'/ntvdm.exe.map" -WowMap "'+$dir+'/WOW32.DLL.map"') 300
  foreach($environment in @('normal','normal-sampled','private')){
   $rows=@(Import-Csv "$root/$case/$environment/latency.csv"|ForEach-Object {[long]$_.microseconds}|Sort-Object)
   if($rows.Count -ne 12){throw 'Missing fixed posted-input observations'}
   [pscustomobject]@{run=$case;version=$(if($case -like 'baseline*'){'baseline'}else{'candidate'});environment=$environment;operations=12;medianUs=($rows[5]+$rows[6])/2.0;p95Us=$rows[11];totalUs=($rows|Measure-Object -Sum).Sum}
  }
 }
 $summary|ConvertTo-Json|Set-Content "$root/summary.json"
 foreach($row in Import-Csv "$root/inputs.csv"){if((Get-FileHash $row.Path).Hash -ne $row.Hash){throw 'Frozen paired source/image changed'}}
 @{fixedSequenceComplete=$true;performanceDispositionPending=$true;published=$false}|ConvertTo-Json|Set-Content "$root/result.json"
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
