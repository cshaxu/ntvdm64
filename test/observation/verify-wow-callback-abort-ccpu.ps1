[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned evidence required'}
$null=New-Item -ItemType Directory $root
try{
 & "$repo/tools/build-fixtures.cmd" x86 wow-callback-abort-ccpu *> "$root/build.log"
 if($LASTEXITCODE){throw 'Native CCPU abort build failed'}
 $worker="$repo/build/.ninja/msvc-x86-release/worker"
 $map=Get-Content "$worker/wow-callback-abort-ccpu.exe.map" -Raw
 foreach($owner in @(@('ccpu386SimulatePtr','wow_abort_tls.obj'),@('call_cpu_function','wow_abort_ccpu.obj'),@('c_cpu_simulate','c_main.obj'))){
  if($map -notmatch ('(?m)\s_'+$owner[0]+'\s+[^\r\n]+'+[regex]::Escape($owner[1]))){throw "Original owner not supplied by inspected TU: $($owner[0])"}
 }
 $image="$worker/wow-callback-abort-ccpu.exe"
 $p=Start-Process $image -ArgumentList '--verify' -WorkingDirectory $root -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/verify.out" -RedirectStandardError "$root/verify.err"
 try{
  @{pid=$p.Id;started=$p.StartTime.ToUniversalTime().ToString('o');image=$image;arguments='--verify'}|ConvertTo-Json|Set-Content "$root/verify-process.json"
  if(!$p.WaitForExit(180000)){throw 'Owned CPU diagnostic deadline'}
  $p.WaitForExit();$p.ExitCode|Set-Content "$root/verify.exit"
  if($p.ExitCode){throw "CPU assertions failed: $($p.ExitCode)"}
 }finally{if(!$p.HasExited){$p.Kill($true);$p.WaitForExit()};$p.Dispose()}
 $output=Get-Content "$root/verify.out" -Raw
 if(([regex]::Matches($output,'CCPU_ABORT_OK thread=')).Count -ne 3 -or
     $output -notmatch 'CCPU_INTERFACE_OK' -or $output -notmatch 'CCPU_CONTROL_OK'){throw 'Missing real CPU/thread/control assertions'}
 Get-FileHash $image,"$worker/wow-callback-abort-ccpu.exe.map","$repo/src/mvdm-host/softpc.new/base/ccpu386/c_main.c","$repo/src/mvdm-host/softpc.new/base/ccpu386/c_xtrn.c","$repo/src/mvdm-host/softpc.new/base/ccpu386/ntthread.c"|Select-Object Path,Hash|ConvertTo-Json|Set-Content "$root/inputs.json"
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/error.txt";1|Set-Content "$root/exit.txt";throw}
