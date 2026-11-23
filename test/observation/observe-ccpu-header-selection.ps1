[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,
 [string]$WorkerBuildRoot=(Join-Path $PSScriptRoot '../../build/.ninja/msvc-x86-release/worker'))
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned output required'}
$null=New-Item -ItemType Directory $root
$worker=(Resolve-Path $WorkerBuildRoot).Path
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
RunOwned preprocess $env:ComSpec ('/d /s /c ""'+$repo+'/tools/build-fixtures.cmd" x86 ccpu-header-selection "'+$worker+'""')
$cache="$worker/diagnostics/ccpu-headers"
$nativeObjects=@(Get-Content "$cache/objects.txt"|Where-Object {$_})
if($nativeObjects.Count -ne $units.Count){throw 'Incorrect native owner inventory'}
$targets=($nativeObjects|ForEach-Object {'"'+[IO.Path]::GetRelativePath($worker,[IO.Path]::GetFullPath($_)).Replace('\','/')+'"'}) -join ' '
RunOwned commands $ninja ('-t commands ccpu-header-selection')
$commandText=Get-Content "$root/commands.stdout.txt" -Raw
# CMake uses a native batch file when the combined command exceeds CMD limits.
$commandLine=$commandText.Trim()
if($commandLine -match '^(CMakeFiles[\\/][^"\r\n ]+\.bat) [A-Za-z0-9]+$'){
 $batch=Join-Path $worker $Matches[1]
 $commandText=[IO.File]::ReadAllText($batch)
 Get-FileHash $batch|Select-Object Path,Hash|ConvertTo-Json|Set-Content "$root/native-batch-input.json"
}
$commandText|Set-Content "$root/compiler-commands.txt"
if(([regex]::Matches($commandText,'/P /Fi')).Count -ne $units.Count -or
 $commandText -match '(?i)\b(python|powershell|pwsh|node|cscript|wscript)(\.exe)?\b'){
 throw 'Expected eight native preprocessing commands'
}
RunOwned deps $ninja ('-t deps '+$targets)
$deps=Get-Content "$root/deps.stdout.txt" -Raw
if(([regex]::Matches($deps,'(?m)^.*: #deps .*\(VALID\)')).Count -ne $units.Count){throw 'Expected eight VALID native dependency records'}
$profiles=@(Get-Content "$cache/profiles.tsv")
if($profiles.Count -ne $units.Count){throw 'Incorrect profile inventory'}
$records=@(foreach($line in $profiles){
 $fields=$line -split "`t",3
 if($fields.Count -ne 3 -or $fields[0] -notin $units -or $fields[2] -notmatch '/DCPU_40_STYLE\b' -or $fields[2] -notmatch '/DSPC386\b'){throw 'Wrong selected source/profile'}
 $unit=$fields[0]
 Copy-Item "$cache/$unit.i" "$root/$unit.i"
 @{unit=$unit;source=$fields[1];profile=$fields[2];output="$root/$unit.i";sha256=(Get-FileHash "$root/$unit.i").Hash;bytes=(Get-Item "$root/$unit.i").Length}
})
foreach($r in $before){if((Get-FileHash $r.Path).Hash -ne $r.Hash){throw 'Cached production input changed'}}
$before|Select-Object Path,Hash|ConvertTo-Json|Set-Content "$root/cache-inputs.json"
Get-FileHash $PSCommandPath,"$worker/build.ninja",$ninja|Select-Object Path,Hash|ConvertTo-Json|Set-Content "$root/tool-inputs.json"
@{result='pass';units=$records;production_changed=$false;guest_changed=$false;scope='Exact selected compiler preprocessing and VALID dependency observations; preprocessing does not type-check or prove runtime behavior'}|ConvertTo-Json -Depth 5|Set-Content "$root/result.json"
'PASS eight selected translation units preprocessed; cached production inputs unchanged'
