# Test-only private-desktop input and actual published-frame assertions.
function Invoke-EditorFullscreenProbe {
 param([string]$ProbeRoot,[int]$FrontendPid,[string]$Desktop,[string]$OutputPrefix,
       [ValidateSet('Native','Dos')][string]$BlockingEditor,[string]$ThreadSnapshot,
       [switch]$WindowedDos)
 $probe="$ProbeRoot/probe.exe";$hook="$ProbeRoot/mouse-hook.dll";$script:fullProbeStep=0
 function Write-FullProbeKey($scan,$key,$down,$mods,[switch]$Console) {
  $kind=if($Console){'--console-key'}else{'--key'}
  $script:fullProbeStep++
  & $probe $FrontendPid $Desktop $hook $kind $scan $key $down $mods "$OutputPrefix-key-$script:fullProbeStep.txt"
  if($LASTEXITCODE -eq 71 -and !$Console){
   # A blocking shell-out can hand presentation to Console between key down/up.
   $vk=switch($key){131073 {13} 196609 {27} 196611 {16} 196612 {17} 196613 {18} default {$key}}
   & $probe $FrontendPid $Desktop $hook --console-key $scan $vk $down $mods "$OutputPrefix-key-$script:fullProbeStep.txt"
  }
  if($LASTEXITCODE){throw "Private key delivery failed: $LASTEXITCODE"}
 }
 function Write-FullProbeChord {
  Write-FullProbeKey 56 196613 1 2
  Write-FullProbeKey 28 131073 1 2
  Write-FullProbeKey 28 131073 0 2
  Write-FullProbeKey 56 196613 0 0
 }
 function Write-FullProbeTap($scan,$key) {
  Write-FullProbeKey $scan $key 1 0
  Write-FullProbeKey $scan $key 0 0
  Start-Sleep -Milliseconds 100
 }
 function Write-FullProbeControl($scan,$key) {
  Write-FullProbeKey 29 196612 1 1
  Write-FullProbeKey $scan $key 1 1
  Write-FullProbeKey $scan $key 0 1
  Write-FullProbeKey 29 196612 0 0
 }
 function Write-FullProbeClick($x,$y) {
  foreach($move in @(@(-4096,-4096,0),@($x,$y,0),@(0,0,1),@(0,0,0))){
   $script:fullProbeStep++
   & $probe $FrontendPid $Desktop $hook $move[0] $move[1] $move[2] "$OutputPrefix-mouse-$script:fullProbeStep.txt"
   if($LASTEXITCODE){throw 'Fullscreen mouse delivery failed'}
   Start-Sleep -Milliseconds 100
  }
 }
 function Read-FullProbeFrame($kind,$suffix,$pattern='') {
  $until=[DateTime]::UtcNow.AddSeconds(10);$caf=$false
  do {
   & $probe $FrontendPid $Desktop --frame "$OutputPrefix-$suffix.txt"
   $code=$LASTEXITCODE
   if(!$code){$text=Get-Content "$OutputPrefix-$suffix.txt" -Raw;if($text.StartsWith($kind) -and (!$pattern -or $text -match $pattern)){return $text}}
   if($code -eq 71 -and $kind -eq 'text' -and !$caf){
    Write-FullProbeKey 33 70 1 3 -Console
    Write-FullProbeKey 33 70 0 0 -Console
    $caf=$true
   }
   Start-Sleep -Milliseconds 50
  } while([DateTime]::UtcNow -lt $until)
  if(Test-Path "$ProbeRoot/surface.exe"){
   & "$ProbeRoot/surface.exe" $FrontendPid $Desktop "$OutputPrefix-$suffix.bmp"
  }
  if(Test-Path "$ProbeRoot/console-snapshot.exe"){
   # No mutation argument: retain the actual Console contents on failure.
   & "$ProbeRoot/console-snapshot.exe" $FrontendPid "$OutputPrefix-$suffix-console.txt"
  }
  $frontend=Get-CimInstance Win32_Process -Filter "ProcessId=$FrontendPid"
  if($frontend.ExecutablePath){
   $root=Split-Path $frontend.ExecutablePath
   $owned=@(Get-CimInstance Win32_Process | Where-Object {$_.ExecutablePath -like "$root\*"})
   $owned | Select-Object Name,ProcessId,ParentProcessId,CommandLine |
    ConvertTo-Json | Set-Content "$OutputPrefix-$suffix-processes.json"
   if($ThreadSnapshot){foreach($worker in $owned | Where-Object Name -eq 'ntvdm.exe'){
    & $ThreadSnapshot $worker.ProcessId > "$OutputPrefix-$suffix-worker-$($worker.ProcessId).txt"
   }}
  }
  throw "Expected $kind frame at $suffix"
 }
 $null=Read-FullProbeFrame 'graphics' 'initial'
 if($BlockingEditor){
  # Native shell-out blocks the guest controller. DOS reentry is submitted
  # by that controller and reaches this handoff after guest progress.
  $letter=if($BlockingEditor -eq 'Native'){@(30,65)}else{@(48,66)}
  if($BlockingEditor -eq 'Native'){Write-FullProbeTap 28 131073}
  # DOS reentry can remain in the guest DOS window. Exercise its ordinary
  # fullscreen transition instead of assuming native Console presentation.
  if($BlockingEditor -eq 'Dos' -and $WindowedDos){Write-FullProbeChord}
  $seed=if($BlockingEditor -eq 'Native'){'seedA'}else{'seedB'}
  $null=Read-FullProbeFrame 'text' 'blocking-editor' $seed
  Write-FullProbeTap 1 196609
  foreach($n in 1..3){Write-FullProbeTap $letter[0] $letter[1]}
  Write-FullProbeClick 40 4
  $menu=Read-FullProbeFrame 'text' 'blocking-menu'
  if($menu -notmatch 'Save As|New File'){throw 'Blocking editor File menu did not open'}
  if($BlockingEditor -eq 'Dos'){
   # EDIT's menu geometry differs from the native editor. Use its menu
   # accelerators after proving that the frontend mouse opened File.
   Write-FullProbeTap 31 83
   Write-FullProbeClick 40 4
   Write-FullProbeTap 45 88
   if($WindowedDos){
    $null=Read-FullProbeFrame 'text' 'dos-prompt-return' 'PATCH>'
    Write-FullProbeChord
   }
  } else {
   # The preceding DOS application can change text rows/font geometry.
   # Exercise the native editor's advertised shortcuts, not fixed pixels.
   Write-FullProbeTap 1 196609
   Write-FullProbeControl 31 83
   Write-FullProbeControl 16 81
  }
  $null=Read-FullProbeFrame 'graphics' 'blocking-return'
  return
 }
 foreach($round in 1,2){
  Write-FullProbeChord
  $null=Read-FullProbeFrame 'text' "full$round"
  Write-FullProbeClick 40 4
  $until=[DateTime]::UtcNow.AddSeconds(5)
  do {
   $menu=Read-FullProbeFrame 'text' "menu$round"
   if($menu -match 'Save As|New File'){break}
   Start-Sleep -Milliseconds 50
  } while([DateTime]::UtcNow -lt $until)
  if($menu -notmatch 'Save As|New File'){throw 'Mouse click did not open fullscreen editor File menu'}
  Write-FullProbeKey 1 196609 1 0
  Write-FullProbeKey 1 196609 0 0
  Write-FullProbeChord
  $null=Read-FullProbeFrame 'graphics' "window$round"
 }
}
