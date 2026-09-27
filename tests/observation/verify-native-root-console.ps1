[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$BuildRoot,
    [string]$PackageRoot='O:\winnt',
    [Parameter(Mandatory)][string]$LogPrefix,
    [string[]]$SelectedCases
)
$ErrorActionPreference='Stop'
if($LogPrefix -notmatch '^[a-z0-9-]+$'){throw 'Invalid log prefix'}
$Observer=(Resolve-Path -LiteralPath $Observer).Path
$BuildRoot=(Resolve-Path -LiteralPath $BuildRoot).Path
$PackageRoot=(Resolve-Path -LiteralPath $PackageRoot).Path
$launcher=Join-Path $BuildRoot 'run16.exe'
$broker=Join-Path $BuildRoot 'ntsrv.exe'
$nested=(Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..\app\native-console-nested.cmd')).Path
$guiBatch=(Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..\app\frontend-gui-boundary.cmd')).Path
$gui=(Resolve-Path -LiteralPath (Join-Path $BuildRoot 'frontend-gui-boundary-test.exe')).Path
$guiOutput=Join-Path $PackageRoot "logs\$LogPrefix-gui-output.txt"
$segments=(Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..\app\frontend-segments.cmd')).Path
$identity=(Resolve-Path -LiteralPath (Join-Path $BuildRoot 'frontend-bootstrap-test.exe')).Path
if(Test-Path -LiteralPath $guiOutput){throw 'Use a fresh GUI output prefix'}
# No candidate deployment and no desktop switch. An incompatible shared broker
# must not be mistaken for evidence about the new native presentation path.
if(@(Get-CimInstance Win32_Process -Filter "Name='ntsrv.exe'" |
    Where-Object {$_.ExecutablePath -ne $broker}).Count){throw 'Another broker is in use'}
$oldPrivate=$env:MVDM_OBSERVER_PRIVATE_DESKTOP
$oldHistory=$env:MVDM_OBSERVER_SHORT_HISTORY
$cases=@(
    @{Name='output';Args=@('cmd.exe','/d','/c','echo ROOT-NATIVE-VISIBLE & exit /b 37');Code='00000025';Text='ROOT-NATIVE-VISIBLE'},
    @{Name='input';Args=@('cmd.exe','/d');Input="echo ROOT-INPUT-OK`rexit /b 23`r";Code='00000017';Text='ROOT-INPUT-OK'},
    @{Name='shell';Args=@('echo','ROOT-SHELL-VISIBLE');Code='00000000';Text='ROOT-SHELL-VISIBLE'},
    @{Name='native-nested';Args=@('cmd.exe','/d','/c',$nested,$launcher);Code='00000017';Text='INNER-NATIVE-RETURNED'},
    @{Name='native-interactive';Args=@('cmd.exe','/d');Input=('"'+$launcher+'" cmd.exe /d'+"`recho INNER-INTERACTIVE-OK`rexit /b 37`recho OUTER-INTERACTIVE-RETURN-%errorlevel%`rexit /b 23`r");Code='00000017';Text='OUTER-INTERACTIVE-RETURN-37';InputMarker='OUTER-INTERACTIVE-RETURN-37'},
    @{Name='history';Args=@('cmd.exe','/d','/c','echo ROOT-HISTORY-OK & exit /b 19');Code='00000013';Text='ROOT-HISTORY-OK';History=$true},
    @{Name='gui-boundary';Args=@('cmd.exe','/d','/c',$guiBatch,$launcher,$gui,$guiOutput);Code='00000025';Text='GUI-NO-FRONTEND-CAPABILITY';ExtraText='GUI-RETURN-37'},
    @{Name='gui-segments';Args=@('cmd.exe','/d','/c',$segments,$launcher,$gui,$identity,($guiOutput+'.segments'));Code='00000025';Text='GUI-NO-FRONTEND-CAPABILITY';Segments=$true}
)
foreach($name in $SelectedCases){if($name -notin $cases.Name){throw "Unknown case $name"}}
try {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    foreach($case in $cases){
        if($SelectedCases -and $case.Name -notin $SelectedCases){continue}
        $report=Join-Path $PackageRoot "logs\$LogPrefix-$($case.Name).txt"
        if(Test-Path -LiteralPath $report){throw 'Use a fresh log prefix'}
        $env:MVDM_OBSERVER_SHORT_HISTORY=if($case.History){'1'}else{$null}
        $parameters=@($launcher,$PackageRoot,$report,'--observation-timeout-ms','20000')
        if($case.Input){$parameters+=@('--observe-console-input-text',$case.Input)}
        $parameters+=$case.Args
        & $Observer @parameters
        if($LASTEXITCODE){throw "Observer failed: $LASTEXITCODE"}
        $record=Get-Content -LiteralPath $report -Raw
        $screen=Get-Content -LiteralPath ($report+'.console.txt') -Raw
        if($record -notmatch '(?m)^result=exited' -or $record -notmatch "(?m)^exit=0x$($case.Code)\r?$" -or
            $screen -notmatch ([regex]::Escape($case.Text))){throw "Native root $($case.Name) failed: inspect $report"}
        $inputMarker=if($case.InputMarker){$case.InputMarker}else{'ROOT-INPUT-OK'}
        if($case.Input -and ($record -notmatch '(?m)^scripted-console-input=delivered' -or
            $screen -notmatch ('(?m)^\[\d+\] '+[regex]::Escape($inputMarker)+'\r?$'))){throw 'No executed input/output witness'}
        if($case.History -and ($screen -notmatch 'prior shell output' -or
            $screen -notmatch '^# buffer=80,300')){throw 'Prior Console history or geometry lost'}
        if($case.ExtraText -and $screen -notmatch ([regex]::Escape($case.ExtraText))){throw 'Missing GUI direct-target result witness'}
        if($case.Segments){
            if([regex]::Matches($screen,'GUI-NO-FRONTEND-CAPABILITY').Count -ne 3){throw 'Not all three GUI boundaries executed'}
            $owners=@{}
            foreach($label in @('BEFORE','INNER','AFTER')){
                $matches=[regex]::Matches($screen,'FRONTEND-'+$label+'=(\d+)')
                if($matches.Count -ne 1){throw "Missing/unexpected frontend identity count: $label"}
                $owners[$label]=[uint32]$matches[0].Groups[1].Value
            }
            if(!$owners.BEFORE -or !$owners.INNER -or $owners.BEFORE -ne $owners.AFTER -or
                $owners.BEFORE -eq $owners.INNER){throw 'GUI-separated character sessions share/lost frontend identity'}
        }
        if($screen -match 'not recognized as an internal|Bad command or filename'){throw 'Command failed despite exit status'}
        Write-Output "PASS ordinary root CLI $($case.Name): visible text and actual exit $($case.Code)"
    }
} finally {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP=$oldPrivate
    $env:MVDM_OBSERVER_SHORT_HISTORY=$oldHistory
}
