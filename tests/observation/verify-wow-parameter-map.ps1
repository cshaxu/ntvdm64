[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,[Parameter(Mandatory)][string]$CompilerDriver)
# Exact original algorithms plus scoped service fault doubles; not a guest pass.
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'));$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
 $root.StartsWith($repo+'\build\.ninja\',[StringComparison]::OrdinalIgnoreCase) -or
 $root.StartsWith($repo+'\build\.ccache\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh disposable evidence required'}
$null=New-Item -ItemType Directory $root
$baseline='18d0ec060024912e6b03aec69bdded6c9181af03';$selections=@();$steps=@()
function Read($path,$old){if($old){$s=(& git -C $repo show "${baseline}:$path") -join "`n";if($LASTEXITCODE){throw 'Missing baseline'};return $s};[IO.File]::ReadAllText("$repo/$path").Replace("`r`n","`n")}
function Region($s,$begin,$end,$name){$a=$s.IndexOf($begin,[StringComparison]::Ordinal);$b=if($a -ge 0){$s.IndexOf($end,$a,[StringComparison]::Ordinal)}else{-1};if($a -lt 0 -or $b -lt 0){throw "Missing $name"};$length=$b+$end.Length-$a;$script:selections+=@{name=$name;offset=$a;length=$length};$s.Substring($a,$length)}
function Body($s,$name){
 $m=[regex]::Matches($s,'(?m)^(?:static\s+)?(?:BOOL|VOID|DWORD|PVOID|ULONG|void)\s+(?:(?:FASTCALL|WINAPI)\s+)?'+[regex]::Escape($name)+'\s*\([^;{}]*\)\s*\{')
 if($m.Count -ne 1){throw "Unique body required $name"};$a=$m[0].Index;$b=$a+$m[0].Length;$depth=1
 while($depth -and $b -lt $s.Length){if($s[$b] -eq '{'){++$depth}elseif($s[$b] -eq '}'){--$depth};++$b};if($depth){throw 'Unbalanced body'}
 $script:selections+=@{name=$name;offset=$a;length=$b-$a};$s.Substring($a,$b-$a)
}
function Run($name,$file,$arguments,$expected,$seconds){
 $options=@{FilePath=$file;WorkingDirectory=$root;WindowStyle='Hidden';PassThru=$true;RedirectStandardOutput="$root/$name.stdout.log";RedirectStandardError="$root/$name.stderr.log"};if($arguments.Count){$options.ArgumentList=$arguments}
 $p=Start-Process @options;$null=$p.Handle;$row=[ordered]@{name=$name;pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');file=$file;arguments=$arguments;deadlineSeconds=$seconds};$script:steps+=,$row;$script:steps|ConvertTo-Json -Depth 5|Set-Content "$root/steps.json"
 if(!$p.WaitForExit($seconds*1000)){$p.Kill($true);throw "$name deadline"};$p.Refresh();$row.exit=$p.ExitCode;$row.end=[DateTime]::UtcNow.ToString('o');$script:steps|ConvertTo-Json -Depth 5|Set-Content "$root/steps.json";if($p.ExitCode -ne $expected){throw "$name actual $($p.ExitCode), expected $expected"}
}
try{
 $paths=@('src/mvdm/inc/wow.h','src/mvdm/inc/wowusr.h','src/mvdm/inc/wowcmpat.h','src/mvdm/wow32/walias.h','src/mvdm/wow32/wmdisp32.h','src/mvdm/wow32/wparam.h','src/mvdm/wow32/wparam.c','src/mvdm/wow32/wmsg16.c','src/mvdm/wow32/wmsgcb.c','src/mvdm/wow32/wmdisp32.c','src/mvdm/wow32/wcntl32.c','src/mvdm/wow32/wuwind.c','src/wow32-dll/source/wow_message_lifetime.c','src/wow32-dll/include/wow_message_lifetime.h','tests/observation/wow_parameter_map_fixture.c','tests/observation/verify-wow-parameter-map.ps1')
 $paths|ForEach-Object{Get-FileHash "$repo/$_"}|Export-Csv -NoTypeInformation "$root/inputs.csv"
 Get-FileHash "$repo/src/opennt-abi/host-compat/include/wow_control_message_abi.h"|Export-Csv -NoTypeInformation "$root/control-abi-input.csv"
 foreach($kind in @('baseline','candidate')){
  $old=$kind -eq 'baseline';$dir="$root/$kind";$null=New-Item -ItemType Directory $dir
  $wow=Read 'src/mvdm/inc/wow.h' $old;$aliases=Read 'src/mvdm/wow32/walias.h' $old;$forward=Read 'src/mvdm/wow32/wmdisp32.h' $old
  $types="#pragma pack(push,1)`n"+(Region $wow 'typedef struct _PARMWP' '} PARM16, *PPARM16;' 'original PARM16')+"`n"
  $types+=Region (Read 'src/mvdm/inc/wowusr.h' $old) 'typedef struct _SETWINDOWTEXT16' 'typedef SETWINDOWTEXT16 UNALIGNED *PSETWINDOWTEXT16;' 'original SetWindowText arguments'
  $types+="`n#pragma pack(pop)`n"+(Region $aliases 'typedef struct _DLGDATA' '} MSGPARAMEX;' 'original reverse records')+"`n"+(Region $forward 'typedef struct _WM32MSGPARAMEX *LPWM32MSGPARAMEX;' '} WM32MSGPARAMEX;' 'original forward records')
  foreach($name in @('OLDCB_DIR','OLDCBEC_SETCOMBOFOCUS','OLDCBEC_KILLCOMBOFOCUS')){$line=[regex]::Match($wow,'(?m)^#define '+$name+'[^\n]*');if(!$line.Success){throw 'Missing original constant'};$types+="`n"+$line.Value}
  $compat=Read 'src/mvdm/inc/wowcmpat.h' $old
  foreach($name in @('WOWCF_NOCBDIRTHUNK','WOWCF_DSBASEDSTRINGPOINTERS','WOWCF_DBASEHANDLEBUG')){$types+="`n"+[regex]::Match($compat,'(?m)^#define '+$name+'[^\n]*').Value}
  [IO.File]::WriteAllText("$dir/parameter_types.h",$types)
  $map=Read 'src/mvdm/wow32/wparam.c' $old
  [IO.File]::WriteAllText("$dir/parameter_map_records.h",(Region $map '#define MAPCACHESIZE' 'MAPPARAM gParamMap;' 'original map records/constants'))
  $cacheStart=$map.IndexOf('BOOL IsCacheBlock(PBLKCACHE pc, LPVOID pv);',[StringComparison]::Ordinal);if($cacheStart -lt 0){throw 'Original complete cache missing'}
  # Git's line-array transport omits the file's final newline. Select through
  # the actual last function body, not transport-dependent trailing blank lines.
  $cacheEnd=Body $map 'IsCacheBlock'
  [IO.File]::WriteAllText("$dir/parameter_cache.h",(Region $map 'BOOL IsCacheBlock(PBLKCACHE pc, LPVOID pv);' $cacheEnd 'complete original cache'))
  $bodies=@();foreach($name in @('FindParamMap','GetParam16','AddParamMap','ParamMapUpdateNode','DeleteParamMap','W32CheckThunkParamFlag','FreeParamMap','InitParamMap')){$bodies+=Body $map $name}
  $bodies+=Body (Read 'src/mvdm/wow32/wmdisp32.c' $old) 'WM32SetText'
  $bodies+=Body (Read 'src/mvdm/wow32/wcntl32.c' $old) 'WM32CBDir'
  $combo=Read 'src/mvdm/wow32/wmsgcb.c' $old;foreach($name in @('ThunkCBMsg16','UnThunkCBMsg16')){$bodies+=Body $combo $name}
  $messages=Read 'src/mvdm/wow32/wmsg16.c' $old
  $case=Region $messages '    case WM_SETTEXT:' '    case WM_ACTIVATEAPP:' 'original reverse text acquisition';$case=$case.Substring(0,$case.LastIndexOf('    case WM_ACTIVATEAPP:'))
  $release=$messages.Substring($messages.IndexOf('VOID FASTCALL UnThunkWMMsg16'))
  $release=Region $release '    case WM_SETTEXT:' '    case WM_GETTEXT:' 'original reverse text release';$release=$release.Substring(0,$release.LastIndexOf('    case WM_GETTEXT:'))
  $bodies+='BOOL FASTCALL ThunkWMMsg16(LPMSGPARAMEX lpmpex){LONG lParam=lpmpex->Parm16.WndProc.lParam;PLONG plParamNew=&lpmpex->lParam;switch(lpmpex->Parm16.WndProc.wMsg){'+"`n"+$case+"`n}return TRUE;}"
  $bodies+='VOID FASTCALL UnThunkWMMsg16(LPMSGPARAMEX lpmpex){switch(lpmpex->Parm16.WndProc.wMsg){'+"`n"+$release+"`n}}"
  $bodies+=Body (Read 'src/mvdm/wow32/wuwind.c' $old) 'WU32SetWindowText'
  $lifetime=Read 'src/wow32-dll/source/wow_message_lifetime.c' $old
  if(!$old){foreach($name in @('wow_message_free16','release_mdi','wow_message_release32')){$bodies+=Body $lifetime $name}}
  foreach($name in @('wow_message_unwind32','wow_message_unwind16')){$bodies+=Body $lifetime $name}
  [IO.File]::WriteAllText("$dir/parameter_exact.h",($bodies -join "`n`n"))
  $define=if($old){'/DBASELINE_IMAGE'}else{''}
  [IO.File]::WriteAllText("$dir/build.cmd","@echo off`r`ncall `"$CompilerDriver`" cl.exe /nologo /TC /O2 /MT /W4 /WX /wd4706 /wd4127 /wd4213 /wd4306 $define /I`"$dir`" `"$PSScriptRoot/wow_parameter_map_fixture.c`" /Fo`"$dir/unit.obj`" /Fe`"$dir/unit.exe`"`r`nexit /b %errorlevel%`r`n")
  Run "compile-$kind" cmd.exe @('/d','/c',"`"$dir/build.cmd`"") 0 180
  Run "body-$kind" "$dir/unit.exe" @() $(if($old){1}else{0}) 30
 }
 $negative=Get-Content "$root/body-baseline.stdout.log" -Raw
 foreach($name in @('NULL_PAIR','NULL_LOOKUP','UPDATE_FALSE_ROLLBACK','UPDATE_SEH_ROLLBACK','REVERSE_TEXT_OPTIONAL','REVERSE_CB_OPTIONAL','FORWARD_TEXT_ABORT','FORWARD_CB_ABORT','HEAP_COPY_ABORT','STACK_COPY_ABORT','REVERSE_TEXT_ABORT','REVERSE_CB_ABORT','API_OPTIONAL','API_ABORT','REVERSE_TEXT_MUTATION','REVERSE_CB_MUTATION','REVERSE_CB_ABORT_MUTATION')){if($negative -notmatch "(?m)^FAIL $name\r?`$"){throw "Missing exact-source negative $name"}}
 if((Get-Content "$root/body-candidate.stdout.log" -Raw) -notmatch 'PARAMETER_MAP failures=0 scenarios=\d+ reverse=140 buffer=84 forward=72'){throw 'Incomplete current result/layout'}
 if((Get-FileHash "$root/baseline/parameter_cache.h").Hash -ne (Get-FileHash "$root/candidate/parameter_cache.h").Hash){throw 'Original cache was changed'}
 foreach($row in Import-Csv "$root/inputs.csv"){if((Get-FileHash $row.Path).Hash -ne $row.Hash){throw 'Input changed during cohort'}}
 foreach($row in Import-Csv "$root/control-abi-input.csv"){if((Get-FileHash $row.Path).Hash -ne $row.Hash){throw 'Control ABI input changed'}}
 $selections|ConvertTo-Json|Set-Content "$root/selections.json"
 @{passed=$true;baseline=$baseline;scope='exact original source/cache and actual finalizer; lower services doubled, not actual guest';end=[DateTime]::UtcNow.ToString('o')}|ConvertTo-Json|Set-Content "$root/result.json"
}catch{$_|Out-String|Set-Content "$root/failure.txt";exit 1}
