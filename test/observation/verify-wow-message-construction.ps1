[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,[switch]$Transaction)
# Supplementary exact selected original bodies, not a substitute runtime.
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
 $root.StartsWith($repo+'\build\.ninja\',[StringComparison]::OrdinalIgnoreCase) -or
 $root.StartsWith($repo+'\build\.ccache\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){
 throw 'Fresh disposable build evidence required'
}
$null=New-Item -ItemType Directory $root
$steps=[Collections.Generic.List[object]]::new()
function Owned($name,$file,$arguments,$seconds,$expected){
 $start=@{FilePath=$file;WorkingDirectory=$root;WindowStyle='Hidden';PassThru=$true;RedirectStandardOutput="$root/$name.stdout.log";RedirectStandardError="$root/$name.stderr.log"}
 if($arguments.Count){$start.ArgumentList=$arguments}
 $p=Start-Process @start;$null=$p.Handle
 $row=[ordered]@{name=$name;pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');file=$file;arguments=$arguments;deadlineSeconds=$seconds;exit=$null}
 $steps.Add($row);$steps|ConvertTo-Json -Depth 5|Set-Content "$root/steps.json"
 if(!$p.WaitForExit($seconds*1000)){& taskkill.exe /PID $p.Id /T /F;throw "Owned deadline $name"}
 $row.exit=$p.ExitCode;$row.end=[DateTime]::UtcNow.ToString('o');$steps|ConvertTo-Json -Depth 5|Set-Content "$root/steps.json"
 if($p.ExitCode -ne $expected){throw "$name actual $($p.ExitCode), expected $expected"}
}
try{
 $sources=@("$repo/src/mvdm-host/inc/wow.h",
  "$repo/src/compat/include/wow_message_lifetime.h",
  "$repo/src/compat/include/wow_message_progress.h",
  "$repo/src/compat/include/wow_control_message_abi.h")
 foreach($name in 'wmdisp32.c','wmdisp32.h','wmsg16.c','wmsgcb.c','wparam.c',
  'walias.h','wudlg.c','walloc16.c','wcntl32.c','wcommdlg.c','wowhooks.c'){
  $sources+="$repo/src/mvdm-host/wow32/$name"
 }
 Get-FileHash $sources|Export-Csv -NoTypeInformation "$root/source-inputs.csv"
 & "$repo/tools/build-fixtures.cmd" x86 wow-message-construction *> "$root/build.log"
 if($LASTEXITCODE){throw 'Native construction build failed'}
 $kind=if($Transaction){'transaction'}else{'construction'}
 $native="$repo/build/.ninja/fixtures-x86"
 Copy-Item "$native/wow-message-$kind-contract.exe" "$root/unit.exe"
 Get-FileHash "$native/message-$kind/*.h","$root/unit.exe",$PSCommandPath,
  "$repo/test/observation/wow_message_construction_fixture.c",
  "$repo/tools/build/fixtures/wow-construction.cmake",
  "$repo/tools/build/SourceProjection.cmake"|Export-Csv -NoTypeInformation "$root/inputs.csv"
 Owned body-candidate "$root/unit.exe" @() 30 0
 $current=Get-Content "$root/body-candidate.stdout.log" -Raw
 if($current -notmatch 'COMBO_DIR modes=4 errors=0 exact_functions=2 guest_proof=0'){throw 'Incomplete exact combo converter proof'}
 if($current -notmatch 'MESSAGE_CONSTRUCTION failures=0 scenarios=\d+ abi=72 scratch_offset=56 scratch_slots=4 parm=16'){throw 'Incomplete candidate ownership/ABI evidence'}
 if($Transaction -and $current -notmatch 'TRANSACTION_GROUP errors=0 heap_families=19 nested=1 global_locks=1 stack=1 cbt=1 guest_proof=0'){throw 'Incomplete whole transaction proof'}
 if($Transaction){
  $families=@([regex]::Matches($current,'(?m)^TRANSACTION_FAMILY index=(\d+) message=([0-9A-F]+) modes=5 errors=0 copy_fault=([01])\r?$'))
  $hookCases=@([regex]::Matches($current,'(?m)^HOOK_COMPLETION family=([01]) mode=([0-3]) trapped=([01]) restored=([01])\r?$'))
  if($hookCases.Count -ne 8){throw 'Missing journal/mouse completion paths'}
  $expectedDialogControl=0
  if($current -notmatch "DIALOG_GROUP errors=0 original_control=$expectedDialogControl guest_proof=0"){throw 'Incomplete exact dialog entry evidence'}
  if(@([regex]::Matches($current,'(?m)^DIALOG_ENTRY original=0 mode=')).Count -ne 5 -or
     @([regex]::Matches($current,'(?m)^ISDIALOG_ENTRY original=0 mode=')).Count -ne 3){throw 'Missing dialog entry cases'}
  if($families.Count -ne 19){throw 'Each delegated heap family must complete its five cases'}
  for($i=0;$i -lt 19;++$i){
   if([int]$families[$i].Groups[1].Value -ne $i){throw 'Family ordering missing'}
   if($i -notin @(4,11,14,16) -and $families[$i].Groups[3].Value -ne '1'){throw 'Missing actual constructor-copy fault'}
  }
 }
 foreach($row in @((Import-Csv "$root/inputs.csv")+(Import-Csv "$root/source-inputs.csv"))){if((Get-FileHash $row.Path).Hash -ne $row.Hash){throw 'Construction inputs changed during run'}}
 @{passed=$true;scope='current exact-body service doubles, not runtime/guest proof';end=[DateTime]::UtcNow.ToString('o')}|ConvertTo-Json|Set-Content "$root/result.json"
}catch{$_|Out-String|Set-Content "$root/failure.txt";exit 1}
