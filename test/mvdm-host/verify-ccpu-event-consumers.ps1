param([Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($OutputRoot,$repo)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh repository build output required'}
New-Item -ItemType Directory $out|Out-Null
& "$repo/tools/build-fixtures.cmd" x86 ccpu-event-consumers *> "$out/build.log"
if($LASTEXITCODE){throw 'CCPU event consumer fixture build failed'}
$p=Start-Process "$repo/build/.ninja/msvc-x86-release/worker/ccpu-event-consumers.exe" -WorkingDirectory $out -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/result.txt" -RedirectStandardError "$out/result.err"
try {
 if(!$p.WaitForExit(10000)){$p.Kill();$p.WaitForExit();throw 'Owned CCPU fixture timeout'}
 $code=$p.ExitCode;Get-Content "$out/result.txt"
 $code|Set-Content "$out/result.exit"
 if($code){throw "CCPU event consumer assertions failed: $code"}
}finally{$p.Dispose()}
