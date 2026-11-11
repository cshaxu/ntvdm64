[CmdletBinding()]
param(
    [Parameter(Mandatory=$true)][string]$BuildRoot,
    [Parameter(Mandatory=$true)][string]$Observer,
    [Parameter(Mandatory=$true)][string]$LogRoot
)
$ErrorActionPreference='Stop'
if(!(Test-Path -LiteralPath $LogRoot -PathType Container)){throw 'Create the admitted build log directory first'}
$cases=@(
    @{Image='frontend-window-mouse-test.exe';Name='mouse';Args=@()},
    @{Image='frontend-window-keyboard-test.exe';Name='keyboard';Args=@('--cooked')},
    @{Image='frontend-window-controller-test.exe';Name='controller';Args=@()},
    @{Image='console-frontend-test.exe';Name='codec';Args=@()},
    @{Image='ntvwm-text-frame-test.exe';Name='native-mouse';Args=@((Join-Path $LogRoot 'native-mouse.txt'))},
    @{Image='ntvwm-presentation-test.exe';Name='presentation';Args=@((Join-Path $LogRoot 'presentation.txt'))},
    @{Image='ntvwm-presentation-test.exe';Name='input-return';Args=@((Join-Path $LogRoot 'input-return.txt'),'--input-return')}
)
foreach($case in $cases){
    $report=Join-Path $LogRoot ($case.Name+'.observer')
    if(Test-Path -LiteralPath $report){throw "Do not overwrite evidence: $report"}
    $start=[Diagnostics.ProcessStartInfo]::new($Observer)
    $start.UseShellExecute=$false;$start.WindowStyle=[Diagnostics.ProcessWindowStyle]::Hidden
    $start.Environment['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
    foreach($arg in @((Join-Path $BuildRoot $case.Image),$BuildRoot,$report)+$case.Args+@('--observation-timeout-ms','45000')){
        $start.ArgumentList.Add($arg)
    }
    $process=[Diagnostics.Process]::Start($start)
    try{
        if(!$process.WaitForExit(55000)){throw "Fixture timeout: $($case.Name)"}
        if(!(Test-Path -LiteralPath $report)){throw "Missing observer report: $($case.Name)"}
        $result=Get-Content -LiteralPath $report -Raw
        if($process.ExitCode -or $result -notmatch 'result=exited' -or $result -notmatch 'exit=0x00000000'){
            throw "Fixture failed: $($case.Name): $result"
        }
        "PASS $($case.Name)"
    }finally{$process.Dispose()}
}
