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
    if($command.Count -ne 1 -or $command[0] -notmatch '/TC' -or
            $command[0] -notmatch '/MT' -or
            $command[0] -notmatch '/O2' -or $command[0] -notmatch '/Gz'){
        throw 'Require one selected C /MT /O2 /Gz WOW compile row; fixture proves x86 ABI'
    }
    $command[0]|Set-Content "$out/production-command.txt"
    $source=Join-Path $repo 'test/adapter-mvdm-host-out/wow/wow_public_memory_abi_fixture.c'
    $abi=$command[0].Replace('/Gz','/Gd') -replace '/Fo\S+',('/Fo"'+$out+'/abi.obj"')
    $abi=$abi -replace '\S+wow_user_thunk_scope\.c$',('"'+$source+'"')
    if($abi -eq $command[0]){throw 'ABI fixture substitution failed'}
    Get-FileHash $source|Export-Csv -NoTypeInformation "$out/source-sha256.csv"
    Compile public-abi ($abi+' /we4113 /we4047 /we4028 /we4273')
    Get-FileHash "$out/abi.obj"|Export-Csv -NoTypeInformation "$out/artifact-sha256.csv"
    'WOW_PUBLIC_MEMORY_ABI_OK'|Set-Content "$out/result.txt"
    '0'|Set-Content "$out/exit.txt"
}catch{$_|Out-String|Set-Content "$out/failure.txt";'1'|Set-Content "$out/exit.txt";exit 1}
