[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$FixtureRoot,
    [string]$LogPrefix='m0-t413-s4-ip',
    [string[]]$Cases,
    [switch]$ObserveKnownFault
)
$ErrorActionPreference='Stop'
$Observer=(Resolve-Path $Observer).Path
$FixtureRoot=(Resolve-Path $FixtureRoot).Path
if($LogPrefix -notmatch '^[a-z0-9-]+$'){throw 'Invalid log prefix'}
$products=@('O:\winnt\run16.exe','O:\winnt\ntvdm.exe','O:\winnt\basesrv.exe')
function PackageProcesses {
    @(Get-CimInstance Win32_Process -Filter "Name='run16.exe' OR Name='ntvdm.exe' OR Name='basesrv.exe'" |
        Where-Object {$_.ExecutablePath -in $products})
}
if((PackageProcesses).Count){throw 'Package in use; will not terminate an existing session'}
if(Test-Path Z:\){throw 'Z: is occupied'}
$results=@()
try {
    subst Z: $FixtureRoot
    if($LASTEXITCODE){throw 'Fixture mapping failed'}
    foreach($name in (Get-Content (Join-Path $FixtureRoot 'guest-cases.json') -Raw | ConvertFrom-Json)) {
        if($Cases -and $name -notin $Cases){continue}
        $report="O:\winnt\logs\$LogPrefix-$name.txt"
        if(Test-Path $report){throw 'Use a fresh log prefix'}
        $launcher=0
        $p=Start-Process -FilePath $Observer -ArgumentList @('O:\winnt\run16.exe','O:\winnt',$report,"Z:\$name.EXE",'--observation-timeout-ms','12000') -WorkingDirectory O:\winnt -WindowStyle Hidden -PassThru
        try {
            if(!$p.WaitForExit(30000)){throw "Observer timeout: $name"}
            $text=Get-Content $report -Raw
            $launcher=[int]([regex]::Match($text,'(?m)^pid=(\d+)').Groups[1].Value)
            if(!$launcher){throw 'Missing test launcher identity'}
            if($ObserveKnownFault -and $name -eq 'FAULT'){
                if($text -notmatch '(?m)^result=timeout'){throw 'Known original NTVDM fault boundary changed; inspect before accepting'}
                $results += [pscustomobject]@{Name=$name;Result='known-fault-timeout-not-functional-pass';Report=$report}
                Write-Output "OBSERVED original fault timeout: $name (not a functional pass)"
            } else {
                if($text -notmatch '(?m)^result=exited' -or $text -notmatch '(?m)^exit=0x0+\r?$'){throw "Guest failed: $name; inspect $report"}
                $results += [pscustomobject]@{Name=$name;Result='pass';Report=$report}
                Write-Output "PASS guest $name"
            }
        } finally {
            if($launcher){
                $owned=@($launcher)
                for($depth=0;$depth -lt 5;$depth++){
                    $children=@(PackageProcesses | Where-Object {$_.ParentProcessId -in $owned -and $_.ProcessId -notin $owned})
                    if(!$children.Count){break}
                    $owned+=@($children.ProcessId)
                }
                foreach($id in ($owned | Sort-Object -Descending)){
                    if(PackageProcesses | Where-Object {$_.ProcessId -eq $id}){Stop-Process -Id $id}
                }
            }
        }
        if((PackageProcesses).Count){throw 'Unowned session remains; stopping'}
    }
} finally {
    subst Z: /d
    $results | ConvertTo-Json | Set-Content "O:\winnt\logs\$LogPrefix-summary.json" -Encoding utf8
}
