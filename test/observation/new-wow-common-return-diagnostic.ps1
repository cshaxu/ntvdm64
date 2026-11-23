param([Parameter(Mandatory)][string]$BuildRoot,[Parameter(Mandatory)][string]$ProviderBuildRoot)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$out=[IO.Path]::GetFullPath($BuildRoot);$provider=(Resolve-Path $ProviderBuildRoot).Path
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh build-owned diagnostic output required'}
$null=New-Item -ItemType Directory $out
$kind='common-return'
$normal=(Get-FileHash "$provider/wow32.dll").Hash
$line='/d /s /c ""'+$repo+'/tools/build-fixtures.cmd" x86 wow-'+$kind+' "'+$provider+'""'
try {
$p=Start-Process $env:ComSpec -ArgumentList $line -WorkingDirectory $repo -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/build.stdout" -RedirectStandardError "$out/build.stderr"
try {
 $null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$line;timeoutSeconds=180}|ConvertTo-Json|Set-Content "$out/build.owner.json"
 if(!$p.WaitForExit(180000)){$p.Kill($true);$p.WaitForExit();throw 'WOW common return producer deadline'}
 $p.WaitForExit();$p.ExitCode|Set-Content "$out/build.exit"
 if($p.ExitCode){throw 'Native WOW common return build failed'}
} finally {$p.Dispose()}
$cache="$provider/diagnostics/$kind"
Copy-Item "$cache/WOW32.dll","$cache/wow_user_task_lifecycle.c","$cache/wow_user_thunk_scope.c" $out
Copy-Item "$cache/WOW32.map" "$out/diagnostic.map"
Copy-Item "$cache/WOW32.lib" "$out/diagnostic.lib"

if((Get-FileHash "$provider/wow32.dll").Hash -ne $normal){throw 'Normal provider changed'}
Get-FileHash "$repo/src/compat/wow_user_task_lifecycle.c","$repo/src/compat/wow_user_thunk_scope.c",$PSCommandPath,"$out/WOW32.DLL","$out/wow_user_task_lifecycle.c","$out/wow_user_thunk_scope.c"|Export-Csv -NoTypeInformation "$out/inputs.csv"
@{kind='diagnostic-only';publishable=$false;source='Two project-owned exact copies with failure-only observation';guestChanged=$false}|ConvertTo-Json|Set-Content "$out/result.json"
}catch{$_|Out-String|Set-Content "$out/failure.txt";exit 1}
