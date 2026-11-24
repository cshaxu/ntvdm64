param([Parameter(Mandatory)][string]$BuildRoot)
# Run through an owned hidden background supervisor.
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($BuildRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh build-owned output required'}
$null=New-Item -ItemType Directory $out
try{
 & "$repo/tools/build-fixtures.cmd" x86 wow-registry-shutdown *> "$out/build.log"
 if($LASTEXITCODE){throw 'Native registry contract build failed'}
 $native="$repo/build/.ninja/msvc-x86-release/wow32"
 Copy-Item "$native/registry-shutdown.exe" "$out/fixture.exe"
 $p=Start-Process "$out/fixture.exe" -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/result.log" -RedirectStandardError "$out/stderr.log"
 $null=$p.Handle
 @{pid=$p.Id;started=$p.StartTime.ToUniversalTime().ToString('o');deadlineSeconds=30}|ConvertTo-Json|Set-Content "$out/process.json"
 if(!$p.WaitForExit(30000)){& taskkill.exe /PID $p.Id /T /F;throw 'Owned registry deadline'}
 $p.WaitForExit();$p.ExitCode|Set-Content "$out/process.exit"
 if($p.ExitCode -or (Get-Content "$out/result.log" -Raw) -notmatch 'WOW_REGISTRY_POPULATED_SHUTDOWN_OK'){throw 'Registry shutdown contract failed'}
 $object=(Get-Content "$native/registry-shutdown-provider.txt" -Raw).Trim()
 Get-FileHash "$repo/test/compat/wow_registry_shutdown_fixture.c",$object,"$out/fixture.exe"|Export-Csv -NoTypeInformation "$out/inputs.csv"
 0|Set-Content "$out/exit.txt"
}catch{$_|Out-String|Set-Content "$out/failure.txt";1|Set-Content "$out/exit.txt";throw}
