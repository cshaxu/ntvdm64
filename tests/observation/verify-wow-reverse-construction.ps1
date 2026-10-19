[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,[Parameter(Mandatory)][string]$CompilerDriver)
# Run as an owned hidden background job. Exact original source is selected;
# allocation/memory doubles supplement, never replace, real guest acceptance.
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
 $root.StartsWith($repo+'\build\.ninja\',[StringComparison]::OrdinalIgnoreCase) -or
 $root.StartsWith($repo+'\build\.ccache\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh disposable root required'}
$null=New-Item -ItemType Directory $root
$baseline='049ae27da';$selections=@();$steps=@()
function Read($path,$old){if($old){$s=(& git -C $repo show "${baseline}:$path") -join "`n";if($LASTEXITCODE){throw 'Missing baseline'};return $s};[IO.File]::ReadAllText("$repo/$path").Replace("`r`n","`n")}
function Slice($source,$start,$end,$label){
 $a=$source.IndexOf($start,[StringComparison]::Ordinal);if($a -lt 0){throw "Missing $label start"}
 $b=$source.IndexOf($end,$a,[StringComparison]::Ordinal);if($b -lt 0){throw "Missing $label end"}
 $script:selections+=@{name=$label;offset=$a;length=$b-$a}
 $source.Substring($a,$b-$a)
}
function Body($source,$name){
 $m=[regex]::Matches($source,'(?m)^(?:BOOL|VOID)\s+(?:FASTCALL\s+)?'+[regex]::Escape($name)+'\s*\([^;{}]*\)\s*\{')
 if($m.Count -ne 1){throw "Unique original body required $name"}
 $a=$m[0].Index;$b=$a+$m[0].Length;$depth=1
 while($depth -and $b -lt $source.Length){if($source[$b] -eq '{'){++$depth}elseif($source[$b] -eq '}'){--$depth};++$b}
 if($depth){throw 'Unbalanced source'}
 $script:selections+=@{name=$name;offset=$a;length=$b-$a};$source.Substring($a,$b-$a)
}
function Run($name,$file,$Arguments,$expected,$seconds){
 $options=@{FilePath=$file;WorkingDirectory=$root;WindowStyle='Hidden';PassThru=$true;RedirectStandardOutput="$root/$name.stdout.log";RedirectStandardError="$root/$name.stderr.log"};if($Arguments.Count){$options.ArgumentList=$Arguments}
 $p=Start-Process @options;$null=$p.Handle
 $row=[ordered]@{name=$name;pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');file=$file;arguments=$Arguments;deadlineSeconds=$seconds}
 $script:steps+=,$row;$script:steps|ConvertTo-Json -Depth 5|Set-Content "$root/steps.json"
 if(!$p.WaitForExit($seconds*1000)){$p.Kill($true);throw "$name deadline"};$p.Refresh();$row.exit=$p.ExitCode;$row.end=[DateTime]::UtcNow.ToString('o');$script:steps|ConvertTo-Json -Depth 5|Set-Content "$root/steps.json"
 if($p.ExitCode -ne $expected){throw "$name actual $($p.ExitCode), expected $expected"}
}
try{
 $paths=@('src/mvdm/inc/wow.h','src/mvdm/wow32/walias.h','src/mvdm/wow32/wstruc.h','src/mvdm/wow32/wstruc.c','src/mvdm/wow32/wmsgem.c','src/mvdm/wow32/wmsglb.c','src/mvdm/wow32/wmsg16.c','src/opennt-host/windows/core/ntuser/inc/user.h','src/opennt-host/windows/core/ntuser/client/lboxrare.c','tests/observation/wow_reverse_construction_fixture.c','tests/observation/verify-wow-reverse-construction.ps1')
 $paths|ForEach-Object{Get-FileHash "$repo/$_"}|Export-Csv -NoTypeInformation "$root/inputs.csv"
 foreach($kind in @('baseline','candidate')){
  $old=$kind -eq 'baseline';$dir="$root/$kind";$null=New-Item -ItemType Directory $dir
  $wow=Read 'src/mvdm/inc/wow.h' $old;$alias=Read 'src/mvdm/wow32/walias.h' $old;$struct=Read 'src/mvdm/wow32/wstruc.h' $old
  $types="#pragma pack(push,1)`n"+(Slice $wow 'typedef struct _PARMWP' '} PARM16, *PPARM16;' 'original PARM16')+"} PARM16, *PPARM16;`n#pragma pack(pop)`n"
  $types+=(Slice $alias 'typedef struct _DLGDATA' '#define MSG16NEEDSTHUNKING' 'original message/inline records')
  [IO.File]::WriteAllText("$dir/reverse_types.h",$types)
  $bodies=@((Slice $struct '#define STACKORHEAPALLOC' '#define PUTINTARRAY16' 'original stack-or-heap macros'))
  $user=Read 'src/opennt-host/windows/core/ntuser/inc/user.h' $old
  $bodies+=Slice $user '#define MultDiv' '#define XPixFromXDU' 'original dialog-unit arithmetic'
  $conversions=Read 'src/mvdm/wow32/wstruc.c' $old
  foreach($name in @('getintarray16','putintarray16')){$bodies+=Body $conversions $name}
  foreach($pair in @(@('wmsgem.c','ThunkEMMsg16','UnThunkEMMsg16'),@('wmsglb.c','ThunkLBMsg16','UnThunkLBMsg16'))){$source=Read "src/mvdm/wow32/$($pair[0])" $old;foreach($name in $pair[1..2]){$bodies+=Body $source $name}}
  $native=Read 'src/opennt-host/windows/core/ntuser/client/lboxrare.c' $old
  $bodies+=Body $native 'LBSetTabStops'
  $destruction=Slice $native '        if (pLBIV->iTabPixelPositions != NULL) {' '        Unlock(&pLBIV->spwnd);' 'original tab destruction'
  $bodies+='VOID DestroyTabs(PLBIV pLBIV){'+"`n"+$destruction+"`n}"
  $source=Read 'src/mvdm/wow32/wmsg16.c' $old
  $case=Slice $source '    case WM_WINHELP:' '    case WM_NCCALCSIZE:' 'original help acquisition'
  $release=$source.Substring($source.IndexOf('VOID FASTCALL UnThunkWMMsg16'))
  $caseFree=Slice $release '    case WM_WINHELP:' '    case WM_NCCALCSIZE:' 'original help release'
  $bodies+='BOOL ThunkHelp(LPMSGPARAMEX lpmpex){LONG lParam=lpmpex->Parm16.WndProc.lParam;PLONG plParamNew=&lpmpex->lParam;switch(lpmpex->Parm16.WndProc.wMsg){'+"`n"+$case+"`n}return TRUE;}"
  $bodies+='VOID UnThunkHelp(LPMSGPARAMEX lpmpex){switch(lpmpex->Parm16.WndProc.wMsg){'+"`n"+$caseFree+"`n}}"
  [IO.File]::WriteAllText("$dir/reverse_exact.h",($bodies -join "`n`n"))
  $cmd="@echo off`r`ncall `"$CompilerDriver`" cl.exe /nologo /TC /O2 /MT /W4 /WX /wd4706 /wd4127 /wd4213 /I`"$dir`" `"$PSScriptRoot/wow_reverse_construction_fixture.c`" /Fo`"$dir/unit.obj`" /Fe`"$dir/unit.exe`" /link user32.lib`r`nexit /b %errorlevel%`r`n"
  [IO.File]::WriteAllText("$dir/build.cmd",$cmd)
  Run "compile-$kind" cmd.exe @('/d','/c',"`"$dir/build.cmd`"") 0 180
  Run "body-$kind" "$dir/unit.exe" @() $(if($old){1}else{0}) 30
 }
 $negative=Get-Content "$root/body-baseline.stdout.log" -Raw
 foreach($name in @('EDIT_REPLACE_ALLOC','EDIT_TABS_ALLOC','LIST_SELECTION_ALLOC','LIST_TABS_ALLOC','HELP_ALLOC','EDIT_TABS_SIGNED_RELEASE','LIST_TABS_SIGNED_RELEASE','LIST_TABS_RESET_CLEARS_OWNERSHIP','LIST_TABS_FAILED_FREE_KEEPS_OWNERSHIP')){if($negative -notmatch "(?m)^FAIL $name\r?`$"){throw "Missing negative $name"}}
 if((Get-Content "$root/body-candidate.stdout.log" -Raw) -notmatch 'REVERSE_CONSTRUCTION failures=0 scenarios=21 buffer=\d+ record=\d+'){throw 'Incomplete candidate'}
 foreach($row in Import-Csv "$root/inputs.csv"){if((Get-FileHash $row.Path).Hash -ne $row.Hash){throw 'Source changed'}}
 $selections|ConvertTo-Json|Set-Content "$root/selections.json"
 @{passed=$true;baseline=$baseline;negative='five missing native buffers, two signed releases and native tab reset/failure';scope='exact-source service doubles, not runtime acceptance';end=[DateTime]::UtcNow.ToString('o')}|ConvertTo-Json|Set-Content "$root/result.json"
}catch{$_|Out-String|Set-Content "$root/failure.txt";exit 1}
