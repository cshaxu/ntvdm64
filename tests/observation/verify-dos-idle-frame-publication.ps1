param(
    [Parameter(Mandatory=$true)][string]$RuntimeRoot,
    [Parameter(Mandatory=$true)][string]$Observer,
    [Parameter(Mandatory=$true)][string]$ReportRoot,
    [ValidateSet('Console','Window')][string]$Display='Console'
)
$ErrorActionPreference='Stop'
$runtime=(Resolve-Path $RuntimeRoot).Path
$observerPath=(Resolve-Path $Observer).Path
$reports=(Resolve-Path $ReportRoot).Path
$build=(Resolve-Path (Join-Path $PSScriptRoot '../../build')).Path+'\'
if(!$reports.StartsWith($build,[StringComparison]::OrdinalIgnoreCase)){
    throw 'Probe reports must remain below repository build/'
}
if(Test-Path Z:\){throw 'Z: already in use'}
if(@(Get-CimInstance Win32_Process -Filter "Name='ntsrv.exe'").Count){throw 'Existing broker; no test takeover'}
function Stop-DiagnosticPackage {
    # Z: is this invocation's validated mapping, and is removed only after
    # cleanup. Windows may report either the mapped or physical image path.
    Get-CimInstance Win32_Process | Where-Object {
        $_.ExecutablePath -and
        ([IO.Path]::GetDirectoryName($_.ExecutablePath) -ieq $runtime -or
         [IO.Path]::GetDirectoryName($_.ExecutablePath) -ieq 'Z:\') -and
        $_.Name -in @('run16.exe','ntsrv.exe','ntvdm.exe','ntcon.exe','ntvwm.exe')
    } | ForEach-Object {Stop-Process -Id $_.ProcessId -Force -ErrorAction SilentlyContinue}
}
# Runtime must contain the existing test-only wire-observed NTVDM at its normal
# name. It observes the production client, not a substitute frame producer.
& subst.exe Z: $runtime
if($LASTEXITCODE){throw 'SUBST failed'}
try {
    foreach($case in @('command','edit')) {
        $prefix=Join-Path $reports $case
        if(Test-Path ($prefix+'.txt')){throw 'Fresh reports required'}
        $start=[Diagnostics.ProcessStartInfo]::new($observerPath)
        $start.UseShellExecute=$false;$start.WindowStyle=[Diagnostics.ProcessWindowStyle]::Hidden
        $start.Environment['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
        $start.Environment['MVDM_TEST_CONSOLE_WIRE_PATH']=$prefix
        $start.Environment.Remove('MVDM_OBSERVER_WINDOW_INPUT') | Out-Null
        if($Display -eq 'Window'){$start.Environment['MVDM_OBSERVER_WINDOW_INPUT']='1'}
        $arguments=@('Z:\run16.exe','Z:\',($prefix+'.txt'),'command',
            '--observation-timeout-ms','12000')
        if($case -eq 'edit'){$arguments+=@('--observe-console-input-text',"edit`r")}
        elseif($Display -eq 'Window'){$arguments+=@('--observe-console-input-text',"ver`r")}
        foreach($argument in $arguments){$start.ArgumentList.Add($argument)}
        $elapsed=[Diagnostics.Stopwatch]::StartNew()
        $process=[Diagnostics.Process]::Start($start)
        try {
            if(!$process.WaitForExit(25000)){throw "Observer timeout PID=$($process.Id)"}
            $elapsed.Stop()
            $report=Get-Content ($prefix+'.txt') -Raw
            if(!$report.Contains('result=timeout') -or !$report.Contains('exit=0x53504354')){throw 'Guest did not stay running'}
            $requests=@(Get-ChildItem ($prefix+'-*.log') | ForEach-Object {
                $workerPid=[regex]::Match($_.Name,'-(\d+)\.log$').Groups[1].Value
                foreach($line in Get-Content $_.FullName) {
                    if($line -match '^(\d+) .* begin .* write requested=\d+ .* error=0 words=00000019,([0-9a-f]{8}),([0-9a-f]{8}),([0-9a-f]{8}),([0-9a-f]{8}),([0-9a-f]{8}),([0-9a-f]{8}),([0-9a-f]{8})') {
                        [pscustomobject]@{Time=[uint64]$matches[1];Pid=$workerPid;
                            Generation=[Convert]::ToUInt32($matches[2],16);Sequence=[Convert]::ToUInt32($matches[3],16);
                            Operation=[Convert]::ToUInt32($matches[4],16);X=[Convert]::ToUInt32($matches[6],16);Y=[Convert]::ToUInt32($matches[7],16)}
                    }
                }
            })
            if(!$requests.Count){throw 'No authenticated worker protocol requests observed'}
            # Resolve VIDEO_BEGIN from the retained I/O25 enum; assert against source,
            # not a guessed message count or receiver paint approximation.
            $enum=Get-Content (Join-Path $PSScriptRoot '../../src/common/protocol/console_io.h') -Raw
            $names=[regex]::Matches(($enum -split 'enum console_io_operation')[1].Split('}')[0],'CONSOLE_IO_[A-Z_]+')
            $video=1
            while($names[$video-1].Value -ne 'CONSOLE_IO_VIDEO_BEGIN'){$video++}
            $first=($requests|Measure-Object Time -Minimum).Minimum
            $late=@($requests|Where-Object {$_.Time -ge $first+4000 -and $_.Time -le $first+9000})
            $frames=@($requests|Where-Object {$_.Operation -eq $video -and $_.X -eq 1}|Sort-Object Time,Sequence)
            $initialFrames=$frames.Count
            $screen=Get-Content ($prefix+'.txt.console.txt') -Raw
            # A healthy idle video worker may emit no queries either. Prove the
            # bounded live observation and real output, not artificial traffic.
            if($elapsed.ElapsedMilliseconds -lt 11000 -or ($Display -eq 'Console' -and $case -eq 'command' -and $screen -notmatch 'Microsoft\(R\) Windows NT DOS') -or
                ($Display -eq 'Console' -and $case -eq 'edit' -and $screen -notmatch 'Welcome to the MS-DOS Editor') -or
                ($Display -eq 'Window' -and !$initialFrames)){throw 'Missing actual guest output or continuing guest observation'}
            if($Display -eq 'Window'){
                $caf=Get-Content ($prefix+'.txt.caf.txt') -Raw
                if($caf -notmatch 'caf-visible-window=1' -or $caf -notmatch 'result=pass error=0'){
                    throw 'Window was not actually observed'
                }
            }
            $previous=@{};$changed=0;$redundant=0
            foreach($frame in $frames){
                $key="$($frame.Pid):$($frame.Generation)"
                $path="$prefix-$($frame.Pid)-$($frame.Generation)-$($frame.Sequence).frame"
                $bytes=[IO.File]::ReadAllBytes($path)
                # Copied description: five uint32 fields, 256 palette values,
                # kind. Followed by the actual acknowledged text payload.
                $headerBytes=262*4
                if($bytes.Length -lt $headerBytes+36 -or $bytes.Length -ne $headerBytes+[BitConverter]::ToUInt32($bytes,16)){
                    throw 'Truncated real frame snapshot'
                }
                $exact=[Convert]::ToBase64String($bytes)
                $isLate=$frame.Time -ge $first+4000 -and $frame.Time -le $first+9000
                if($previous.ContainsKey($key)){
                    $prior=$previous[$key]
                    $mutations=@($requests|Where-Object {
                        $_.Pid -eq $frame.Pid -and $_.Generation -eq $frame.Generation -and
                        $_.Sequence -gt $prior.Sequence -and $_.Sequence -lt $frame.Sequence -and
                        (($_.Operation -eq 3 -and ($_.X -ne $prior.X -or $_.Y -ne $prior.Y)) -or
                         $_.Operation -in @(1,4,5,6,7,8,14,15,16,17,46) -or
                         ($_.Operation -eq $video -and $_.X -ne 1))
                    })
                    if($isLate -and $exact -eq $prior.Exact -and !$mutations.Count){$redundant++}
                    if($isLate -and $exact -ne $prior.Exact){$changed++}
                }
                $previous[$key]=@{Exact=$exact;Sequence=$frame.Sequence;
                    X=[BitConverter]::ToInt32($bytes,$headerBytes+8);Y=[BitConverter]::ToInt32($bytes,$headerBytes+12)}
            }
            if($redundant){throw "Idle $case still publishes $redundant identical frames without a modifying operation"}
            "$case idle-redundant-frame-publications=0 interval-ms=5000 total-publications=$initialFrames changed-idle-publications=$changed other-requests=$($late.Count)" |
                Set-Content ($prefix+'-assertions.txt')
            "PASS $case real guest idle publication"
        }finally{$process.Dispose()}
        # Stop only this diagnostic package's product processes after the
        # bounded idle observation; never terminate an unrelated application.
        Stop-DiagnosticPackage
    }
}finally{
    # Failure is not a pass, but must not leave this private diagnostic package
    # holding the singleton broker. Validate the mapped executable's owner.
    Stop-DiagnosticPackage
    & subst.exe Z: /d
}
