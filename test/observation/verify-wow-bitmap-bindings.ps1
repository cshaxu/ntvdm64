param([Parameter(Mandatory)][string]$BuildRoot)
# Run this driver as an owned hidden background process. Native fixtures do
# not replace real Win16 acceptance through the production registrar.
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path "$PSScriptRoot/../..").Path
$out = [IO.Path]::GetFullPath($BuildRoot)
if (!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
        (Test-Path $out)) { throw 'Fresh repository build output required' }
$null = New-Item -ItemType Directory $out
function Invoke-Owned([string]$name,[string]$file,[string]$arguments,[int]$seconds) {
    $start = [DateTime]::UtcNow
    $p = Start-Process $file -ArgumentList $arguments -WorkingDirectory $out -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/$name.stdout.log" -RedirectStandardError "$out/$name.stderr.log"
    $null = $p.Handle
    @{pid=$p.Id;start=$start.ToString('o');deadline=$start.AddSeconds($seconds).ToString('o');file=$file;arguments=$arguments;workingDirectory=$out} | ConvertTo-Json | Set-Content "$out/$name.process.json"
    if (!$p.WaitForExit($seconds*1000)) {
        & taskkill.exe /PID $p.Id /T /F *> "$out/$name.timeout.log"
        throw "$name timeout"
    }
    $p.Refresh()
    $p.ExitCode | Set-Content "$out/$name.exit"
    if ($p.ExitCode) { throw "$name failed: $($p.ExitCode)" }
}
try {
    & "$repo/tools/build-fixtures.cmd" x86 wow-bitmap *> "$out/build.log"
    if($LASTEXITCODE){throw 'Native bitmap build failed'}
    $provider="$repo/build/.ninja/msvc-x86-release/wow32"
    foreach($case in 'binding','binding_failure'){
        Invoke-Owned "$case-run" "$provider/bitmap-$case.exe" ' ' 60
        $marker=if($case -eq 'binding'){'WOW_BITMAP_BINDING_OK'}else{'WOW_BITMAP_BINDING_FAILURE_OK'}
        if(!(Select-String -LiteralPath "$out/$case-run.stdout.log" -SimpleMatch $marker)){throw "Missing $marker"}
    }
    Invoke-Owned 'registration' (Get-Process -Id $PID).Path ('-NoProfile -ExecutionPolicy Bypass -File "'+$repo+'/test/observation/verify-wow32-registration-surface.ps1" -RepositoryRoot "'+$repo+'"') 60
    0 | Set-Content "$out/suite.exit"
} catch {
    $_ | Out-String | Set-Content "$out/failure.txt"
    1 | Set-Content "$out/suite.exit"
    exit 1
}
