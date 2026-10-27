[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,
 [string]$WorkerBuildRoot=(Join-Path $PSScriptRoot '../../build/.ninja/msvc-x86-release/worker'),
 [string]$CompilerDriver=(Join-Path $PSScriptRoot '../../build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd'))
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path -LiteralPath $root)){throw 'Fresh build-owned output required'}
$null=New-Item -ItemType Directory $root
$worker=(Resolve-Path -LiteralPath $WorkerBuildRoot).Path
$driver=(Resolve-Path -LiteralPath $CompilerDriver).Path
$source=Join-Path $repo 'tests/mvdm-host/ccpu_neg64_test.c'
$object=Join-Path $worker 'obj/ccpu/c_neg64.obj'
$before=Get-FileHash $object,(Join-Path $worker 'original-ccpu386.lib'),(Join-Path $worker 'ntvdm.exe')
$line=@(Get-Content "$worker/build.ninja"|Where-Object {$_ -like 'ccpu_flags = *'})
if($line.Count -ne 1){throw 'One actual CCPU flag definition required'}
$flags=$line[0].Substring('ccpu_flags = '.Length).Replace('$:',':')
if($flags -notmatch '/DCPU_40_STYLE\b' -or $flags -notmatch '/DSPC386\b' -or $flags -match '/G[zr]\b'){throw 'Wrong selected CPU/ABI profile'}
$results=[Collections.Generic.List[object]]::new()
function RunOwned([string]$Name,[string]$Image,[string]$Arguments,[int]$Expected){
 $start=@{FilePath=$Image;WorkingDirectory=$root;WindowStyle='Hidden';PassThru=$true;RedirectStandardOutput="$root/$Name.stdout.txt";RedirectStandardError="$root/$Name.stderr.txt"}
 if($Arguments){$start.ArgumentList=$Arguments}
 $p=Start-Process @start
 try{
  @{pid=$p.Id;started=$p.StartTime.ToUniversalTime().ToString('o');image=$Image;arguments=$Arguments;diagnostic_deadline_seconds=60}|ConvertTo-Json|Set-Content "$root/$Name.process.json"
  if(!$p.WaitForExit(60000)){& taskkill.exe /PID $p.Id /T /F;throw "Owned $Name deadline"}
  $p.WaitForExit();$code=$p.ExitCode
  $results.Add(@{case=$Name;pid=$p.Id;exit=$code;expected=$Expected;finished=$p.ExitTime.ToUniversalTime().ToString('o')})
  if(($Expected -eq 0 -and $code -ne 0) -or ($Expected -ne 0 -and $code -eq 0)){throw "$Name unexpected exit $code"}
 }finally{$p.Dispose()}
}
foreach($name in @('normal','wrong-abi')){
 $define=if($name -eq 'wrong-abi'){' /DCCPU_NEG64_WRONG_ABI=1'}else{''}
 $command='@echo off'+"`r`n"+'call "'+$driver+'" cl.exe '+$flags+' /O2 /WX /wd4005'+$define+' /Fo"'+$root+'/'+$name+'.obj" "'+$source+'"'+"`r`n"+'exit /b %errorlevel%'+"`r`n"
 [IO.File]::WriteAllText("$root/$name.cmd",$command,[Text.Encoding]::ASCII)
 RunOwned $name $env:ComSpec ('/d /s /c ""'+$root+'/'+$name+'.cmd""') $(if($name -eq 'normal'){0}else{1})
}
if((Get-Content "$root/wrong-abi.stdout.txt" -Raw) -notmatch 'C4113.*differs in parameter lists'){throw 'Wrong original declaration signature not specifically rejected'}
$command='@echo off'+"`r`n"+'call "'+$driver+'" link.exe /nologo /machine:x86 /subsystem:console /out:"'+$root+'/fixture.exe" /map:"'+$root+'/fixture.map" "'+$root+'/normal.obj" "'+$object+'" kernel32.lib legacy_stdio_definitions.lib'+"`r`n"+'exit /b %errorlevel%'+"`r`n"
[IO.File]::WriteAllText("$root/link.cmd",$command,[Text.Encoding]::ASCII)
RunOwned link $env:ComSpec ('/d /s /c ""'+$root+'/link.cmd""') 0
RunOwned behavior "$root/fixture.exe" '' 0
if((Get-Content "$root/behavior.stdout.txt" -Raw) -notmatch 'PASS CCPU NEG64 cases=631410 pointer_orders=2 unsigned_mod64=1 guards=1 involution=1'){throw 'Behavior witness missing'}
RunOwned wrong-oracle "$root/fixture.exe" '--wrong-oracle' 1
if((Get-Content "$root/wrong-oracle.stdout.txt" -Raw) -notmatch 'FAIL NEG64 case=0'){throw 'Wrong oracle did not fail at the first case'}
foreach($record in $before){if((Get-FileHash $record.Path).Hash -ne $record.Hash){throw 'Cached product changed'}}
$before|Select-Object Path,Hash|ConvertTo-Json|Set-Content "$root/normal-cache-inputs.json"
Get-FileHash $source,$PSCommandPath,$driver,"$worker/build.ninja",$object,"$repo/src/mvdm/softpc.new/base/ccpu386/c_neg64.c","$repo/src/mvdm/softpc.new/base/ccpu386/c_neg64.h","$root/fixture.exe"|Select-Object Path,Hash|ConvertTo-Json|Set-Content "$root/inputs.json"
@{result='pass';cases=$results;observed_cases=631410;original_object_used=$true;cpu_state_stubs=$false;product_changed=$false;guest_changed=$false;scope='Selected native helper and typed ABI, distinct valid IS32 output words; not signed DIV/IMUL decoder or guest fault acceptance'}|ConvertTo-Json -Depth 6|Set-Content "$root/result.json"
Get-Content "$root/result.json"
