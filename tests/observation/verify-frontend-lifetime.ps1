[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$ProcessPackageRoot,
    [Parameter(Mandatory)][string]$EvidenceRoot,
    [Parameter(Mandatory)][string]$LogPrefix,
    [string]$LogRoot='O:\winnt\logs',
    [switch]$ExpandedFaults,
    [switch]$Window,
    [ValidateSet('normal','frontend','launcher','worker','helper')][string[]]$Cases
)
$ErrorActionPreference='Stop'
if($LogPrefix -notmatch '^[a-z0-9-]+$'){throw 'Invalid log prefix'}
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$Observer=(Resolve-Path $Observer).Path
$PackageRoot=(Resolve-Path $PackageRoot).Path
$ProcessPackageRoot=(Resolve-Path $ProcessPackageRoot).Path
$EvidenceRoot=[IO.Path]::GetFullPath($EvidenceRoot)
$build=(Join-Path $repo 'build')+'\'
if(!$ProcessPackageRoot.StartsWith($build,[StringComparison]::OrdinalIgnoreCase) -or
   !$EvidenceRoot.StartsWith($build,[StringComparison]::OrdinalIgnoreCase)){
    throw 'Use an isolated build candidate and build evidence directory'
}
if(Test-Path $EvidenceRoot){throw 'Use fresh evidence'}
$paths=@()
foreach($name in @('run16.exe','ntkvm.exe','ntvdm.exe','ntsrv.exe')){
    $launch=Join-Path $PackageRoot $name
    $physical=Join-Path $ProcessPackageRoot $name
    if((Get-FileHash $launch).Hash -ne (Get-FileHash $physical).Hash){throw 'Candidate identity mismatch'}
    $paths+=@($launch,$physical)
}
if(@(Get-CimInstance Win32_Process -Filter "Name='ntsrv.exe'").Count){throw 'Broker already running'}
foreach($name in @('NOIOLIFE.EXE','NOIO.COM')){
    if(!(Test-Path (Join-Path $PackageRoot "tests\$name"))){throw "Missing authored fixture $name"}
}
# The authored guest exchanges gate files here; a fresh build-only candidate
# need not already contain this fixture prerequisite. Runtime reports still
# use LogRoot. Never create this directory outside the validated build root.
$gateRoot=Join-Path $ProcessPackageRoot 'logs'
if(!(Test-Path -LiteralPath $gateRoot)){
    $null=New-Item -ItemType Directory -Path $gateRoot
}
$markers=@('NIOREADY','NIOGO','NIODONE','NIOPID','NIOROOT','NIOMEM')
foreach($name in $markers){if(Test-Path (Join-Path $PackageRoot "logs\$name")){throw 'Existing fixture markers'}}
$null=New-Item -ItemType Directory -Path $EvidenceRoot
$oldPrivate=$env:MVDM_OBSERVER_PRIVATE_DESKTOP
$oldWindow=$env:MVDM_LIFETIME_WINDOW
try {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    if($Window){$env:MVDM_LIFETIME_WINDOW='1'}else{Remove-Item Env:MVDM_LIFETIME_WINDOW -ErrorAction SilentlyContinue}
    $selected=@('normal','frontend','launcher','worker')
    if($ExpandedFaults){$selected+='helper'}
    if($Cases){$selected=@($selected | Where-Object {$_ -in $Cases})}
    if(!$selected.Count){throw 'No selected cases; helper requires ExpandedFaults'}
    foreach($case in $selected){
        $report=Join-Path $LogRoot "$LogPrefix-$case.txt"
        if(Test-Path $report){throw 'Use a fresh log prefix'}
        try {
            $arguments=@((Join-Path $PackageRoot 'tests\NOIOLIFE.EXE'),$PackageRoot,$report,
                '--observation-timeout-ms',$(if($ExpandedFaults){'45000'}else{'30000'}))
            if($case -ne 'frontend' -or $ExpandedFaults){$arguments+= $(if($case -eq 'normal'){'--normal'}else{"--$case-loss"})}
            if($ExpandedFaults){$arguments+='--isolation'}
            & $Observer @arguments
            if($LASTEXITCODE){throw "Observer failed for $case"}
            $record=Get-Content $report -Raw
            $screen=Get-Content ($report+'.console.txt') -Raw
            if($record -notmatch '(?m)^result=exited\r?$' -or
               $record -notmatch '(?m)^exit=0x00000000\r?$' -or
               $screen -notmatch 'PASS ' -or $screen -match 'FAIL |Existing marker'){
                throw "Lifetime assertion failed for $case; inspect $report"
            }
            $cells=($screen -replace '(?m)^\[\d+\] ','') -replace '\s',''
            if($Window -and !$cells.Contains('testedfrontendhasactualvisibleWindowbeforelifecycletransition')){
                throw "Missing actual Window before fault: $case"
            }
            if($ExpandedFaults -and !$cells.Contains('distinctunrelatedcharacterfrontendandnativetargetsurvive')){
                throw "Missing unrelated-session survival assertion: $case"
            }
            if($case -eq 'helper' -and !$cells.Contains('originalpipeerrorawaitsexplicitTerminate,DOSresult1067')){
                throw 'Missing mixed helper-loss assertion'
            }
            Write-Output "PASS frontend lifetime $case (real fixture assertions and output)"
        } finally {
            # Only exact candidate images, after the fixture records its verdict.
            # Cleanup is not evidence of product retirement.
            Get-CimInstance Win32_Process | Where-Object {$_.ExecutablePath -in $paths} | ForEach-Object {
                $process=Get-Process -Id $_.ProcessId -ErrorAction SilentlyContinue
                if($process){try {
                    $null=$process.Handle
                    if($process.Path -notin $paths){throw 'Pinned process identity changed'}
                    $process.Kill()
                    if(!$process.WaitForExit(5000)){throw 'Candidate cleanup timeout'}
                } finally {$process.Dispose()}}
            }
            foreach($name in $markers){
                $source=Join-Path $ProcessPackageRoot "logs\$name"
                if(Test-Path $source){Move-Item -LiteralPath $source -Destination (Join-Path $EvidenceRoot "$case-$name")}
            }
        }
    }
} finally {$env:MVDM_OBSERVER_PRIVATE_DESKTOP=$oldPrivate;$env:MVDM_LIFETIME_WINDOW=$oldWindow}
