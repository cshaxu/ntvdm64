[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$MonitorRpc,
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$ProcessPackageRoot,
    [Parameter(Mandatory)][string]$LogRoot,
    [Parameter(Mandatory)][string]$Prefix,
    [ValidateSet('command','cmd')][string]$Target='command'
)
$ErrorActionPreference='Stop'
if($Prefix -notmatch '^[a-z0-9-]+$'){throw 'Invalid report prefix'}
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$stage=$null
$scope=$null
# COMMAND's original real-mode startup has a bounded physical-path contract.
# Stage the already coherent candidate beneath the owned short test root; this
# is test containment only, never a drive alias or a published package path.
$stage=New-PhysicalRuntimeStage (Resolve-Path $ProcessPackageRoot).Path
$scope=New-IsolatedPackageScope $stage
$package=$stage
$binary=Get-PackageBinaryRoot $package
$report=Join-Path $LogRoot "$Prefix.txt"
$result="$report.handoff.txt"
$oldPostExit=$env:MVDM_OBSERVER_POST_EXIT_MS
$oldPrivateDesktop=$env:MVDM_OBSERVER_PRIVATE_DESKTOP
$oldCafHold=$env:MVDM_OBSERVER_CAF_HOLD
$oldWindowInput=$env:MVDM_OBSERVER_WINDOW_INPUT
$oldCafOwnedConsole=$env:MVDM_OBSERVER_CAF_OWNED_CONSOLE
$oldManagementCloseTool=$env:MVDM_OBSERVER_MANAGEMENT_CLOSE_TOOL
$oldOuterCommand=$env:MVDM_OBSERVER_CAF_OUTER_COMMAND
$observerProcess=$null
$frontend=$null
$worker=$null
try {
    if((Test-Path $report) -or (Test-Path $result)){throw 'Use a fresh evidence prefix'}
    $env:MVDM_OBSERVER_POST_EXIT_MS='5000'
    # Exercise the exact owner report: CAF creates NTCON's alternate Console
    # surface and holds its Window open.  The management RPC below, not a
    # synthetic WM_CLOSE, owns the eventual shutdown.
    # The observer owns a fresh AllocConsole.  Exercise CAF against that
    # exact disposable Console rather than the test suite's private-desktop
    # launcher, whose independent strict-startup fixture can fail before
    # NTCON exists.
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP=$null
    $env:MVDM_OBSERVER_CAF_HOLD='1'
    $env:MVDM_OBSERVER_WINDOW_INPUT='1'
    $env:MVDM_OBSERVER_CAF_OWNED_CONSOLE='1'
    # The observer starts this exact client as its own child so the close RPC
    # uses the same logon/LUID scope as the disposable Console and BaseSrv.
    # An external PowerShell test runner can have a different ncalrpc scope.
    $env:MVDM_OBSERVER_MANAGEMENT_CLOSE_TOOL=(Resolve-Path $MonitorRpc).Path
    $outerCmd="$env:SystemRoot\System32\cmd.exe /d /q /k"
    $env:MVDM_OBSERVER_CAF_OUTER_COMMAND=('"'+(Join-Path $binary 'run16.exe')+'" '+$Target+"`r") -replace '\\"','"'
    # Start-Process joins an argument array into one command line.  Keep the
    # outer CMD command (which deliberately contains spaces) as exactly one
    # observer argument, or argv[2]/argv[3] shift and the CAF branch is never
    # reached.
    $observerArguments=('"{0}" "{1}" "{2}" --observation-timeout-ms 30000' -f
        $outerCmd,$package,$report)
    # CAF is a real visible Window-mode transition.  Hiding its Console-owning
    # parent also suppresses LibKvmWindow, which tests a different failure
    # before the management-close edge is reached.
    $observerProcess=Start-Process -FilePath (Resolve-Path $Observer).Path -ArgumentList $observerArguments -PassThru
    $cafReport="$report.caf.txt"
    if(!$observerProcess.WaitForExit(35000)){throw 'Observer did not complete the managed CAF close'}
    $caf=if(Test-Path -LiteralPath $cafReport){Get-Content -LiteralPath $cafReport -Raw}else{''}
    $caf | Set-Content -LiteralPath $result
    if($observerProcess.ExitCode -or
       $caf -notmatch '(?m)^caf-visible-window=1\b' -or
       $caf -notmatch '(?m)^management-console-close-exit=0x00000000$' -or
       $caf -notmatch '(?m)^management-close-complete=1\b' -or
       $caf -notmatch '(?m)^echo-before-enter=1 echo-after-enter=[01]') {
        throw ($Target+' did not complete the authenticated CAF handoff probe')
    }
    'PASS '+$Target+' managed CAF close produced a bounded parent-CMD echo observation'
} finally {
    $env:MVDM_OBSERVER_POST_EXIT_MS=$oldPostExit
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP=$oldPrivateDesktop
    $env:MVDM_OBSERVER_CAF_HOLD=$oldCafHold
    $env:MVDM_OBSERVER_WINDOW_INPUT=$oldWindowInput
    $env:MVDM_OBSERVER_CAF_OWNED_CONSOLE=$oldCafOwnedConsole
    $env:MVDM_OBSERVER_MANAGEMENT_CLOSE_TOOL=$oldManagementCloseTool
    $env:MVDM_OBSERVER_CAF_OUTER_COMMAND=$oldOuterCommand
    if($observerProcess){if(!$observerProcess.HasExited){$observerProcess.Kill();$observerProcess.WaitForExit(5000)|Out-Null};$observerProcess.Dispose()}
    if($worker){$worker.Dispose()}; if($frontend){$frontend.Dispose()}
    if($scope){Stop-IsolatedPackageScope $scope}
    if($stage){Remove-PhysicalRuntimeStage $stage}
}
