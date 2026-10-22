[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$Harness,
 [Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$LogRoot,
 [ValidateSet('cmd','command','ntmon')][string[]]$Targets=@('cmd','command','ntmon'),
 [ValidateSet('SelfDelete','ExternalClose','NaturalExit')][string]$Mode='SelfDelete',
 [string]$MonitorRpc,
 [switch]$InheritPath
)
# Run from an owned background runner. Real in-worker NTMON Delete/Y, not an
# external management RPC by default. Additional modes exercise the same
# observer's external-close and natural-return paths, not another harness.
# The native harness owns its conhost/process Job.
$ErrorActionPreference='Stop'
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$repo=[IO.Path]::GetFullPath("$PSScriptRoot/../..")
$log=[IO.Path]::GetFullPath($LogRoot)
if(!$log.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $log)){
 throw 'Require fresh repository-build evidence directory'
}
$observerPath=(Resolve-Path $Observer).Path
$harnessPath=(Resolve-Path $Harness).Path
if($Mode -eq 'NaturalExit' -and $Targets -contains 'ntmon'){
 throw 'NaturalExit requires explicit cmd/command targets; NTMON does not accept exit'
}
$managementTool=if($Mode -eq 'ExternalClose'){(Resolve-Path $MonitorRpc).Path}else{$null}
$null=New-Item -ItemType Directory $log
$saved=@{};$results=@()
$settings=@{
 MVDM_OBSERVER_POST_EXIT_MS='1000';MVDM_OBSERVER_PRIVATE_DESKTOP=$null
 MVDM_OBSERVER_CAF_HOLD='1';MVDM_OBSERVER_WINDOW_INPUT='1'
 MVDM_OBSERVER_CAF_OWNED_CONSOLE='1'
 MVDM_OBSERVER_CAF_SELF_NTMON=if($Mode -eq 'SelfDelete'){'1'}else{$null}
 MVDM_OBSERVER_CAF_EXIT_IN_WINDOW=if($Mode -eq 'NaturalExit'){'1'}else{$null}
 MVDM_OBSERVER_MANAGEMENT_CLOSE_TOOL=$managementTool
 MVDM_OBSERVER_CAF_RETURN_BEFORE_CLOSE=$null
 MVDM_OBSERVER_SELF_PACE='1';MVDM_OBSERVER_CAF_DIRECT_NTMON=$null
 MVDM_OBSERVER_CAF_OUTER_COMMAND=$null
}
foreach($name in @('PATH')+@($settings.Keys)){$saved[$name]=[Environment]::GetEnvironmentVariable($name)}
try{
 foreach($name in $settings.Keys){
  if($null -eq $settings[$name]){Remove-Item -LiteralPath ('Env:'+$name) -ErrorAction SilentlyContinue}
  else{[Environment]::SetEnvironmentVariable($name,$settings[$name])}
 }
 foreach($target in $Targets){
  $stage=$null;$scope=$null;$process=$null;$started=[DateTime]::UtcNow
  try{
   # COMMAND renders its DOS path in uppercase. Use the same exact spelling
   # in both outer and inner prompts; retain the strict idle-prompt assertion.
   $stage=(New-PhysicalRuntimeStage $PackageRoot).ToUpperInvariant()
   $scope=New-IsolatedPackageScope $stage
   # Fixed probe commands come from this package or host System32. Inheriting
   # a build-tool PATH changes DOS startup before the handoff under test.
   if(!$InheritPath){$env:PATH="$stage\system32;"+(Join-Path $env:SystemRoot 'System32')}
   $env:MVDM_OBSERVER_CAF_DIRECT_NTMON=if($target -eq 'ntmon'){'1'}else{$null}
   $env:MVDM_OBSERVER_CAF_OUTER_COMMAND='"'+$stage+'\system32\run16.exe" '+$target+"`r"
   $report=Join-Path $log "$target.txt"
   $arguments='"'+$observerPath+'" "'+$stage+'" "'+$report+'" "'+$env:SystemRoot+'\System32\conhost.exe"'
   $process=Start-Process -FilePath $harnessPath -ArgumentList $arguments -WindowStyle Hidden -PassThru -RedirectStandardOutput "$log/$target.stdout" -RedirectStandardError "$log/$target.stderr"
   $null=$process.Handle
   if(!$process.WaitForExit(60000)){
    & taskkill.exe /PID $process.Id /T /F | Out-Null
    $process.WaitForExit();throw 'Owned conhost probe exceeded 60s diagnostic budget'
   }
   $process.WaitForExit()
   $text=Get-Content ($report+'.caf.txt') -Raw
   $passed=$process.ExitCode -eq 0
   if($Mode -eq 'NaturalExit'){
    $passed=$passed -and $text -match '(?m)^caf-natural-exit-returned=1 input-mode=0x[0-9a-f]+ echo=1 cell=\d+,\d+ frontend-alive=1'
   }else{
    $callerProof=if($Mode -eq 'SelfDelete'){'self-ntmon-confirmed=1'}else{'management-console-close-exit=0x00000000'}
    $passed=$passed -and $text -match $callerProof -and
     $text -match 'management-close-complete=1' -and $text -match '(?m)^echo-before-enter=1' -and
     $text -match 'active-echo-before-enter=1'
   }
   $results+=@{mode=$Mode;target=$target;passed=$passed;exit=$process.ExitCode;pid=$process.Id;start=$started;end=[DateTime]::UtcNow;report=$report}
  }catch{$results+=@{mode=$Mode;target=$target;passed=$false;start=$started;end=[DateTime]::UtcNow;error=$_.Exception.Message}}
  finally{
   if($process){$process.Dispose()}
   if($scope){Stop-IsolatedPackageScope $scope}
   if($stage){Remove-PhysicalRuntimeStage $stage}
   $results|ConvertTo-Json -Depth 4|Set-Content "$log/results.json"
  }
 }
}finally{foreach($name in $saved.Keys){
 if($null -eq $saved[$name]){Remove-Item -LiteralPath ('Env:'+$name) -ErrorAction SilentlyContinue}
 else{[Environment]::SetEnvironmentVariable($name,$saved[$name])}
}}
if(@($results|Where-Object {!$_.passed}).Count){throw "CAF $Mode failed; inspect results.json"}
"PASS CAF $Mode return and echo before Enter"
