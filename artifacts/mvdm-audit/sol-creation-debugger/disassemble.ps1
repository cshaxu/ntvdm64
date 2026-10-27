$ErrorActionPreference='Stop'
$repo='O:\repos.hobby\ntvdm64'
$out=Join-Path $repo 'build/t473-s10/sol-claim-code'
if(Test-Path -LiteralPath $out){throw 'Fresh disassembly directory required'}
$null=New-Item -ItemType Directory $out
& "$repo/build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd" dumpbin.exe /nologo /disasm '/range:0x10072ac0,0x10072d10' "$repo/build/t473-s10/candidate-validation/package/system32/WOW32.DLL" *> "$out/claim.txt"
if($LASTEXITCODE){exit $LASTEXITCODE}
Get-FileHash "$repo/build/t473-s10/candidate-validation/package/system32/WOW32.DLL","$repo/build/.ninja/msvc-x86-release/wow32/wow32.dll.map"|Select-Object Path,Hash|ConvertTo-Json|Set-Content "$out/inputs.json"
Get-Content "$out/claim.txt"
