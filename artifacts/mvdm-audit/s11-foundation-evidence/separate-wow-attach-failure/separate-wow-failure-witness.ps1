$ErrorActionPreference='Stop'
$repo=(Resolve-Path '.').Path
$out="$repo/build/t473-s11/separate-wow-failure-baseline"
if(Test-Path $out){throw 'fresh result required'}
New-Item -ItemType Directory $out | Out-Null
$driver="$repo/build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd"
& $driver cl.exe /nologo /O2 /MT /W4 /WX "/Fo$out/host.obj" "/Fe$out/host.exe" "$repo/build/t473-s11/separate-wow-failure-witness.c" *> "$out/compile.txt"
if($LASTEXITCODE){throw 'native witness compilation failed'}
$source="$repo/build/t473-s11/idle-separate-formal-final/IDLPROBE.EXE"
if((Get-FileHash $source).Hash -ne '080881A754E5871E739174C0DAF7EC93EBAFAF243BA5D1A8D9C5D6CEF08FD5F9'){throw 'guest identity changed'}
. "$repo/tests/observation/isolated_package_cleanup.ps1"
$package="$repo/build/t473-s11/wow-attach-marked-baseline-package/package"
$runtime=$null;$scope=$null;$proc=$null
try {
 $runtime=New-PhysicalRuntimeStage $package
 $scope=New-IsolatedPackageScope $runtime
 New-Item -ItemType Directory "$runtime/tests" -Force | Out-Null
 Copy-Item "$out/host.exe" "$runtime/tests/host.exe"
 Copy-Item $source "$runtime/tests/IDLPROBE.EXE"
 $scope.Paths+="$runtime\tests\host.exe"
 Save-IsolatedPackageManifest $runtime "$out/manifest.json"
 $start=[Diagnostics.ProcessStartInfo]::new("$repo/build/.ninja/console-observers-x64/console-startup-observer.exe")
 $start.UseShellExecute=$false;$start.CreateNoWindow=$true
 foreach($arg in @("$runtime/system32/run16.exe","$runtime/tests","$out/observer.txt",'--wait',"$runtime/tests/host.exe","$runtime/tests/IDLPROBE.EXE","$out/native.txt",'--observation-timeout-ms','40000')){$start.ArgumentList.Add($arg)}
 $start.Environment['TEMP']=$runtime;$start.Environment['TMP']=$runtime
 $start.Environment['PATH']="$runtime/system32;"+$env:SystemRoot+'\System32'
 $start.Environment['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
 $start.Environment['MVDM_S42_WOW_RETURN_REPORT_PATH']="$out/stage.txt"
 $start.Environment['MVDM_TEST_WOW_ATTACH_MARKER']="$out/attach.txt"
 $proc=[Diagnostics.Process]::Start($start)
 if(!$proc.WaitForExit(45000)){throw 'observer did not finish'}
 if($proc.ExitCode -or (Get-Content "$out/observer.txt" -Raw) -notmatch 'exit=0x00000000' -or
    (Get-Content "$out/native.txt" -Raw) -notmatch 'separate-startup-exit=255' -or
    (Get-Content "$out/native.txt" -Raw) -notmatch 'image=.*run16.exe' -or
    (Get-Content "$out/attach.txt" -Raw) -ne 'attach-failure' -or
    (Get-Content "$out/stage.txt" -Raw) -notmatch 'wow-stage stage=load-failed') {throw 'separate failure evidence incomplete'}
 'PASS separate WOW startup reports forced real provider attach failure'|Set-Content "$out/result.txt"
} finally {
 if($proc){if(!$proc.HasExited){$proc.Kill();$proc.WaitForExit()};$proc.Dispose()}
 if($scope){Stop-IsolatedPackageScope $scope}
 if($runtime){Remove-PhysicalRuntimeStage $runtime}
}
