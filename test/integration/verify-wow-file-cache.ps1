param([string]$Object='build/.ninja/msvc-x86-release/wow32/obj/wkfileio.obj',[string]$Output='build/t478-s16/current')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
Set-Location $repo
New-Item -ItemType Directory -Force $Output | Out-Null
$out=(Resolve-Path $Output).Path
& objcopy --redefine-sym '__imp__CreateFileMappingA@24=_cache_imp_Create' --redefine-sym '__imp__MapViewOfFile@20=_cache_imp_Map' --redefine-sym '__imp__CloseHandle@4=_cache_imp_Close' --redefine-sym '__imp__UnmapViewOfFile@4=_cache_imp_Unmap' $Object "$out/wkfileio-test.obj"
if($LASTEXITCODE){throw 'Object import substitution failed'}
& i686-w64-mingw32-gcc -O2 -ffunction-sections -fdata-sections test/integration/wow_file_cache_test.c "$out/wkfileio-test.obj" '-Wl,--gc-sections' -o "$out/cache-test.exe" 2>&1 | Tee-Object "$out/build.txt"
if($LASTEXITCODE){throw 'Fixture link failed'}
Push-Location $out
try { & ./cache-test.exe | Tee-Object run.txt; $result=$LASTEXITCODE } finally { Pop-Location }
exit $result
