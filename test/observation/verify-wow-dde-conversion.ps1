[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,
      [Parameter(Mandatory)][string]$CompilerDriver)
# Exact selected converter bodies with bounded allocator/registration doubles.
# This proves failure cleanup and original read ordering, not guest transport.
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if([IO.Path]::GetDirectoryName($root) -ne (Join-Path $repo 'build') -or
    (Test-Path $root)){throw 'Require a fresh direct build child'}
$driver=(Resolve-Path $CompilerDriver).Path
$null=New-Item -ItemType Directory $root
$steps=[Collections.Generic.List[object]]::new()
function Run([string]$Name,[string]$File,[string[]]$Arguments,[int]$Seconds,[int]$Expected){
 $p=Start-Process $File -ArgumentList $Arguments -WorkingDirectory $root -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/$Name.out" -RedirectStandardError "$root/$Name.err"
 $null=$p.Handle
 $row=[pscustomobject]@{name=$Name;pid=$p.Id;start=$p.StartTime;file=$File;arguments=$Arguments;exit=$null}
 $steps.Add($row)
 if(!$p.WaitForExit($Seconds*1000)){& taskkill.exe /PID $p.Id /T /F;throw "$Name exceeded its bound"}
 $p.WaitForExit();$row.exit=$p.ExitCode
 $steps|ConvertTo-Json -Depth 5|Set-Content "$root/steps.json"
 if($p.ExitCode -ne $Expected){throw "$Name exit $($p.ExitCode), expected $Expected"}
}
try {
 foreach($variant in 'baseline','candidate'){
  $path='src/mvdm-host/wow32/wdde.c'
  $source=if($variant -eq 'baseline'){
   $text=(& git -C $repo show "fa427360e8d8305b1f9b01b6aa50a08981afc4d0:$path") -join "`n"
   if($LASTEXITCODE){throw 'Cannot read frozen failure baseline'}
   $text
  }else{[IO.File]::ReadAllText((Join-Path $repo $path)).Replace("`r`n","`n")}
  foreach($direction in 16,32){
   $dir=Join-Path $root "$variant-$direction";$null=New-Item -ItemType Directory $dir
   $pattern='(?m)^(?:HAND16|HANDLE)\s+DDECopyhData'+$direction+'\([^;{}]*\)\s*\{'
   $m=[regex]::Match($source,$pattern);if(!$m.Success){throw 'Missing exact converter body'}
   $end=$m.Index+$m.Length;$depth=1
   while($depth -and $end -lt $source.Length){if($source[$end] -eq '{'){$depth++}elseif($source[$end] -eq '}'){$depth--};$end++}
   if($depth){throw 'Unbalanced converter'}
   [IO.File]::WriteAllText("$dir/conversion$direction-body.h",$source.Substring($m.Index,$end-$m.Index))
   $fixture=Join-Path $PSScriptRoot "wow_dde_conversion${direction}_fixture.c"
   [IO.File]::WriteAllText("$dir/build.cmd","@echo off`r`ncall `"$driver`" cl /nologo /TC /O2 /MT /I`"$dir`" /Fo`"$dir/unit.obj`" /Fe`"$dir/unit.exe`" `"$fixture`"`r`nexit /b %errorlevel%`r`n")
   Run "$variant-$direction-build" 'cmd.exe' @('/d','/c',"`"$dir/build.cmd`"") 180 0
   Run "$variant-$direction-run" "$dir/unit.exe" @('test') 30 $(if($variant -eq 'baseline'){1}else{0})
   $expected=if($variant -eq 'candidate'){0}elseif($direction -eq 16){1}else{4}
   $output=Get-Content "$root/$variant-$direction-run.out" -Raw
   if($output -notmatch "conversion${direction}_errors=$expected guest_proof=0" -or
      ([regex]::Matches($output,'(?m)^case=')).Count -ne 4){throw 'Incomplete converter result'}
  }
 }
 @{passed=$true;cases=8;scope='Exact bodies; allocator/registration doubles; no guest proof'}|ConvertTo-Json|Set-Content "$root/result.json"
}catch{$_|Out-String|Set-Content "$root/failure.txt";exit 1}
