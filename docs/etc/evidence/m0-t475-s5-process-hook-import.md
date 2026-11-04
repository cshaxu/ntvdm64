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

## P2 — C packet parser and cross-width payload mechanism

`tests/lib-win32/process-hook/verify-cross-payload.ps1` builds isolated x86
and x64 targets and same-width helper DLLs using the unchanged imported C
library. Run `build/t475-s5/cross-payload-r3.process` exited 0: x86-to-x86,
x86-to-x64, x64-to-x86 and x64-to-x64 payloads were delivered, read and
restored without loading the helper DLL into the target. Both cross-width
helpers rejected corrupted packets while their targets remained alive; each
fixture then cleaned up its own suspended child. The draft product C packet
parser accepted a valid packet and rejected ten malformed cases on both widths.
These tests establish a candidate mechanism, not equivalence of the product
launcher, actual hook installation, all failures or production lifecycle.
The product now selects the C packet parser, while the pre-existing Detours
transport remains active.

An earlier helper-based product draft made pure payloads depend on the matching
Hook DLL, unlike the old path. It was not selected or published. The direct
payload candidate below addresses that specific dependency; product-level
equivalence and missing-DLL tests remain open.

P2 adds a generic direct-payload API that allocates an opaque, bounded payload
in a low-address private region of the suspended target; the receiver validates,
copies and frees it. It does not patch the PE import directory, inject a DLL,
or start a helper. The API requires a single non-concurrent payload and leaves
a rejected consumer's allocation for a retry. `verify-direct-payload.ps1`
under MSVC (`build/t475-s5/direct-payload-r7.process`) and
`verify-direct-mingw.ps1` with explicit GCC toolchains
(`build/t475-s5/direct-mingw-r8.process`) each passed all four x86/x64 pairs.
The fixtures verify exact bytes, absent helper DLL, unchanged PE imports,
duplicate refusal, restricted-handle failures, write-failure recovery, damaged
header rejection, consumer-failure retry and released allocation. The API is
not yet selected by the product. Per the owner, other repositories and
worktrees are outside this task; no synchronization is performed. The 23
earlier imported upstream files remain byte-exact.

The selected product C packet parser retained its previous Detours transport.
The x86/x64 product build and existing ownership tests passed
(`p2/build.process`, exit 0); original Hook fixtures passed 118 x86, 118 x64
and 168 cross-width assertions (`p2/hooks-wrapper.process`, exit 0). Staging
verified 63 immutable guest files and 274 ordinals; Control passed
(`p2/stage.process`, `p2/control.process`, exit 0). Local release package
`b8fbc836-9f8c-482d-80c7-7861110808ed` has ten verified hashes. Full and
DOS4GW/NESticle were not run. Product replacement, helper identity, startup
transaction, and both projects' integrated acceptance remain open.

## P3 — product hook and direct payload migration

The product now selects the C context reader, installer and CreateProcess Hook;
the original C++ provider files were removed. Run16-only context uses the
direct payload API without a Hook DLL or cross-width helper. Hook installation
uses shared import staging and startup trampolines; opposite-width Hook
installation uses the bounded helper. Actual product link maps for Run16, both
workers and Hook widths contain the new providers and no Detours symbol
(`tests/lib-win32/process-hook/verify-product-maps.py`, `p3f/product-maps.process`).
Some diagnostics and their old C++/Detours sources remain and are not product
closure; S5 remains active.

A real propagation failure exposed MSVC C static `dllimport` initializer
thunks; resolving both function addresses at installation time fixed the
actual shared system entry. A real helper timeout test then exposed asynchronous
helper termination being followed by a zero-deadline wait. `helper.c` now waits
up to five seconds for the owned helper before freeing its target allocation;
if it cannot join, it leaves that allocation to the required target abort. This
is one documented local change to the 23-file original import; upstream hash
remains in `upstream-sha256.txt`, and `downstream-sha256.txt` pins the fix.
The identity test confirms 22 original blobs plus that one recorded difference.
No other repository or worktree was modified.

The direct payload implementation also permits a native 64-bit high-address
allocation when low space is exhausted, and retains the actual process-query
error. MSVC and GCC runs (`direct-payload-r10.process`, `direct-mingw-r10.process`)
passed all four width pairs, invalid input/rights, malformed header, failed
write cleanup, consumer retry, actual allocation release, unchanged import
table, absent DLL and a low-address exhaustion/high-address test. These are
controlled fixtures, not all possible memory layouts. The actual product
installer was tested in four width pairs with both Hook DLLs absent
(`tests/lib-win32/process-hook/verify-missing-dll.ps1`, `p3f/missing.process`).
Original Hook regressions passed 118 x86, 118 x64, 168 cross-width assertions,
including helper creation failure, timeout and target death (`p3f/hooks-wrapper.process`).

The x86/x64 product build and ownership tests passed (`p3f/build.process`);
staging verified 63 immutable guest files and 274 NTVDM ordinal exports
(`p3f/stage.process`). The selected Control suite passed (`p3f/control.process`),
and real Win16 WRITE startup reached main/document/page-info in four snapshots
(`p3f/wow.process`); WRITE editing and exit were not verified. Package
`144d08b7-5c28-49ba-b41f-85d58908098a` has ten worktree-release hashes and
the installed MIT/MinHook notices verified. Full, OS/2 NE and external
DOS4GW/NESticle were not run.

## P4 — C Hook and native observation fixtures

The production Hook integration fixture and native process observation fixture
were converted from C++ to C. Array capacities are explicit and the same
controlled cases and failure assertions are retained; build graphs now select C.
The x86/x64 product build passed (`p4/build.process`); Hook fixtures passed
118 x86, 118 x64 and 168 cross-width assertions (`p4/hooks-wrapper.process`).
The C observation fixture was rebuilt for both widths (`p4-native-probes.process`)
and exercised against real package processes, reporting actual ChildFirst
CREATE/EXIT and parent/Direct identities for both widths
(`p4/native-observation-r3.process`). The first two observation attempts failed
because the monitor tool lacked the isolated package identity and then was
outside the isolated package directory; r3 stamped a private copy and placed it
with that isolated package. Those failures are not passing evidence.

Staging checked 63 immutable guest files and 274 ordinal exports
(`p4/stage.process`); Control passed (`p4/control.process`); the ten images
and license in local release package `845cebf2-9632-40b0-8509-912517e8fad0`
were hash verified. Full and DOS4GW/NESticle were not run. Diagnostic probes,
legacy fixtures and the test-only Detours implementation remain outstanding.

## P5 — C startup Console trace

The startup-only Win31 Console trace now uses the shared C trampoline
transaction; it preserves logging, original call/result and LastError.
`tests/observation/verify-console-trace-c.ps1` exercised real interception,
log content, the original function and LastError under x86 and x64 MSVC
(`build/t475-s5/console-trace.process`, exit 0). A test-only NTVDM relink
using the actual product object archive and the C trace also succeeded
(`p5-console-relink.process`, exit 0); that diagnostic image was not run as a
real guest and was not published. Its build script retains the old C++ branch
for the not-yet-converted write guard.

P5's x86/x64 product build and focused checks passed (`p5/build.process`);
staging verified 63 immutable guest files and 274 ordinal exports; Control
passed (`p5/stage.process`, `p5/control.process`, exit 0). Ten local release
hashes and the license match package `f49f5403-2030-433e-87b3-77b5858a2b35`.
Full and DOS4GW/NESticle were not run. Remaining C++/Detours diagnostics and
legacy test sources remain open in S5.

## P6 — C startup palette trace

The Win101 palette observation now uses the shared C startup hook transaction;
the four original calls, their results and LastError remain intact. The
controlled x86/x64 fixture exercised all four real hooks, original calls,
errors and log records (`palette-trace.process`, exit 0). The diagnostic
builder relinked the actual product object archive (`p6-palette-relink-r2.process`,
exit 0); a separately staged private diagnostic package reached Win16 WRITE
main, document and page-info in four snapshots and recorded real palette calls
(`p6-palette-live-r2.process`, exit 0). WRITE editing and exit were not tested.
The first live attempt reached those observations but failed stage cleanup due
to modifying its package after staging; its stage is preserved and does not
count as a clean pass.

The x86/x64 product build passed (`p6/build.process`); staging verified 63
immutable guest files and 274 ordinal exports (`p6/stage.process`). Control
passed (`p6/control.process`), and ten local release image hashes and the
license match package `b5eceac1-5472-4266-8287-75ff14854f64`. Full and
DOS4GW/NESticle were not run. Remaining diagnostic and legacy C++/Detours
sources remain open in S5.

## P7 — retire obsolete Detours mechanism wrapper

The unused `win32_process_hook_apply` wrapper and its Detours-based mock test
were removed. The build target now uses a C test against the selected direct
payload implementation. The controlled self-process fixture checks invalid
arguments, no payload, actual write, duplicate rejection, failed-consumer retry,
actual successful consumption and absence after release: ten checks on both
x86 (`p7/build.process`) and x64 (`p7/test-x64-r2.process`). The first x64
attempt failed to invoke its command script due to Windows path parsing and is
not passing evidence. The previous four-width actual product and Hook failure
integration coverage remains recorded under P3.

The x86/x64 product build and focused checks passed (`p7/build.process`);
staging checked 63 immutable guest files and 274 exports (`p7/stage.process`).
Control passed (`p7/control.process`); ten local release image hashes and the
license match package `392e6339-d0e6-4e4c-a602-3c48f24cde5a`. Full and
DOS4GW/NESticle were not run. Other diagnostics and legacy C++/Detours remain
open in S5.

## P8 — C restricted-write installation guards

Both test-only installation guards use the C startup hook transaction and a
shared path-check implementation; the Win101 and Win31 target and media
allowlists remain distinct. The Detours diagnostic build branch and the two
C++ guard sources were removed. `verify-install-guard-c.ps1` exercises both
x86 configurations against actual file operations (`p8/guard-r6.process`,
exit 0): owned writes and devices succeed; outside writes, deletion, a sibling
with a matching path prefix and cross-boundary renames are denied; read-only
OPEN_IF preserves existing files and does not create missing ones; unapproved
target configuration exits 127. External fixture contents were checked.
These cases are not an exhaustive filesystem security proof.

Both guards linked with the actual product object archive into unpublished
NTVDM images (`p8/relink-r2.process`, exit 0). Both isolated real Win16 WRITE
startups reached the main/document/page-info windows in four snapshots
(`p8/live-r3.process`, exit 0). This does not establish editing, saving or
normal exit. The first live attempt failed because its default temporary
folder lay outside the guard boundary; its stage was preserved. The successful
run directed TEMP and TMP to the already-authorized isolated stage rather than
widening the guard.

The x86/x64 product build and focused checks passed (`p8/build.process`);
staging checked 63 immutable guest files and 274 exports (`p8/stage.process`).
Control passed (`p8/control.process`); ten local release image hashes and the
license match package `454dd50e-b64b-42ed-bed7-453ae71bfd00`. Full and
DOS4GW/NESticle were not run. The running-process console observer, Detours
vendor sources and historical C++ fixtures remain open in S5.

## P9 — C late-install Console observer

The test-only NTCON Console observer now uses the existing C late-patch and
thread guard with the MinHook trampoline backend. Installation is deferred
until the injected input hook runs outside the loader lock; the DLL is pinned
before patches can be published. The five observed APIs retain their original
result and LastError. A failed batch records an error and suppresses input
acknowledgment; installed trampolines remain pinned, and retained suspension
handles are retried before returning to the message loop. The Detours C++
observer source and its diagnostic build selection were removed.

Four independent x64 private-desktop fixture processes exercised actual late
patches, five original API results/errors, trace records, a live pair of other
threads and DLL pinning (`p9/fixture-r4.process`, exit 0). Four additional
processes injected a protection failure after two protections and confirmed
no hook publication, input suppression, preserved calls and continued worker
thread progress (`p9/fixture-fault-r2.process`, exit 0). The final non-debug
DLL was injected by WH_GETMESSAGE into actual isolated NTCON; the real
frontend acknowledged mouse input and the assembled graphics guest completed
with `WINDOW-MOUSE-PASS` (`p9/real-mouse-final2.process`, exit 0). Earlier
attempts failed before injection or configuration due to mixed path separators;
they are not passing evidence. The first WRITE-based attempt also did not
create the required graphics window.

The late primitive checks and suspends the observed thread set but does not
prevent new thread creation, and hook removal is not implemented. The tests do
not prove all concurrency interleavings or normal physical input behavior;
the diagnostic DLL remains pinned until its target exits. These boundaries
are not evidence of a fully general late-hook API.

The x86/x64 product build and focused checks passed (`p9/build.process`);
staging checked 63 immutable guest files and 274 exports (`p9/stage.process`).
Control passed (`p9/control.process`); ten local release image hashes and the
license match package `0abcb77d-13e9-43f6-8049-37fb5636fa3f`. Full and
DOS4GW/NESticle were not run. Vendored Detours and historical C++ fixtures
remain open in S5.
