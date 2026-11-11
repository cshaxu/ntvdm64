[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$PackageRoot,
    [string]$ProcessPackageRoot,
    [Parameter(Mandatory)][string]$LogPrefix,
    [ValidateRange(1,100)][int]$Iterations=4
)
$ErrorActionPreference='Stop'
if($LogPrefix -notmatch '^[a-z0-9-]+$'){throw 'Invalid log prefix'}
$PackageRoot=(Resolve-Path -LiteralPath $PackageRoot).Path
$Observer=(Resolve-Path -LiteralPath $Observer).Path
$paths=@('run16.exe','ntcon.exe','ntvdm.exe','ntsrv.exe') |
    ForEach-Object {Join-Path $PackageRoot $_}
if($ProcessPackageRoot){
    $ProcessPackageRoot=(Resolve-Path -LiteralPath $ProcessPackageRoot).Path
    foreach($name in @('run16.exe','ntcon.exe','ntvdm.exe','ntsrv.exe')){
        $physical=Join-Path $ProcessPackageRoot $name
        if((Get-FileHash $physical).Hash -ne (Get-FileHash (Join-Path $PackageRoot $name)).Hash){
            throw "Process package differs from launch package: $name"
        }
        $paths+=$physical
    }
}
function Get-TestProcesses {
    @(Get-CimInstance Win32_Process | Where-Object {$_.ExecutablePath -in $paths})
}
if((Get-TestProcesses).Count){throw 'Candidate package already in use'}
if(@(Get-CimInstance Win32_Process -Filter "Name='ntsrv.exe'").Count){throw 'Another broker is running'}
$oldPrivate=$env:MVDM_OBSERVER_PRIVATE_DESKTOP
try {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    for($iteration=1;$iteration -le $Iterations;$iteration++){
        foreach($case in @('direct-mem','mem','native-zero')){
            # A new broker, frontend and worker for every case. Warm successes
            # cannot stand in for startup coverage. The observer captures live
            # child stacks on timeout before this exact-package cleanup.
            if((Get-TestProcesses).Count){throw 'Cold start prerequisite failed: previous package process is live'}
            try {
                & "$PSScriptRoot/../../tools/audit/Verify-CommandExitStatus.ps1" `
                    -Observer $Observer -PackageRoot $PackageRoot `
                    -ProcessPackageRoot $ProcessPackageRoot `
                    -LogPrefix "$LogPrefix-$iteration-$case" -Cases $case
            } finally {
                foreach($entry in Get-TestProcesses){
                    $process=Get-Process -Id $entry.ProcessId -ErrorAction SilentlyContinue
                    if($process){
                        try {$null=$process.Handle;$process.Kill();$null=$process.WaitForExit(5000)}
                        finally {$process.Dispose()}
                    }
                }
            }
            if((Get-TestProcesses).Count){throw 'Candidate cleanup incomplete'}
        }
    }
    Write-Output "PASS $($Iterations*3) isolated cold starts; this alone does not explain historical timeouts"
} finally {$env:MVDM_OBSERVER_PRIVATE_DESKTOP=$oldPrivate}
