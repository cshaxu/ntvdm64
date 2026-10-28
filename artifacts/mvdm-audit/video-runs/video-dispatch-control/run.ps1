$ErrorActionPreference='Stop'
$repo='O:\repos.hobby\ntvdm64'
$run=Join-Path $repo 'build/t473-s10/video-dispatch-control'
$hostPath='C:\Users\neko\.cache\codex-runtimes\codex-primary-runtime\dependencies\native\powershell\pwsh.exe'
foreach($case in @('baseline','baseline-negative')){
 $package=if($case.StartsWith('candidate')){'build/t473-s10/candidate-validation/package'}else{'build/package-uuid/package-c'}
 $line='-NoProfile -ExecutionPolicy Bypass -File "'+$repo+'\tests\observation\verify-ccpu-sign-flags-guest.ps1" -Workload BiosDispatch -PackageRoot "'+(Join-Path $repo $package)+'" -Observer "'+$repo+'\build\tools\console-startup-observer.exe" -LogonHelper "'+$repo+'\build\t449-s2-message-construction-r3\logon-env.dll" -BuildRoot "'+$run+'\'+$case+'"'
 if($case.EndsWith('-negative')){$line+=' -NegativeOracle'}
 $p=Start-Process -FilePath $hostPath -ArgumentList $line -WindowStyle Hidden -WorkingDirectory $repo -RedirectStandardOutput "$run/$case.stdout.txt" -RedirectStandardError "$run/$case.stderr.txt" -PassThru
 try{
  @{pid=$p.Id;started=$p.StartTime.ToUniversalTime().ToString('o');image=$hostPath;arguments=$line;diagnostic_budget_seconds=180}|ConvertTo-Json|Set-Content "$run/$case.process.json"
  if(!$p.WaitForExit(180000)){throw 'Owned disk guest diagnostic budget exceeded'}
  $p.WaitForExit();$code=$p.ExitCode
  @{exit=$code;finished=$p.ExitTime.ToUniversalTime().ToString('o')}|ConvertTo-Json|Set-Content "$run/$case.completion.json"
  Get-Content "$run/$case.stdout.txt" -Tail 6;Get-Content "$run/$case.stderr.txt"
  if($code){exit $code}
 }finally{$p.Dispose()}
}
'PASS default BIOS dispatch positive and wrong-oracle controls'
