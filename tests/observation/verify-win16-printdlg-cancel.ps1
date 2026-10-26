param([Parameter(Mandatory)][string]$PackageRoot,[Parameter(Mandatory)][string]$Observer,[Parameter(Mandatory)][string]$BuildRoot,[string]$Nasm='nasm.exe')
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'));$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path -LiteralPath $root)){throw 'Require a fresh build-owned evidence root'}
$null=New-Item -ItemType Directory -Path $root
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$runtime=$null;$scope=$null;$process=$null;$saved=@{}
foreach($name in @('TEMP','TMP','PATH','MVDM_OBSERVER_PRIVATE_DESKTOP')){$saved[$name]=[Environment]::GetEnvironmentVariable($name)}
try{
 $source=Join-Path $PSScriptRoot 'win16_printdlg_cancel_probe.asm';$assembler=(Get-Command $Nasm -ErrorAction Stop).Source
 & $assembler -f bin -o "$root/PDCANCEL.EXE" -l "$root/probe.lst" $source *> "$root/assemble.txt";if($LASTEXITCODE){throw 'PrintDlg cancellation probe assembly failed'}
 $runtime=New-PhysicalRuntimeStage (Resolve-Path -LiteralPath $PackageRoot).Path;$scope=New-IsolatedPackageScope $runtime;$binary=Get-PackageBinaryRoot $runtime;$tests=Join-Path $runtime 'tests'
 $null=New-Item -ItemType Directory -Path $tests -Force
 Copy-Item "$root/PDCANCEL.EXE" "$tests/PDCANCEL.EXE"
 @($source,"$root/PDCANCEL.EXE",(Join-Path $binary 'WOW32.DLL'))|Get-FileHash|Select-Object Path,Hash|ConvertTo-Json|Set-Content "$root/inputs.json"
 $env:TEMP=$runtime;$env:TMP=$runtime;$env:PATH="$binary;"+$env:SystemRoot+'\System32';$env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
 $start=[Diagnostics.ProcessStartInfo]::new((Resolve-Path -LiteralPath $Observer).Path);$start.UseShellExecute=$false;$start.CreateNoWindow=$true
 foreach($arg in @((Join-Path $binary 'run16.exe'),$tests,"$root/observer.txt",'--wait',"$tests/PDCANCEL.EXE",'--observation-timeout-ms','30000')){$start.ArgumentList.Add($arg)}
 $process=[Diagnostics.Process]::Start($start);$null=$process.Handle;if(!$process.WaitForExit(35000)){throw 'Owned PrintDlg cancellation observer exceeded deadline'};$process.Refresh()
 if($process.ExitCode -or (Get-Content "$root/observer.txt" -Raw) -notmatch 'result=exited'){throw 'PrintDlg cancellation probe did not complete normally'}
 $reportPath=@("$tests/PDCANCEL.BIN","$runtime/PDCANCEL.BIN")|Where-Object{Test-Path -LiteralPath $_}|Select-Object -First 1;if(!$reportPath){throw 'PrintDlg cancellation probe did not write its report'}
 Copy-Item $reportPath "$root/guest-report.bin";$bytes=[IO.File]::ReadAllBytes("$root/guest-report.bin")
 if($bytes.Length -ne 14 -or [Text.Encoding]::ASCII.GetString($bytes,0,4) -ne 'PDC1'){throw 'Malformed PrintDlg cancellation report'}
 $row=[ordered]@{result=[BitConverter]::ToUInt16($bytes,4);extendedError=[BitConverter]::ToUInt32($bytes,6);hooks=[BitConverter]::ToUInt16($bytes,10);freed=[BitConverter]::ToUInt16($bytes,12)};$row|ConvertTo-Json|Set-Content "$root/guest-report.json"
 if($row.result -or $row.extendedError -or !$row.hooks -or !$row.freed){throw "Original PrintDlg cancellation contract failed: $($row|ConvertTo-Json -Compress)"}
 'PASS original Win16 PrintDlg hook cancellation and ProcInstance retirement witness'|Set-Content "$root/result.txt";'0'|Set-Content "$root/result.exit"
}catch{$_|Out-String|Set-Content "$root/failure.txt";'1'|Set-Content "$root/result.exit";throw}
finally{if($process){if(!$process.HasExited){$process.Kill();$process.WaitForExit(5000)|Out-Null};$process.Dispose()};try{if($scope){Stop-IsolatedPackageScope $scope}}finally{try{if($runtime){Remove-PhysicalRuntimeStage $runtime}}finally{foreach($name in $saved.Keys){[Environment]::SetEnvironmentVariable($name,$saved[$name])}}}}
