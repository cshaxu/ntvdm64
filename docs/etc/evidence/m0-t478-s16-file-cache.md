# T478 S16 — original WOW file-cache retirement

Baseline: 65beb5b87. Finding 34 has two original cache ownership defects.
Both local OpenNT families have token-identical baseline Init/Find/Alloc/Free,
W32MapViewOfFile and FlushMapFileCaches bodies. These are selected WOW32
providers, not guest code or a replacement implementation.

## Contract and minimal correction

InitMapFileCache links eleven entries; the existing twelve-slot backing array
and capacity are unchanged. FlushMapFileCaches must retire each populated
entry: WK32FileRead calls it on cache traffic thresholds, and WK32WowFailedExec
calls it at failed execution retirement. Original while(next != 0) skips the
tail. FreeMapFileCache also calls FindMapFileCache, which promotes the found
node to the head. DIV-874 visits while(node != 0) and saves the next pointer
before freeing, so cleanup-induced reorder cannot redirect the traversal.
Normal unmapping, mapping close and logical file-position writeback remain
owned by the original FreeMapFileCache.

When CreateFileMapping succeeds but MapViewOfFile fails, the original code
closes the mapping but leaves its handle in the cache. Subsequent free or
eviction closes that numeric handle again. DIV-874 clears the stored handle
immediately after the existing close. The original FALSE result, ordinary
ReadFile fallback, cache entry/file ownership and success path are unchanged.
No retry, transaction, cache redesign, new locking or failure tolerance added.
The original serialized WOW owner and Win32 cleanup calls remain; arbitrary
reentrant external mutations are not a new supported contract.

## Focused evidence

[Test](../../../test/integration/wow_file_cache_test.c) and
[runner](../../../test/integration/verify-wow-file-cache.ps1) link the actual
selected x86 wkfileio.obj, substituting only four import relocations in a copy.
Normal tests use real private files and native read-only mappings, with
FILE_FLAG_DELETE_ON_CLOSE. They cover one through eleven populated entries,
lookup-induced reorder, holes, repeated flush/free, original bytes and logical
position writeback. Controlled view failure uses a simulated mapping handle
and deterministic unrelated reuse; its file/read fallback is real. Mapping
creation failure and cache eviction are also checked. Numeric handle reuse
is simulated, not a claim about Windows allocator timing.

Baseline: 579 checks, eight failures. Current: 577 checks, zero failures.
The two extra baseline checks are the test close shim observing two forbidden
repeat closes. The normal full-cache case demonstrates both retained ownership
and missing tail position writeback before the fix. GNU linker reports the
existing corrupt .drectve warning on the MSVC object; this mixed-toolchain
fixture is focused code evidence, not the production linker. The production
build uses MSVC; its map resolves both changed functions from wkfileio.obj.
No real guest crash or naturally occurring handle reuse is claimed.

## Delivery

Coherent x86/x64 product build passed; all 16 Full gates passed.
[Receipt](../../../artifacts/mvdm-audit/shard-3/file-cache-s16-results.json)
records source/object identities, baseline/current focused checks, Full timings
and all ten release hashes. Package b4a13bec-806d-48d3-ad50-728178335129
is published only to this worktree assets/release. DOS4GW/NESticle remain
owner-excluded, not counted as passing. No O:/winnt write, push, guest
modification or external repository change. S17/finding 42 follows.
