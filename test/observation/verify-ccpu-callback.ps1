[CmdletBinding()]
param([Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($OutputRoot,$repo)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
    (Test-Path $out)){throw 'Require a fresh evidence directory under build/.'}
$null=New-Item -ItemType Directory $out
& "$repo/tools/build-fixtures.cmd" x86 ccpu-callback *> "$out/build.log"
if($LASTEXITCODE){throw 'CCPU callback build failed'}
$worker="$repo/build/.ninja/msvc-x86-release/worker"
$map="$worker/ccpu-high-linear-page-test.exe.map"
if((Get-Content $map -Raw) -notmatch '(?m)_CALLF\s+[^\r\n]*original-ccpu386:call\.obj'){
    throw 'Selected original CALLF owner not found'
}
$saved=[Environment]::GetEnvironmentVariable('CCPU_CALLF_CHECK')
try {
$env:CCPU_CALLF_CHECK='1'
$rows=@(foreach($name in 'original-callback-ccpu','ccpu-high-linear-page-test'){
    $exe="$repo/build/.ninja/msvc-x86-release/worker/$name.exe"
    $p=Start-Process $exe -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/$name.out" -RedirectStandardError "$out/$name.err"
    if(!$p.WaitForExit(30000)){$p.Kill();$p.WaitForExit();throw "Timeout: $name"}
    $p.WaitForExit()
    if($p.ExitCode){throw "Failed: $name ($($p.ExitCode))"}
    [pscustomobject]@{name=$name;pid=$p.Id;exit=$p.ExitCode;sha256=(Get-FileHash $exe).Hash}
})
if(!(Select-String -LiteralPath "$out/ccpu-high-linear-page-test.err" -SimpleMatch 'CCPU_CALLF_RETF_OK')){
    throw 'Missing CALLF/RETF register and stack witness'
}
} finally {[Environment]::SetEnvironmentVariable('CCPU_CALLF_CHECK',$saved)}
[pscustomobject]@{exit=0;cases=$rows} | ConvertTo-Json -Depth 4 | Set-Content "$out/result.json"
