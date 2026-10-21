param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$Fixture,
    [Parameter(Mandatory)][string]$Report
)
$ErrorActionPreference='Stop'
$start=[Diagnostics.ProcessStartInfo]::new((Resolve-Path $Observer).Path)
$start.UseShellExecute=$false;$start.WindowStyle=[Diagnostics.ProcessWindowStyle]::Hidden
$start.Environment['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
$image=(Resolve-Path $Fixture).Path
foreach($arg in @($image,[IO.Path]::GetDirectoryName($image),$Report,
    '--observation-timeout-ms','30000','--restore-failures')){$start.ArgumentList.Add($arg)}
$child=[Diagnostics.Process]::Start($start)
try {
    if(!$child.WaitForExit(40000)){throw "Owned restoration observer still active: $($child.Id)"}
    if($child.ExitCode -or (Get-Content $Report -Raw) -notmatch 'exit=0x00000000' -or
        (Get-Content "$Report.console.txt" -Raw) -notmatch 'PASS restoration API faults'){
        throw 'Frontend restoration assertions failed'
    }
    'PASS frontend restoration independent API failure matrix'
}finally{$child.Dispose()}
