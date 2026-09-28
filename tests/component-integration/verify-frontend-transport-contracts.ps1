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
    @{Name='native-console-frontend-test';Witness='bounds ConPTY cleanup'},
    @{Name='console-channel-lifetime-test';Witness='85 real channel lifetimes'},
    @{Name='console-client-test';Suffix='normal';Exit='00000049';Witness='original-shape close callback'},
    @{Name='console-client-test';Suffix='broken';Arg='--broken-pipe';Witness='native error and idle frontend loss'},
    @{Name='console-client-test';Suffix='close-hang';Arg='--close-hang';Exit='c000013a';Witness='original-shape close callback'}
)
$backendCases=@(
    @{Name='native-console-backend-test';Witness='NATIVE-BACKEND PASS'},
    @{Name='native-console-frontend-test';Suffix='concurrent';Arg='--concurrent';Witness='real ConPTY child output survives frontend DOS screen transaction'},
    @{Name='native-console-frontend-test';Suffix='controls';Arg='--controls';Witness='default exit and paused-DOS I/O state'},
    @{Name='native-console-members-test';Witness='cancellation is not empty membership'}
)
if($ExpandedBackend){
    # Replaces retired helper protocol/EOF/control tests with their actual
    # ConPTY resource contracts: stream masks/aliases/failures, child/descendant
    # completion, explicit close, raw/cooked/control input and retained reuse.
    # Build from current production sources; never accept an old passing log.
    $resourceRoot=Join-Path $BuildRoot ($LogPrefix+'-conpty-resource')
    & (Join-Path $repo 'tests/observation/verify-conpty-launch.ps1') `
        -BuildRoot $resourceRoot.Substring($repo.Length+1) -LogPath (Join-Path $LogRoot ($LogPrefix+'-conpty-resource.log'))
    $cases+=$backendCases
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
