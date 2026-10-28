$ErrorActionPreference='Stop'
$repo='O:\repos.hobby\ntvdm64'
$run=Join-Path $repo 'build/t473-s10/dos-command-inputs-r4'
$image=(Get-Command python -ErrorAction Stop).Source
foreach($case in 'observer','merge'){
 $script=if($case -eq 'observer'){'dos-command-input-observer.py'}else{'dos-command-input-role-merge.py'}
 $line='-B -X utf8 "'+$repo+'\artifacts\mvdm-audit\'+$script+'"'
 $p=Start-Process -FilePath $image -ArgumentList $line -WindowStyle Hidden -PassThru -WorkingDirectory $repo -RedirectStandardOutput "$run/$case.stdout.txt" -RedirectStandardError "$run/$case.stderr.txt"
 try{
  @{pid=$p.Id;started=$p.StartTime.ToUniversalTime().ToString('o');image=$image;arguments=$line;diagnostic_budget_seconds=180}|ConvertTo-Json|Set-Content "$run/$case.process.json"
  if(!$p.WaitForExit(180000)){& taskkill.exe /PID $p.Id /T /F;throw 'Owned source audit budget exceeded'}
  $p.WaitForExit();$code=$p.ExitCode
  @{exit=$code;finished=$p.ExitTime.ToUniversalTime().ToString('o')}|ConvertTo-Json|Set-Content "$run/$case.completion.json"
  Get-Content "$run/$case.stdout.txt";Get-Content "$run/$case.stderr.txt"
  if($code){exit $code}
 }finally{$p.Dispose()}
}