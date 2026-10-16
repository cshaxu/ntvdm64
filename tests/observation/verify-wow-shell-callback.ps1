[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,[switch]$CdeclControl,[switch]$ThirdArgumentControl,[switch]$ThrowControl,[switch]$OuterBoundary,
    [string]$AdapterProviderBuildRoot)
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
 $vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
 $source="$PSScriptRoot/wow_shell_callback_probe.c"
 $control=if($CdeclControl){' /DTEST_CDECL_CALLBACK'}else{''}
 if($ThirdArgumentControl){$control+=' /DTEST_THIRD_ARGUMENT'}
 if($OuterBoundary){
    if($CdeclControl -or $ThrowControl -or (!$ThirdArgumentControl -and !$AdapterProviderBuildRoot)){throw 'OuterBoundary requires direct stdcall or actual adapter'}
    $control+=' /DTEST_OUTER_BOUNDARY'
 }
 if($ThrowControl){
    if(!$ThirdArgumentControl -or $CdeclControl -or $AdapterProviderBuildRoot){throw 'Throw control requires direct three-argument stdcall provider'}
    $control+=' /DTEST_THROW_CONTROL'
 }
 $adapterObject=''
 if($AdapterProviderBuildRoot){
    if($CdeclControl -or $ThirdArgumentControl){throw 'Adapter tests the original two-argument stdcall contract'}
    $graph=Get-Content "$AdapterProviderBuildRoot/build.ninja" -Raw
    $flags=[regex]::Match($graph,'(?m)^cflags = (.*)\r?$').Groups[1].Value.Trim()
    if(!$flags.Contains('/DCPU_40_STYLE')){throw 'Require selected provider flags'}
    $adapterSource="$repo/src/wow32-dll/source/wow_shell_callback.c"
    $adapterFixture="$repo/tests/adapter-mvdm-host-out/wow/wow_shell_scope_fixture.c"
    $arguments='/d /s /c "call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && cl.exe '+$flags+' /O2 /Fo"'+$root+'/adapter.obj" "'+$adapterFixture+'"'
    Run-Owned 'adapter' 'cmd.exe' $arguments 120
    Get-FileHash $adapterSource,$adapterFixture | Select-Object Path,Hash | ConvertTo-Json | Set-Content "$root/adapter-input.json"
    $control+=' /DTEST_ADAPTER';$adapterObject=' "'+$root+'/adapter.obj"'
 }
 $arguments='/d /s /c "call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && cl.exe /nologo /O2 /MT /W4 /WX'+$control+' /Fo"'+$root+'/probe.obj" /Fe"'+$root+'/probe.exe" "'+$source+'"'+$adapterObject+' /link kernel32.lib"'
 Run-Owned 'compile' 'cmd.exe' $arguments 120
 @($source,"$root/probe.exe","$env:SystemRoot/SysWOW64/shell32.dll")|Get-FileHash|
     Select-Object Path,Hash|ConvertTo-Json|Set-Content "$root/inputs.json"
 Run-Owned 'probe' "$root/probe.exe" ('"'+$root+'/native-fallback.txt"') 30
 if((Get-Content "$root/probe.out" -Raw) -notmatch 'PASS modern WOWShellExecute'){throw 'Missing provider assertions'}
 if($AdapterProviderBuildRoot -and (Get-Content "$root/probe.out" -Raw) -notmatch 'PASS actual adapter'){throw 'Missing adapter nesting assertions'}
 if($AdapterProviderBuildRoot -and !$OuterBoundary -and (Get-Content "$root/probe.out" -Raw) -notmatch 'PASS actual adapter exception propagation'){throw 'Missing adapter exception/TLS restoration assertions'}
 if($OuterBoundary -and (Get-Content "$root/probe.out" -Raw) -notmatch 'PASS provider invoked again after outer unwind'){throw 'Missing outer-boundary recovery'}
 '0'|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/error.txt";'1'|Set-Content "$root/exit.txt";throw}
