[CmdletBinding()]
param([Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($OutputRoot,$repo)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
    (Test-Path $out)){throw 'Require a fresh evidence directory under build/.'}
$null=New-Item -ItemType Directory $out
$results=[Collections.Generic.List[object]]::new()
foreach($architecture in 'x86','x64'){
    & "$repo/tools/build-fixtures.cmd" $architecture presentation-font *> "$out/font-$architecture-build.log"
    if($LASTEXITCODE){throw "Font build failed: $architecture"}
    $exe="$repo/build/.ninja/fixtures-$architecture/presentation-font-snapshot-fixture.exe"
    & $exe
    if($LASTEXITCODE){throw "Font snapshot failed: $architecture"}
    $results.Add([pscustomobject]@{name='font-snapshot';architecture=$architecture;exit=0;sha256=(Get-FileHash $exe).Hash})
}
& "$repo/tools/build-fixtures.cmd" x86 console-contracts *> "$out/console-build.log"
if($LASTEXITCODE){throw 'Console build failed'}
& "$repo/tools/build-fixtures.cmd" x64 observation-tools *> "$out/observer-build.log"
if($LASTEXITCODE){throw 'Observer build failed'}
$old=$env:MVDM_OBSERVER_PRIVATE_DESKTOP
try {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    foreach($name in 'console-display-toggle','console-input-contract'){
        $exe="$repo/build/.ninja/msvc-x86-release/worker/$name.exe"
        $report="$out/$name.txt"
        & "$repo/build/.ninja/fixtures-x64/console-startup-observer.exe" $exe (Split-Path $exe) $report --observation-timeout-ms 15000
        if($LASTEXITCODE){throw "Observer failed: $name"}
        $lines=Get-Content $report
        if($lines -notcontains 'result=exited' -or $lines -notcontains 'exit=0x00000000'){
            throw "Console fixture failed: $name; see $report"
        }
        $results.Add([pscustomobject]@{name=$name;architecture='x86';exit=0;sha256=(Get-FileHash $exe).Hash})
    }
} finally {$env:MVDM_OBSERVER_PRIVATE_DESKTOP=$old}
[pscustomobject]@{exit=0;cases=$results;scope='Native Console cells/input and font snapshot; no guest GUI acceptance'} |
    ConvertTo-Json -Depth 4 | Set-Content "$out/result.json"
