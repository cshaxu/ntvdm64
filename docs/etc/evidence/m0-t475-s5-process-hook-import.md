# T475 S5 — process-hook import and C++/Detours removal

## Source identity and initial disposition

Owner-supplied `O:/repos.hobby/win16style/src/lib-win32/process-hook` was read
on 2026-11-04. The adjacent project's S27 history identifies product commit
`2227486` for the pure-payload capability. Its currently visible source was
copied as 23 byte-identical files; the hashes below identify that actual source
snapshot and do not assert the entire neighboring checkout equals that commit.
The adjacent project has an MIT root license (`LICENSE.win16style`); the
MinHook slice includes its BSD license and author notice. Binary redistribution
must retain the applicable notices. No source was fetched from the network.
The existing Detours implementation remains selected until an equivalence-proven
replacement is complete; the imported files are the P1 staged library, not yet selected by production.

| Imported path | SHA-256 |
| --- | --- |
| `helper.c` | `b3a349a197ae2d68f2ca7f076420d602ac571ac47aa7dc3cd354577f805f4fac` |
| `helper.h` | `81b42b8a73062340531dd483c1a949d1493b420aca0c46d23acd995c9e57987e` |
| `late_guard.c` | `67b623f41ebd50247d723903134f5b99ffc60053b93fba276758e74d162d2c4c` |
| `late_guard.h` | `8f6805a2cf238da66fcd4c7039bd96bfd392049d6bfa2e83f81e0faeeb2b773f` |
| `late_patch.c` | `f331d76aab38c1f222c71615b74dad38c7ff49a93312898c608a4f5948898c7a` |
| `late_patch.h` | `cdc27ff7cbbd70bb2e08fd741f7aac15bb1b7128b4e2c5afe35d983e077c5f04` |
| `minhook/AUTHORS.txt` | `073b4e8b0a5a0cd9f4dfa838369f4fc7a4f47151d930b4e1fd891644060a897f` |
| `minhook/LICENSE.txt` | `d5c2224982b0b95a16b098a561335fa93f3eaf4d0aa964b7843edb986df78dc8` |
| `minhook/src/buffer.c` | `53228de3f2a14a390f6d97538d702c20a4388eea42e53b5b999532bab018287b` |
| `minhook/src/buffer.h` | `9769702bd625249e59a19ede05c84365fcc7707e1fa0e5df08df42227cee13e3` |
| `minhook/src/hde/hde32.c` | `5da4941e9044f7b325c7f952b5840089e365f374740b7fe7318805d08c0d6c32` |
| `minhook/src/hde/hde32.h` | `5132de5be8ac7a9d054420f5caef0fb01437faa892f03b3cdc77f795973bf15a` |
| `minhook/src/hde/hde64.c` | `824eafa315322e6da105a7ce4ec1fc0e0adeee0106d32152fbb0b59e709bd15c` |
| `minhook/src/hde/hde64.h` | `e99aa4997bda14b534c614c3d8cb78a72c4aca91a1212c8b03ec605d1d75e36e` |
| `minhook/src/hde/pstdint.h` | `604d1cdf45c5a95bc9cf1bbf39ca05ef83013541101d34971c5cd7577e3d5247` |
| `minhook/src/hde/table32.h` | `43bf687a9fff071b5f46070db4d36c846f8e069cdc57485ee897c499aacdde23` |
| `minhook/src/hde/table64.h` | `b774446d2f110ce954fb0a710f4693c5562ddbd8d56fe84106f2ee80db8b50a2` |
| `minhook/src/trampoline.c` | `f13a169f803ff5a8e45c58ce668b4377a9878c0fc4452369b8a1325065186b2b` |
| `minhook/src/trampoline.h` | `53ed9934492320617a27bfd952ffe7f5b7a4a1d09963d4512447082fe7b727b4` |
| `staging.c` | `f3d8ec879d3663d6fce42812959d7ca15c43e92ce283d24c541095f21fa48025` |
| `staging.h` | `910166162612075fc4916b0d4cdcd05dcdc29f191e5b6ffd963681f3c35b8514` |
| `startup_hooks.c` | `0cbeff0b7f738d84c5a79d388206b5c8847bcb20dd23e71165ca38cf7861cf4c` |
| `startup_hooks.h` | `7c7c7b8007199447567895982b9e65805be36165d2e7a4783a573559d4d0a89d` |

## Initial contract mapping and open proof

- Same-width DLL staging and startup trampoline transactions have candidate C
  interfaces; compare behavior against actual NTHook callers and failure paths.
- Run16 receives a payload without injecting its own Hook DLL; the original
  Detours GUID payload differs from the adjacent library's format. The imported
  pure-payload path accepts only same-width targets. A target-width helper may
  be able to call the unchanged pure-payload API, but loader/ordinal startup,
  helper recognition, capability, malformed packet and cleanup semantics need
  implementation and proof; no current cross-width compatibility is claimed.
- Product context parsing, packet validation, duplicate handles, failure
  cleanup, launcher identity and process policy stay with their existing owners.
- The audit must cover diagnostics and historical test sources as well as the
  selected production graph; several diagnostics use C++ standard library
  strings and Detours. Replacing/removing them without functional disposition
  is not sufficient to satisfy the owner's codebase-wide requirement.

No product library integration or behavior equivalence has been validated yet.
Four-width propagation, negative and lifecycle tests remain open.

## MSVC portability and isolated same-width checks

The imported library was built in `build/t475-s5/portability` with the project
MSVC toolchain. The initial x86 link failed because the original GCC-targeted
startup hook uses `__builtin_unreachable`; this is a compiler compatibility
issue, not a changed library source. A forced MSVC-only header maps the intrinsic
to `__assume(0)` without modifying the imported file. Serial x86 and x64
rebuilds plus the adjacent link and payload fixtures each exited zero; their
`x86.final.process`, `x64.final.process`, and corresponding `.stdout` record
PIDs, times, actual exits and `PROCESS_HOOK_LINK_PASS` and
`PAYLOAD_ONLY_PASS`. The payload fixture verifies repeated same-width staging,
reading and restoration, released allocation, and absence of injected DLLs.
These are isolated library tests, not product or cross-width proof. An earlier
parallel runner failed to record child exit codes due to its PowerShell array
construction, and is not used as passing evidence. The serial runs followed
confirmation that its child processes had exited.

## P1 — local import and baseline package

The checked-in `tests/lib-win32/process-hook/verify-upstream.py` verified all
23 recorded source hashes (`build/t475-s5/p1/identity.process`, exit 0).
`tests/lib-win32/process-hook/verify-msvc.ps1` rebuilt and exercised original
link and same-width payload fixtures under both MSVC widths
(`build/t475-s5/portability-checked.process`, exit 0). The project x86/x64
build and existing ownership and process tests passed (`p1/build.process`,
exit 0); staging verified 63 immutable guest files and 274 exports
(`p1/stage.process`, exit 0); selected Control passed (`p1/control.process`,
exit 0). Local release package `8ab5e971-e071-41f7-bf24-c5e694fcbbff` has ten
hash-verified images. Full and external DOS4GW/NESticle were not run. P1
only imports and independently tests the library: existing Detours production
selection remains, and product migration is not complete. Main publication
and push remain deferred per owner.
