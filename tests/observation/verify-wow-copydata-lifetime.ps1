param([Parameter(Mandatory)][string]$BuildRoot,[Parameter(Mandatory)][string]$CompilerDriver)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'));$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned source evidence required'}
$null=New-Item -ItemType Directory $root
function Owned($name,$exe,$line,$seconds,$expected=0){
 $p=Start-Process $exe -ArgumentList $line -WindowStyle Hidden -WorkingDirectory $root -PassThru -RedirectStandardOutput "$root/$name.stdout.txt" -RedirectStandardError "$root/$name.stderr.txt";$null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command="$exe $line";deadlineSeconds=$seconds}|ConvertTo-Json|Set-Content "$root/$name.process.json"
 if(!$p.WaitForExit($seconds*1000)){taskkill.exe /PID $p.Id /T /F;throw "Owned source deadline $name"}
 $p.WaitForExit();$code=$p.ExitCode;$p.Dispose();$code|Set-Content "$root/$name.exit.txt";if($code -ne $expected){throw "$name actual $code, expected $expected"}
}
function Body($source,$name,[switch]$BeforeDdeScope){
 if($BeforeDdeScope){
  $relative=$source.Substring($repo.Length+1).Replace('\','/')
  $text=(& git -C $repo show "7835ba25e:$relative") -join "`n"
  if($LASTEXITCODE){throw 'Cannot acquire immutable selector counterfactual'}
 }else{$text=[IO.File]::ReadAllText($source).Replace("`r`n","`n")}
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
 # Queue tests use exact conversion intervals, not a reimplementation of the
 # full Get/Peek loop or a claim to validate its protocol failure recovery.
 $queue=[IO.File]::ReadAllText("$original/wumsg.c").Replace("`r`n","`n")
 $getStart=$queue.IndexOf('    { /* DIVERGENCE(MVDM-HOST-DIV-350): GetMessage consumed its own queue item. */')
 $getEnd=$queue.IndexOf('    // NOTE: Call to putmsg16', $getStart)
 $peekStart=$queue.IndexOf('        /* DIVERGENCE(MVDM-HOST-DIV-350): only this conversion owns the selector. */')
 $peekEnd=$queue.IndexOf('        if (((t1.message == WM_DDE_DATA)', $peekStart)
 if($getStart -lt 0 -or $getEnd -lt 0 -or $peekStart -lt 0 -or $peekEnd -lt 0){throw 'Queue selector source intervals missing'}
 $queueBodies='static ULONG fixture_get_queue(void){DWORD vpMsg=0;MSG t1={0};ULONG ulReturn=0;'+"`n"+
    $queue.Substring($getStart,$getEnd-$getStart)+"`nreturn ulReturn;}`n"+
    'static ULONG fixture_peek_queue(UINT f5){DWORD vpf1=0;MSG t1={0};ULONG ulReturn=0;PPOSTAPPMESSAGE16 parg16=NULL;PVDMFRAME pFrame=NULL;'+"`n"+
    $queue.Substring($peekStart,$peekEnd-$peekStart)+"`nreturn ulReturn;}"
 [IO.File]::WriteAllText("$root/queue_selector_intervals.h",$queueBodies)
 $script:selections|ConvertTo-Json|Set-Content "$root/selections.json"
 $fixture="$PSScriptRoot/wow_copydata_lifetime_fixture.c"
 Get-FileHash "$original/wdde.c","$original/wmdisp32.c","$original/wmsg16.c","$original/wowhooks.c","$original/wumsg.c",$adapter,"$repo/src/wow32-dll/include/wow_message_lifetime.h",$fixture,$header,"$root/queue_selector_intervals.h",$CompilerDriver,$PSCommandPath|Export-Csv -NoTypeInformation "$root/inputs.csv"
 Owned compile $env:ComSpec ('/d /s /c "call "'+$CompilerDriver+'" cl.exe /nologo /TC /O2 /MT /W4 /WX /wd4127 /wd4706 /wd4055 /wd4054 /I"'+$root+'" /I"'+$repo+'/src/wow32-dll/include" "'+$fixture+'" /Fo"'+$root+'/unit.obj" /Fe"'+$root+'/unit.exe""') 180
 Owned body "$root/unit.exe" '' 30
 $output=Get-Content "$root/body.stdout.txt" -Raw
 if($output -notmatch 'COPYDATA_LIFETIME errors=0 scenarios=59 directions=2 stages=4 nested=1 immutable_release=1 exception=2 thread_status=1 hook_roles=2 filter_nested_different=1 embedded_headers=2 filter_output_rollback=1 guest_proof=0' -or
    $output -notmatch 'FILTER_SELECTOR cases=14 entry_values=2 modes=7 errors=0 guest_proof=0' -or
    $output -notmatch 'QUEUE_SELECTOR cases=18 owners=3 modes=3 errors=0 full_queue_proof=0 guest_proof=0'){
    throw 'Incomplete original COPYDATA and filter-selector source proof'
 }
 # Replace only the complete filter owner with its immutable pre-350 body;
 # retain every other current body/service and the original 27 assertions.
 $currentFilter=Body "$original/wowhooks.c" 'ThunkMsgFilterHook'
 $oldFilter=Body "$original/wowhooks.c" 'ThunkMsgFilterHook' -BeforeDdeScope
 $whole=[IO.File]::ReadAllText($header)
 if(!$whole.Contains($currentFilter)){throw 'Counterfactual source boundary missing'}
 $negative=Join-Path $root 'before-selector'
 $null=New-Item -ItemType Directory $negative
 [IO.File]::WriteAllText("$negative/copydata_exact_body.h",$whole.Replace($currentFilter,$oldFilter))
 Owned negative-compile $env:ComSpec ('/d /s /c "call "'+$CompilerDriver+'" cl.exe /nologo /TC /O2 /MT /W4 /WX /wd4127 /wd4706 /wd4055 /wd4054 /I"'+$negative+'" /I"'+$root+'" /I"'+$repo+'/src/wow32-dll/include" "'+$fixture+'" /Fo"'+$negative+'/unit.obj" /Fe"'+$negative+'/unit.exe""') 180
 Owned negative-body "$negative/unit.exe" '' 30 1
 $oldOutput=Get-Content "$root/negative-body.stdout.txt" -Raw
 if($oldOutput -notmatch 'FILTER_SELECTOR cases=14 entry_values=2 modes=7 errors=7 guest_proof=0' -or
    $oldOutput -notmatch 'COPYDATA_LIFETIME errors=7 scenarios=59 ' -or
    @([regex]::Matches($oldOutput,'(?m)^FAIL line=\d+ fThunkDDEmsg==\(BOOL\)entering&&!live&&!pCPDataFirst&&!hook_depth\r?$')).Count -ne 7){
    throw 'Immutable filter must fail only the seven entry-selector preservation controls'
 }
 Get-FileHash "$negative/copydata_exact_body.h","$negative/unit.exe"|
    Export-Csv -NoTypeInformation "$negative/inputs.csv"
 foreach($row in Import-Csv "$root/inputs.csv"){if((Get-FileHash $row.Path).Hash -ne $row.Hash){throw 'Frozen source changed'}}
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
