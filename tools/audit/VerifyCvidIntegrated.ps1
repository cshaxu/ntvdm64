[CmdletBinding()]
param([Parameter(Mandatory)][string]$FixtureRoot,[string]$LogPrefix='m0-t413-s4-video')
$ErrorActionPreference='Stop'
$FixtureRoot=(Resolve-Path $FixtureRoot).Path
if($LogPrefix -notmatch '^[a-z0-9-]+$'){throw 'Invalid prefix'}
$rows=@(@(80,25,''),@(45,34,''),@(60,50,''),@(120,30,''),@(80,25,'--mouse'),@(80,25,'--resize'))
foreach($row in $rows){
    $tag=if($row[2]){$row[2].Substring(2)}else{"$($row[0])x$($row[1])"}
    $log="O:\winnt\logs\$LogPrefix-$tag.raw"
    if(Test-Path $log){throw 'Use fresh log prefix'}
    $args=@($row[0],$row[1],$log);if($row[2]){$args+=$row[2]}
    $p=Start-Process -FilePath (Join-Path $FixtureRoot 'terminal-observer.exe') -ArgumentList $args -WorkingDirectory O:\winnt -WindowStyle Hidden -RedirectStandardOutput "$log.runner.txt" -RedirectStandardError "$log.stderr.txt" -PassThru
    if(!$p.WaitForExit(55000)){throw "Terminal observer timeout: $tag"}
    $result=Get-Content "$log.runner.txt" -Raw
    if($result -notmatch '(?m)^wait=0 exit=1\r?$'){throw "Terminal observer failed: $tag; $result"}
    & "$PSScriptRoot/Verify-ConsoleResizeCapture.ps1" -Capture $log
    Write-Output "PASS video $tag"
}
$saved=$env:MVDM_OBSERVER_SHORT_HISTORY
try {
    $env:MVDM_OBSERVER_SHORT_HISTORY='1'
    foreach($i in 1..5){
        $prefix="$LogPrefix-short-$i"
        & "$PSScriptRoot/Verify-CommandExitStatus.ps1" -Observer (Join-Path $FixtureRoot 'observer.exe') -Cases edit -LogPrefix $prefix
        $report="O:\winnt\logs\$prefix-edit.txt"
        $geometry=Get-Content "$report.geometry.txt" -Raw
        if($geometry -notmatch 'buffer=80,300 view=0,36,79,40 cursor=0,40 prefill=40'){throw "Short geometry mismatch: $geometry"}
        foreach($line in 1..4){
            $screen=Get-Content ("$report.line-{0:D2}.console.txt" -f $line) -Raw
            if($screen -notmatch 'MS-DOS resident in High Memory'){throw 'Missing post-EDIT MEM completion'}
        }
        Write-Output "PASS native short-window $i"
    }
} finally {$env:MVDM_OBSERVER_SHORT_HISTORY=$saved}
