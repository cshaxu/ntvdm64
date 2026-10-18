[CmdletBinding()]
param([Parameter(Mandatory)][string]$BaselineFixture,
 [Parameter(Mandatory)][string]$CandidateFixture,
 [Parameter(Mandatory)][string]$ObserverExecutable,
 [Parameter(Mandatory)][string]$BuildRoot,
 [ValidateRange(1,32)][int]$Rounds=8)
# Reuse wow_exception_observer --exe. Debugger completion is not fixture success.
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned evidence required'}
$null=New-Item -ItemType Directory $root
$observer=(Resolve-Path $ObserverExecutable).Path
$results=@()
try{
 foreach($spec in @(@('baseline',$BaselineFixture),@('candidate',$CandidateFixture))){
  $label=$spec[0];$source=(Resolve-Path $spec[1]).Path
  $out=Join-Path $root $label;$null=New-Item -ItemType Directory $out
  # Preserve fixture.exe basename: this fixture independently copies itself
  # to run16.exe to assert its existing native child-launch protocol.
  Copy-Item -LiteralPath $source -Destination "$out/fixture.exe"
  Copy-Item -LiteralPath ([IO.Path]::ChangeExtension($source,'map')) -Destination "$out/fixture.map"
  Get-FileHash $source,"$out/fixture.exe","$out/fixture.map",$observer,$PSCommandPath,"$PSScriptRoot/wow_exception_observer.c"|Export-Csv -NoTypeInformation "$out/inputs.csv"
  foreach($round in 1..$Rounds){
   $log="$out/debug-$round.txt"
   $line='--exe "'+$out+'/fixture.exe" --verify "'+$log+'"'
   $p=Start-Process $observer -ArgumentList $line -WindowStyle Hidden -WorkingDirectory $out -PassThru -RedirectStandardOutput "$out/round-$round.stdout.txt" -RedirectStandardError "$out/round-$round.stderr.txt";$null=$p.Handle
   @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');image=$observer;command=$line;timeoutSeconds=135}|ConvertTo-Json|Set-Content "$out/round-$round.process.json"
   try{
    if(!$p.WaitForExit(135000)){taskkill.exe /PID $p.Id /T /F;throw 'Owned debug observer deadline'}
    $p.WaitForExit();$p.ExitCode|Set-Content "$out/round-$round.observer-exit.txt"
    if($p.ExitCode){throw 'Observer failed before a trustworthy fixture verdict'}
   }finally{$p.Dispose()}
   $events=Get-Content $log -Raw
   $created=[regex]::Match($events,'(?m)^create pid=(\d+) image=.*[\\/]fixture\.exe\r?$')
   if(!$created.Success){throw 'Missing exact fixture process identity'}
   $exited=[regex]::Match($events,'(?m)^exit pid='+$created.Groups[1].Value+' code=([0-9a-fA-F]{8})\r?$')
   if(!$exited.Success){throw 'No normal debugger-observed fixture process completion'}
   $stderr=Get-Content "$out/round-$round.stderr.txt" -Raw
   $passed=$exited.Groups[1].Value -eq '00000000' -and
    $stderr -match 'WOW_USER_TASK_LIFECYCLE errors=0 native_direct=5' -and
    $stderr -match 'WOW_NATIVE_RECEIVER_EXIT_RETURN held=0 ' -and
    $stderr -match 'WOW_NATIVE_RECEIVER_EXIT_RETURN held=1 '
   $results+=[pscustomobject]@{version=$label;round=$round;processId=$created.Groups[1].Value;fixtureExit=$exited.Groups[1].Value;complete=$passed;heapCorruption=$events.Contains('code=c0000374')}
  }
  foreach($input in Import-Csv "$out/inputs.csv"){if((Get-FileHash $input.Path).Hash -ne $input.Hash){throw 'Sealed diagnostic input changed'}}
 }
 $results|ConvertTo-Json|Set-Content "$root/results.json"
 'Native fixtures under debugger; not guest/product acceptance. All expected workload markers remain mandatory.'|Set-Content "$root/limits.txt"
 if(@($results|Where-Object {!$_.complete}).Count){throw 'Observed native fixture failures; diagnostic completion is not a pass'}
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
