param([Parameter(Mandatory)][string]$BuildRoot,
      [string]$CompilerDriver)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($BuildRoot)
if (!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)) {
    throw 'A fresh repository build directory is required'
}
New-Item -ItemType Directory $out | Out-Null
$source=Join-Path $repo 'src/mvdm-host/wow32/wow32.c'
$command='/d /s /c ""'+$repo+'/tools/build-fixtures.cmd" x86 wow-thunk-nls"'
$p=Start-Process $env:ComSpec -ArgumentList $command -WorkingDirectory $repo -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/build.log" -RedirectStandardError "$out/build.stderr.log"
try {
    $null=$p.Handle
    @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$command;timeoutSeconds=180}|ConvertTo-Json|Set-Content "$out/producer.json"
    if(!$p.WaitForExit(180000)){$p.Kill($true);$p.WaitForExit();throw 'ThunkNLS producer deadline'}
    $p.WaitForExit();$p.ExitCode|Set-Content "$out/producer.exit.txt"
    if($p.ExitCode){throw 'Compilation failed'}
} finally {$p.Dispose()}
$cache="$repo/build/.ninja/fixtures-x86"
Copy-Item "$cache/wow-thunk-nls.c" "$out/fixture.c"
Copy-Item "$cache/wow-thunk-nls.exe" "$out/fixture.exe"
& "$out/fixture.exe" | Tee-Object "$out/result.log"
if ($LASTEXITCODE) {throw 'Original ThunkNLS selection failed'}
Get-FileHash $source,"$out/fixture.c","$out/fixture.exe" | Export-Csv -NoTypeInformation "$out/inputs.csv"
