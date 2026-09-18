[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$FixtureRoot,
    [string]$LogPrefix='m0-t413-s4-video',
    [string]$VideoGuestFixturePath,
    [ValidateSet('int10', 'direct-vram')][string]$VideoGuestRoute='int10',
    [switch]$VideoOnly
)
$ErrorActionPreference='Stop'
$FixtureRoot=(Resolve-Path $FixtureRoot).Path
if($LogPrefix -notmatch '^[a-z0-9-]+$'){throw 'Invalid prefix'}
function Assert-GuestConsoleTranscript {
    param(
        [Parameter(Mandatory)][string]$Path,
        [Parameter(Mandatory)][string]$Case,
        [bool]$RequireEditor = $true,
        [int]$MinimumMemReports = 3
    )
    if (!(Test-Path -LiteralPath $Path -PathType Leaf)) {
        throw "Missing guest Console transcript: $Case"
    }
    $transcript=Get-Content -LiteralPath $Path -Raw
    # The observer's process exit says only that the host-side test tree
    # finished.  These are guest-owned COMMAND/MEM/EDIT witnesses captured
    # from the same ConPTY stream; do not let command-resolution failure
    # masquerade as a successful geometry run.
    foreach($unexpected in @('Bad command or filename','is not recognized as an internal or external command')) {
        if ($transcript -match [regex]::Escape($unexpected)) {
            throw "Guest Console reported command-resolution failure ($Case): $unexpected"
        }
    }
    $markers=@('Microsoft(R) Windows NT DOS','bytes total conventional memory')
    if($RequireEditor){$markers += 'MS-DOS Editor'}
    foreach($marker in $markers) {
        if ($transcript -notmatch [regex]::Escape($marker)) {
            throw "Missing guest Console marker ($Case): $marker"
        }
    }
    if ([regex]::Matches($transcript,[regex]::Escape('bytes total conventional memory')).Count -lt $MinimumMemReports) {
        throw "Missing required MEM guest evidence: $Case"
    }
}
$rows=if($VideoOnly){@()}else{@(@(80,25,''),@(45,34,''),@(60,50,''),@(120,30,''),@(80,25,'--mouse'),@(80,25,'--resize'))}
foreach($row in $rows){
    $tag=if($row[2]){$row[2].Substring(2)}else{"$($row[0])x$($row[1])"}
    $log="O:\winnt\logs\$LogPrefix-$tag.raw"
    if(Test-Path $log){throw 'Use fresh log prefix'}
    $args=@($row[0],$row[1],$log);if($row[2]){$args+=$row[2]}
    $p=Start-Process -FilePath (Join-Path $FixtureRoot 'terminal-observer.exe') -ArgumentList $args -WorkingDirectory O:\winnt -WindowStyle Hidden -RedirectStandardOutput "$log.runner.txt" -RedirectStandardError "$log.stderr.txt" -PassThru
    if(!$p.WaitForExit(55000)){throw "Terminal observer timeout: $tag"}
    $result=Get-Content "$log.runner.txt" -Raw
    if($result -notmatch '(?m)^wait=0 exit=1\r?$'){throw "Terminal observer failed: $tag; $result"}
    Assert-GuestConsoleTranscript -Path $log -Case $tag
    & "$PSScriptRoot/Verify-ConsoleResizeCapture.ps1" -Capture $log
    Write-Output "PASS video $tag"
}
if($VideoOnly -and !$VideoGuestFixturePath) {throw 'VideoOnly requires VideoGuestFixturePath'}
if($VideoGuestFixturePath) {
    $videoGuest=(Resolve-Path -LiteralPath $VideoGuestFixturePath).Path
    if($videoGuest -notmatch '\\build\\M[0-9]+-T[0-9]+\\S[0-9]+\\' -or
        [IO.Path]::GetFileName($videoGuest) -ne 'VIDTST.COM') {
        throw 'Video guest fixture must be the task-owned build VIDTST.COM artifact'
    }
    $videoDrive=@('W:','V:','U:','T:') | Where-Object { -not (Test-Path "$_\\") } | Select-Object -First 1
    if(!$videoDrive){throw 'No reserved short DOS fixture drive is available'}
    $log="O:\winnt\logs\$LogPrefix-video-$VideoGuestRoute.raw"
    if(Test-Path $log){throw 'Use fresh log prefix'}
    subst $videoDrive (Split-Path -Parent $videoGuest)
    if($LASTEXITCODE){throw 'Could not map short DOS video-fixture drive'}
    $savedVideoCommand=$env:MVDM_TEST_VIDEO_COMMAND
    try {
        $env:MVDM_TEST_VIDEO_COMMAND="$videoDrive\VIDTST.COM"
        $p=Start-Process -FilePath (Join-Path $FixtureRoot 'terminal-observer.exe') -ArgumentList 80,25,$log,'--video-int10' -WorkingDirectory O:\winnt -WindowStyle Hidden -RedirectStandardOutput "$log.runner.txt" -RedirectStandardError "$log.stderr.txt" -PassThru
        if(!$p.WaitForExit(55000)){throw "$VideoGuestRoute video observer timeout"}
        $result=Get-Content "$log.runner.txt" -Raw
        if($result -notmatch '(?m)^video-int10 wait=0 exit=1\r?$'){throw "$VideoGuestRoute guest did not return through COMMAND: $result"}
        Assert-GuestConsoleTranscript -Path $log -Case "video-$VideoGuestRoute" -RequireEditor:$false -MinimumMemReports 1
        foreach($marker in @('VVVV','S23I','S23_INT10_WRITER_OK')) {
            if((Get-Content -LiteralPath $log -Raw) -notmatch [regex]::Escape($marker)) {
                throw "Missing $VideoGuestRoute guest marker: $marker"
            }
        }
        if($VideoGuestRoute -eq 'direct-vram') {
            Write-Output 'PASS video direct-VRAM guest read/write/presentation'
        } else {
            Write-Output 'PASS video INT 10 guest write/scroll'
        }
    } finally {
        $env:MVDM_TEST_VIDEO_COMMAND=$savedVideoCommand
        subst $videoDrive /d
    }
}
if(!$VideoOnly) {
    $saved=$env:MVDM_OBSERVER_SHORT_HISTORY
    try {
        $env:MVDM_OBSERVER_SHORT_HISTORY='1'
        foreach($i in 1..5){
            $prefix="$LogPrefix-short-$i"
            & "$PSScriptRoot/Verify-CommandExitStatus.ps1" -Observer (Join-Path $FixtureRoot 'observer.exe') -Cases edit -LogPrefix $prefix
            $report="O:\winnt\logs\$prefix-edit.txt"
            $geometry=Get-Content "$report.geometry.txt" -Raw
            if($geometry -notmatch 'buffer=80,300 view=0,36,79,40 cursor=0,40 prefill=40'){throw "Short geometry mismatch: $geometry"}
            foreach($line in 1..4){
                $screen=Get-Content ("$report.line-{0:D2}.console.txt" -f $line) -Raw
                if($screen -notmatch 'MS-DOS resident in High Memory'){throw 'Missing post-EDIT MEM completion'}
            }
            Write-Output "PASS native short-window $i"
        }
    } finally {$env:MVDM_OBSERVER_SHORT_HISTORY=$saved}
}
