param([Parameter(Mandatory)][string]$ProviderBuildRoot,
      [Parameter(Mandatory)][string]$BuildRoot,[string]$Ninja='ninja.exe')
# Ninja retained for historical callers; native CMD owns production commands.
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$provider=(Resolve-Path $ProviderBuildRoot).Path;$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned evidence required'}
$null=New-Item -ItemType Directory $root
$normal=(Get-FileHash "$provider/wow32.dll").Hash
$line='/d /s /c ""'+$repo+'/tools/build-fixtures.cmd" x86 wow-callback-abort "'+$provider+'""'
try {
$p=Start-Process $env:ComSpec -ArgumentList $line -WorkingDirectory $repo -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/build.stdout" -RedirectStandardError "$root/build.stderr"
try {
    $null=$p.Handle
    @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$line;timeoutSeconds=180}|ConvertTo-Json|Set-Content "$root/build.owner.json"
    if(!$p.WaitForExit(180000)){$p.Kill($true);$p.WaitForExit();throw 'Callback producer deadline'}
    $p.WaitForExit();$p.ExitCode|Set-Content "$root/build.exit"
    if($p.ExitCode){throw 'Native callback abort build failed'}
} finally {$p.Dispose()}
$cache="$provider/diagnostics/callback-abort"
Copy-Item "$cache/WOW32.DLL","$cache/WOW32.map","$cache/WOW32.lib" $root
if((Get-FileHash "$provider/wow32.dll").Hash -ne $normal){throw 'Normal provider changed'}
$objects=@(Get-Content "$cache/objects.txt"|Where-Object {$_})
@($PSCommandPath,"$PSScriptRoot/wow_callback_abort_provider.c","$repo/src/mvdm-host/wow32/wcall16.c","$provider/build.ninja")+$objects|Get-FileHash|Export-Csv -NoTypeInformation "$root/inputs.csv"
@{kind='diagnostic-only';publishable=$false;target='SHLPROBE';fault='actual CallBack16 resolver Preparation/Returned';normalProvider=$normal;diagnosticProvider=(Get-FileHash "$root/WOW32.DLL").Hash}|ConvertTo-Json|Set-Content "$root/diagnostic-manifest.json"
0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/error.txt";1|Set-Content "$root/exit.txt";throw}
