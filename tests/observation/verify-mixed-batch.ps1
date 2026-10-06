[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$RuntimeRoot,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$Probe32,
 [Parameter(Mandatory)][string]$Probe64,
 [Parameter(Mandatory)][string]$GuestFixture,
 [Parameter(Mandatory)][string]$LogRoot,
 [string]$Gui32,
 [string]$Gui64,
 [string]$PifBuilder,
 [string]$WindowObserver,
 [ValidateSet('Mixed','Search','Native','Wow')][string[]]$Cases=@('Mixed'),
 [switch]$Typeahead,
 [string[]]$Shells=@('COMMAND','CMD32','CMD64'),
 [string[]]$Modes=@('Console','Window')
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$runtime=(Resolve-Path $RuntimeRoot).Path;$observerPath=(Resolve-Path $Observer).Path
$build=(Join-Path $repo 'build')+'\';$log=[IO.Path]::GetFullPath($LogRoot)
if(!$runtime.StartsWith($build,[StringComparison]::OrdinalIgnoreCase) -or
 !$log.StartsWith($build,[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $log)){throw 'Require build-owned runtime and fresh evidence'}
if(@($Shells|Where-Object {$_ -notin @('COMMAND','CMD32','CMD64')}).Count -or
 @($Modes|Where-Object {$_ -notin @('Console','Window')}).Count){throw 'Unknown shell/display case'}
. "$PSScriptRoot/isolated_package_cleanup.ps1"
. "$PSScriptRoot/batch_journal_witness.ps1"
$scope=New-IsolatedPackageScope $runtime
if(Test-Path Z:\){throw 'Do not replace occupied Z:'}
$binary=Get-PackageBinaryRoot $runtime
$null=New-Item -ItemType Directory -Path $log
$fixture=Join-Path $runtime 'tests\S7';$null=New-Item -ItemType Directory -Path $fixture -Force
function Machine([string]$Image){$b=[IO.File]::ReadAllBytes($Image);[BitConverter]::ToUInt16($b,[BitConverter]::ToInt32($b,60)+4)}
if((Machine $Probe32) -ne 0x14c -or (Machine $Probe64) -ne 0x8664){throw 'Actual probe machine mismatch'}
Copy-Item $Probe32 (Join-Path $fixture 'P32.EXE');Copy-Item $Probe64 (Join-Path $fixture 'P64.EXE')
Copy-Item $GuestFixture (Join-Path $fixture 'G7.COM')
Copy-Item $Probe32 (Join-Path $fixture 'PICK.EXE')
if($Cases -contains 'Native'){
 if(!$Gui32 -or !$Gui64 -or (Machine $Gui32) -ne 0x14c -or (Machine $Gui64) -ne 0x8664){throw 'Native cases require both GUI probe widths'}
 Copy-Item $Gui32 (Join-Path $fixture 'G32.EXE');Copy-Item $Gui64 (Join-Path $fixture 'G64.EXE')
}
if($Cases -contains 'Search' -and !$PifBuilder){throw 'Search requires authored PIF builder'}
if($Cases -contains 'Wow' -and !$WindowObserver){throw 'WOW BAT needs actual private-desktop window observation'}
$templates=@{}
foreach($name in 'mixed_batch.bat','mixed_batch_child.bat','mixed_batch_native.bat','mixed_batch_search.bat','mixed_batch_wow.bat'){$templates[$name]=Get-Content (Join-Path $PSScriptRoot $name) -Raw}
@{ObserverHash=(Get-FileHash $observerPath).Hash;Probe32Hash=(Get-FileHash $Probe32).Hash;Probe64Hash=(Get-FileHash $Probe64).Hash;GuestHash=(Get-FileHash $GuestFixture).Hash;Typeahead=[bool]$Typeahead;Templates=$templates}|ConvertTo-Json -Depth 5|Set-Content "$log/inputs.json"
$identity=@(foreach($name in 'run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe','ntmon.exe','WOW32.DLL','VDMREDIR.DLL','nthook32.dll','nthook64.dll'){
 $path=Join-Path $binary $name;@{Name=$name;Sha256=(Get-FileHash $path).Hash}
})
$identity|ConvertTo-Json|Set-Content "$log/package.json"
# Keep independent input identity separate from the product manifest.
@{RunnerHash=(Get-FileHash $PSCommandPath).Hash;CheckerHash=(Get-FileHash "$PSScriptRoot/batch_journal_witness.ps1").Hash;
  Gui32Hash=$(if($Gui32){(Get-FileHash $Gui32).Hash});Gui64Hash=$(if($Gui64){(Get-FileHash $Gui64).Hash});
  PifBuilderHash=$(if($PifBuilder){(Get-FileHash $PifBuilder).Hash})}|ConvertTo-Json|Set-Content "$log/harness.json"
$saved=@{};foreach($name in @('MVDM_OBSERVER_PRIVATE_DESKTOP','MVDM_OBSERVER_SHORT_HISTORY','MVDM_OBSERVER_WINDOW_INPUT','MVDM_OBSERVER_MILESTONE_INPUT','MVDM_OBSERVER_POST_EXIT_MS')){$saved[$name]=[Environment]::GetEnvironmentVariable($name,'Process')}
$results=[Collections.Generic.List[object]]::new();$mapped=$false
try{
 subst.exe Z: $runtime;if($LASTEXITCODE){throw 'Cannot map Z:'};$mapped=$true
 $scope.Paths+=@($scope.Paths|ForEach-Object {'Z:\'+$_.Substring($runtime.Length+1)})
 # Fixture-owned native targets can outlive a failing launcher. Their exact
 # copied images are cleanup-owned too; never kill host CMD/GUI by name.
 foreach($name in 'P32.EXE','P64.EXE','PICK.EXE','G32.EXE','G64.EXE'){
  $scope.Paths+=@(Join-Path $fixture $name),('Z:\tests\S7\'+$name)
 }
 $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1';$env:MVDM_OBSERVER_SHORT_HISTORY='1'
 foreach($mode in $Modes){foreach($shell in $Shells){foreach($kind in $Cases){
  if($kind -eq 'Native' -and $shell -eq 'COMMAND'){continue} # START is native CMD syntax.
  $id=([guid]::NewGuid().ToString('N')).Substring(0,8)
  $case="$shell-$mode-$kind-$id";$dir=Join-Path $fixture $id;$null=New-Item -ItemType Directory -Path $dir
  $alias="Z:\tests\S7\$id";$trace="$alias\TRACE.TXT"
  $tokens=@{'@CASE@'=$case;'@TRACE@'=$trace;'@DOS@'='Z:\tests\S7\G7.COM';'@N32@'='Z:\tests\S7\P32.EXE';'@N64@'='Z:\tests\S7\P64.EXE';'@N32OUT@'="$alias\N32.OUT";'@N32ERR@'="$alias\N32.ERR";'@CHILD@'="$alias\CHILD.BAT"}
  $eventBase="Local\S7-$id";$eventBase2="Local\S7B-$id";$events=[Collections.Generic.List[IDisposable]]::new()
  if($kind -eq 'Native'){
   foreach($name in @($eventBase,$eventBase2)){foreach($suffix in 'R','G','D'){$events.Add([Threading.EventWaitHandle]::new($false,[Threading.EventResetMode]::ManualReset,"$name-$suffix"))}}
  }
  $tokens['@GUI32@']='Z:\tests\S7\G32.EXE';$tokens['@GUI64@']='Z:\tests\S7\G64.EXE'
  $tokens['@EVENT@']=$eventBase;$tokens['@EVENT2@']=$eventBase2;$tokens['@INPUT@']="$alias\INPUT.TXT"
  $tokens['@RUN16@']='Z:\system32\run16.exe';$tokens['@MISSING@']="$alias\NOMISS.EXE"
  $tokens['@DIR@']=$alias;$tokens['@PIF@']="$alias\G7.PIF"
  if($kind -eq 'Search'){
   Copy-Item $GuestFixture (Join-Path $dir 'DUAL.COM');Copy-Item $Probe32 (Join-Path $dir 'DUAL.EXE')
   Copy-Item $Probe64 (Join-Path $dir 'PICK.EXE')
   $scope.Paths+=@(Join-Path $dir 'PICK.EXE'),("$alias\PICK.EXE")
   [IO.File]::WriteAllText((Join-Path $dir 'FIND.BAT'),"@echo off`r`necho FOUND:$case>>$trace`r`n",[Text.ASCIIEncoding]::new())
   & $PifBuilder "$alias\G7.PIF" 'Z:\tests\S7\G7.COM';if($LASTEXITCODE){throw 'PIF generation failed'}
  }
  foreach($template in @(@{Source='mixed_batch.bat';Name='MAIN.BAT'},@{Source='mixed_batch_child.bat';Name='CHILD.BAT'})){
   $source=if($template.Name -eq 'MAIN.BAT' -and $kind -ne 'Mixed'){$('mixed_batch_'+$kind.ToLowerInvariant()+'.bat')}else{$template.Source}
   $contents=$templates[$source]
   foreach($token in $tokens.Keys){$contents=$contents.Replace($token,$tokens[$token])}
   [IO.File]::WriteAllText((Join-Path $dir $template.Name),($contents -replace '\r?\n',"`r`n"),[Text.ASCIIEncoding]::new())
  }
  $target=if($shell -eq 'COMMAND'){'Z:\system32\COMMAND.COM'}elseif($shell -eq 'CMD32'){Join-Path $env:WINDIR 'SysWOW64\cmd.exe'}else{Join-Path $env:WINDIR 'System32\cmd.exe'}
  if($shell -ne 'COMMAND' -and (Machine $target) -ne $(if($shell -eq 'CMD32'){0x14c}else{0x8664})){throw 'Real CMD machine mismatch'}
  if($Typeahead){Remove-Item Env:MVDM_OBSERVER_MILESTONE_INPUT -ErrorAction SilentlyContinue}else{$env:MVDM_OBSERVER_MILESTONE_INPUT='1'}
  if($mode -eq 'Window'){$env:MVDM_OBSERVER_WINDOW_INPUT='1'}else{Remove-Item Env:MVDM_OBSERVER_WINDOW_INPUT -ErrorAction SilentlyContinue}
  # Launch an interactive shell first: Window route must exist before the BAT.
  # The evidence is the journal, not the old prompt or submission acknowledgement.
  $input="$alias\MAIN.BAT`rexit`r"
  $report=Join-Path $log "$shell-$mode-$kind.txt"
  $args=@('Z:\system32\run16.exe','Z:\system32',$report,$target)
  if($shell -ne 'COMMAND'){$args+='/d'}
  $args+=@('--observe-console-input-text',$input,'--observation-timeout-ms','45000')
  $env:MVDM_OBSERVER_POST_EXIT_MS=if($kind -eq 'Wow'){'5000'}else{'0'}
  if($Typeahead){$args+=@('--observe-console-line-delay-ms','0')}
  $timer=[Diagnostics.Stopwatch]::StartNew();$passed=$false
  $wowWindows=''
  try{
   if($kind -eq 'Wow'){
    $start=[Diagnostics.ProcessStartInfo]::new($observerPath)
    $start.UseShellExecute=$false;$start.WindowStyle=[Diagnostics.ProcessWindowStyle]::Hidden
    foreach($argument in $args){$start.ArgumentList.Add($argument)}
    $process=[Diagnostics.Process]::Start($start)
    try{
     $deadline=[DateTime]::UtcNow.AddSeconds(45)
     while(!$process.HasExited -and [DateTime]::UtcNow -lt $deadline){
      foreach($worker in @(Get-CimInstance Win32_Process -Filter "Name='ntvdm.exe'"|Where-Object {$_.ExecutablePath -in $scope.Paths})){
       $sample=@(& $WindowObserver ([string]$worker.ProcessId) ('NTVDMConsoleTest-'+$process.Id)) -join "`n"
       if(!$LASTEXITCODE){$wowWindows+=$sample+"`n"}
      }
      if($wowWindows -match '(?m)^window-utf16=\S+ visible=1 class=(626B96F7|00C900A800C000D7) text=\1\r?$'){break}
      $null=$process.WaitForExit(100)
     }
     if(!$process.WaitForExit(50000) -or $process.ExitCode){throw 'WOW BAT observer did not complete'}
    }finally{if(!$process.HasExited){$process.Kill();$process.WaitForExit()};$process.Dispose()}
   }else{& $observerPath @args;if($LASTEXITCODE){throw "Observer failed $shell/$mode"}}
   $reportText=Get-Content $report -Raw
   if($reportText -notmatch '(?m)^result=exited\r?$'){throw 'Root shell did not complete'}
   $rootExit=if($shell -eq 'COMMAND'){$(if($kind -eq 'Wow'){'00000000'}else{'00000001'})}else{'00000013'}
   if($reportText -notmatch "(?m)^exit=0x$rootExit\r?`$"){throw "Wrong root completion $shell/$mode"}
   $lines=@(Get-Content (Join-Path $dir 'TRACE.TXT'))
   $expected=@("BEGIN:$case","DOS7:$case","N32:$case|bits=32|arg=two words|hook=1","RC32:$case","CHILD:$case","CHILD64:$case|bits=64|arg=|hook=1","CHILD64RC:$case","CHILDDOS7:$case","CHILD32:$case|bits=32|arg=|hook=1","RETURN64:$case|bits=64|arg=|hook=1","CHILDEND:$case","CALLRETURN:$case","DOSAGAIN:$case","N64:$case|bits=64|arg=|hook=1","RC64:$case","DONE:$case")
   if($kind -eq 'Search'){$expected=@("SEARCHBEGIN:$case","COMFIRST:$case","CWD64:$case|bits=64|arg=|hook=1","LAUNCH64:$case|bits=64|arg=|hook=1","FOUND:$case","BAREBAT:$case","PIF7:$case","PATH32:$case|bits=32|arg=|hook=1","SEARCHMISSING:$case","FINAL:$case|bits=64|arg=|hook=1")}
   if($kind -eq 'Native'){$expected=@("ADVBEGIN:$case","IN32:$case|bits=32|arg=|hook=1","STDIN32:$case","IN64:$case|bits=64|arg=|hook=1","STDIN64:$case","GUIWAIT32:$case|bits=32|arg=|hook=1","GUIWAITED32:$case","GUIWAIT64:$case|bits=64|arg=|hook=1","GUIWAITED64:$case","GUISTART:$case|bits=32|arg=$eventBase|hook=1","RELEASE:$case|bits=64|arg=$eventBase|hook=1","GEND:GUISTART:$case","STARTRETURN:$case","GUIROOT:$case|bits=64|arg=$eventBase2|hook=1","RELEASE2:$case|bits=32|arg=$eventBase2|hook=1","GEND:GUIROOT:$case","GUIROOTRETURN:$case","GUIEXPLICIT:$case|bits=32|arg=|hook=1","GUIEXPLICIT37:$case","MISSINGFAILED:$case","FINAL:$case|bits=64|arg=|hook=1")}
   if($kind -eq 'Wow'){
    $expected=@("WOWBEGIN:$case","WOWSTARTED:$case","FINAL:$case|bits=64|arg=|hook=1")
    $windows=$wowWindows
    $windows|Set-Content (Join-Path $log "$shell-$mode-Wow-windows.txt")
    if($windows -notmatch '(?m)^window-utf16=\S+ visible=1 class=(626B96F7|00C900A800C000D7) text=\1\r?$'){throw 'Actual Mines UI frontier absent after BAT startup'}
   }
   Assert-BatchJournal $lines $expected
   if($kind -in @('Mixed','Native')){
    $out=Get-Content (Join-Path $dir 'N32.OUT') -Raw;$err=Get-Content (Join-Path $dir 'N32.ERR') -Raw
    $step=if($kind -eq 'Mixed'){'N32'}else{'IN32'}
    if(!$out.Contains("S7-NATIVE-32:${step}:${case}:") -or !$err.Contains("S7-STDERR-32:${step}:$case")){throw 'Native stdout/stderr redirection changed'}
   }
   Copy-Item (Join-Path $dir 'TRACE.TXT') (Join-Path $log "$shell-$mode-$kind-journal.txt")
   $passed=$true;"PASS $kind BAT $shell/$mode; exact sequence, actual widths/results and cleanup"
  }finally{
   Stop-IsolatedPackageScope $scope
   foreach($event in $events){$event.Dispose()}
   $results.Add(@{Shell=$shell;Mode=$mode;Kind=$kind;RunId=$id;Passed=$passed;ElapsedMs=$timer.ElapsedMilliseconds;Fixture=$dir})
  }
 }}}
 foreach($row in $identity){if((Get-FileHash (Join-Path $binary $row.Name)).Hash -ne $row.Sha256){throw 'Production package changed during tests'}}
}finally{
 try{Stop-IsolatedPackageScope $scope}finally{if($mapped){subst.exe Z: /d}}
 foreach($name in $saved.Keys){[Environment]::SetEnvironmentVariable($name,$saved[$name],'Process')}
 $results|ConvertTo-Json -Depth 5|Set-Content "$log/results.json"
}
