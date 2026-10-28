$ErrorActionPreference='Stop'
$repo='O:\repos.hobby\ntvdm64'
$run=Join-Path $repo 'build/t473-s10/disk-owner-review'
$hostPath='C:\Users\neko\.cache\codex-runtimes\codex-primary-runtime\dependencies\native\powershell\pwsh.exe'
foreach($case in @('image','ios','qevnt')){
 $image=if($case -eq 'image'){(Get-Command python).Source}else{$hostPath}
 $line=if($case -eq 'image'){'-B -X utf8 "'+$repo+'\artifacts\mvdm-audit\disk-image-observer.py"'}else{'-NoProfile -ExecutionPolicy Bypass -File "'+$repo+'\tests\observation\verify-softpc-object-contract.ps1" -Contract '+$case+' -BuildRoot "'+$run+'\'+$case+'"'}
 $p=Start-Process -FilePath $image -ArgumentList $line -WindowStyle Hidden -WorkingDirectory $repo -RedirectStandardOutput "$run/$case.stdout.txt" -RedirectStandardError "$run/$case.stderr.txt" -PassThru
 try{
  @{pid=$p.Id;started=$p.StartTime.ToUniversalTime().ToString('o');image=$image;arguments=$line;diagnostic_budget_seconds=180}|ConvertTo-Json|Set-Content "$run/$case.process.json"
  if(!$p.WaitForExit(180000)){throw 'Owned disk audit diagnostic budget exceeded'}
  $p.WaitForExit();$code=$p.ExitCode
  @{exit=$code;finished=$p.ExitTime.ToUniversalTime().ToString('o')}|ConvertTo-Json|Set-Content "$run/$case.completion.json"
  Get-Content "$run/$case.stdout.txt" -Tail 8;Get-Content "$run/$case.stderr.txt"
  if($code){exit $code}
 }finally{$p.Dispose()}
}
'PASS disk image and unchanged IOS/quick-event shared-runner controls'
