param([Parameter(Mandatory)][string]$BuildRoot,
      [Parameter(Mandatory)][string]$Python)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)) {
    throw 'Fresh build-owned output required'
}
$source=Join-Path $repo 'src/addon/mouse31/mouse31.asm'
$assembler=(Get-Command nasm.exe -ErrorAction Stop).Source
$pythonPath=(Resolve-Path -LiteralPath $Python).Path
New-Item -ItemType Directory -Path $root|Out-Null
$line='/d /s /c ""'+$repo+'/tools/build-addon.cmd" mouse31-tests"'
$p=Start-Process $env:ComSpec -ArgumentList $line -WorkingDirectory $repo -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/build.stdout" -RedirectStandardError "$root/build.stderr"
try {
    $null=$p.Handle
    @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$line;timeoutSeconds=120}|ConvertTo-Json|Set-Content "$root/build.owner.json"
    if(!$p.WaitForExit(120000)){$p.Kill($true);$p.WaitForExit();throw 'Mouse native build deadline'}
    $p.WaitForExit();$p.ExitCode|Set-Content "$root/build.exit"
    if($p.ExitCode){throw 'Native mouse test build failed'}
} finally {$p.Dispose()}
Copy-Item "$repo/build/.ninja/addon/mouse31/MOUSE31.DRV" "$root/MOUSE31.DRV"
Copy-Item "$repo/build/.ninja/addon/mouse31-M31TEST.COM/M31TEST.COM" "$root/M31TEST.COM"
Copy-Item "$repo/build/.ninja/addon/mouse31-M31REENT.COM/M31REENT.COM" "$root/M31REENT.COM"
Copy-Item "$repo/build/.ninja/addon/mouse31-M31PM.COM/M31PM.COM" "$root/M31PM.COM"
Copy-Item "$repo/build/.ninja/addon/mouse31-M31REAL.COM/M31REAL.COM" "$root/M31REAL.COM"
Copy-Item "$repo/build/.ninja/addon/mouse31-M31CTX.COM/M31CTX.COM" "$root/M31CTX.COM"
Copy-Item "$repo/build/.ninja/addon/mouse31-M31WIN.EXE/M31WIN.EXE" "$root/M31WIN.EXE"
Copy-Item "$repo/build/.ninja/addon/mouse31-M31SWAP.EXE/M31SWAP.EXE" "$root/M31SWAP.EXE"
Copy-Item "$repo/build/.ninja/addon/mouse31-M31ALOC.EXE/M31ALOC.EXE" "$root/M31ALOC.EXE"
Copy-Item "$repo/build/.ninja/addon/mouse31-M31VM.EXE/M31VM.EXE" "$root/M31VM.EXE"
Copy-Item "$repo/build/.ninja/addon/mouse31-M31VMCTL.EXE/M31VMCTL.EXE" "$root/M31VMCTL.EXE"
Copy-Item "$repo/build/.ninja/addon/mouse31-M31CHLD.COM/M31CHLD.COM" "$root/M31CHLD.COM"
Copy-Item "$repo/build/.ninja/addon/mouse31-M31VXD.COM/M31VXD.COM" "$root/M31VXD.COM"
Copy-Item "$repo/build/.ninja/addon/mouse31-M31OWNR.EXE/M31OWNR.EXE" "$root/M31OWNR.EXE"
Copy-Item "$repo/build/.ninja/addon/mouse31-M31CTRL.EXE/M31CTRL.EXE" "$root/M31CTRL.EXE"
& $pythonPath "$PSScriptRoot/audit-win31-mouse.py" --driver "$root/MOUSE31.DRV" --output "$root/contract.json" | Out-Null
if($LASTEXITCODE){throw 'NE audit failed'}
$audit=Get-Content "$root/contract.json" -Raw|ConvertFrom-Json
$expected=@(1,2,3,4,8)
if($audit.neVersion -ne '0x30a' -or $audit.automaticDataSegment -ne 2 -or
   $audit.flags -ne '0x8309' -or $audit.imports.Count -ne 0 -or $audit.entries.Count -ne 5) {
    throw 'Wrong protected driver/module ABI'
}
for($i=0;$i -lt $expected.Count;$i++) {
    if($audit.entries[$i].ordinal -ne $expected[$i] -or $audit.entries[$i].segment -ne 1) {
        throw 'Wrong Windows entrypoint'
    }
}
if(!@($audit.residentNames|Where-Object {$_.name -eq 'WEP' -and $_.ordinal -eq 8}).Count) {
    throw 'Missing named Windows cleanup export'
}
if($audit.segments[0].flags -ne '0x140' -or $audit.segments[1].flags -ne '0x41') {
    throw 'Callbacks/data must be fixed and resident'
}
[ordered]@{role='independent-guest-driver-build-not-runtime-pass';source=$source;
    sourceSha256=(Get-FileHash $source).Hash;assembler=$assembler;
    assemblerVersion=(& $assembler -v);driverSha256=(Get-FileHash "$root/MOUSE31.DRV").Hash;
    contract=$audit} |ConvertTo-Json -Depth 8|Set-Content "$root/build.json"
'PASS guest assembly and fixed/resident Win3.1 NE ABI; callback/provider/Windows tests remain required'
