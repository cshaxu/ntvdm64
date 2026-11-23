[CmdletBinding()]
param([Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($OutputRoot,$repo)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
    (Test-Path $out)){throw 'Require a fresh evidence directory under build/.'}
$null=New-Item -ItemType Directory $out
& "$repo/tools/build-fixtures.cmd" x86 host-thread-boundaries *> "$out/build.log"
if($LASTEXITCODE){throw 'Host thread boundary build failed'}
$rows=@(foreach($name in 'softpc-delay-heartbeat','softpc-ica-lock'){
    $exe="$repo/build/.ninja/msvc-x86-release/worker/$name.exe"
    $p=Start-Process $exe -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/$name.out" -RedirectStandardError "$out/$name.err"
    if(!$p.WaitForExit(15000)){$p.Kill();$p.WaitForExit();throw "Timeout: $name"}
    $p.WaitForExit()
    if($p.ExitCode){throw "Failed: $name ($($p.ExitCode))"}
    [pscustomobject]@{name=$name;pid=$p.Id;exit=$p.ExitCode;sha256=(Get-FileHash $exe).Hash}
})
[pscustomobject]@{exit=0;cases=$rows} | ConvertTo-Json -Depth 4 | Set-Content "$out/result.json"
