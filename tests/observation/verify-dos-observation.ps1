[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$RuntimeRoot,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$MonitorRpc,
 [Parameter(Mandatory)][string]$ProbeRoot,
 [Parameter(Mandatory)][string]$LogRoot,
 [ValidateSet('Normal','Reuse','Tsr','Exclusions')][string]$Case='Normal',
 [switch]$Command,
 [switch]$NativeFirst,
 [ValidateSet(0,1)][Nullable[int]]$DosOnly=$null,
 [string]$PifBuilder=''
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$physical=(Resolve-Path $RuntimeRoot).Path
$scope=New-IsolatedPackageScope $physical
$log=[IO.Path]::GetFullPath($LogRoot)
if(!$log.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $log)){throw 'Fresh build evidence required'}
if(Test-Path Z:\){throw 'Z occupied'}
New-Item -ItemType Directory -Path $log|Out-Null
$fixture=Join-Path $physical 'tests\OBS';New-Item -ItemType Directory -Path $fixture -Force|Out-Null
$root=(Resolve-Path $ProbeRoot).Path
Copy-Item (Join-Path $root $(if($Case -eq 'Reuse'){'P2.COM'}elseif($Case -eq 'Exclusions'){'PN.COM'}else{'P.COM'})) (Join-Path $fixture 'P.COM')
Copy-Item (Join-Path $root $(if($Case -eq 'Tsr'){'TSR.COM'}else{'C.COM'})) (Join-Path $fixture 'C.COM')
Copy-Item (Join-Path $root 'F.COM') (Join-Path $fixture 'F.COM')
foreach($name in @('J.BIN','GO0','GO1')){if(Test-Path (Join-Path $fixture $name)){Remove-Item -LiteralPath (Join-Path $fixture $name)}}
if($NativeFirst) {
 $Command=$true
 $batch=[IO.File]::ReadAllText("$PSScriptRoot/dos-observation-resume.bat")
 [IO.File]::WriteAllText((Join-Path $fixture 'B.BAT'),[regex]::Replace($batch,'\r?\n',"`r`n"),[Text.Encoding]::ASCII)
 if(Test-Path (Join-Path $fixture 'N.OK')){Remove-Item -LiteralPath (Join-Path $fixture 'N.OK')}
}
if($null -ne $DosOnly) {
 if(!$PifBuilder){throw 'Explicit DOSONLY requires authored PIF builder'}
 $Command=$true;$pif=Join-Path $fixture 'P.PIF'
 if(Test-Path $pif){Remove-Item -LiteralPath $pif}
 & (Resolve-Path $PifBuilder).Path $pif 'Z:\system32\command.com' '/c Z:\tests\OBS\P.COM' ([string][int]$DosOnly)
 if($LASTEXITCODE){throw 'Authored NT31 PIF failed'}
 Copy-Item $pif "$log/P.PIF"
}
$oldPrivate=$env:MVDM_OBSERVER_PRIVATE_DESKTOP;$mapped=$false;$process=$null
$tool=(Resolve-Path $MonitorRpc).Path
function Journal {
 $path=Join-Path $fixture 'J.BIN';if(!(Test-Path $path)){return @()}
 $stream=$null
 try {
  $stream=[IO.File]::Open($path,[IO.FileMode]::Open,[IO.FileAccess]::Read,[IO.FileShare]::ReadWrite)
  $buffer=[IO.MemoryStream]::new();$stream.CopyTo($buffer);$bytes=$buffer.ToArray();$buffer.Dispose()
 }catch [IO.IOException] {
  # A currently open DOS create may temporarily exclude readers. This is
  # readiness of this invocation's journal, not a retry of a failed case.
  $native=$_.Exception.HResult -band 0xffff
  if($native -in @(32,33)){return @()};throw
 }finally{if($stream){$stream.Dispose()}}
 $rows=@()
 for($index=0;$index+10 -le $bytes.Length;$index+=10){
  if([BitConverter]::ToUInt16($bytes,$index) -ne 0x4344){throw 'This invocation journal corrupt'}
  $rows+=,[pscustomobject]@{Role=[char]$bytes[$index+2];Stage=$bytes[$index+3];PSP=[BitConverter]::ToUInt16($bytes,$index+4);Parent=[BitConverter]::ToUInt16($bytes,$index+6);Result=[BitConverter]::ToUInt16($bytes,$index+8)}
 }
 return $rows
}
function Trace([int]$WorkerId,[string]$Name) {
 $text=& $tool --trace-json $WorkerId;if($LASTEXITCODE){throw 'Actual trace RPC failed'}
 $text|Set-Content "$log/$Name.json";return (($text -join "`n")|ConvertFrom-Json)
}
try {
 subst.exe Z: $physical;if($LASTEXITCODE){throw 'subst failed'};$mapped=$true
 $scope.Paths+=@($scope.Paths|ForEach-Object {Join-Path 'Z:\' $_.Substring($physical.Length+1)})
 $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
 $report=Join-Path $log 'observer.txt'
 $target=if($Command){@('Z:\system32\command.com','/c','Z:\tests\OBS\P.COM')}else{@('Z:\tests\OBS\P.COM')}
 if($null -ne $DosOnly){$target=@('Z:\tests\OBS\P.PIF')}
 if($NativeFirst){$target=@('Z:\system32\command.com','/c','Z:\tests\OBS\B.BAT')}
 $arguments=@('Z:\system32\run16.exe','Z:\',$report)+$target+@('--observation-timeout-ms','20000')
 $process=Start-Process -FilePath (Resolve-Path $Observer).Path -ArgumentList $arguments -WindowStyle Hidden -PassThru
 $deadline=[DateTime]::UtcNow.AddSeconds(10)
 $childIndex=if($Case -eq 'Exclusions'){4}else{1};$afterIndex=$childIndex+1
 do {$journal=@(Journal);if($journal.Count -ge $childIndex+1){break};if($process.HasExited){throw 'Guest ended before actual child witness'};Start-Sleep -Milliseconds 20}while([DateTime]::UtcNow -lt $deadline)
 if($journal.Count -ne $childIndex+1 -or $journal[0].Role -ne 'R' -or $journal[$childIndex].Role -ne 'C' -or
    $journal[$childIndex].Parent -ne $journal[0].PSP){throw 'Actual DOS EXEC/PSP witness absent'}
 if($Case -eq 'Exclusions' -and ($journal[1].Stage -ne 10 -or $journal[1].Result -ne 2 -or
    $journal[2].Stage -ne 11 -or $journal[2].Result -ne 0 -or $journal[3].Stage -ne 12 -or $journal[3].Result -ne 0)){throw 'Actual failed/load-only/overlay operations unproved'}
 $text=& $tool --tree-json;if($LASTEXITCODE){throw 'Actual management snapshot failed'}
 $workers=@(($text -join "`n")|ConvertFrom-Json|Where-Object {$_.category -eq 2 -and $_.kind -eq 0})
 if($workers.Count -ne 1 -or $workers[0].stack -ne 1){throw 'Internal EXEC changed Direct stack'}
 $worker=$workers[0];$deadline=[DateTime]::UtcNow.AddSeconds(5)
 do {
  $created=Trace $worker.pid 'created'
  $directPsp=if($Command){$journal[0].Parent}else{$journal[0].PSP}
  $direct=@($created.nodes|Where-Object {$_.relation -eq 1 -and $_.psp -eq $directPsp})
  # PSP is a source address, not an occurrence identity. The held program
  # must not be confused with an earlier exited program at the same address.
  $program=@($created.nodes|Where-Object {$_.psp -eq $journal[0].PSP -and $_.state -ne 2})
  $child=@($created.nodes|Where-Object {$_.relation -eq 2 -and $_.psp -eq $journal[$childIndex].PSP -and $_.state -ne 2})
  if($direct.Count -eq 1 -and $child.Count -eq 1){break};Start-Sleep -Milliseconds 20
 }while([DateTime]::UtcNow -lt $deadline)
 if($direct.Count -ne 1 -or $program.Count -ne 1 -or $child.Count -ne 1 -or
    $child[0].parent -ne $program[0].node -or $child[0].source -ne 3){throw 'Real root/child PSP association wrong or missing'}
 if($Command -and ($program[0].relation -ne 2 -or $program[0].parent -ne $direct[0].node)){throw 'COMMAND internal EXEC invented a Direct record or lost parent'}
 if($Case -eq 'Exclusions' -and $created.nodes.Count -ne $(if($Command){3}else{2})){throw 'Failed/load-only/overlay created fake running nodes'}
 if($NativeFirst -and (!(Test-Path (Join-Path $fixture 'N.OK')) -or
    !(Get-Content (Join-Path $fixture 'N.OK') -Raw).Contains('S4-NATIVE-RETURN') -or
    @($created.nodes|Where-Object {$_.state -ne 2}).Count -ne 3)){
    throw 'Native shell-out/resume or active occurrence isolation not proved'
 }
 [IO.File]::WriteAllBytes((Join-Path $fixture 'GO0'),[byte[]]@(1))
 $needed=if($Case -eq 'Reuse'){5}else{$afterIndex+1};$deadline=[DateTime]::UtcNow.AddSeconds(5)
 do {$journal=@(Journal);if($journal.Count -ge $needed){break};Start-Sleep -Milliseconds 20}while([DateTime]::UtcNow -lt $deadline)
 if($journal.Count -ne $needed -or ($journal[$afterIndex].Result -band 255) -ne 7){throw 'Actual child completion/result absent'}
 $deadline=[DateTime]::UtcNow.AddSeconds(5)
 do {
  $finished=Trace $worker.pid 'finished'
  $old=@($finished.nodes|Where-Object {$_.node -eq $child[0].node})
  if($Case -eq 'Tsr' -or ($old.Count -eq 1 -and $old[0].state -eq 2)){break};Start-Sleep -Milliseconds 20
 }while([DateTime]::UtcNow -lt $deadline)
 if($Case -eq 'Tsr') {
  if(($journal[2].Result -shr 8) -ne 3 -or $old.Count -ne 1 -or $old[0].state -ne 3){throw 'TSR return was falsely presented as terminal EXIT'}
 }elseif($old.Count -ne 1 -or $old[0].state -ne 2 -or ($old[0].flags -band 8)){throw 'Original termination fact/code uncertainty incorrect'}
 if($Case -eq 'Reuse') {
  $last=@($finished.nodes|Where-Object {$_.relation -eq 2 -and $_.node -ne $old[0].node})
  if($journal[3].Role -ne 'F' -or $journal[3].PSP -ne $journal[1].PSP -or
     $last.Count -ne 1 -or $last[0].psp -ne $journal[3].PSP -or $last[0].parent -ne $program[0].node -or $last[0].state -ne 2){throw 'Actual PSP reuse lost independent occurrence/history'}
 }
 if($process.HasExited){throw 'Observed child completion completed Direct early'}
 [IO.File]::WriteAllBytes((Join-Path $fixture 'GO1'),[byte[]]@(1))
 if($Case -eq 'Tsr') {
  # Same S3 baseline keeps the launcher waiting at COMMAND after a TSR.
  # Verify the real root's termination callback without manufacturing a
  # receipt or treating the stay-resident child as dead. Cleanup is explicit.
  $deadline=[DateTime]::UtcNow.AddSeconds(5)
  do {
   $returned=Trace $worker.pid 'root-returned'
   $rootFact=@($returned.nodes|Where-Object {$_.node -eq $program[0].node})
   if($rootFact.Count -eq 1 -and $rootFact[0].state -eq 2){break};Start-Sleep -Milliseconds 20
  }while([DateTime]::UtcNow -lt $deadline)
  if($rootFact.Count -ne 1 -or $rootFact[0].state -ne 2 -or $process.HasExited){throw 'TSR changed original root-return/wait contract'}
  Copy-Item (Join-Path $fixture 'J.BIN') "$log/journal.bin"
  $journal|ConvertTo-Json|Set-Content "$log/journal.json"
  'PASS actual TSR entry/return uncertainty, real root EXIT fact and unchanged baseline launcher wait; explicit cleanup'
  return
 }
 if(!$process.WaitForExit(5000)){throw 'Actual root completion stranded'}
 $expected=if($Command -and !$NativeFirst){'00000000'}else{'00000025'}
 if($process.ExitCode -or (Get-Content $report -Raw) -notmatch "(?m)^exit=0x$expected\r?`$"){throw 'Actual baseline Direct completion changed'}
 Copy-Item (Join-Path $fixture 'J.BIN') "$log/journal.bin"
 $journal|ConvertTo-Json|Set-Content "$log/journal.json"
 "PASS actual DOS $Case PSP/parent/exit facts; baseline Direct$expected unchanged"
}finally {
 foreach($name in @('GO0','GO1')){[IO.File]::WriteAllBytes((Join-Path $fixture $name),[byte[]]@(1))}
 if($process){if(!$process.HasExited){$null=$process.WaitForExit(3000)};if(!$process.HasExited){$process.Kill();$null=$process.WaitForExit(5000)};$process.Dispose()}
 $env:MVDM_OBSERVER_PRIVATE_DESKTOP=$oldPrivate
 try{Stop-IsolatedPackageScope $scope}finally{if($mapped){subst.exe Z: /d}}
}
