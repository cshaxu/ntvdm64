[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$BuildRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$LogPrefix,
    [switch]$ExpandedBackend
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$BuildRoot=(Resolve-Path -LiteralPath $BuildRoot).Path
$Observer=(Resolve-Path -LiteralPath $Observer).Path
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
    @{Name='native-console-capture-test';Witness='native capture/presentation'},
    @{Name='native-console-frontend-test';Witness='bounds permanently stalled helper cleanup'},
    @{Name='console-channel-lifetime-test';Witness='85 real channel lifetimes'},
    @{Name='console-client-test';Suffix='normal';Exit='00000049';Witness='original-shape close callback'},
    @{Name='console-client-test';Suffix='broken';Arg='--broken-pipe';Witness='native error and idle frontend loss'},
    @{Name='console-client-test';Suffix='close-hang';Arg='--close-hang';Exit='c000013a';Witness='original-shape close callback'}
)
$backendCases=@(
    @{Name='native-console-host-test';Witness='actual result 41'},
    @{Name='native-console-host-test';Suffix='control-input';Arg='--control-input';Witness='helper retained'},
    @{Name='native-console-host-test';Suffix='completion';Arg='--completion';Witness='does not terminate a live native target'},
    @{Name='native-console-host-test';Suffix='close-timeout';Arg='--close-timeout';Witness='peer observes EOF and exits without forced termination'},
    @{Name='native-console-frontend-test';Suffix='controls';Arg='--controls';Witness='default exit and paused-DOS I/O state'},
    @{Name='native-console-members-test';Witness='helper failure is not empty membership'}
)
if($ExpandedBackend){
    # The host fixture deliberately starts its stream child in a different
    # cwd. Supply that empty build-only directory, not a product dependency.
    $null=New-Item -ItemType Directory -Path (Join-Path $BuildRoot 'tests') -Force
    $cases+=$backendCases
}
$previous=$env:MVDM_OBSERVER_PRIVATE_DESKTOP
try {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    foreach($case in $cases){
        $name=$case.Name
        if($case.Suffix){$name+='-'+$case.Suffix}
        $report=Join-Path 'O:/winnt/logs' ($LogPrefix+'-'+$name+'.txt')
        if(Test-Path -LiteralPath $report){throw ('Refusing to overwrite run evidence: '+$report)}
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
    }
} finally {$env:MVDM_OBSERVER_PRIVATE_DESKTOP=$previous}
