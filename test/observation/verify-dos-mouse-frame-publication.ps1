param(
    [Parameter(Mandatory=$true)][string]$RuntimeRoot,
    [Parameter(Mandatory=$true)][string]$Observer,
    [Parameter(Mandatory=$true)][string]$ReportRoot,
    [ValidateSet('Mouse','Registers','Route')][string]$Case='Mouse',
    [string]$GuestPath='O:\winnt\tests\MCTEXT.COM'
)
$ErrorActionPreference='Stop'
$runtime=(Resolve-Path $RuntimeRoot).Path
$observerPath=(Resolve-Path $Observer).Path
$reports=(Resolve-Path $ReportRoot).Path
$build=(Resolve-Path (Join-Path $PSScriptRoot '../../build')).Path+'\'
if(!$reports.StartsWith($build,[StringComparison]::OrdinalIgnoreCase)){throw 'Reports must remain below build/'}
if(Test-Path Z:\){throw 'Z already in use'}
if(@(Get-CimInstance Win32_Process -Filter "Name='ntsrv.exe'").Count){throw 'Existing broker; no test takeover'}
function Stop-TestPackage {
    Get-CimInstance Win32_Process | Where-Object {
        $_.ExecutablePath -and [IO.Path]::GetDirectoryName($_.ExecutablePath) -in @($runtime,'Z:\') -and
        $_.Name -in @('run16.exe','ntsrv.exe','ntvdm.exe','ntcon.exe','ntvwm.exe')
    } | ForEach-Object {Stop-Process -Id $_.ProcessId -Force -ErrorAction SilentlyContinue}
}
$prefix=Join-Path $reports 'mouse'
if(Test-Path "$prefix.txt"){throw 'Fresh reports required'}
& subst.exe Z: $runtime
if($LASTEXITCODE){throw 'SUBST failed'}
try {
    $start=[Diagnostics.ProcessStartInfo]::new($observerPath)
    $start.UseShellExecute=$false;$start.WindowStyle=[Diagnostics.ProcessWindowStyle]::Hidden
    $start.Environment['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
    $start.Environment['MVDM_OBSERVER_WINDOW_INPUT']='1'
    $start.Environment['MVDM_OBSERVER_TEXT_CURSOR']='1'
    $start.Environment['MVDM_TEST_CONSOLE_WIRE_PATH']=$prefix
    if($Case -eq 'Route'){
        $start.Environment['MVDM_OBSERVER_MOUSE_HOOK']='O:\winnt\tests\S7MOUSE.dll'
        $start.Environment['MVDM_OBSERVER_MOUSE_RETIRE']='1'
    }
    foreach($arg in @('Z:\run16.exe','Z:\',"$prefix.txt",'--observation-timeout-ms','60000',$GuestPath)){$start.ArgumentList.Add($arg)}
    $process=[Diagnostics.Process]::Start($start)
    try {
        if(!$process.WaitForExit(65000)){throw 'Observer timeout'}
        $report=Get-Content "$prefix.txt" -Raw
        $screen=Get-Content "$prefix.txt.console.txt" -Raw
        $caf=Get-Content "$prefix.txt.caf.txt" -Raw
        $marker=switch($Case){Mouse {'CURSOR-TEXT-PASS'} Registers {'CURSOR-REGISTERS-PASS'} Route {'CURSOR-ROUTE-PASS'}}
        if($process.ExitCode -or $report -notmatch 'result=exited' -or $report -notmatch 'exit=0x00000000' -or
            $screen -notmatch $marker -or $caf -notmatch 'caf-visible-window=1' -or $caf -notmatch 'result=pass error=0'){
            throw 'Actual Window/guest cursor probe failed'
        }
        # MCTEXT's unchanged blank cells at (20,10)/(21,10) are 0720.
        # INT33 displays XOR 7700, moves once, then hides, with no text writes
        # between these stages. Guest-memory success alone does not prove that
        # the frontend received the intermediate pointer states.
        $frames=@(Get-ChildItem "$prefix-*.frame" | Sort-Object {
            [uint32]([regex]::Match($_.Name,'-(\d+)\.frame$').Groups[1].Value)
        })
        $stage=0;$count=0;$registerGrid=$null
        foreach($frame in $frames){
            $bytes=[IO.File]::ReadAllBytes($frame.FullName)
            $descriptionBytes=1048;$styleBytes=16420
            if($bytes.Length -lt $descriptionBytes+$styleBytes+4000 -or
                [BitConverter]::ToUInt32($bytes,0) -ne 80 -or
                [BitConverter]::ToUInt32($bytes,8) -ne 160 -or
                [BitConverter]::ToUInt32($bytes,1044) -ne 1){throw 'Invalid real text-frame snapshot'}
            $first=[BitConverter]::ToUInt16($bytes,$descriptionBytes+$styleBytes+1640)
            $second=[BitConverter]::ToUInt16($bytes,$descriptionBytes+$styleBytes+1642)
            if($Case -eq 'Registers'){
                $col=[BitConverter]::ToInt32($bytes,1056);$row=[BitConverter]::ToInt32($bytes,1060)
                $startLine=[BitConverter]::ToInt32($bytes,1064);$height=[BitConverter]::ToInt32($bytes,1068)
                $visible=[BitConverter]::ToUInt32($bytes,1080)
                $grid=[Convert]::ToBase64String($bytes, $descriptionBytes+$styleBytes,4000)
                # Original vga_prts::do_new_cursor uses end-start (5-2), not +1.
                if($stage -eq 0 -and $col -eq 20 -and $row -eq 10 -and $startLine -eq 2 -and $height -eq 3 -and $visible -eq 1){$stage=1;$registerGrid=$grid}
                elseif($stage -eq 1 -and $col -eq 20 -and $row -eq 10 -and $visible -eq 0 -and $grid -eq $registerGrid){$stage=2}
                elseif($stage -eq 2 -and $col -eq 21 -and $row -eq 10 -and $startLine -eq 2 -and $height -eq 3 -and $visible -eq 1 -and $grid -eq $registerGrid){$stage=3}
            }elseif($Case -eq 'Route'){
                if($stage -eq 0 -and $first -eq 0x7020){$stage=1}
                elseif($stage -eq 1 -and $first -eq 0x0720){$stage=3}
            }else{
                if($stage -eq 0 -and $first -eq 0x7020 -and $second -eq 0x0720){$stage=1}
                elseif($stage -eq 1 -and $first -eq 0x0720 -and $second -eq 0x7020){$stage=2}
                elseif($stage -eq 2 -and $first -eq 0x0720 -and $second -eq 0x0720){$stage=3}
            }
            $count++
        }
        if($stage -ne 3){throw "Guest passed but actual $Case frames missed ordered states: stage=$stage frames=$count"}
        "PASS actual Window text frames $Case ordered states, without intervening guest text output; frames=$count" |
            Set-Content "$prefix-assertions.txt"
        Get-Content "$prefix-assertions.txt"
    }finally{$process.Dispose()}
}finally{Stop-TestPackage; & subst.exe Z: /d}
