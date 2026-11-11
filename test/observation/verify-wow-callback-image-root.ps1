[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,
 [Parameter(Mandatory)][string]$CandidateImages,
 [Parameter(Mandatory)][string]$CandidateWorker)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'));$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned image-input controls required'}
$null=New-Item -ItemType Directory $root;$shell=(Get-Process -Id $PID).Path
try{
 $published=@(Get-Content "$repo/assets/release/release-manifest.json" -Raw|ConvertFrom-Json)
 Get-FileHash $PSCommandPath,"$PSScriptRoot/verify-wow-callback-cost.ps1","$repo/assets/release/release-manifest.json","$CandidateImages/release-manifest.json","$CandidateWorker/ntvdm.exe"|Export-Csv -NoTypeInformation "$root/inputs.csv"
 foreach($name in @('duplicate','hash')){$null=New-Item -ItemType Directory "$root/$name-images"}
 $duplicate=@(1..13|ForEach-Object {[pscustomobject]@{name=$published[0].name;sha256=$published[0].sha256}})
 $duplicate|ConvertTo-Json|Set-Content "$root/duplicate-images/release-manifest.json"
 $bad=@($published|ForEach-Object {[pscustomobject]@{name=$_.name;sha256=$_.sha256}});$bad[0].sha256='0'*64
 $bad|ConvertTo-Json|Set-Content "$root/hash-images/release-manifest.json"
 Copy-Item "$repo/assets/release/$($published[0].name)" "$root/hash-images/$($published[0].name)"
 $cases=@(@('outside','O:\winnt\system32','Only manifest-coherent build/release'),@('duplicate',"$root/duplicate-images",'Coherent13 image identities'),@('hash',"$root/hash-images",'Selected image manifest mismatch'))
 # The caller supplies the new worker, deliberately paired with P9 images.
 if((Get-FileHash "$CandidateWorker/ntvdm.exe").Hash -eq ($published|Where-Object name -eq 'ntvdm.exe').sha256){throw 'Negative MAP/image control requires a distinct candidate worker'}
 $cases+=,@('map',"$repo/assets/release",'Image/MAP baseline mismatch')
 $results=foreach($case in $cases){
  $out="$root/$($case[0])";$line='-NoProfile -File "'+$PSScriptRoot+'/verify-wow-callback-cost.ps1" -BuildRoot "'+$out+'" -ImageRoot "'+$case[1]+'" -WorkerBuildRoot "'+$CandidateWorker+'"'
  $p=Start-Process $shell -ArgumentList $line -WindowStyle Hidden -WorkingDirectory $repo -PassThru -RedirectStandardOutput "$root/$($case[0]).stdout.txt" -RedirectStandardError "$root/$($case[0]).stderr.txt";$null=$p.Handle
  @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$line;deadlineSeconds=30}|ConvertTo-Json|Set-Content "$root/$($case[0]).process.json"
  if(!$p.WaitForExit(30000)){taskkill.exe /PID $p.Id /T /F;throw 'Input guard did not stop before native work'}
  $p.WaitForExit();$code=$p.ExitCode;$p.Dispose();$code|Set-Content "$root/$($case[0]).exit.txt"
  $failure=Get-Content "$out/failure.txt" -Raw
  if($code -ne 1 -or !$failure.Contains($case[2])){throw "Wrong input rejection $($case[0])"}
  foreach($path in @('unit','callback-profile.c','compile.process.json','media')){if(Test-Path "$out/$path"){throw 'Invalid input reached fixture/compiler/runtime work'}}
  [pscustomobject]@{case=$case[0];expectedFailure=$case[2];exit=$code;rejectedBeforeFixture=$true}
 }
 $results|ConvertTo-Json|Set-Content "$root/results.json"
 foreach($row in Import-Csv "$root/inputs.csv"){if((Get-FileHash $row.Path).Hash -ne $row.Hash){throw 'Input guard source/image changed'}}
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
