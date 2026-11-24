param(
    [Parameter(Mandatory)][string]$BuildRoot
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
try {
    & "$repo/tools/build-fixtures.cmd" x86 wow-resource-calls *> "$out/build.log"
    if($LASTEXITCODE){throw 'Native resource contracts build failed'}
    $native="$repo/build/.ninja/fixtures-x86"
    foreach($kind in @('cursor','bitmap','cursor-resource')) {
        $target=if($kind -eq 'cursor-resource'){'wow-cursor-resource'}else{"wow-$kind-call"}
        Copy-Item "$native/$target.exe" "$out/$kind.exe"
    }
    Invoke-Owned 'cursor-run' "$out/cursor.exe" ' ' 30
    if(!(Select-String -LiteralPath "$out/cursor-run.stdout.log" -SimpleMatch 'WOW_CURSOR_CALL_ORDER errors=0 cases=3 output=3 aliases=3 output_errors=0 alias_errors=0 return_errors=0')){throw 'Cursor caller marker missing'}
    Invoke-Owned 'bitmap-run' "$out/bitmap.exe" ' ' 30
    if(!(Select-String -LiteralPath "$out/bitmap-run.stdout.log" -SimpleMatch 'WOW_BITMAP_CALL_ORDER errors=0 output=2')){throw 'Bitmap caller marker missing'}
    foreach($mode in @('binding','public')) {
        $arguments=if($mode -eq 'public'){'public'}else{' '}
        Invoke-Owned "cursor-resource-$mode" "$out/cursor-resource.exe" $arguments 30
        if(!(Select-String -LiteralPath "$out/cursor-resource-$mode.stdout.log" -SimpleMatch 'WOW_CURSOR_RESOURCE_BOUNDARY_OK')){throw "Cursor resource $mode marker missing"}
    }
    0|Set-Content "$out/suite.exit"
}catch{$_|Out-String|Set-Content "$out/failure.txt";1|Set-Content "$out/suite.exit";exit 1}
