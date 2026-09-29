[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$BuildRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$LogPrefix,
    [string]$LogRoot='O:/winnt/logs',
    [switch]$ExpandedBackend
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$BuildRoot=(Resolve-Path -LiteralPath $BuildRoot).Path
$Observer=(Resolve-Path -LiteralPath $Observer).Path
$LogRoot=(Resolve-Path -LiteralPath $LogRoot).Path
if(!$BuildRoot.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase)){
    throw 'Use the repository build cache, not a published package'
}
if($LogPrefix -notmatch '^[a-z0-9-]+$'){throw 'Invalid log prefix'}
# These are native transport/component contracts, not DOS or RPC acceptance.
# Nonzero expected results witness the actual callback/forced-close branches.
$cases=@(
    @{Name='console-frontend-test';Witness='real Console stream'},
    @{Name='console-video-test';Witness='copied frame chunks'},
    @{Name='console-pointer-contract-test';Witness='mock host only'},
    @{Name='frontend-scope-lifetime-test';Witness='final join complete'},
    @{Name='frontend-request-client-test';Witness='without UI ownership'},
    @{Name='native-console-capture-test';Witness='native Console presentation'},
    @{Name='console-channel-lifetime-test';Witness='85 real channel lifetimes'},
    @{Name='console-client-test';Suffix='normal';Exit='00000049';Witness='original-shape close callback'},
    @{Name='console-client-test';Suffix='broken';Arg='--broken-pipe';Witness='native error and idle frontend loss'},
    @{Name='console-client-test';Suffix='close-hang';Arg='--close-hang';Exit='c000013a';Witness='original-shape close callback'}
)
if($ExpandedBackend){
    & (Join-Path $repo 'tests/observation/verify-ntcon-console-state.ps1') `
        -BuildRoot ((Join-Path $BuildRoot ($LogPrefix+'-ntcon-state')).Substring($repo.Length+1)) `
        -LogPath (Join-Path $LogRoot ($LogPrefix+'-ntcon-state.log'))
    foreach($test in @('ntcon-text-frame','ntcon-presentation','frontend-text-handoff')) {
        $log=Join-Path $LogRoot ($LogPrefix+'-'+$test+'.log')
        if(Test-Path -LiteralPath $log){throw "Refusing to overwrite $log"}
        $process=Start-Process -FilePath (Join-Path $BuildRoot ($test+'-test.exe')) `
            -ArgumentList ('"'+$log+'"') -WindowStyle Hidden -PassThru
        if(!$process.WaitForExit(30000)){Stop-Process -Id $process.Id;throw "Timeout: $test"}
        if($process.ExitCode -or !(Select-String -LiteralPath $log -Pattern 'checks=\d+ failures=0' -Quiet)) {
            throw "Failed: $test"
        }
        Get-Content -LiteralPath $log | Select-Object -Last 1
    }
    # These are boundary tests, not a substitute for real native request,
    # surviving-client, management/isolation and DOS/native round-trip gates.
}
$previous=$env:MVDM_OBSERVER_PRIVATE_DESKTOP
$failures=[Collections.Generic.List[string]]::new()
try {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    foreach($case in $cases){
        $name=$case.Name
        if($case.Suffix){$name+='-'+$case.Suffix}
        $report=Join-Path $LogRoot ($LogPrefix+'-'+$name+'.txt')
        if(Test-Path -LiteralPath $report){throw ('Refusing to overwrite run evidence: '+$report)}
        try {
        $arguments=@((Join-Path $BuildRoot ($case.Name+'.exe')),$BuildRoot,$report,
            '--observation-timeout-ms','30000')
        if($case.Arg){$arguments+=$case.Arg}
        & $Observer @arguments
        if($LASTEXITCODE){throw ('Observer failed: '+$name)}
        $expected=if($case.Exit){$case.Exit}else{'00000000'}
        $result=Get-Content -LiteralPath $report
        if($result -notcontains 'result=exited' -or $result -notcontains ('exit=0x'+$expected)){
            throw ('Unexpected result: '+$name)
        }
        $screen=Get-Content -LiteralPath ($report+'.console.txt') -Raw
        # The observer records screen rows, so a word may wrap mid-token.
        # Strip its row labels and whitespace, not program output characters.
        $cells=($screen -replace '(?m)^\[\d+\] ','') -replace '\s',''
        $witness=$case.Witness -replace '\s',''
        if($screen -notmatch 'PASS' -or !$cells.Contains($witness)){
            throw ('Missing capability witness: '+$name)
        }
        Write-Output ('PASS '+$name+' expected exit '+$expected)
        } catch {
            $failures.Add($name+': '+$_.Exception.Message)
            Write-Warning ('FAIL '+$name+': '+$_.Exception.Message)
        }
    }
    if($failures.Count){throw ('Frontend contract failures: '+($failures -join '; '))}
} finally {$env:MVDM_OBSERVER_PRIVATE_DESKTOP=$previous}
