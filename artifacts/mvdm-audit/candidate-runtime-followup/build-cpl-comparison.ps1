$ErrorActionPreference='Stop'
$repo='O:\repos.hobby\ntvdm64'
$provider=Join-Path $repo 'build/.ninja/msvc-x86-release/worker'
$out=Join-Path $repo 'build/t473-s10/candidate-validation/cpl-comparison-r4'
if(Test-Path -LiteralPath $out){throw 'Fresh comparison output required'}
$null=New-Item -ItemType Directory $out
$ninja=(Get-Command ninja.exe -CommandType Application|Select-Object -First 1).Source
$driver=Join-Path $repo 'build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd'
$source=Join-Path $repo 'src/mvdm/dpmi32/modesw.c'
$text=[IO.File]::ReadAllText($source)
$pattern='(?m)^#ifndef i386\r?\n    /\* (?:CPU40 keeps CPL independently of CR0; real mode executes at CPL 0\.|Match the real-mode privilege before executing the DOSX continuation\.) \*/\r?\n    setCPL\(0\);\r?\n#endif\r?\n'
if([regex]::Matches($text,$pattern).Count -ne 2){throw 'Expected exactly the two new CPL assignments'}
$comparison=[regex]::Replace($text,$pattern,'')
$expected=(& git -C $repo show '35424a126^:src/mvdm/dpmi32/modesw.c') -join "`n"
if($LASTEXITCODE -or $comparison.Replace("`r`n","`n").TrimEnd() -cne $expected.TrimEnd()){throw 'Comparison must equal the complete pre-354 mode-switch file'}
$copy=Join-Path $out 'modesw.c'
[IO.File]::WriteAllText($copy,$comparison)
$normal=@('obj/dpmi/modesw.obj','original-mvdm-dpmi32.lib','ntvdm.exe','ntvdm.lib')|ForEach-Object{Get-FileHash (Join-Path $provider $_)}
$normal|Select-Object Path,Hash|ConvertTo-Json|Set-Content "$out/normal-inputs.json"
Push-Location $provider
try {
 $compile=@(& $ninja -t commands obj/dpmi/modesw.obj)[-1]
 if($LASTEXITCODE){throw 'Actual DPMI compiler command unavailable'}
 $compile=$compile.Replace($source.Replace('\','/'),$copy.Replace('\','/')).Replace('obj/dpmi/modesw.obj',($out+'/modesw.obj').Replace('\','/'))
 if($compile.Contains('obj/dpmi/modesw.obj') -or !$compile.Contains($copy.Replace('\','/'))){throw 'Compile substitution failed'}
 $library=@(& $ninja -t commands original-mvdm-dpmi32.lib)[-1]
 if($LASTEXITCODE -or $library -notmatch '^lib(?:\.exe)? '){throw 'Actual DPMI library command unavailable'}
 $library=$library.Replace('obj/dpmi/modesw.obj',($out+'/modesw.obj').Replace('\','/')).Replace('/out:original-mvdm-dpmi32.lib',('/out:'+($out+'/dpmi-comparison.lib').Replace('\','/')))
 if($library.Contains('/out:original-mvdm-dpmi32.lib')){throw 'Library output substitution failed'}
 $link=@(& $ninja -t commands ntvdm.exe)[-1]
 if($LASTEXITCODE -or $link -notmatch '^(?:cmd\.exe /d /s /c ")?link(?:\.exe)? ' -or !$link.Contains('original-mvdm-dpmi32.lib')){throw 'Actual worker link unavailable'}
 $link=$link.Replace('original-mvdm-dpmi32.lib',($out+'/dpmi-comparison.lib').Replace('\','/')).Replace('/out:ntvdm.exe',('/out:'+($out+'/ntvdm.exe').Replace('\','/')))
 $link=$link.Replace('ntvdm.exe.map',($out+'/ntvdm.exe.map').Replace('\','/'))
 $link=[regex]::Replace($link,'(?i)/implib:\S+',('/implib:'+($out+'/comparison.lib').Replace('\','/')))
 if($link.Contains('/out:ntvdm.exe') -or !$link.Contains(($out+'/ntvdm.exe').Replace('\','/'))){throw 'Worker output substitution failed'}
 foreach($step in @(@('compile',$compile),@('library',$library),@('link',$link))){
  $step[1]|Set-Content "$out/$($step[0])-command.txt"
  & cmd.exe /d /s /c ('call "'+$driver+'" '+$step[1]) *> "$out/$($step[0]).log"
  if($LASTEXITCODE){throw "Comparison $($step[0]) failed"}
 }
 foreach($record in $normal){if((Get-FileHash $record.Path).Hash -ne $record.Hash){throw 'Normal build cache changed'}}
 Get-FileHash $source,$copy,$PSCommandPath,"$out/ntvdm.exe","$provider/build.ninja"|Select-Object Path,Hash|ConvertTo-Json|Set-Content "$out/inputs.json"
 @{kind='diagnostic-only';publishable=$false;guestChanged=$false;productionCacheChanged=$false;variable='Only the two added real-mode CPL assignments removed; complete file equals pre-354 source';comparisonTo='35424a126^'}|ConvertTo-Json|Set-Content "$out/result.json"
} finally {Pop-Location}
