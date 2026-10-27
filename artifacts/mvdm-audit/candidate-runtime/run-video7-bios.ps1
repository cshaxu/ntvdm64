$ErrorActionPreference='Stop'
$repo='O:\repos.hobby\ntvdm64'
$phase=Join-Path $repo 'build/t473-s10/candidate-validation'
$nasm=(Get-Command nasm.exe -CommandType Application | Select-Object -First 1).Source
. "$repo/tests/observation/isolated_package_cleanup.ps1"
$runtime=$null;$scope=$null;$passed=$false
try {
 $runtime=New-PhysicalRuntimeStage (Join-Path $phase 'package')
 $scope=New-IsolatedPackageScope $runtime
 $runtime|Set-Content (Join-Path $phase 'video7-r3-physical-stage.txt')
 $config=Join-Path $runtime 'VCFG.NT';$autoexec=Join-Path $runtime 'VAUTO.NT'
 foreach($pair in @(@('CONFIG.NT',$config),@('AUTOEXEC.NT',$autoexec))){
  $text=[IO.File]::ReadAllText((Join-Path $runtime ('system32/'+$pair[0])))
  $text=[regex]::Replace($text,[regex]::Escape('O:\winnt'),[Text.RegularExpressions.MatchEvaluator]{param($match) $runtime},[Text.RegularExpressions.RegexOptions]::IgnoreCase)
  [IO.File]::WriteAllText($pair[1],$text,[Text.Encoding]::ASCII)
 }
 $pif=Join-Path $runtime 'VFLAGS.PIF'
 & "$runtime/system32/PIF.EXE" create $pif --title 'Owned Video7 mode flags' --program "$runtime/system32/COMMAND.COM" --directory $runtime --arguments '/c ver' --config $config --autoexec $autoexec --close-on-exit *> (Join-Path $phase 'video7-pif.txt')
 if($LASTEXITCODE){throw 'Owned PIF create failed'}
 $inputs=@($config,$autoexec,$pif,"$runtime/system32/ntvdm.exe","$runtime/system32/COMMAND.COM",$nasm)
 $inputs|Get-FileHash|Select-Object Path,Hash|ConvertTo-Json|Set-Content (Join-Path $phase 'video7-r3-inputs.json')
 $env:PATH=(Split-Path $nasm)+';C:\Windows\System32;C:\Windows';$env:TEMP=$runtime;$env:TMP=$runtime
 $env:MVDM_OBSERVER_SHORT_HISTORY='1'
 & "$repo/tests/observation/verify-video7-mode-flags.ps1" -RuntimeRoot $runtime -ObserverPath "$repo/build/.ninja/console-observers-x64/console-startup-observer.exe" -InputPif $pif -CommandCom "$runtime/system32/COMMAND.COM" -ProbeDirectory $runtime -OutputRoot (Join-Path $phase 'video7-bios-r3') -AllModes
 $passed=$true
} finally {
 if($scope){Stop-IsolatedPackageScope $scope}
 if($runtime -and $passed){Remove-PhysicalRuntimeStage $runtime}
}
