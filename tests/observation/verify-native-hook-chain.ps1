[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$RuntimeRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$LogRoot,
    [string]$WindowObserver,
    [ValidateSet('x86','x64')][string]$NativeMachine='x86',
    [switch]$IncludeCrossWidth
)
$ErrorActionPreference='Stop'
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$runtime=(Resolve-Path -LiteralPath $RuntimeRoot).Path
$observerPath=(Resolve-Path -LiteralPath $Observer).Path
$log=[IO.Path]::GetFullPath($LogRoot)
$build=(Resolve-Path "$PSScriptRoot/../../build").Path+'\'
if(!$log.StartsWith($build,[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $log)){
    throw 'Require fresh build-only evidence'
}
$binary=Get-PackageBinaryRoot $runtime
$hookName=if($NativeMachine -eq 'x64'){'nthook64.dll'}else{'nthook32.dll'}
if(!(Test-Path (Join-Path $binary $hookName))){throw 'Missing matching hook-enabled package'}
if(Test-Path Z:\){throw 'Z: in use'}
$scope=New-IsolatedPackageScope $runtime
$null=New-Item -ItemType Directory -Path $log
$saved=[Environment]::GetEnvironmentVariable('MVDM_OBSERVER_PRIVATE_DESKTOP')
$savedPost=[Environment]::GetEnvironmentVariable('MVDM_OBSERVER_POST_EXIT_MS')
$savedInput=[Environment]::GetEnvironmentVariable('MVDM_OBSERVER_MILESTONE_INPUT')
$env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
$names=@('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntmon.exe','WOW32.DLL','VDMREDIR.DLL','nthook32.dll')
foreach($name in @('ntvwm.exe','ntvwm32.exe','ntvwm64.exe','nthook64.dll')){
    if(Test-Path (Join-Path $binary $name)){$names+=$name}
}
$identity=foreach($name in $names){[pscustomobject]@{Name=$name;Hash=(Get-FileHash (Join-Path $binary $name)).Hash}}
$identity|ConvertTo-Json|Set-Content "$log/package.json"
& subst.exe Z: $runtime
if($LASTEXITCODE){throw 'SUBST failed'}
$scope.Paths=@($scope.Paths)+@($scope.Paths|ForEach-Object {Join-Path Z:\ $_.Substring($runtime.Length+1)})
# The observer and launcher remain x86. Sysnative selects the actual native64
# image without changing the Hook's legacy search or execution assertions.
$cmd=Join-Path $env:WINDIR $(if($NativeMachine -eq 'x64'){'Sysnative\cmd.exe'}else{'SysWOW64\cmd.exe'})
$minePattern='(?m)^window-utf16=\S+ visible=1 class=(626B96F7|00C900A800C000D7) text=\1\r?$'
$cases=@(
    @{Name='absolute';Command='Z:\system32\command.com /c ver';Marker='MS-DOS Version 5.00.500'},
    @{Name='bare';Command='cd /d Z:\system32 & command.com /c ver';Marker='MS-DOS Version 5.00.500'},
    @{Name='bare-command-stem';Command='cd /d Z:\system32 & command /c ver';Marker='MS-DOS Version 5.00.500'},
    @{Name='bare-mem-stem';Command='cd /d Z:\system32 & mem';Marker='bytes total conventional memory'},
    @{Name='bare-mem-extension';Command='cd /d Z:\system32 & MEM.EXE';Marker='bytes total conventional memory'},
    @{Name='absolute-mem';Command='Z:\system32\MEM.EXE';Marker='bytes total conventional memory'},
    @{Name='mem-parent-return';Command='cd /d Z:\system32 & mem & echo MEM-PARENT-RETURN';Marker='bytes total conventional memory';After='MEM-PARENT-RETURN'},
    @{Name='interactive-mem';Text="cd /d Z:\system32`rmem`recho MEM-INTERACTIVE-RETURN`rexit`r";Marker='bytes total conventional memory';After='MEM-INTERACTIVE-RETURN'},
    @{Name='parent-return';Command='Z:\system32\command.com /c ver & echo HOOK-PARENT-RETURN';Marker='HOOK-PARENT-RETURN'},
    @{Name='explicit-launcher';Command='Z:\system32\run16.exe Z:\system32\command.com /c ver';Marker='MS-DOS Version 5.00.500'}
)
if($IncludeCrossWidth) {
    foreach($name in @('nthook32.dll','nthook64.dll')) {
        if(!(Test-Path (Join-Path $binary $name))){throw 'Cross-width chains require both actual hooks'}
    }
    $oppositeCmd=Join-Path $env:WINDIR $(if($NativeMachine -eq 'x64'){'SysWOW64\cmd.exe'}else{'Sysnative\cmd.exe'})
    # This observer CRT-quotes an entire forwarded /c argument. Embedding
    # another quoted executable then supplies literal backslash-quotes to CMD
    # (whose parser is not the CRT parser). Use the actual short fixture path,
    # as the existing COMMAND/MEM cases do; do not reinterpret product syntax.
    if($oppositeCmd -match '\s'){throw 'Cross-CMD observer fixture requires a host Windows path without spaces'}
    $cases+=@(
        @{Name='cross-native-mem';Command=($oppositeCmd+' /d /c Z:\system32\MEM.EXE & echo CROSS-NATIVE-RETURN');Marker='bytes total conventional memory';After='CROSS-NATIVE-RETURN'},
        @{Name='cross-native-dos';Command=($oppositeCmd+' /d /c Z:\system32\run16.exe Z:\system32\command.com /c ver & echo CROSS-DOS-RETURN');Marker='MS-DOS Version 5.00.500';After='CROSS-DOS-RETURN'}
    )
}
try {
    foreach($case in $cases){
        $report=Join-Path $log ($case.Name+'.txt')
        try {
            $arguments=@('Z:\system32\run16.exe','Z:\',$report,$cmd,'/d')
            if($case.Text){
                $env:MVDM_OBSERVER_MILESTONE_INPUT='1'
                $arguments+=@('--observe-console-input-text',$case.Text,
                    '--observe-console-line-delay-ms','1000')
            }else{$arguments+=@('/c',$case.Command)}
            $arguments+=@('--observation-timeout-ms','20000')
            & $observerPath @arguments
            if($LASTEXITCODE){throw "Observer failed: $($case.Name)"}
            $result=Get-Content -LiteralPath $report -Raw
            $screen=Get-Content -LiteralPath ($report+'.console.txt') -Raw
            if($case.Text -and $result -notmatch '(?m)^scripted-console-input=delivered'){
                throw "Interactive input delivery failed: $($case.Name)"
            }
            if($result -notmatch 'result=exited' -or $result -notmatch 'exit=0x00000000' -or
               !$screen.Contains($case.Marker)){throw "Hook chain assertions failed: $($case.Name)"}
            if($case.After -and ($screen.IndexOf($case.After) -le $screen.IndexOf($case.Marker))){
                throw "Direct DOS completion/parent output order failed: $($case.Name)"
            }
            "PASS $($case.Name): actual $NativeMachine CMD/Windows completion/current Console marker"
        } finally {Stop-IsolatedPackageScope $scope}
    }
    if($WindowObserver){
        $windowProbe=(Resolve-Path -LiteralPath $WindowObserver).Path
        $env:MVDM_OBSERVER_POST_EXIT_MS='5000'
        $report=Join-Path $log 'winmine.txt'
        $start=[Diagnostics.ProcessStartInfo]::new($observerPath)
        foreach($argument in @('Z:\system32\run16.exe','Z:\',$report,$cmd,'/d','/c',
            'Z:\system32\WINMINE.EXE & echo HOOK-WOW-STARTED','--observation-timeout-ms','20000')){
            $start.ArgumentList.Add($argument)
        }
        $start.UseShellExecute=$false;$start.WindowStyle=[Diagnostics.ProcessWindowStyle]::Hidden
        $process=[Diagnostics.Process]::Start($start)
        try {
            # Inspect the actual UI frontier, not a guessed startup duration.
            # The post-exit lease keeps the private desktop available. These
            # bounded read-only samples add no production wait or scheduler.
            $deadline=[DateTime]::UtcNow.AddSeconds(20)
            $windows='';$workers=@()
            do {
                if(!$workers.Count){
                    $workers=@(Get-CimInstance Win32_Process -Filter "Name='ntvdm.exe'" |
                        Where-Object {$_.ExecutablePath -in $scope.Paths})
                }
                $windows+=(@(foreach($worker in $workers){
                    & $windowProbe ([string]$worker.ProcessId) ('NTVDMConsoleTest-'+$process.Id)
                    if($LASTEXITCODE){throw 'WOW window probe failed'}
                }) -join "`n")+"`n"
                if($windows -match $minePattern){break}
                $null=$process.WaitForExit(500)
            } while(!$process.HasExited -and [DateTime]::UtcNow -lt $deadline)
            $windows|Set-Content (Join-Path $log 'winmine-windows.txt')
            if(!$process.WaitForExit(25000)){throw 'WOW chain observer timeout'}
            $result=Get-Content -LiteralPath $report -Raw
            $screen=Get-Content -LiteralPath ($report+'.console.txt') -Raw
            if($process.ExitCode -or $result -notmatch 'result=exited' -or
                $result -notmatch 'exit=0x00000000' -or !$screen.Contains('HOOK-WOW-STARTED') -or
                $windows -notmatch $minePattern){
                throw 'Hook Win16 startup-only receipt/window assertions failed'
            }
            "PASS winmine: $NativeMachine CMD legacy redirect/startup-only receipt/actual Mines window; not gameplay"
        } finally {
            if(!$process.HasExited){$process.Kill();$null=$process.WaitForExit(5000)}
            $process.Dispose();Stop-IsolatedPackageScope $scope
        }
    }
    foreach($row in $identity){if((Get-FileHash (Join-Path $binary $row.Name)).Hash -ne $row.Hash){throw 'Package changed'}}
} finally {
    try {Stop-IsolatedPackageScope $scope} finally {
        & subst.exe Z: /d
        [Environment]::SetEnvironmentVariable('MVDM_OBSERVER_PRIVATE_DESKTOP',$saved)
        [Environment]::SetEnvironmentVariable('MVDM_OBSERVER_POST_EXIT_MS',$savedPost)
        [Environment]::SetEnvironmentVariable('MVDM_OBSERVER_MILESTONE_INPUT',$savedInput)
    }
}
