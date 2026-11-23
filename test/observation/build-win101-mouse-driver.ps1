param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$build=[IO.Path]::GetFullPath($(if([IO.Path]::IsPathRooted($BuildRoot)){$BuildRoot}else{Join-Path $repo $BuildRoot}))
if (!$build.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or
    (Test-Path -LiteralPath $build)) { throw 'Fresh build-owned output required' }
New-Item -ItemType Directory -Path $build | Out-Null
$source=Join-Path $repo 'src/addon/mouse101/mouse101.asm'
$assembler=(Get-Command nasm.exe -ErrorAction Stop).Source
$line='/d /s /c ""'+$repo+'/tools/build-addon.cmd" mouse101-tests"'
$p=Start-Process $env:ComSpec -ArgumentList $line -WorkingDirectory $repo -WindowStyle Hidden -PassThru -RedirectStandardOutput "$build/build.stdout" -RedirectStandardError "$build/build.stderr"
try {
    $null=$p.Handle
    @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$line;timeoutSeconds=120}|ConvertTo-Json|Set-Content "$build/build.owner.json"
    if(!$p.WaitForExit(120000)){$p.Kill($true);$p.WaitForExit();throw 'Mouse native build deadline'}
    $p.WaitForExit();$p.ExitCode|Set-Content "$build/build.exit"
    if($p.ExitCode){throw 'Native mouse test build failed'}
} finally {$p.Dispose()}
Copy-Item "$repo/build/.ninja/addon/mouse101/MOUSE101.DRV" "$build/MOUSE101.DRV"
Copy-Item "$repo/build/.ninja/addon/mouse101-M101TEST.COM/M101TEST.COM" "$build/M101TEST.COM"
Copy-Item "$repo/build/.ninja/addon/mouse101-M101REAL.COM/M101REAL.COM" "$build/M101REAL.COM"
$audit=(& "$PSScriptRoot/audit-win101-mouse-contract.ps1" -Driver "$build/MOUSE101.DRV" -Snapshot '') | ConvertFrom-Json
if ($audit.entries.Count -ne 3 -or $audit.automaticDataSegment -ne 2) { throw 'Unexpected guest driver ABI' }
for ($index=0;$index -lt 3;$index++) {
    if ($audit.entries[$index].ordinal -ne $index+1 -or $audit.entries[$index].segment -ne 1) {
        throw 'Invalid Windows export'
    }
}
[ordered]@{
    kind='guest-build-structure-only-not-runtime-pass'
    source=$source; sourceSha256=(Get-FileHash $source).Hash
    assembler=$assembler; assemblerVersion=(& $assembler -v)
    driver="$build/MOUSE101.DRV"; driverSha256=(Get-FileHash "$build/MOUSE101.DRV").Hash
    contract=$audit
} | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath "$build/build.json"
'PASS independent guest driver assembly and NE structure; runtime not yet verified'
