param([Parameter(Mandatory)][string]$BuildRoot,[Parameter(Mandatory)][string]$CompilerDriver)
# Invoke only from an owned hidden background supervisor.
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned root required'}
$null=New-Item -ItemType Directory $root
try {
 $source="$repo/src/ntvdm-exe/softpc/mvdm_softpc_termination.c"
 $probe="$PSScriptRoot/softpc_observer_error_probe.c"
 Push-Location $root
 try {
  & $CompilerDriver cl.exe /nologo /O2 /MT /Gy /W3 /FIwindows.h "/I$repo/src" "/I$repo/src/ntvdm-exe/softpc/include" "/Fo$root/" "/Fe$root/probe.exe" $probe $source /link /OPT:REF *> "$root/compile.txt"
  if($LASTEXITCODE){throw 'Observer error probe compile failed'}
 }finally{Pop-Location}
 @($source,$probe,"$root/probe.exe")|Get-FileHash|Select-Object Path,Hash|ConvertTo-Json|Set-Content "$root/inputs.json"
 $line='"'+$root+'/trace.txt" "'+$root+'/absent/trace.txt"'
 $p=Start-Process "$root/probe.exe" -ArgumentList $line -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/probe.out" -RedirectStandardError "$root/probe.err"
 $null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$line;deadline=$p.StartTime.AddSeconds(20).ToUniversalTime().ToString('o')}|ConvertTo-Json|Set-Content "$root/process.json"
 if(!$p.WaitForExit(20000)){& taskkill.exe /PID $p.Id /T /F;throw 'Owned observer probe timeout'}
 $p.WaitForExit();$p.Refresh()
 if($p.ExitCode -or (Get-Content "$root/probe.out" -Raw) -notmatch 'OBSERVER_LAST_ERROR checks=50 result=0'){throw 'Observer error transparency failed'}
 $trace=Get-Content "$root/trace.txt" -Raw
 foreach($marker in @('wow-bop','wow-dispatch-return','wow-stage stage=fixture')){if(!$trace.Contains($marker)){throw "Missing enabled output: $marker"}}
 '0'|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/error.txt";'1'|Set-Content "$root/exit.txt";throw}
