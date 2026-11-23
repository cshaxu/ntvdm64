param([Parameter(Mandatory)][string]$ProviderBuildRoot,
 [Parameter(Mandatory)][string]$BuildRoot,[switch]$ShellException)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$out=[IO.Path]::GetFullPath($BuildRoot);$provider=(Resolve-Path $ProviderBuildRoot).Path
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh build-owned diagnostic output required'}
$null=New-Item -ItemType Directory $out
$kind=if($ShellException){'shell-exception'}else{'startup-failure'}
$normal=(Get-FileHash "$provider/wow32.dll").Hash
$line='/d /s /c ""'+$repo+'/tools/build-fixtures.cmd" x86 wow-'+$kind+' "'+$provider+'""'
try {
$p=Start-Process $env:ComSpec -ArgumentList $line -WorkingDirectory $repo -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/build.stdout" -RedirectStandardError "$out/build.stderr"
try {
 $null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$line;timeoutSeconds=180}|ConvertTo-Json|Set-Content "$out/build.owner.json"
 if(!$p.WaitForExit(180000)){$p.Kill($true);$p.WaitForExit();throw 'WOW startup producer deadline'}
 $p.WaitForExit();$p.ExitCode|Set-Content "$out/build.exit"
 if($p.ExitCode){throw 'Native WOW startup build failed'}
} finally {$p.Dispose()}
$cache="$provider/diagnostics/$kind"
Copy-Item "$cache/WOW32.dll" $out
Copy-Item "$cache/WOW32.map" "$out/diagnostic.map"
Copy-Item "$cache/WOW32.lib" "$out/diagnostic.lib"
if((Get-FileHash "$provider/wow32.dll").Hash -ne $normal){throw 'Normal provider changed'}
$source=if($ShellException){"$PSScriptRoot/wow_shell_exception_provider.c"}else{"$PSScriptRoot/wow_startup_failure_provider.c"}
$objects=@(Get-Content "$cache/objects.txt"|Where-Object {$_})
@($PSCommandPath,$source,"$provider/build.ninja","$provider/WOW32.dll","$out/WOW32.dll")+$objects|Get-FileHash|Select-Object Path,Hash|ConvertTo-Json|Set-Content "$out/inputs.json"
$target=if($ShellException){'SHLPROBE'}else{'GENPROBE'}
$fault=if($ShellException){'exception after original Shell callback return'}else{'second original InitTask allocation'}
@{kind='diagnostic-only';publishable=$false;target=$target;fault=$fault;normalProvider=$normal;diagnosticProvider=(Get-FileHash "$out/WOW32.DLL").Hash}|ConvertTo-Json|Set-Content "$out/diagnostic-manifest.json"
0|Set-Content "$out/exit.txt"
}catch{$_|Out-String|Set-Content "$out/failure.txt";exit 1}
