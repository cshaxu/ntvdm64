[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,
 [Parameter(Mandatory)][string]$BaselineImages,
 [Parameter(Mandatory)][string]$BaselineWorker,
 [Parameter(Mandatory)][string]$BaselineProvider,
 [Parameter(Mandatory)][string]$CandidateImages,
 [Parameter(Mandatory)][string]$CandidateWorker,
 [Parameter(Mandatory)][string]$CandidateProvider,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$CompilerDriver)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'));$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned paired callback evidence required'}
$null=New-Item -ItemType Directory $root;$shell=(Get-Process -Id $PID).Path
function Owned($name,$line,$seconds){
 $p=Start-Process $shell -ArgumentList $line -WindowStyle Hidden -WorkingDirectory $repo -PassThru -RedirectStandardOutput "$root/$name.stdout.txt" -RedirectStandardError "$root/$name.stderr.txt";$null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$line;deadlineSeconds=$seconds}|ConvertTo-Json|Set-Content "$root/$name.process.json"
 if(!$p.WaitForExit($seconds*1000)){taskkill.exe /PID $p.Id /T /F;throw "Owned paired callback deadline $name"}
 $p.WaitForExit();$code=$p.ExitCode;$p.Dispose();$code|Set-Content "$root/$name.exit.txt";if($code){throw "$name failed $code; preserve fixed-order failure"}
}
try{
 $inputs=@($PSCommandPath,"$PSScriptRoot/verify-wow-callback-cost.ps1","$PSScriptRoot/summarize-wow-callback-cost.ps1",$Observer,$CompilerDriver)
 foreach($dir in @($BaselineImages,$CandidateImages)){
  $inputs+=Join-Path $dir 'release-manifest.json'
  $images=@(Get-Content "$dir/release-manifest.json" -Raw|ConvertFrom-Json);if($images.Count -ne 13){throw 'Coherent13 required'}
  foreach($image in $images){$path=Join-Path $dir $image.name;if((Get-FileHash $path).Hash -ne $image.sha256){throw 'Selected manifest mismatch'};$inputs+=$path}
 }
 $inputs|Sort-Object -Unique|Get-FileHash|Export-Csv -NoTypeInformation "$root/inputs.csv"
 $sequence=@('baseline','composed','composed-repeat','baseline-repeat')
 @{sequence=$sequence;environments=@('normal','normal-sampled','private');flagPairs=12;revealPairs=3;protocol='Fixed ABBA; unchanged original-runtime workload plus reversible diagnostic copy per version; no retry-until-green';limits='Inclusive callback wall time; signed observation lag and one-tick ordering uncertainty; random reveal depth; not physical input or speedup proof'}|ConvertTo-Json|Set-Content "$root/plan.json"
 $summary=foreach($case in $sequence){
  $baseline=$case -like 'baseline*'
  $images=if($baseline){$BaselineImages}else{$CandidateImages}
  $worker=if($baseline){$BaselineWorker}else{$CandidateWorker}
  $provider=if($baseline){$BaselineProvider}else{$CandidateProvider}
  Owned $case ('-NoProfile -File "'+$PSScriptRoot+'/verify-wow-callback-cost.ps1" -BuildRoot "'+$root+'/'+$case+'" -ImageRoot "'+$images+'" -WorkerBuildRoot "'+$worker+'" -ProviderBuildRoot "'+$provider+'" -Observer "'+$Observer+'" -CompilerDriver "'+$CompilerDriver+'"') 1800
  if($case -eq 'composed'){
   $null=New-Item -ItemType Directory "$root/composed-inputs"
   $timed=foreach($image in Get-Content "$images/release-manifest.json" -Raw|ConvertFrom-Json){Get-FileHash "$root/composed/sealed-images/$($image.name)"}
   $timed|Export-Csv -NoTypeInformation "$root/composed-inputs/images.csv"
  }
  foreach($row in Get-Content "$root/$case/summary.json" -Raw|ConvertFrom-Json){$row|Add-Member -NotePropertyName run -NotePropertyValue $case -PassThru}
 }
 $summary|ConvertTo-Json -Depth 5|Set-Content "$root/summary.json"
 foreach($input in Import-Csv "$root/inputs.csv"){if((Get-FileHash $input.Path).Hash -ne $input.Hash){throw 'Frozen paired input changed'}}
 @{fixedSequenceComplete=$true;instrumentedCallbacks=360;actualPairsPerVersionRun=45;performanceDispositionPending=$true;published=$false;goalComplete=$false}|ConvertTo-Json|Set-Content "$root/result.json"
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
