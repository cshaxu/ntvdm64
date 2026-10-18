[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$ProviderBuildRoot,
    [Parameter(Mandatory)][string]$BuildRoot,
    [string]$VsDevCmd='C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/Common7/Tools/VsDevCmd.bat'
)
# Invoke as an owned hidden background process. Reuse the production x86
# /MT /O2 /Gz compile contract; never execute a stale fixture after failure.
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$provider=(Resolve-Path -LiteralPath $ProviderBuildRoot).Path
$out=[IO.Path]::GetFullPath($BuildRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
        (Test-Path -LiteralPath $out)){throw 'Require fresh build-owned output'}
$null=New-Item -ItemType Directory -Path $out
function Owned([string]$name,[string]$exe,[string]$arguments,[int]$seconds){
    $options=@{FilePath=$exe;WorkingDirectory=$provider;WindowStyle='Hidden';PassThru=$true;
        RedirectStandardOutput="$out/$name.stdout.txt";RedirectStandardError="$out/$name.stderr.txt"}
    if($arguments){$options.ArgumentList=$arguments}
    $p=Start-Process @options
    $null=$p.Handle
    @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$arguments;timeoutSeconds=$seconds}|
        ConvertTo-Json|Set-Content "$out/$name.process.json"
    if(!$p.WaitForExit($seconds*1000)){& taskkill.exe /PID $p.Id /T /F|Out-Host;throw "Owned deadline $name"}
    $p.WaitForExit();$code=$p.ExitCode;$code|Set-Content "$out/$name.exit.txt";$p.Dispose()
    if($code){throw "$name failed $code"}
}
function Compile([string]$name,[string]$command){
    $line='/d /s /c "call "'+$VsDevCmd+'" -arch=x86 -host_arch=x64 >nul && '+$command+'"'
    Owned $name $env:ComSpec $line 120
}
try {
    $commands=@(& ninja.exe -C $provider -t commands wow32.dll)
    if($LASTEXITCODE){throw 'Cannot recover production commands'}
    $command=@($commands|Where-Object {$_ -match '[/\\]wow_user_thunk_scope\.c$'})
    if($command.Count -ne 1 -or $command[0] -notmatch '/DCPU_40_STYLE' -or
            $command[0] -notmatch '/DCCPU' -or $command[0] -notmatch '/MT' -or
            $command[0] -notmatch '/O2' -or $command[0] -notmatch '/Gz' -or
            $command[0] -match '/DCPU_30_STYLE'){
        throw 'Require one original CCPU40 x86 production trace compile row'
    }
    $command[0]|Set-Content "$out/production-command.txt"
    $inputs=@('src/wow32-dll/source/wow_user_thunk_scope.c',
        'src/wow32-dll/include/wow_user_thunk_scope.h',
        'src/opennt-abi/host-compat/include/wow_user_declarations.h',
        'src/mvdm/wow32/isvwow.h','src/mvdm/wow32/wow32.h',
        'tests/adapter-mvdm-host-out/wow/wow_thunk_trace_fixture.c',
        'tests/adapter-mvdm-host-out/wow/wow_public_memory_abi_fixture.c')
    $inputs|ForEach-Object {Get-FileHash -LiteralPath (Join-Path $repo $_)}|
        Export-Csv -NoTypeInformation "$out/source-sha256.csv"
    $trace=$command[0] -replace '/Fo\S+',('/Fo"'+$out+'/trace.obj"')
    if($trace -eq $command[0]){throw 'Trace output substitution failed'}
    Compile trace-owner $trace
    $fixture=$trace -replace '/Fo"[^"]+"',('/Fo"'+$out+'/fixture.obj"')
    $fixture=$fixture -replace '\S+wow_user_thunk_scope\.c$',('"'+$repo+'/tests/adapter-mvdm-host-out/wow/wow_thunk_trace_fixture.c"')
    if($fixture -eq $trace){throw 'Fixture source substitution failed'}
    Compile trace-fixture $fixture
    $abi=$fixture.Replace('/Gz','/Gd') -replace '/Fo"[^"]+"',('/Fo"'+$out+'/abi.obj"')
    $abi=$abi -replace '"[^"]+wow_thunk_trace_fixture\.c"$',('"'+$repo+'/tests/adapter-mvdm-host-out/wow/wow_public_memory_abi_fixture.c"')
    if($abi -eq $fixture){throw 'ABI fixture substitution failed'}
    Compile public-abi ($abi+' /we4113 /we4047 /we4028 /we4273')
    Compile link ('link /nologo /machine:x86 /subsystem:console /out:"'+$out+'/fixture.exe" "'+$out+'/fixture.obj" "'+$out+'/trace.obj" kernel32.lib user32.lib gdi32.lib legacy_stdio_definitions.lib')
    Owned run "$out/fixture.exe" '' 30
    if((Get-Content "$out/run.stdout.txt" -Raw) -notmatch 'WOW_THUNK_TRACE errors=0 disabled=1 enabled=2 failed_output=1 exact_text=1 last_error=1 no_scope=1'){
        throw 'Trace contract markers absent'
    }
    foreach($input in Import-Csv "$out/source-sha256.csv"){
        if((Get-FileHash $input.Path).Hash -ne $input.Hash){throw 'Input changed during verification'}
    }
    Get-FileHash "$out/fixture.exe","$out/trace.obj","$out/abi.obj"|
        Export-Csv -NoTypeInformation "$out/artifact-sha256.csv"
    '0'|Set-Content "$out/exit.txt"
}catch{$_|Out-String|Set-Content "$out/failure.txt";'1'|Set-Content "$out/exit.txt";exit 1}
