# WOW file-map cache: selected-object failure and flush witnesses

This bounded diagnostic examines `src/mvdm/wow32/wkfileio.c`, not a guest application. It runs selected cached x86 `wkfileio.obj` code with test-only fake Win32 import slots; it does not establish that a real application has encountered either condition. The original guest and production binaries were not modified.

## Identity and composition

- Source SHA-256: `0593995bf9159b23bb21afff66a961ef83c6e052bc09adb5c6a6cb09d559879a`.
- Existing `.ninja/msvc-x86-release/wow32/obj/wkfileio.obj` SHA-256: `4a3455d0adcaa26fbdee4910bd9dfad96aa9670e934e129f2f9116a637fe3688`.
- Existing built `wow32.dll` SHA-256: `c97c512c6f6bd6f16a6fb68f340760f551cd2186b5262b1a6ae892fc8a06bc58`; map SHA-256: `37ce4b735da1536d2dee3492169d5a46785c04111e305a378e239b33d552b1f5`.
- The map lists `_W32MapViewOfFile@8`, `_FreeMapFileCache@4`, `_FlushMapFileCaches@0`, `_InitMapFileCache@0` and `_AllocMapFileCache@0` from `wkfileio.obj`. This establishes extraction into that built image, not the identity of an installed image or execution by a real guest.
- Fixture `file-cache-selected-object-fixture.c` SHA-256: `0f1416a5a151f6a31bd2f3e0ba7b33909a0ca6361b3c42b49a3695de8661e908`. The fixture redirects six imported functions in a **copy** of the object; it does not replace the original code bodies. Fake handle value 42 and deliberate reuse represent a possible sequence, not a Windows handle allocator guarantee.

## Observations

1. The controlled `CreateFileMapping` succeeds, `MapViewOfFile` fails, and the source closes the mapping at `wkfileio.c:700` but retains the nonzero `hMappedFileObject` in the cache. The fixture then simulates reuse of that numeric handle and calls `FreeMapFileCache`; the fake close receives it a second time, closing the simulated new owner. Output: `failed-map closes=2 reused-handle-alive=0`. This is an explicit failure-path lifetime risk; whether the intervening reuse occurs in a product execution is unproved. A repair needs to invalidate the stored handle after the failed-map close, with failure and later cleanup coverage, and source-divergence assessment under project policy.
2. `MAX_MAPPED_FILES` is 12; `InitMapFileCache` links 11 entries. Filling those entries and calling `FlushMapFileCaches` leaves one occupied (`flush remaining=1`) because the loop tests `hpfNext != 0` before examining the last node. The test deliberately models 11 distinct entries; it does not prove application data corruption. The intended eviction/flush contract and interactions with close/write must be checked against the original call chain before choosing a fix.

Fixture exit status was 0 and both expected observations were printed. These are targeted negative/lifetime witnesses, not DOS/WOW/Win3.x end-to-end passes. In particular, successful real mappings, actual native handle reuse, concurrent calls, target task retirement and production error reporting remain untested.

## Reproduction

From the repository root in PowerShell, with GNU `objcopy` 2.46.1 and `i686-w64-mingw32-gcc` 16.1.0 installed, and the SHA-matching cached object present:

```powershell
New-Item -ItemType Directory -Force build/t473-s10/cache-fixture | Out-Null
objcopy --redefine-sym '__imp__CreateFileMappingA@24=_fake_imp_Create' --redefine-sym '__imp__MapViewOfFile@20=_fake_imp_Map' --redefine-sym '__imp__UnmapViewOfFile@4=_fake_imp_Unmap' --redefine-sym '__imp__CloseHandle@4=_fake_imp_Close' --redefine-sym '__imp__SetFilePointer@16=_fake_imp_Seek' --redefine-sym '__imp__GetFileSize@8=_fake_imp_Size' build/.ninja/msvc-x86-release/wow32/obj/wkfileio.obj build/t473-s10/cache-fixture/wkfileio-test.obj
i686-w64-mingw32-gcc -O2 -ffunction-sections -fdata-sections artifacts/mvdm-audit/cross-369/file-cache-selected-object-fixture.c build/t473-s10/cache-fixture/wkfileio-test.obj '-Wl,--gc-sections' -o build/t473-s10/cache-fixture/file-cache-test.exe
& build/t473-s10/cache-fixture/file-cache-test.exe
$LASTEXITCODE
```

GNU linker emitted `Warning: corrupt .drectve at end of def file`; the executable completed with status zero and expected observations. This warning and import substitutions limit the witness; they do not establish equivalent MSVC linking. No unsupported sections are intentionally exercised.
