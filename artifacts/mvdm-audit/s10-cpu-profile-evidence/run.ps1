$ErrorActionPreference='Stop'
$repo='O:\repos.hobby\ntvdm64'
$d="$repo\build\t473-s10\cpu-profile-audit"
$cases=@(@{name='sasCdef';src='sasCdef.c';include='#include\s+"gdpvar.h"';key='gdpvar'},@{name='sasCdef';src='sasCdef.c';include='#include\s+"Univer_c.h"';key='univer'},@{name='c_reg';src='c_reg.c';include='#include\s+<Pigger_c.h>';key='pigger'})
foreach($case in $cases){
 $source=Get-Content -Raw "$repo\src\mvdm\softpc.new\base\ccpu386\$($case.src)"; $matches=[regex]::Matches($source,$case.include); if($matches.Count -ne 1){throw "unexpected include count $($case.key) $($matches.Count)"}
 $command=Get-Content "$d\$($case.name)-command.txt" -Raw; $command=$command.Substring($command.IndexOf('cl.exe ')).Trim(); $command=$command.Replace('/c /MT','/EP /MT').Replace('/showIncludes ',''); $command=[regex]::Replace($command,'/Foobj/ccpu/\S+\s+O:/repos\.hobby/ntvdm64/src/mvdm/softpc\.new/base/ccpu386/\S+\.c',('"'+$d.Replace('\','/')+'/input.c"'))
 foreach($variant in 'baseline','without'){
  $text=if($variant -eq 'baseline'){$source}else{[regex]::Replace($source,$case.include,'')}; [IO.File]::WriteAllText("$d\input.c",$text,[Text.Encoding]::ASCII)
  @('@echo off','call "O:\repos.hobby\ntvdm64\build\.ninja\msvc-x86-release\wow32\msvc-x86.cmd"','cd /d "O:\repos.hobby\ntvdm64\build\.ninja\msvc-x86-release\worker"',($command+' > "'+$d+'\'+$case.key+'-'+$variant+'.i" 2> "'+$d+'\'+$case.key+'-'+$variant+'.err"'),'exit /b %errorlevel%') | Set-Content "$d\preprocess.cmd" -Encoding ASCII
  & cmd /c "$d\preprocess.cmd"; if($LASTEXITCODE){"$($case.key) $variant failed $LASTEXITCODE";break}
 }
 $a="$d\$($case.key)-baseline.i";$b="$d\$($case.key)-without.i"; if((Test-Path $a) -and (Test-Path $b)){"$($case.key) baseline=$((Get-FileHash $a).Hash) removed=$((Get-FileHash $b).Hash)"}
}
