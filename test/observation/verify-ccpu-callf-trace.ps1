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
if($mapText -notmatch '(?m)_CALLF\s+[^\r\n]*original-ccpu386:call\.obj'){
 throw 'Selected original CALLF owner not found'
}
$saved=[Environment]::GetEnvironmentVariable('CCPU_CALLF_CHECK')
try {
 $env:CCPU_CALLF_CHECK='1'
 $p=Start-Process $image -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/stdout.txt" -RedirectStandardError "$root/stderr.txt"
 $p.WaitForExit(); $p.Refresh()
 if($p.ExitCode -or !(Select-String -LiteralPath "$root/stderr.txt" -SimpleMatch 'CCPU_CALLF_RETF_OK')){throw 'CCPU CALLF fixture failed'}
 @("$repo/test/mvdm-host/ccpu_bounded_execution_fixture.c", "$worker/obj/ccpu/call.obj", $image, $map) |
    Get-FileHash -Algorithm SHA256 | Export-Csv -NoTypeInformation "$root/inputs.csv"
 'PASS original CCPU CALLF/RETF register and stack behavior' | Set-Content "$root/result.txt"
}finally{[Environment]::SetEnvironmentVariable('CCPU_CALLF_CHECK',$saved)}
