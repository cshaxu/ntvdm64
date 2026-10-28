$ErrorActionPreference='Stop'
$repo='O:\repos.hobby\ntvdm64'
$run=Join-Path $repo 'build/t473-s10/input-object-contracts'
$hostPath='C:\Users\neko\.cache\codex-runtimes\codex-primary-runtime\dependencies\native\powershell\pwsh.exe'
function RunOwned($name,$image,$line,$expected){
 $p=Start-Process -FilePath $image -ArgumentList $line -WindowStyle Hidden -WorkingDirectory $repo -RedirectStandardOutput "$run/$name.stdout.txt" -RedirectStandardError "$run/$name.stderr.txt" -PassThru
 try{
  @{pid=$p.Id;started=$p.StartTime.ToUniversalTime().ToString('o');image=$image;arguments=$line;diagnostic_budget_seconds=180}|ConvertTo-Json|Set-Content "$run/$name.process.json"
  if(!$p.WaitForExit(180000)){throw 'Owned original input contract diagnostic budget exceeded'}
  $p.WaitForExit();$code=$p.ExitCode
  @{exit=$code;finished=$p.ExitTime.ToUniversalTime().ToString('o');expected_exit=$expected}|ConvertTo-Json|Set-Content "$run/$name.completion.json"
  Get-Content "$run/$name.stdout.txt" -Tail 6;Get-Content "$run/$name.stderr.txt"
  if($code -ne $expected){throw "Unexpected $name result: $code"}
 }finally{$p.Dispose()}
}
foreach($case in @('keycode','nls','ios')){
 $line='-NoProfile -ExecutionPolicy Bypass -File "'+$repo+'\tests\observation\verify-softpc-object-contract.ps1" -Contract '+$case+' -BuildRoot "'+$run+'\'+$case+'"'
 RunOwned $case $hostPath $line 0
}
RunOwned boundary "$run/keycode/keycode-object-contract.exe" '--boundary' 1
$text=[IO.File]::ReadAllText("$run/boundary.stdout.txt")
if($text -notmatch 'KEYCODE_BOUNDARY expected_rejected=2 actual_rejected=0' -or ([regex]::Matches($text,'sentinel=123 observed=123')).Count -ne 2){throw 'Actual-object one-past read was not established'}
'PASS bounded input/NLS/IOS cases; strict keycode bound contract fails on actual original object under isolated data sentinel'
