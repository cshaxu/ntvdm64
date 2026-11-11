[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$PackageRoot,
    [string]$FixtureRoot='O:\winnt\tests',
    [string]$LogRoot='O:\winnt\Logs2',
    [Parameter(Mandatory)][string]$LogPrefix
)
$ErrorActionPreference='Stop'
$Observer=(Resolve-Path -LiteralPath $Observer).Path
$FixtureRoot=(Resolve-Path -LiteralPath $FixtureRoot).Path
$LogRoot=(Resolve-Path -LiteralPath $LogRoot).Path
if($LogPrefix -notmatch '^[a-z0-9-]+$'){throw 'Invalid log prefix'}
if(@(Get-CimInstance Win32_Process -Filter "Name='ntsrv.exe'").Count){
    throw 'Existing broker: do not adopt an unrelated session'
}
$cases=@(
    @{Name='count';Guest='MCOUNT.COM';Marker='CURSOR-COUNT-PASS'},
    @{Name='video';Guest='MCVIDEO.COM';Marker='CURSOR-VIDEO-PASS'},
    @{Name='text';Guest='MCTEXT.COM';Marker='CURSOR-TEXT-PASS';Text=$true},
    @{Name='move';Guest='WMS7.COM';Marker='WINDOW-MOUSE-PASS';Hook=$true},
    @{Name='retire';Guest='WMS7.COM';Marker='WINDOW-MOUSE-PASS';Hook=$true;Retire=$true}
)
$controls=@('MVDM_OBSERVER_PRIVATE_DESKTOP','MVDM_OBSERVER_WINDOW_INPUT',
    'MVDM_OBSERVER_TEXT_CURSOR','MVDM_OBSERVER_MOUSE_HOOK','MVDM_OBSERVER_MOUSE_RETIRE')
$previous=@{}
foreach($name in $controls){$previous[$name]=[Environment]::GetEnvironmentVariable($name)}
try {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    foreach($case in $cases){
        foreach($name in $controls | Select-Object -Skip 1){
            Remove-Item -LiteralPath ('Env:'+$name) -ErrorAction SilentlyContinue
        }
        if($case.Text){$env:MVDM_OBSERVER_WINDOW_INPUT='1';$env:MVDM_OBSERVER_TEXT_CURSOR='1'}
        if($case.Hook){$env:MVDM_OBSERVER_MOUSE_HOOK=Join-Path $FixtureRoot 'S7MOUSE.dll'}
        if($case.Retire){$env:MVDM_OBSERVER_MOUSE_RETIRE='1'}
        $report=Join-Path $LogRoot ($LogPrefix+'-'+$case.Name+'.txt')
        if(Test-Path -LiteralPath $report){throw 'Fresh reports required'}
        & $Observer (Join-Path $PackageRoot 'run16.exe') $PackageRoot $report `
            --observation-timeout-ms 60000 (Join-Path $FixtureRoot $case.Guest)
        if($LASTEXITCODE){throw "Observer failed: $($case.Name), exit=$LASTEXITCODE"}
        $result=Get-Content -LiteralPath $report -Raw
        if($result -notmatch '(?m)^result=exited\r?$' -or
            $result -notmatch '(?m)^exit=0x00000000\r?$'){
            throw "Guest did not pass: $($case.Name)"
        }
        $texts=@(Get-ChildItem -LiteralPath $LogRoot -File |
            Where-Object {$_.Name.StartsWith([IO.Path]::GetFileName($report))} |
            ForEach-Object {Get-Content -LiteralPath $_.FullName -Raw}) -join "`n"
        if($texts -notmatch [regex]::Escape($case.Marker)){throw "Missing guest marker: $($case.Marker)"}
        "PASS real guest mouse $($case.Name): $($case.Marker)"
    }
} finally {
    foreach($name in $controls){
        if($null -eq $previous[$name]){Remove-Item -LiteralPath ('Env:'+$name) -ErrorAction SilentlyContinue}
        else{Set-Item -LiteralPath ('Env:'+$name) -Value $previous[$name]}
    }
}
