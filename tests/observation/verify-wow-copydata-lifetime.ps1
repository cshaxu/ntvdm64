param([Parameter(Mandatory)][string]$BuildRoot,[Parameter(Mandatory)][string]$CompilerDriver)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'));$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned source evidence required'}
$null=New-Item -ItemType Directory $root
function Owned($name,$exe,$line,$seconds){
 $p=Start-Process $exe -ArgumentList $line -WindowStyle Hidden -WorkingDirectory $root -PassThru -RedirectStandardOutput "$root/$name.stdout.txt" -RedirectStandardError "$root/$name.stderr.txt";$null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command="$exe $line";deadlineSeconds=$seconds}|ConvertTo-Json|Set-Content "$root/$name.process.json"
 if(!$p.WaitForExit($seconds*1000)){taskkill.exe /PID $p.Id /T /F;throw "Owned source deadline $name"}
 $p.WaitForExit();$code=$p.ExitCode;$p.Dispose();$code|Set-Content "$root/$name.exit.txt";if($code){throw "$name failed $code"}
}
function Body($source,$name){
 $text=[IO.File]::ReadAllText($source).Replace("`r`n","`n")
 $matchesFound=[regex]::Matches($text,'(?m)^(?:BOOL|VPVOID|PCPDATA|HWND|LONG|ULONG)\s+(?:FASTCALL\s+)?'+[regex]::Escape($name)+'\s*\([^;{}]*\)\s*\{')
 if($matchesFound.Count -ne 1){throw "One exact original function required $name"}
 $m=$matchesFound[0];$at=$m.Index+$m.Length;$depth=1
 while($depth -and $at -lt $text.Length){if($text[$at] -eq '{'){++$depth}elseif($text[$at] -eq '}'){--$depth};++$at}
 if($depth){throw 'Unbalanced source function'}
 $script:selections+=@{source=$source;name=$name;offset=$m.Index;length=$at-$m.Index}
 return $text.Substring($m.Index,$at-$m.Index)
}
function CopyCase($source){
 $text=[IO.File]::ReadAllText($source).Replace("`r`n","`n")
 $matchesFound=[regex]::Matches($text,'(?m)^    case WM_COPYDATA:')
 if($matchesFound.Count -ne 2){throw 'Exactly thunk and unthunk COPYDATA cases required'}
 foreach($m in $matchesFound){$next=[regex]::Match($text.Substring($m.Index+$m.Length),'(?m)^    case WM_');if(!$next.Success){throw 'Missing exact case boundary'};$length=$m.Length+$next.Index;$script:selections+=@{source=$source;name='COPYDATA-case';offset=$m.Index;length=$length};$text.Substring($m.Index,$length)}
}
try{
 $script:selections=@();$original="$repo/src/mvdm/wow32";$bodies=@()
 foreach($name in @('CopyDataAddNode','CopyDataFindData16','CopyDataFindData32','CopyDataDeleteNode')){$bodies+=Body "$original/wdde.c" $name}
 $bodies+=Body "$original/wmdisp32.c" 'WM32CopyData'
 $cases=@(CopyCase "$original/wmsg16.c")
 $bodies+='BOOL FASTCALL ThunkWMMsg16(LPMSGPARAMEX lpmpex) { WORD wParam=lpmpex->Parm16.WndProc.wParam; LONG lParam=lpmpex->Parm16.WndProc.lParam; PLONG plParamNew=&lpmpex->lParam; switch(lpmpex->Parm16.WndProc.wMsg) {'+"`n"+$cases[0]+"`n"+'} return TRUE; }'
 $bodies+='VOID FASTCALL UnThunkWMMsg16(LPMSGPARAMEX lpmpex) { switch(lpmpex->Parm16.WndProc.wMsg) {'+"`n"+$cases[1]+"`n"+'} }'
 $bodies+=Body "$original/wmsg16.c" 'ThunkMsg16'
 $bodies+=Body "$original/wumsg.c" 'WU32PostAppMessage'
 foreach($name in @('ThunkCallWndProcHook','ThunkMsgFilterHook')){$bodies+=Body "$original/wowhooks.c" $name}
 $adapter="$repo/src/wow32-dll/source/wow_message_lifetime.c"
 $bodies+=[regex]::Replace([IO.File]::ReadAllText($adapter),'(?m)^#include[^\r\n]*\r?\n','')
 $header="$root/copydata_exact_body.h";[IO.File]::WriteAllText($header,($bodies -join "`n`n"),[Text.UTF8Encoding]::new($false))
 $script:selections|ConvertTo-Json|Set-Content "$root/selections.json"
 $fixture="$PSScriptRoot/wow_copydata_lifetime_fixture.c"
 Get-FileHash "$original/wdde.c","$original/wmdisp32.c","$original/wmsg16.c","$original/wowhooks.c","$original/wumsg.c",$adapter,"$repo/src/wow32-dll/include/wow_message_lifetime.h",$fixture,$header,$CompilerDriver,$PSCommandPath|Export-Csv -NoTypeInformation "$root/inputs.csv"
 Owned compile $env:ComSpec ('/d /s /c "call "'+$CompilerDriver+'" cl.exe /nologo /TC /O2 /MT /W4 /WX /wd4127 /wd4706 /wd4055 /wd4054 /I"'+$root+'" /I"'+$repo+'/src/wow32-dll/include" "'+$fixture+'" /Fo"'+$root+'/unit.obj" /Fe"'+$root+'/unit.exe""') 180
 Owned body "$root/unit.exe" '' 30
 if((Get-Content "$root/body.stdout.txt" -Raw) -notmatch 'COPYDATA_LIFETIME errors=0 scenarios=27 directions=2 stages=4 nested=1 immutable_release=1 exception=2 thread_status=1 hook_roles=2 filter_nested_different=1 embedded_headers=2 filter_output_rollback=1 guest_proof=0'){throw 'Incomplete exact source proof'}
 foreach($row in Import-Csv "$root/inputs.csv"){if((Get-FileHash $row.Path).Hash -ne $row.Hash){throw 'Frozen source changed'}}
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
