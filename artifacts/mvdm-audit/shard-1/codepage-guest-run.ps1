$ErrorActionPreference='Stop'
. (Join-Path (Get-Location) 'tests/observation/isolated_package_cleanup.ps1')
$root=(Resolve-Path 'build/t473-cp-audit').Path
$source=(Resolve-Path 'build/t473-s9/package-r10').Path
$observer=(Resolve-Path 'build/.ninja/console-observers-x64/console-startup-observer.exe').Path
$report=Join-Path $root 'observer-glyph3.txt'
$probe=Join-Path $root 'pages-glyph3.txt'
if((Test-Path $report) -or (Test-Path $probe)){throw 'Use fresh result files'}
$existing=Get-Process ntsrv,ntvdm,ntvwm,ntcon,run16 -ErrorAction SilentlyContinue
if($existing){throw 'Product processes already exist; do not interfere with another run'}
$old=@{ PATH=$env:PATH; CP_PROBE_FILE=$env:CP_PROBE_FILE; MVDM_OBSERVER_PRIVATE_DESKTOP=$env:MVDM_OBSERVER_PRIVATE_DESKTOP }
$stage=$null;$scope=$null
try {
 $stage=New-PhysicalRuntimeStage $source
 $scope=New-IsolatedPackageScope $stage
 $bin=Get-PackageBinaryRoot $stage
 Copy-Item (Join-Path $root 'CPGLYPH.EXE') (Join-Path $bin 'CPGLYPH.EXE')
 Copy-Item (Join-Path $root 'CPTEXT1.COM') (Join-Path $bin 'CPTEXT1.COM')
 Copy-Item (Join-Path $root 'CPTEXT2.COM') (Join-Path $bin 'CPTEXT2.COM')
 $env:PATH=$bin+';'+$env:SystemRoot+'\System32'
 $env:CP_PROBE_FILE=$probe
 $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
 & $observer (Join-Path $bin 'run16.exe') $stage $report '--observation-timeout-ms' '35000' '--observe-console-input-marker' 'Microsoft(R) Windows NT DOS' '--observe-console-input-escaped' 'CPTEXT1\r' '--observe-console-input-next-marker' 'CPTEXT1:' '--observe-console-input-next-escaped' 'CPGLYPH control1\r' '--observe-console-input-next2-marker' 'CPPROBE-control1' '--observe-console-input-next2-escaped' 'CPGLYPH set\r' '--observe-console-input-next3-marker' 'CPPROBE-set' '--observe-console-input-next3-escaped' 'CPTEXT2\r' '--observe-console-input-next4-marker' 'CPTEXT2:' '--observe-console-input-next4-escaped' 'CPGLYPH control2\r' '--observe-console-input-next5-marker' 'NONEXISTENT-GATE-TEST' '--observe-console-input-next5-escaped' 'exit\r' (Join-Path $bin 'COMMAND.COM')
 $result=$LASTEXITCODE
 "observer exit=$result"
 if(Test-Path $probe){Get-Content $probe}
 if(Test-Path $report){Get-Content $report}
 if($result){throw "observer failed $result"}
} finally {foreach($name in $old.Keys){[Environment]::SetEnvironmentVariable($name,$old[$name],'Process')};if($scope){Stop-IsolatedPackageScope $scope};if($stage){Remove-PhysicalRuntimeStage $stage}}
