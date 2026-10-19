[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,[Parameter(Mandatory)][string]$CompilerDriver,
 [switch]$Transaction)
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
$baseline=if($Transaction){'4b8a7c8ae6eaaca3350d640650dd9d57201fd581'}else{'58fa75e55'}
$steps=[Collections.Generic.List[object]]::new();$selections=@()
function Read-Source($path,$old){
 if($old){$value=(& git -C $repo show "${baseline}:$path") -join "`n";if($LASTEXITCODE){throw "Baseline source $path"};return $value}
 [IO.File]::ReadAllText((Join-Path $repo $path)).Replace("`r`n","`n")
}
function Region($text,$start,$end,$label){
 $at=$text.IndexOf($start,[StringComparison]::Ordinal);if($at -lt 0){throw "Missing $label start"}
 $last=$text.IndexOf($end,$at,[StringComparison]::Ordinal);if($last -lt 0){throw "Missing $label end"}
 $script:selections+=@{name=$label;offset=$at;length=$last+$end.Length-$at}
 $text.Substring($at,$last+$end.Length-$at)
}
function Body($text,$name){
 $matchesFound=[regex]::Matches($text,'(?m)^(?:BOOL|VOID|DWORD|PVOID|VPVOID|LONG)\s+(?:FASTCALL\s+)?'+[regex]::Escape($name)+'\s*\([^;{}]*\)\s*\{')
 if($matchesFound.Count -ne 1){throw "One exact body required: $name"}
 $m=$matchesFound[0];$at=$m.Index+$m.Length;$depth=1
 while($depth -and $at -lt $text.Length){if($text[$at] -eq '{'){++$depth}elseif($text[$at] -eq '}'){--$depth};++$at}
 if($depth){throw "Unbalanced $name"}
 $script:selections+=@{name=$name;offset=$m.Index;length=$at-$m.Index}
 $text.Substring($m.Index,$at-$m.Index)
}
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
 $fixture="$PSScriptRoot/wow_message_construction_fixture.c"
 Get-FileHash "$repo/src/mvdm/inc/wow.h","$repo/src/mvdm/wow32/wmdisp32.c","$repo/src/mvdm/wow32/wmdisp32.h","$repo/src/mvdm/wow32/wmsg16.c","$repo/src/mvdm/wow32/wparam.c","$repo/src/mvdm/wow32/walias.h","$repo/src/wow32-dll/include/wow_message_lifetime.h",$fixture,$PSCommandPath,$CompilerDriver|Export-Csv -NoTypeInformation "$root/source-inputs.csv"
 Get-FileHash "$repo/src/mvdm/wow32/walloc16.c","$repo/src/mvdm/wow32/wcntl32.c","$repo/src/mvdm/wow32/wcommdlg.c","$repo/src/mvdm/wow32/wowhooks.c","$repo/src/wow32-dll/source/wow_message_lifetime.c"|Export-Csv -NoTypeInformation "$root/transaction-inputs.csv"
 foreach($kind in @('baseline','candidate')){
  $old=$kind -eq 'baseline';$dir="$root/$kind";$null=New-Item -ItemType Directory $dir
  $wow=Read-Source 'src/mvdm/inc/wow.h' $old
  $header=Read-Source 'src/mvdm/wow32/wmdisp32.h' $old
  $dispatch=Read-Source 'src/mvdm/wow32/wmdisp32.c' $old
  $messages=Read-Source 'src/mvdm/wow32/wmsg16.c' $old
  $map=Read-Source 'src/mvdm/wow32/wparam.c' $old
  $types='#pragma pack(push,1)' + "`n" +
   (Region $wow 'typedef struct _PARMWP' '} PARM16, *PPARM16;' 'callback-records') + "`n" +
   (Region $wow 'typedef struct _POINT16' '} POINT16;' 'point16') + "`n" +
   (Region $wow 'typedef struct _MSG16' '} MSG16;' 'message16') + "`n" +
   (Region $wow 'typedef struct _CREATESTRUCT16' 'typedef CREATESTRUCT16 UNALIGNED *PCREATESTRUCT16;' 'create16') + "`n" +
   (Region $wow 'typedef struct _MDICREATESTRUCT16' 'typedef MDICREATESTRUCT16 UNALIGNED *PMDICREATESTRUCT16;' 'mdi16') +
   "`n" + (Region $wow 'typedef struct _RECT16' 'typedef RECT16 UNALIGNED *PRECT16;' 'rect16') +
   "`n" + (Region $wow 'typedef struct _DROPSTRUCT16' 'typedef DROPSTRUCT16 UNALIGNED *PDROPSTRUCT16;' 'drop16') +
   "`n" + (Region $wow 'typedef struct _MOUSEHOOKSTRUCT16' 'typedef MOUSEHOOKSTRUCT16 UNALIGNED *PMOUSEHOOKSTRUCT16;' 'mouse-hook16') +
   "`ntypedef VPVOID VPCREATESTRUCT16;`n" +
   (Region $wow 'typedef struct _CBT_CREATEWND16' 'typedef CBT_CREATEWND16 UNALIGNED *PCBT_CREATEWND16;' 'cbt-create16') +
   "`n" + (Region $wow 'typedef struct _CBTACTIVATESTRUCT16' 'typedef CBTACTIVATESTRUCT16 UNALIGNED *PCBTACTIVATESTRUCT16;' 'cbt-activate16') +
   "`n#pragma pack(pop)`n" +
   (Region $header 'typedef struct _WM32MSGPARAMEX *LPWM32MSGPARAMEX;' '} WM32MSGPARAMEX;' 'message-record')
  $constant=[regex]::Match($wow,'(?m)^#define WM_QUERYDROPOBJECT[^\n]*')
  if(!$constant.Success){throw 'Original drag-object message identity missing'}
  $types+="`n"+$constant.Value
  [IO.File]::WriteAllText("$dir/message_construction_types.h",$types)
  # The original current text owner now uses the actual inline call scratch.
  $aliases=Read-Source 'src/mvdm/wow32/walias.h' $old
  $start=$aliases.IndexOf('typedef struct _DLGDATA',[StringComparison]::Ordinal)
  $end=$aliases.IndexOf('#define MSG16NEEDSTHUNKING',$start,[StringComparison]::Ordinal)
  if($start -lt 0 -or $end -lt 0){throw 'Original message records missing'}
  [IO.File]::WriteAllText("$dir/message_construction_messages.h",$aliases.Substring($start,$end-$start))
  [IO.File]::WriteAllText("$dir/message_construction_map_types.h",(Region $map 'typedef struct tagParamNode* LPPARAMNODE;' 'MAPPARAM gParamMap;' 'original-map-records'))
  $bodies=@(Body (Read-Source 'src/mvdm/wow32/walloc16.c' $old) 'stackalloc16')
  foreach($name in @('FindParamMap','GetParam16','AddParamMap','ParamMapUpdateNode','DeleteParamMap','FreeParamMap')){$bodies+=Body $map $name}
  foreach($name in @('WM32MDICreate','WM32Create','WM32GetText','WM32SetText','WM32GetDlgCode','WM32AskCBFormatName','WM32WinHelp','WM32SizeClipBoard','WM32DropObject')){$bodies+=Body $dispatch $name}
  $controls=Read-Source 'src/mvdm/wow32/wcntl32.c' $old
  $bodies+=Body $controls 'WM32CBDir'
  if($Transaction){
   foreach($name in @('WM32EMGetRect','WM32EMSetRect','WM32EMReplaceSel','WM32EMGetLine','WM32EMSetTabStops','WM32CBAddString','WM32CBGetLBText','WM32CBGetDropDownControlRect','WM32LBGetText','WM32LBDir','WM32LBGetSelItems','WM32LBSetTabStops','WM32LBGetItemRect','WM32LBAddString')){
    $body=Body $controls $name
    # Preserve the original explicit INT-to-WORD assignment, not a source edit
    # to satisfy a stricter fixture warning policy than the historical owner.
    if($name -eq 'WM32EMGetLine'){$body="#pragma warning(push)`n#pragma warning(disable:4244)`n"+$body+"`n#pragma warning(pop)"}
    $bodies+=$body
   }
   $bodies+=Body (Read-Source 'src/mvdm/wow32/wcommdlg.c' $old) 'WM32msgSHAREVIOLATION'
   $bodies+=Body (Read-Source 'src/mvdm/wow32/wowhooks.c' $old) 'ThunkCbtHook'
  }
  $case=Region $messages '    case WM_SETTEXT:' '    case WM_ACTIVATEAPP:' 'text-thunk-case'
  $case=$case.Substring(0,$case.LastIndexOf('    case WM_ACTIVATEAPP:'))
  $releaseAt=$messages.IndexOf('VOID FASTCALL UnThunkWMMsg16(')
  $release=Region $messages.Substring($releaseAt) '    case WM_SETTEXT:' '    case WM_GETTEXT:' 'text-unthunk-case'
  $release=$release.Substring(0,$release.LastIndexOf('    case WM_GETTEXT:'))
  $bodies+='BOOL FASTCALL ThunkWMMsg16(LPMSGPARAMEX lpmpex) { LONG lParam=lpmpex->Parm16.WndProc.lParam; PLONG plParamNew=&lpmpex->lParam; switch(lpmpex->Parm16.WndProc.wMsg) {'+"`n"+$case+"`n"+'} return TRUE; }'
  $bodies+='VOID FASTCALL UnThunkWMMsg16(LPMSGPARAMEX lpmpex) { switch(lpmpex->Parm16.WndProc.wMsg) {'+"`n"+$release+"`n"+'} }'
  if(!$old -or $Transaction){
   $adapter=Read-Source 'src/wow32-dll/source/wow_message_lifetime.c' $old
   $bodies+=[regex]::Replace($adapter,'(?m)^#include[^\n]*\n','')
  }
  if($Transaction -and !$old){
   $branch=Region $dispatch '    if (uMsg == EM_SCROLLCARET) {' "        return 0;   // notification message, no return code`n    }" 'native-only-return'
   $marker='        stack.completed = TRUE; /* DIV-349: successful native-only notification. */'
   if(!$branch.Contains($marker)){throw 'Successful native-only return must commit, not restore its nested call'}
   $dialog=Region (Read-Source 'src/mvdm/wow32/wcommdlg.c' $false) "UINT APIENTRY`nWCD32CommonDialogProc(" 'ULONG FASTCALL WCD32ChooseColor' 'common-dialog-normal-return'
   if($dialog -notmatch '(?s)uMsg == msgWOWCHOOSEFONT\).*?stack\.completed = TRUE; /\* DIV-349: intentionally filtered, not failed\. \*/\s*return\(FALSE\);'){throw 'Intentional common-dialog filter must commit normally'}
   $finalizer=Region $dispatch '    } __finally {' "`n}`n" 'wndproc-transaction-finalizer'
   foreach($variant in @('current','omitted')){
    $selected=if($variant -eq 'current'){$branch}else{$branch.Replace($marker,'')}
    $bodies+='static LONG fixture_notification_'+$variant+'(HWND hwnd,UINT uMsg,UINT uParam,LONG lParam) { wow_message_stack stack; WM32MSGPARAMEX wm32mpex = {0}; (void)uParam; (void)lParam; wow_message_stack_enter(&stack); __try {'+"`n"+$selected+"`n"+'return -1;'+"`n"+$finalizer
   }
  }
  [IO.File]::WriteAllText("$dir/message_construction_exact_body.h",($bodies -join "`n`n"))
  $define=if($old){'/DBASELINE_IMAGE'}else{''}
  if($Transaction){$define+=' /DTRANSACTION_GROUP'}
  $command="@echo off`r`ncall `"$CompilerDriver`" cl.exe /nologo /TC /O2 /MT /W4 /WX /wd4700 /wd4701 /wd4706 /wd4127 /wd4213 $define /I`"$dir`" `"$fixture`" /Fo`"$dir/unit.obj`" /Fe`"$dir/unit.exe`"`r`nexit /b %errorlevel%`r`n"
  [IO.File]::WriteAllText("$dir/build.cmd",$command)
  Owned "compile-$kind" cmd.exe @('/d','/c',"`"$dir/build.cmd`"") 180 0
  Owned "body-$kind" "$dir/unit.exe" @() 30 $(if($old){1}else{0})
  Get-FileHash "$dir/message_construction_types.h","$dir/message_construction_map_types.h","$dir/message_construction_exact_body.h","$dir/unit.exe"|Export-Csv -NoTypeInformation "$dir/inputs.csv"
 }
 $negative=Get-Content "$root/body-baseline.stdout.log" -Raw
 if(!$Transaction -and $negative -notmatch 'abi=64 scratch_offset=56 scratch_slots=2 parm=16'){throw 'Baseline ABI mismatch'}
 if($Transaction){
  foreach($name in @('TRANSACTION_HEAP_ABORT','TRANSACTION_CREATE_COPY_ABORT','TRANSACTION_NESTED_COPY_ABORT','TRANSACTION_GLOBAL_LOCKED_ABORT','TRANSACTION_CONTROL_ABORT','TRANSACTION_CBT_ABORT')){if($negative -notmatch "(?m)^FAIL $name\r?`$"){throw "Missing P21 transaction negative $name"}}
 }else{
 foreach($name in @('GETTEXT_CONSTRUCTOR','MDI_TITLE_OWNERSHIP','NESTED_CREATE_REJECTION','MAP_FAILURE_TEXT','MAP_NULL_ENTRY','CREATE_DEAD_WINDOW_ROLLBACK')){
  if($negative -notmatch "(?m)^FAIL $name\r?`$"){throw "Missing baseline defect $name"}
 }
 }
 $current=Get-Content "$root/body-candidate.stdout.log" -Raw
 if($current -notmatch 'MESSAGE_CONSTRUCTION failures=0 scenarios=\d+ abi=72 scratch_offset=56 scratch_slots=4 parm=16'){throw 'Incomplete candidate ownership/ABI evidence'}
 if($Transaction -and $current -notmatch 'TRANSACTION_GROUP errors=0 heap_families=19 nested=1 global_locks=1 stack=1 cbt=1 guest_proof=0'){throw 'Incomplete whole transaction proof'}
 if($Transaction){
  $families=@([regex]::Matches($current,'(?m)^TRANSACTION_FAMILY index=(\d+) message=([0-9A-F]+) modes=5 errors=0 copy_fault=([01])\r?$'))
  if($families.Count -ne 19){throw 'Each delegated heap family must complete its five cases'}
  for($i=0;$i -lt 19;++$i){
   if([int]$families[$i].Groups[1].Value -ne $i){throw 'Family ordering missing'}
   if($i -notin @(4,11,14,16) -and $families[$i].Groups[3].Value -ne '1'){throw 'Missing actual constructor-copy fault'}
  }
 }
 $selections|ConvertTo-Json|Set-Content "$root/selections.json"
 foreach($row in Import-Csv "$root/source-inputs.csv"){if((Get-FileHash $row.Path).Hash -ne $row.Hash){throw 'Construction source changed during group'}}
 foreach($row in Import-Csv "$root/transaction-inputs.csv"){if((Get-FileHash $row.Path).Hash -ne $row.Hash){throw 'Transaction source changed during group'}}
 @{passed=$true;baseline=$baseline;negative='six named source defects';scope='exact-body service doubles, not runtime/guest proof';end=[DateTime]::UtcNow.ToString('o')}|ConvertTo-Json|Set-Content "$root/result.json"
}catch{$_|Out-String|Set-Content "$root/failure.txt";exit 1}
