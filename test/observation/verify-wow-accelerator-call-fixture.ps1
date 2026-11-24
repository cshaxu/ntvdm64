param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($BuildRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh repository build output required'}
$null=New-Item -ItemType Directory $out
try {
    & "$repo/tools/build-fixtures.cmd" x86 wow-accelerator *> "$out/build.log"
    if($LASTEXITCODE){throw 'Native accelerator build failed'}
    foreach($kind in 'call','alias'){
        $image="$repo/build/.ninja/fixtures-x86/wow-accelerator-$kind.exe"
        $p=Start-Process $image -WorkingDirectory $out -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/$kind.stdout.log" -RedirectStandardError "$out/$kind.stderr.log"
        try{
            @{pid=$p.Id;started=$p.StartTime.ToUniversalTime().ToString('o');image=$image}|ConvertTo-Json|Set-Content "$out/$kind.process.json"
            if(!$p.WaitForExit(30000)){throw 'Accelerator runtime deadline'}
            $p.WaitForExit()
            $p.ExitCode|Set-Content "$out/$kind.exit"
            if($p.ExitCode){throw "$kind failed: $($p.ExitCode)"}
        }finally{if(!$p.HasExited){$p.Kill($true);$p.WaitForExit()};$p.Dispose()}
    }
    if(!(Select-String -LiteralPath "$out/call.stdout.log" -SimpleMatch 'WOW_ACCEL_CREATE32 errors=0 entries=2 partial_tail=1')){throw 'Accelerator marker missing'}
    if(!(Select-String -LiteralPath "$out/alias.stdout.log" -SimpleMatch 'WOW_ACCEL_ALIAS_RETIRE errors=0 flags=2 destroys=2')){throw 'Alias marker missing'}
    Get-FileHash "$repo/src/mvdm-host/wow32/waccel.c","$repo/build/.ninja/fixtures-x86/accel-create32.inc","$repo/build/.ninja/fixtures-x86/accel-alias.inc","$repo/build/.ninja/fixtures-x86/wow-accelerator-call.exe","$repo/build/.ninja/fixtures-x86/wow-accelerator-alias.exe"|Select-Object Path,Hash|ConvertTo-Json|Set-Content "$out/inputs.json"
    0|Set-Content "$out/suite.exit"
}catch{$_|Out-String|Set-Content "$out/failure.txt";1|Set-Content "$out/suite.exit";throw}
