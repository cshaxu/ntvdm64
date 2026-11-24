param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($BuildRoot)
if (!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)) {
    throw 'A fresh repository build directory is required'
}
New-Item -ItemType Directory $out | Out-Null
& "$repo/tools/build-fixtures.cmd" x86 wow-user-declarations *> "$out/layout.log"
if($LASTEXITCODE){throw 'Original layouts or multipass pointer identity failed'}
& "$repo/tools/build-fixtures.cmd" x86 wow-user-capacity-overflow *> "$out/overflow.log"
if(!$LASTEXITCODE -or !(Select-String -LiteralPath "$out/overflow.log" -SimpleMatch -Pattern 'The size of the function array is greater than the allocated storage')) {
    throw 'Original capacity overflow was not rejected'
}
Get-FileHash "$repo/src/non-mvdm/windows/core/ntuser/inc/user.h","$repo/test/opennt-host/user_declarations.c","$repo/build/.ninja/msvc-x86-release/wow32/user-capacity-overflow.c" | Export-Csv -NoTypeInformation "$out/inputs.csv"
'PASS original USER layouts, multipass pointer identity and overflow rejection'
