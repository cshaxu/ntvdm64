[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,
    [ValidateSet('Adapter','AdapterOuter','Native','NativeOuter')][string]$Case='Adapter')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Require fresh build-owned directory'}
$null=New-Item -ItemType Directory $root
function Run-Owned([string]$name,[string]$file,[string]$arguments,[int]$seconds){
 $p=Start-Process $file -ArgumentList $arguments -WorkingDirectory $root -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/$name.out" -RedirectStandardError "$root/$name.err"
 $null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$arguments;executable=$file;deadline=$p.StartTime.AddSeconds($seconds).ToUniversalTime().ToString('o')}|ConvertTo-Json|Set-Content "$root/$name-process.json"
 if(!$p.WaitForExit($seconds*1000)){& taskkill.exe /PID $p.Id /T /F *> "$root/$name-timeout.txt";throw "Timeout $name"}
 $p.WaitForExit();$p.Refresh();$p.ExitCode|Set-Content "$root/$name.exit"
 if($p.ExitCode){throw "Failed $name"}
}
try {
 $source="$PSScriptRoot/wow_shell_callback_probe.c"
 & "$repo/tools/build-fixtures.cmd" x86 wow-shell-callback *> "$root/build.log"
 if($LASTEXITCODE){throw 'Native shell callback build failed'}
 $adapter=$Case.StartsWith('Adapter');$outer=$Case.EndsWith('Outer')
 $kind=if($adapter){'adapter'}else{'native'}
 if($outer){$kind+='-outer'}
 $native="$repo/build/.ninja/msvc-x86-release/wow32"
 Copy-Item "$native/shell-callback-$kind.exe" "$root/probe.exe"
 if($adapter){
   Copy-Item "$native/shell-callback-profile.txt" "$root/adapter-profile.txt"
   Get-FileHash "$repo/src/compat/wow_shell_callback.c","$repo/test/adapter-mvdm-host-out/wow/wow_shell_scope_fixture.c"|Export-Csv -NoTypeInformation "$root/adapter-inputs.csv"
 }
 @($source,"$root/probe.exe","$env:SystemRoot/SysWOW64/shell32.dll")|Get-FileHash|
     Select-Object Path,Hash|ConvertTo-Json|Set-Content "$root/inputs.json"
 Run-Owned 'probe' "$root/probe.exe" ('"'+$root+'/native-fallback.txt"') 30
 if((Get-Content "$root/probe.out" -Raw) -notmatch 'PASS modern WOWShellExecute'){throw 'Missing provider assertions'}
 if($adapter -and (Get-Content "$root/probe.out" -Raw) -notmatch 'PASS actual adapter'){throw 'Missing adapter nesting assertions'}
 if($adapter -and !$outer -and (Get-Content "$root/probe.out" -Raw) -notmatch 'PASS actual adapter exception propagation'){throw 'Missing adapter exception/TLS restoration assertions'}
 if($outer -and (Get-Content "$root/probe.out" -Raw) -notmatch 'PASS provider invoked again after outer unwind'){throw 'Missing outer-boundary recovery'}
 '0'|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/error.txt";'1'|Set-Content "$root/exit.txt";throw}
