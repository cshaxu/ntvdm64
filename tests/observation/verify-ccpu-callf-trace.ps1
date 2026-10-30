param([Parameter(Mandatory)][string]$BuildRoot,[Parameter(Mandatory)][string]$CompilerDriver)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned root required'}
$driver=(Resolve-Path -LiteralPath $CompilerDriver).Path
$worker=(Resolve-Path "$repo/build/.ninja/msvc-x86-release/worker").Path
$null=New-Item -ItemType Directory $root
& $driver ninja.exe -C $worker ccpu-high-linear-page-test.exe *> "$root/build.log"
if($LASTEXITCODE){throw 'Selected CCPU fixture build failed'}
$image="$worker/ccpu-high-linear-page-test.exe"
$map="$image.map"
$mapText=Get-Content $map -Raw
if($mapText -notmatch '(?m)_CALLF\s+[^\r\n]*original-ccpu386:call\.obj' -or
   $mapText -notmatch '(?m)_mvdm_softpc_report_wow_far_call_attempt\s+[^\r\n]*mvdm_softpc_termination\.obj'){
 throw 'Selected CALLF and diagnostic owners not found'
}
$saved=[Environment]::GetEnvironmentVariable('CCPU_CALLF_TRACE_PATH')
try {
 $env:CCPU_CALLF_TRACE_PATH="$root/trace.txt"
 $p=Start-Process $image -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/stdout.txt" -RedirectStandardError "$root/stderr.txt"
 $p.WaitForExit(); $p.Refresh()
 if($p.ExitCode -or !(Select-String -LiteralPath "$root/stderr.txt" -SimpleMatch 'CCPU_CALLF_TRACE_EQUIVALENT_OK modes=3')){throw 'CCPU CALLF fixture failed'}
 $lines=@(Get-Content "$root/trace.txt")
 if($lines.Count -ne 1 -or $lines[0] -notmatch '^far-call selector=0100 offset=00000020 caller=f000:0000fff5 '){throw 'Trace filtering did not match the actual CALLF'}
 if($lines[0] -match 'code=|source=|shadow='){throw 'Trace sampled guest memory'}
 @("$repo/tests/mvdm-host/ccpu_bounded_execution_fixture.c", "$worker/obj/ccpu/call.obj", $image, $map) |
    Get-FileHash -Algorithm SHA256 | Export-Csv -NoTypeInformation "$root/inputs.csv"
 'PASS CCPU CALLF/RETF disabled, unmatched and matching trace' | Set-Content "$root/result.txt"
}finally{[Environment]::SetEnvironmentVariable('CCPU_CALLF_TRACE_PATH',$saved)}
