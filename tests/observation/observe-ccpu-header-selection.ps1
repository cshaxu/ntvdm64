[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,
 [string]$WorkerBuildRoot=(Join-Path $PSScriptRoot '../../build/.ninja/msvc-x86-release/worker'),
 [string]$CompilerDriver=(Join-Path $PSScriptRoot '../../build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd'))
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned output required'}
$null=New-Item -ItemType Directory $root
$worker=(Resolve-Path $WorkerBuildRoot).Path
$driver=(Resolve-Path $CompilerDriver).Path
$units=@('c_div64','c_tlb','c_tsksw','c_xfer','c_xtrn','c_main','c_reg','zfrsrvd')
$objects=@($units|ForEach-Object {Join-Path $worker "obj/ccpu/$_.obj"})
$before=Get-FileHash ($objects+@("$worker/original-ccpu386.lib","$worker/ntvdm.exe"))
function RunOwned($Name,$Image,$Arguments){
 $p=Start-Process -FilePath $Image -ArgumentList $Arguments -WorkingDirectory $worker -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/$Name.stdout.txt" -RedirectStandardError "$root/$Name.stderr.txt"
 try{
  @{pid=$p.Id;started=$p.StartTime.ToUniversalTime().ToString('o');image=$Image;arguments=$Arguments;diagnostic_budget_seconds=60}|ConvertTo-Json|Set-Content "$root/$Name.process.json"
  if(!$p.WaitForExit(60000)){& taskkill.exe /PID $p.Id /T /F;throw "Owned $Name budget exceeded"}
  $p.WaitForExit();$code=$p.ExitCode
  @{exit=$code;finished=$p.ExitTime.ToUniversalTime().ToString('o')}|ConvertTo-Json|Set-Content "$root/$Name.completion.json"
  if($code){throw "$Name failed"}
 }finally{$p.Dispose()}
}
$ninja=(Get-Command ninja).Source
$targets=($units|ForEach-Object {'obj/ccpu/'+$_+'.obj'}) -join ' '
RunOwned commands $ninja ('-t commands '+$targets)
RunOwned deps $ninja ('-t deps '+$targets)
$commands=@(Get-Content "$root/commands.stdout.txt")
if($commands.Count -ne $units.Count){throw 'Unexpected transitive commands; do not guess compiler selection'}
$records=@(foreach($unit in $units){
 $target='obj/ccpu/'+$unit+'.obj'
 $command=@($commands|Where-Object {$_ -match [regex]::Escape('/Fo'+$target)+'\b'})
 if($command.Count -ne 1 -or $command[0] -notmatch '/DCPU_40_STYLE\b' -or $command[0] -notmatch '/DSPC386\b'){throw 'Wrong selected source/profile command'}
 $pp=$command[0].Replace('/Fo'+$target,('/P /Fi"'+$root+'/'+$unit+'.i"'))
 $body='@echo off'+"`r`n"+'call "'+$driver+'" '+$pp+"`r`n"+'exit /b %errorlevel%'+"`r`n"
 [IO.File]::WriteAllText("$root/$unit.cmd",$body,[Text.Encoding]::ASCII)
 RunOwned $unit $env:ComSpec ('/d /s /c ""'+$root+'/'+$unit+'.cmd""')
 @{unit=$unit;original_command=$command[0];preprocess_command=$pp;output="$root/$unit.i";sha256=(Get-FileHash "$root/$unit.i").Hash;bytes=(Get-Item "$root/$unit.i").Length}
})
foreach($r in $before){if((Get-FileHash $r.Path).Hash -ne $r.Hash){throw 'Cached production input changed'}}
$before|Select-Object Path,Hash|ConvertTo-Json|Set-Content "$root/cache-inputs.json"
Get-FileHash $PSCommandPath,$driver,"$worker/build.ninja",$ninja|Select-Object Path,Hash|ConvertTo-Json|Set-Content "$root/tool-inputs.json"
@{result='pass';units=$records;production_changed=$false;guest_changed=$false;scope='Exact selected compiler preprocessing and VALID dependency observations; preprocessing does not type-check or prove runtime behavior'}|ConvertTo-Json -Depth 5|Set-Content "$root/result.json"
'PASS eight selected translation units preprocessed; cached production inputs unchanged'
