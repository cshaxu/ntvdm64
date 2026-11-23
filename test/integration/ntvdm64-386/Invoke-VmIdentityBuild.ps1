# Test-runner delegation only; CMD/native owns all compilation and caching.
function Invoke-VmIdentityBuild([string]$Output,[string]$Kind,[string[]]$Defines=@()) {
 $repository=(Resolve-Path "$PSScriptRoot/../../..").Path
 $line='/d /s /c ""'+$repository+'/tools/build-vm-identity.cmd" "'+$Output+'" '+$Kind+' "'+($Defines -join ';')+'""'
 $p=Start-Process $env:ComSpec -ArgumentList $line -WindowStyle Hidden -PassThru -RedirectStandardOutput "$Output.build.stdout.txt" -RedirectStandardError "$Output.build.stderr.txt"
 $null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$line}|ConvertTo-Json|Set-Content "$Output.build.process.json"
 try {
  if(!$p.WaitForExit(300000)){taskkill.exe /PID $p.Id /T /F;throw 'Identity probe build deadline'}
  $p.WaitForExit();$p.ExitCode|Set-Content "$Output.build.exit.txt"
  if($p.ExitCode){throw 'Identity probe build failed'}
 }finally{$p.Dispose()}
}
