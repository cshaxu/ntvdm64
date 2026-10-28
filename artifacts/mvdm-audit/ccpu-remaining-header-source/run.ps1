$ErrorActionPreference='Stop'
$repo='O:\repos.hobby\ntvdm64'
$run=Join-Path $repo 'build/t473-s10/ccpu-schema-review'
$worker=Join-Path $repo 'build/.ninja/msvc-x86-release/worker'
$ninja=(Get-Command ninja -ErrorAction Stop).Source
Get-FileHash "$worker/build.ninja","$worker/original-ccpu386.lib","$worker/ntvdm.exe",$ninja | Export-Csv "$run/inputs.csv" -NoTypeInformation
foreach($case in 'deps','commands'){
 $line='-t '+$case
 $p=Start-Process -FilePath $ninja -ArgumentList $line -WorkingDirectory $worker -WindowStyle Hidden -PassThru -RedirectStandardOutput "$run/$case.stdout.txt" -RedirectStandardError "$run/$case.stderr.txt"
 try{
  @{pid=$p.Id;started=$p.StartTime.ToUniversalTime().ToString('o');image=$ninja;arguments=$line;working_directory=$worker;diagnostic_budget_seconds=60}|ConvertTo-Json|Set-Content "$run/$case.process.json"
  if(!$p.WaitForExit(60000)){& taskkill.exe /PID $p.Id /T /F;throw 'Owned graph observation budget exceeded'}
  $p.WaitForExit();$code=$p.ExitCode
  @{exit=$code;finished=$p.ExitTime.ToUniversalTime().ToString('o')}|ConvertTo-Json|Set-Content "$run/$case.completion.json"
  if($code){throw 'Actual Ninja observation failed'}
 }finally{$p.Dispose()}
}
'PASS actual whole-worker command and dependency records captured; no rebuild'