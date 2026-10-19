[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,[Parameter(Mandatory)][string]$CompilerDriver)
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
$baseline='58fa75e55';$steps=[Collections.Generic.List[object]]::new();$selections=@()
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
 $matchesFound=[regex]::Matches($text,'(?m)^(?:BOOL|VOID|DWORD|PVOID)\s+(?:FASTCALL\s+)?'+[regex]::Escape($name)+'\s*\([^;{}]*\)\s*\{')
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
   "`n#pragma pack(pop)`n" +
   (Region $header 'typedef struct _WM32MSGPARAMEX *LPWM32MSGPARAMEX;' '} WM32MSGPARAMEX;' 'message-record')
  [IO.File]::WriteAllText("$dir/message_construction_types.h",$types)
  # The original current text owner now uses the actual inline call scratch.
  $aliases=Read-Source 'src/mvdm/wow32/walias.h' $old
  $start=$aliases.IndexOf('typedef struct _DLGDATA',[StringComparison]::Ordinal)
  $end=$aliases.IndexOf('#define MSG16NEEDSTHUNKING',$start,[StringComparison]::Ordinal)
  if($start -lt 0 -or $end -lt 0){throw 'Original message records missing'}
  [IO.File]::WriteAllText("$dir/message_construction_messages.h",$aliases.Substring($start,$end-$start))
  [IO.File]::WriteAllText("$dir/message_construction_map_types.h",(Region $map 'typedef struct tagParamNode* LPPARAMNODE;' 'MAPPARAM gParamMap;' 'original-map-records'))
  $bodies=@();foreach($name in @('FindParamMap','GetParam16','AddParamMap','ParamMapUpdateNode','DeleteParamMap','FreeParamMap')){$bodies+=Body $map $name}
  foreach($name in @('WM32MDICreate','WM32Create','WM32GetText','WM32SetText','WM32GetDlgCode','WM32AskCBFormatName','WM32WinHelp')){$bodies+=Body $dispatch $name}
  $case=Region $messages '    case WM_SETTEXT:' '    case WM_ACTIVATEAPP:' 'text-thunk-case'
  $case=$case.Substring(0,$case.LastIndexOf('    case WM_ACTIVATEAPP:'))
  $releaseAt=$messages.IndexOf('VOID FASTCALL UnThunkWMMsg16(')
  $release=Region $messages.Substring($releaseAt) '    case WM_SETTEXT:' '    case WM_GETTEXT:' 'text-unthunk-case'
  $release=$release.Substring(0,$release.LastIndexOf('    case WM_GETTEXT:'))
  $bodies+='BOOL FASTCALL ThunkWMMsg16(LPMSGPARAMEX lpmpex) { LONG lParam=lpmpex->Parm16.WndProc.lParam; PLONG plParamNew=&lpmpex->lParam; switch(lpmpex->Parm16.WndProc.wMsg) {'+"`n"+$case+"`n"+'} return TRUE; }'
  $bodies+='VOID FASTCALL UnThunkWMMsg16(LPMSGPARAMEX lpmpex) { switch(lpmpex->Parm16.WndProc.wMsg) {'+"`n"+$release+"`n"+'} }'
  [IO.File]::WriteAllText("$dir/message_construction_exact_body.h",($bodies -join "`n`n"))
  $define=if($old){'/DBASELINE_IMAGE'}else{''}
  $command="@echo off`r`ncall `"$CompilerDriver`" cl.exe /nologo /TC /O2 /MT /W4 /WX /wd4700 /wd4701 /wd4706 /wd4127 /wd4213 $define /I`"$dir`" `"$fixture`" /Fo`"$dir/unit.obj`" /Fe`"$dir/unit.exe`"`r`nexit /b %errorlevel%`r`n"
  [IO.File]::WriteAllText("$dir/build.cmd",$command)
  Owned "compile-$kind" cmd.exe @('/d','/c',"`"$dir/build.cmd`"") 180 0
  Owned "body-$kind" "$dir/unit.exe" @() 30 $(if($old){1}else{0})
  Get-FileHash "$dir/message_construction_types.h","$dir/message_construction_map_types.h","$dir/message_construction_exact_body.h","$dir/unit.exe"|Export-Csv -NoTypeInformation "$dir/inputs.csv"
 }
 $negative=Get-Content "$root/body-baseline.stdout.log" -Raw
 if($negative -notmatch 'abi=64 scratch_offset=56 scratch_slots=2 parm=16'){throw 'Baseline ABI mismatch'}
 foreach($name in @('GETTEXT_CONSTRUCTOR','MDI_TITLE_OWNERSHIP','NESTED_CREATE_REJECTION','MAP_FAILURE_TEXT','MAP_NULL_ENTRY','CREATE_DEAD_WINDOW_ROLLBACK')){
  if($negative -notmatch "(?m)^FAIL $name\r?`$"){throw "Missing baseline defect $name"}
 }
 $current=Get-Content "$root/body-candidate.stdout.log" -Raw
 if($current -notmatch 'MESSAGE_CONSTRUCTION failures=0 scenarios=\d+ abi=72 scratch_offset=56 scratch_slots=4 parm=16'){throw 'Incomplete candidate ownership/ABI evidence'}
 $selections|ConvertTo-Json|Set-Content "$root/selections.json"
 foreach($row in Import-Csv "$root/source-inputs.csv"){if((Get-FileHash $row.Path).Hash -ne $row.Hash){throw 'Construction source changed during group'}}
 @{passed=$true;baseline=$baseline;negative='six named source defects';scope='exact-body service doubles, not runtime/guest proof';end=[DateTime]::UtcNow.ToString('o')}|ConvertTo-Json|Set-Content "$root/result.json"
}catch{$_|Out-String|Set-Content "$root/failure.txt";exit 1}
