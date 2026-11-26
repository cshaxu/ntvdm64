[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){
    throw 'Require fresh build-owned evidence directory'
}
$null=New-Item -ItemType Directory $root
function Run-Owned([string]$name,[string]$file,[string]$arguments,[int]$seconds,[int]$expected=0){
    $p=Start-Process $file -ArgumentList $arguments -WorkingDirectory $root -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/$name.out" -RedirectStandardError "$root/$name.err"
    $null=$p.Handle
    @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$arguments;executable=$file;deadline=$p.StartTime.AddSeconds($seconds).ToUniversalTime().ToString('o')}|ConvertTo-Json|Set-Content "$root/$name-process.json"
    if(!$p.WaitForExit($seconds*1000)){& taskkill.exe /PID $p.Id /T /F *> "$root/$name-timeout.txt";throw "Timeout $name"}
    $p.WaitForExit();$p.Refresh();$p.ExitCode|Set-Content "$root/$name.exit"
    if($p.ExitCode -ne $expected){throw "Failed $name actual=$($p.ExitCode) expected=$expected"}
}
try {
    & "$repo/tools/build-fixtures.cmd" x86 wow-extension-contracts *> "$root/build.log"
    if($LASTEXITCODE){throw 'Native callback frame build failed'}
    $provider="$repo/build/.ninja/msvc-x86-release/wow32"
    Copy-Item "$provider/extension-original-frame.exe" "$root/fixture.exe"
    Copy-Item "$provider/extension-original-frame.exe.map" "$root/fixture.map"
    $sources=@('src/mvdm-host/wow32/wow32.h','src/mvdm-host/inc/softpc.h','src/mvdm-host/wow32/wcall16.c','src/compat/wow_callback_frame_lease.c',
      'src/compat/wow_user_native_extension.c','test/adapter-mvdm-host-out/wow/wow_callback_abort_fixture.c')
    Run-Owned 'fixture' "$root/fixture.exe" '--verify' 30
    Run-Owned 'abort' "$root/fixture.exe" '--verify-abort' 30
    $output=Get-Content "$root/fixture.out" -Raw
    if($output -notmatch 'PASS selected CallBack16' -or
        ([regex]::Matches($output,'ORIGINAL_CALLBACK_FRAME phase=')).Count -ne 4){throw 'Missing frame assertions'}
    if(([regex]::Matches((Get-Content "$root/abort.out" -Raw),'CALLBACK_ABORT_OK name=')).Count -ne 11){throw 'Missing complete abort phase/mode/terminal assertions'}
    @($sources|ForEach-Object {"$repo/$_"}) + @("$provider/build.ninja","$repo/test/adapter-mvdm-host-out/wow/wow_original_callback_frame_fixture.c","$repo/src/compat/include/wow_callback_frame_lease.h") |
        ForEach-Object { Get-FileHash -LiteralPath $_ } | Select-Object Path,Hash | ConvertTo-Json | Set-Content "$root/inputs.json"
    '0'|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/error.txt";'1'|Set-Content "$root/exit.txt";throw}
