[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$Driver,
    [Parameter(Mandatory)][string]$EvidenceRoot
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$Observer=(Resolve-Path $Observer).Path
$Driver=(Resolve-Path $Driver).Path
$EvidenceRoot=[IO.Path]::GetFullPath($EvidenceRoot)
if(!$EvidenceRoot.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase)){
    throw 'Evidence must remain under repository build'
}
if(Test-Path $EvidenceRoot){throw 'Use a fresh evidence root'}
$null=New-Item -ItemType Directory -Path $EvidenceRoot
$prior=$env:MVDM_OBSERVER_PRIVATE_DESKTOP
try {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    foreach($case in @('selftest','selftest-missing')){
        $report=Join-Path $EvidenceRoot "$case.txt"
        $observation=Join-Path $EvidenceRoot "$case-observer.txt"
        & $Observer $Driver $repo $observation --observation-timeout-ms 15000 "--$case" $report
        if($LASTEXITCODE){throw 'Observer failed'}
        $result=Get-Content $observation -Raw
        $screen=Get-Content ($observation+'.console.txt') -Raw
        $driverReport=Get-Content $report -Raw
        if($result -notmatch '(?m)^result=exited' -or $result -notmatch '(?m)^exit=0x00000000'){
            throw "Driver selftest failed: $case"
        }
        if($case -eq 'selftest'){
            if($screen -notmatch 'PASS INPUT-DRIVER-SELFTEST' -or
                $driverReport -notmatch '(?m)^PASS visible-output-and-input' -or $driverReport -match 'FAIL'){
                throw 'Missing real input/output proof'
            }
        }elseif($screen -notmatch 'PASS MISSING-PROMPT-REJECTED' -or
            $driverReport -notmatch '(?m)^FAIL no visible ready marker' -or $driverReport -match 'PASS|^READY'){
            throw 'Absent prompt was not rejected'
        }
        Write-Output "PASS input-driver $case (native Console fixture, not product integration)"
    }
} finally {$env:MVDM_OBSERVER_PRIVATE_DESKTOP=$prior}
