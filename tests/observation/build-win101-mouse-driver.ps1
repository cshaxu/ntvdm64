param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$build=[IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
if (!$build.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or
    (Test-Path -LiteralPath $build)) { throw 'Fresh build-owned output required' }
New-Item -ItemType Directory -Path $build | Out-Null
$source=Join-Path $repo 'src/addon/win101-mouse-drv/mouse101.asm'
$assembler=(Get-Command nasm.exe -ErrorAction Stop).Source
& $assembler -f bin $source -o "$build/MOUSE101.DRV" -l "$build/mouse101.lst"
if ($LASTEXITCODE) { throw 'Mouse driver assembly failed' }
$include=(Split-Path $source).Replace('\','/')+'/'
& $assembler -f bin "-I$include" "$PSScriptRoot/win101_mouse_driver_guest.asm" -o "$build/M101TEST.COM" -l "$build/guest-test.lst"
if ($LASTEXITCODE) { throw 'Guest driver harness assembly failed' }
& $assembler -f bin "-I$include" "$PSScriptRoot/win101_mouse_driver_real.asm" -o "$build/M101REAL.COM" -l "$build/real-test.lst"
if ($LASTEXITCODE) { throw 'Real-provider guest harness assembly failed' }
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
