[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$BuildRoot,
 [string]$WorkerBuildRoot=(Join-Path $PSScriptRoot '../../build/.ninja/msvc-x86-release/worker'),
 [string]$CompilerDriver=(Join-Path $PSScriptRoot '../../build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd')
)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path -LiteralPath $root)){throw 'Fresh build-owned evidence root required'}
$null=New-Item -ItemType Directory -Path $root
$graph=Join-Path (Resolve-Path -LiteralPath $WorkerBuildRoot).Path 'build.ninja'
$driver=(Resolve-Path -LiteralPath $CompilerDriver).Path
$source=Join-Path $repo 'tests/mvdm-host/ccpu_leaf_declarations.c'
$lines=@(Get-Content -LiteralPath $graph|Where-Object {$_ -like 'ccpu_flags = *'})
if($lines.Count -ne 1){throw 'Expected one selected CCPU flag definition'}
$flags=$lines[0].Substring('ccpu_flags = '.Length).Replace('$:',':')
if($flags -notmatch '/DANSI\b' -or $flags -match '/G[zr]\b'){throw 'Expected selected ANSI/cdecl CPU profile'}
# Original host_def.h repeats NTVDM from the existing product command line.
# Suppress that known macro warning only; incompatible signatures remain errors.
$command='@echo off'+"`r`n"+'call "'+$driver+'" cl.exe '+$flags+' /WX /wd4005 /Zs %* "'+$source+'"'+"`r`n"+'exit /b %errorlevel%'+"`r`n"
$script=Join-Path $root 'compile.cmd'
[IO.File]::WriteAllText($script,$command,[Text.Encoding]::ASCII)
$results=@()
foreach($case in @('normal','negative')){
 $arguments='/d /s /c ""'+$script+'"'+$(if($case -eq 'negative'){' /DCPU_LEAF_ABI_NEGATIVE=1'}else{''})+'"'
 $p=Start-Process -FilePath $env:ComSpec -ArgumentList $arguments -WorkingDirectory $root -WindowStyle Hidden -RedirectStandardOutput "$root/$case.stdout.txt" -RedirectStandardError "$root/$case.stderr.txt" -PassThru
 $start=$p.StartTime.ToUniversalTime()
 try{
  @{pid=$p.Id;started=$start.ToString('o');command=$arguments;diagnostic_deadline_seconds=60}|ConvertTo-Json|Set-Content "$root/$case.process.json"
  if(!$p.WaitForExit(60000)){& taskkill.exe /PID $p.Id /T /F;throw 'Owned declaration compiler deadline'}
  $p.WaitForExit()
  $code=$p.ExitCode
  $text=(Get-Content "$root/$case.stdout.txt" -Raw)+(Get-Content "$root/$case.stderr.txt" -Raw)
  if($case -eq 'normal' -and $code -ne 0){throw 'Original declaration ABI check failed'}
  if($case -eq 'negative' -and ($code -eq 0 -or $text -notmatch 'C4113.*differs in parameter lists')){throw 'Wrong ADD pointer contract was not specifically rejected'}
  $results+=@{case=$case;pid=$p.Id;started=$start.ToString('o');ended=$p.ExitTime.ToUniversalTime().ToString('o');exit=$code}
 }finally{$p.Dispose()}
}
Get-FileHash -LiteralPath $graph,$source,$driver,$PSCommandPath|Export-Csv -NoTypeInformation "$root/inputs.csv"
@{result='pass';cases=$results;profile='Selected original ANSI x86 cdecl declarations';negative='ADD pointer first argument incorrectly declared as IU32 instead of IU32 *';suppressed_warning='C4005 existing NTVDM definition only';product_changed=$false}|ConvertTo-Json -Depth 5|Set-Content "$root/result.json"
