[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,
 [Parameter(Mandatory)][string]$CompilerDriver,[string]$SourceFile,
 [switch]$ScalarControl)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'));$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned exact-body evidence required'}
if(!$SourceFile){$SourceFile="$repo/src/ntvdm-exe/softpc/mvdm_softpc_wow_page_domain.c"}
$SourceFile=(Resolve-Path $SourceFile).Path;$CompilerDriver=(Resolve-Path $CompilerDriver).Path
if(!$SourceFile.StartsWith($repo+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Project or sealed source required'}
$null=New-Item -ItemType Directory $root
function Owned($name,$exe,$line,$seconds){
 $job=Start-Process $exe -ArgumentList $line -WorkingDirectory $root -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/$name.stdout.txt" -RedirectStandardError "$root/$name.stderr.txt";$null=$job.Handle
 @{pid=$job.Id;start=$job.StartTime.ToUniversalTime().ToString('o');command="$exe $line";deadlineSeconds=$seconds}|ConvertTo-Json|Set-Content "$root/$name.process.json"
 if(!$job.WaitForExit($seconds*1000)){taskkill.exe /PID $job.Id /T /F;throw "Owned read-body deadline $name"}
 $job.WaitForExit();$code=$job.ExitCode;$job.Dispose();$code|Set-Content "$root/$name.exit.txt";if($code){throw "$name failed $code"}
}
try{
 $text=[IO.File]::ReadAllText($SourceFile).Replace("`r`n","`n")
 $functions=@('page_domain_copy_owned_ram','page_domain_read_owned_word','page_domain_window_lookup','window_borrow_is_current','mvdm_softpc_wow_page_domain_window_handle','mvdm_softpc_wow_page_domain_handle_metadata')
 $bodies=foreach($name in $functions){
  $pattern='(?m)^(?:static )?(?:void|ULONG|int|unsigned long) '+[regex]::Escape($name)+'\([^;{}]*\)\s*\{'
  $matchesFound=[regex]::Matches($text,$pattern);if($matchesFound.Count -ne 1){throw "One exact function body required: $name"}
  $match=$matchesFound[0];$at=$match.Index+$match.Length;$depth=1
  while($at -lt $text.Length -and $depth){if($text[$at] -eq '{'){++$depth}elseif($text[$at] -eq '}'){--$depth};++$at}
  if($depth){throw 'Unbalanced exact body'}
  $text.Substring($match.Index,$at-$match.Index)
 }
 $header="$root/owned_view_read_body.h";$fixture="$PSScriptRoot/wow_owned_view_read_fixture.c"
 [IO.File]::WriteAllText($header,($bodies -join "`n`n"),[Text.UTF8Encoding]::new($false))
 Get-FileHash $SourceFile,$fixture,$header,$CompilerDriver,$PSCommandPath|Export-Csv -NoTypeInformation "$root/inputs.csv"
 $line='/d /s /c "call "'+$CompilerDriver+'" cl.exe /nologo /TC /O2 /MT /W4 /WX /I"'+$root+'" "'+$fixture+'" /Fo"'+$root+'/unit.obj" /Fe"'+$root+'/unit.exe" /link user32.lib"'
 Owned compile $env:ComSpec $line 120
 $bytes=[IO.File]::ReadAllBytes("$root/unit.exe");$pe=[BitConverter]::ToInt32($bytes,60)
 if([BitConverter]::ToUInt16($bytes,$pe+4) -ne 0x14c){throw 'MSVC Win32/x86 required'}
 $mode=if($ScalarControl){'scalar'}else{'fast'}
 Owned body "$root/unit.exe" ('--'+$mode) 30
 if((Get-Content "$root/body.stdout.txt" -Raw) -notmatch ('OWNED_VIEW_READ errors=0 selected='+$mode+' ')){throw 'Complete read control marker missing'}
 foreach($row in Import-Csv "$root/inputs.csv"){if((Get-FileHash $row.Path).Hash -ne $row.Hash){throw 'Frozen exact body changed'}}
 Get-FileHash "$root/unit.exe","$root/unit.obj"|Export-Csv -NoTypeInformation "$root/artifacts.csv"
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
