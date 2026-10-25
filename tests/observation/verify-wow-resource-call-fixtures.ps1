param(
    [Parameter(Mandatory)][string]$BuildRoot,
    [string]$VsDevCmd = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
)
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path "$PSScriptRoot/../..").Path
$out = [IO.Path]::GetFullPath($BuildRoot)
if (!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
    (Test-Path $out)) { throw 'Fresh repository build output required' }
$null = New-Item -ItemType Directory $out
function Invoke-Owned([string]$name, [string]$file, [string]$arguments, [int]$seconds) {
    $start=[DateTime]::UtcNow
    $p=Start-Process $file -ArgumentList $arguments -WorkingDirectory $out -WindowStyle Hidden -PassThru `
        -RedirectStandardOutput "$out/$name.stdout.log" -RedirectStandardError "$out/$name.stderr.log"
    $null=$p.Handle
    @{pid=$p.Id;start=$start.ToString('o');deadline=$start.AddSeconds($seconds).ToString('o');file=$file;arguments=$arguments}|ConvertTo-Json|Set-Content "$out/$name.process.json"
    if(!$p.WaitForExit($seconds*1000)){& taskkill.exe /PID $p.Id /T /F *> "$out/$name.timeout.log";throw "$name timeout"}
    $p.Refresh();$p.ExitCode|Set-Content "$out/$name.exit"
    if($p.ExitCode){throw "$name failed: $($p.ExitCode)"}
}
function Build([string]$name,[string]$command){
    Invoke-Owned $name 'cmd.exe' ('/d /s /c "call "'+$VsDevCmd+'" -arch=x86 -host_arch=x64 >nul && '+$command+'"') 120
}
try {
    & "$PSScriptRoot/extract-wow-cursor-call-fixture.ps1" -BuildRoot $out -Kind Cursor | Set-Content "$out/extract-cursor.txt"
    & "$PSScriptRoot/extract-wow-cursor-call-fixture.ps1" -BuildRoot $out -Kind Bitmap | Set-Content "$out/extract-bitmap.txt"
    Build 'cursor-compile' ('cl.exe /nologo /TC /c /MT /W4 /WX /I"'+$out+'" /Fo"'+$out+'/cursor.obj" "'+$repo+'/tests/adapter-mvdm-host-out/wow/wow_cursor_call_fixture.c"')
    Build 'cursor-link' ('link.exe /nologo /machine:x86 /subsystem:console /out:"'+$out+'/cursor.exe" "'+$out+'/cursor.obj" user32.lib')
    Invoke-Owned 'cursor-run' "$out/cursor.exe" ' ' 30
    if(!(Select-String -LiteralPath "$out/cursor-run.stdout.log" -SimpleMatch 'WOW_CURSOR_CALL_ORDER errors=0 cases=3 output=3 aliases=3 output_errors=0 alias_errors=0 return_errors=0')){throw 'Cursor caller marker missing'}
    Build 'bitmap-compile' ('cl.exe /nologo /TC /c /MT /W4 /WX /I"'+$out+'" /Fo"'+$out+'/bitmap.obj" "'+$repo+'/tests/adapter-mvdm-host-out/wow/wow_bitmap_call_fixture.c"')
    Build 'bitmap-link' ('link.exe /nologo /machine:x86 /subsystem:console /out:"'+$out+'/bitmap.exe" "'+$out+'/bitmap.obj"')
    Invoke-Owned 'bitmap-run' "$out/bitmap.exe" ' ' 30
    if(!(Select-String -LiteralPath "$out/bitmap-run.stdout.log" -SimpleMatch 'WOW_BITMAP_CALL_ORDER errors=0 output=2 acquire=2 release=1')){throw 'Bitmap caller marker missing'}
    $include=' /I"'+$repo+'/src" /I"'+$repo+'/src/wow32-dll/include"'
    Build 'cursor-resource-compile' ('cl.exe /nologo /TC /c /MT /W4 /WX'+$include+' /Fo"'+$out+'/cursor-resource.obj" "'+$repo+'/tests/adapter-mvdm-host-out/wow/wow_cursor_resource_fixture.c"')
    # Function-level COMDATs keep this witness limited to the cursor/icon
    # output slot.  The same project-owned translation unit also contains
    # menu and default-message helpers whose providers are intentionally
    # outside this S2 resource seam.
    Build 'cursor-binding-compile' ('cl.exe /nologo /TC /c /MT /Gy /W4 /WX'+$include+' /Fo"'+$out+'/cursor-binding.obj" "'+$repo+'/src/wow32-dll/source/wow_private_user_compat.c"')
    Build 'cursor-resource-link' ('link.exe /nologo /machine:x86 /subsystem:console /opt:ref /out:"'+$out+'/cursor-resource.exe" "'+$out+'/cursor-resource.obj" "'+$out+'/cursor-binding.obj" user32.lib gdi32.lib kernel32.lib')
    foreach($mode in @('binding','public')) {
        $arguments=if($mode -eq 'public'){'public'}else{' '}
        Invoke-Owned "cursor-resource-$mode" "$out/cursor-resource.exe" $arguments 30
        if(!(Select-String -LiteralPath "$out/cursor-resource-$mode.stdout.log" -SimpleMatch 'WOW_CURSOR_RESOURCE_BOUNDARY_OK')){throw "Cursor resource $mode marker missing"}
    }
    0|Set-Content "$out/suite.exit"
}catch{$_|Out-String|Set-Content "$out/failure.txt";1|Set-Content "$out/suite.exit";exit 1}
