[CmdletBinding()]
param([Parameter(Mandatory)][string]$ProviderBuildRoot,
      [Parameter(Mandatory)][string]$BuildRoot,
      [ValidateSet('1122ab55d55b4d15699715ce1416b092b5887772')][string]$ReferenceRevision)
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
# This fixture deliberately tests the retained frame body, not the separate
# production execution-admission facade. Keep that declared scope explicit.
$flags += ' /DCallBack16=wow_original_CallBack16'
if(!$flags.Contains('/DCPU_40_STYLE') -or !$flags.Contains('/MT') -or !$release){throw 'Require selected CCPU40 provider graph'}
$vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
function Run-Owned([string]$name,[string]$file,[string]$arguments,[int]$seconds,[int]$expected=0){
    $p=Start-Process $file -ArgumentList $arguments -WorkingDirectory $root -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/$name.out" -RedirectStandardError "$root/$name.err"
    $null=$p.Handle
    @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$arguments;executable=$file;deadline=$p.StartTime.AddSeconds($seconds).ToUniversalTime().ToString('o')}|ConvertTo-Json|Set-Content "$root/$name-process.json"
    if(!$p.WaitForExit($seconds*1000)){& taskkill.exe /PID $p.Id /T /F *> "$root/$name-timeout.txt";throw "Timeout $name"}
    $p.WaitForExit();$p.Refresh();$p.ExitCode|Set-Content "$root/$name.exit"
    if($p.ExitCode -ne $expected){throw "Failed $name actual=$($p.ExitCode) expected=$expected"}
}
try {
    # Compile the complete selected original file, never extract/rewrite the
    # function. /Gy permits discarding unrelated functions at this fixture link.
    $sources=@('src/mvdm/wow32/wcall16.c','src/wow32-dll/source/wow_callback_frame_lease.c','tests/adapter-mvdm-host-out/wow/wow_callback_abort_fixture.c')
    $objects=@()
    foreach($source in $sources){
        $name=[IO.Path]::GetFileNameWithoutExtension($source)
        $object="$root/$name.obj";$objects+=('"'+$object+'"')
        $input="$repo/$source"
        if($ReferenceRevision -and $name -eq 'wcall16'){
            $body=(& git -C $repo show "${ReferenceRevision}:$source") -join "`n"
            if($LASTEXITCODE){throw 'Missing immutable callback reference'}
            $input="$root/reference-wcall16.c";[IO.File]::WriteAllText($input,$body)
        }
        $arguments='/d /s /c "call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && cl.exe '+$flags+' '+$release+' /Gy /Fo"'+$object+'" "'+$input+'""'
        Run-Owned $name 'cmd.exe' $arguments 120
    }
    $arguments='/d /s /c "call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && link.exe /nologo /OPT:REF /out:"'+$root+'/fixture.exe" /map:"'+$root+'/fixture.map" '+($objects -join ' ')+' kernel32.lib legacy_stdio_definitions.lib"'
    Run-Owned 'link' 'cmd.exe' $arguments 120
    if($ReferenceRevision){
        foreach($case in @('prep-init','prep-reserved','return-init','return-reserved','live-init','live-reserved','teardown')){
            Run-Owned "negative-$case" "$root/fixture.exe" "--abort-$case" 30 1
            if((Get-Content "$root/negative-$case.out" -Raw) -notmatch "CALLBACK_ABORT_WRONG name=$case "){throw 'Missing source-causal old frame failure'}
        }
        Run-Owned terminal "$root/fixture.exe" '--abort-terminal' 30
        @{reference=$ReferenceRevision;scope='seven old-body abort counterexamples; unchanged terminal bypass'}|ConvertTo-Json|Set-Content "$root/reference.json"
        return
    }
    Run-Owned 'fixture' "$root/fixture.exe" '--verify' 30
    Run-Owned 'abort' "$root/fixture.exe" '--verify-abort' 30
    $output=Get-Content "$root/fixture.out" -Raw
    if($output -notmatch 'PASS selected CallBack16' -or
        ([regex]::Matches($output,'ORIGINAL_CALLBACK_FRAME phase=')).Count -ne 4){throw 'Missing frame assertions'}
    if(([regex]::Matches((Get-Content "$root/abort.out" -Raw),'CALLBACK_ABORT_OK name=')).Count -ne 11){throw 'Missing complete abort phase/mode/terminal assertions'}
    @($sources|ForEach-Object {"$repo/$_"}) + @("$ProviderBuildRoot/build.ninja","$repo/tests/adapter-mvdm-host-out/wow/wow_original_callback_frame_fixture.c","$repo/src/wow32-dll/include/wow_callback_frame_lease.h") |
        Get-FileHash | Select-Object Path,Hash | ConvertTo-Json | Set-Content "$root/inputs.json"
    '0'|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/error.txt";'1'|Set-Content "$root/exit.txt";throw}
