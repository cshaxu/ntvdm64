param([Parameter(Mandatory)][string]$BuildRoot,[Parameter(Mandatory)][string]$ProviderBuildRoot,
 [ValidateSet('AttachFailure','MissingInit')][string]$FailureMode='AttachFailure')
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$out=[IO.Path]::GetFullPath($BuildRoot);$provider=(Resolve-Path $ProviderBuildRoot).Path
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh build-owned diagnostic output required'}
$null=New-Item -ItemType Directory $out
$kind=if($FailureMode -eq 'AttachFailure'){'attach-failure'}else{'missing-init'}
$normal=(Get-FileHash "$provider/wow32.dll").Hash
$line='/d /s /c ""'+$repo+'/tools/build-fixtures.cmd" x86 wow-'+$kind+' "'+$provider+'""'
try {
$p=Start-Process $env:ComSpec -ArgumentList $line -WorkingDirectory $repo -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/build.stdout" -RedirectStandardError "$out/build.stderr"
try {
 $null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$line;timeoutSeconds=180}|ConvertTo-Json|Set-Content "$out/build.owner.json"
 if(!$p.WaitForExit(180000)){$p.Kill($true);$p.WaitForExit();throw 'WOW attach producer deadline'}
 $p.WaitForExit();$p.ExitCode|Set-Content "$out/build.exit"
 if($p.ExitCode){throw 'Native WOW attach build failed'}
} finally {$p.Dispose()}
$cache="$provider/diagnostics/$kind"
Copy-Item "$cache/WOW32.dll","$cache/wow32.c" $out
Copy-Item "$cache/WOW32.map" "$out/diagnostic.map"
Copy-Item "$cache/WOW32.lib" "$out/diagnostic.lib"
if($FailureMode -eq 'MissingInit'){Copy-Item "$cache/diagnostic.def" $out}
if((Get-FileHash "$provider/wow32.dll").Hash -ne $normal){throw 'Normal provider changed'}
Get-FileHash "$repo/src/mvdm-host/wow32/wow32.c",$PSCommandPath,"$out/WOW32.DLL","$out/wow32.c"|Export-Csv -NoTypeInformation "$out/inputs.csv"
@{kind='diagnostic-only';publishable=$false;source='Original selected WOW32 diagnostic copy';failureMode=$FailureMode;guestChanged=$false}|ConvertTo-Json|Set-Content "$out/result.json"
}catch{$_|Out-String|Set-Content "$out/failure.txt";exit 1}
