[CmdletBinding()]
param([Parameter(Mandatory)][string]$ProviderBuildRoot,
      [Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){
    throw 'Require fresh build-owned evidence directory'
}
$null=New-Item -ItemType Directory $root
$graph=Get-Content "$ProviderBuildRoot/build.ninja" -Raw
$flags=[regex]::Match($graph,'(?m)^cflags = (.*)\r?$').Groups[1].Value.Trim()
$release=[regex]::Match($graph,'(?m)^release_cflags = (.*)\r?$').Groups[1].Value.Trim()
if(!$flags.Contains('/DCPU_40_STYLE') -or !$flags.Contains('/MT') -or !$release){throw 'Require selected CCPU40 provider graph'}
$vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
function Run-Owned([string]$name,[string]$file,[string]$arguments,[int]$seconds){
    $p=Start-Process $file -ArgumentList $arguments -WorkingDirectory $root -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/$name.out" -RedirectStandardError "$root/$name.err"
    $null=$p.Handle
    @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$arguments;executable=$file;deadline=$p.StartTime.AddSeconds($seconds).ToUniversalTime().ToString('o')}|ConvertTo-Json|Set-Content "$root/$name-process.json"
    if(!$p.WaitForExit($seconds*1000)){& taskkill.exe /PID $p.Id /T /F *> "$root/$name-timeout.txt";throw "Timeout $name"}
    $p.WaitForExit();$p.Refresh();$p.ExitCode|Set-Content "$root/$name.exit"
    if($p.ExitCode){throw "Failed $name"}
}
try {
    # Compile the complete selected original file, never extract/rewrite the
    # function. /Gy permits discarding unrelated functions at this fixture link.
    $sources=@('src/mvdm/wow32/wcall16.c','tests/adapter-mvdm-host-out/wow/wow_original_callback_frame_fixture.c')
    $objects=@()
    foreach($source in $sources){
        $name=[IO.Path]::GetFileNameWithoutExtension($source)
        $object="$root/$name.obj";$objects+=('"'+$object+'"')
        $arguments='/d /s /c "call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && cl.exe '+$flags+' '+$release+' /Gy /Fo"'+$object+'" "'+$repo+'/'+$source+'""'
        Run-Owned $name 'cmd.exe' $arguments 120
    }
    $arguments='/d /s /c "call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && link.exe /nologo /OPT:REF /out:"'+$root+'/fixture.exe" /map:"'+$root+'/fixture.map" '+($objects -join ' ')+' kernel32.lib legacy_stdio_definitions.lib"'
    Run-Owned 'link' 'cmd.exe' $arguments 120
    Run-Owned 'fixture' "$root/fixture.exe" '--verify' 30
    $output=Get-Content "$root/fixture.out" -Raw
    if($output -notmatch 'PASS selected CallBack16' -or
        ([regex]::Matches($output,'ORIGINAL_CALLBACK_FRAME phase=')).Count -ne 4){throw 'Missing frame assertions'}
    @($sources|ForEach-Object {"$repo/$_"}) + @("$ProviderBuildRoot/build.ninja") |
        Get-FileHash | Select-Object Path,Hash | ConvertTo-Json | Set-Content "$root/inputs.json"
    '0'|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/error.txt";'1'|Set-Content "$root/exit.txt";throw}
