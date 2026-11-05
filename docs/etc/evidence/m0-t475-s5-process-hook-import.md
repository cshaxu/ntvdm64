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

## P10 — retire vendored Detours

The unreferenced Detours source, header and license were removed from the
working tree, along with the authored-profile checker's Detours exclusion.
The complete historical source and license remain retrievable from this
repository's commit `bd891f96262e5553765be659e616bc33a93b66d9`, under
`src/lib-win32/process-hook/detours/`; the license blob is
`e6a4c563056daedce20576a96a4d77b02a7dc8b6`. Current product and diagnostic
build sources contain no Detours dependency; the product-map verifier retains
an intentional negative-control string.

After removal, the NTCON C observer, both startup traces and both restricted
write guards built and passed their focused tests (`p10/diagnostics.process`,
exit 0); the NTVDM guard diagnostic also relinked without Detours
(`p10/guard-relink.process`, exit 0). Five current product link maps passed
required-provider and Detours-negative checks (`p10/product-maps.process`,
exit 0). Product builds and focused checks passed (`p10/build.process`);
staging verified 63 immutable guest files and 274 ordinal exports
(`p10/stage.process`); Control passed (`p10/control.process`). Ten release
image hashes and the license match package
`1347cbbf-7cc8-4db4-b3d7-4a68910c8575`.

An additional local adaptation of the authored SoftPC profile checker to the
existing combined build root failed (`p10/profile-final.process`, exit 1). Its graph
shows the SoftPC flags on `lib-win32/rpc/local_binding.c`,
`lib-win32/io/native_path.c`, and `lib-win32/crt-compat/crt_compat.c`; it also
flags the generated configuration path and the deliberate WOW thunk boundary
fixture, which require separate classification for this build layout. The
standard check's default release-cache roots are absent in this worktree; this
adapted result is not a passing gate and the pre-existing flag issue is not
caused or resolved by the Detours removal. Full and DOS4GW/NESticle were not
run. Historical C++ fixtures and audit evidence remain open in S5.

## P11 — enforce C native graphs and repair profile leakage

The active native generator now accepts C source edges only; it no longer
selects C++ compilation. Three independent modified reference graphs with
`.cpp`, `.cc` and `.cxx` target inputs were rejected for the intended reason
(`p11/negative-graph-r2.process`, exit 0). The first negative-test attempt
used a directory outside the required Ninja tree and did not reach that check.
Eight selected product graphs exposed 971 compiler commands, none selecting
C++ (`p11/selected-c-commands.process`, exit 0).

The P10 profile issue was traced to missing per-edge flags for x86 local RPC
and native path, and inherited WOW32 original-profile flags for the CRT bridge.
Those edges now use their existing authored-code flags while retaining the
WOW32 x86 calling convention. The checker accepts an explicit combined build
root and classifies the generated original VDM carrier and the deliberately
original-profile WOW boundary fixture according to their actual roles. Before
rebuilding, the same checker rejected the three unintended lib-win32 edges
(`p11/profile-before.process`, exit 1); afterward it passed 443 selected
authored/generated compile commands (`p11/profile-after.process`, exit 0).
This is a selected-graph audit, not proof about every possible configuration.

The x86/x64 product build passed (`p11/build.process`), staging verified 63
immutable guest files and 274 ordinal exports (`p11/stage.process`), and
Control passed (`p11/control.process`). Real Win16 WRITE startup reached its
main/document/page-info windows in four isolated snapshots
(`p11/wow-r2.process`); editing, saving and normal exit were not tested. The
first WRITE attempt ran concurrently with Control and missed the first sample;
the other three samples contained all three windows. That attempt failed and
is not passing evidence; parallel load is a possible cause, not a proven one.
Ten local release image hashes and the license match package
`2edc8463-29b6-49b2-a9e6-c369ecd28ac8`. Full and DOS4GW/NESticle were not
run. Historical C++ artifacts and the late-hook concurrency boundary remain
open in S5.

## P12 historical C++ source index

The following inactive files were removed from the working tree under the
owner's C++ removal request. Their original Git blobs remain reachable in
commit `09b58f5c6`; retrieve an exact blob with `git cat-file blob <blob-id>`.
The audit probe blob is byte-identical to its previously recorded SHA-256; the
six fixture working copies differed from their stored blobs only by CRLF/LF.
`p11/archive-audit-nine.process` verified those identities before removal.
The old Bochs fixtures have no references from active `src`, `tests` or
`tools` and their required Bochs and adapter headers are absent. This is not
a claim that they passed or that their historical behavior was recreated.

The two OpenNT `.hxx` files retain their original provenance as deselected
mirror declarations. They reference each other, have no other source/tool/test
references, and appeared in none of the eight current Ninja dependency logs
(`p11/header-deps.process`). No original source content was rewritten; the
exact original blobs remain in the same commit. The selected-graph evidence
does not prove these declarations useless in other OpenNT configurations.

| Former path | Git blob | Blob SHA-256 |
| --- | --- | --- |
| `tests/component-integration/fixtures/t198_s12_native_composed_ud_fixture.cc` | `8fdea05006f4acb2567e2d6d4c19e184ce9c699c` | `f92856c62d314a9e9304947d994fd08f59faaad01096b0673968716a66ebebd6` |
| `tests/component-integration/fixtures/t198_s18_native_dem_lifecycle_fixture.cc` | `ae67876c1fb0073acc6d7f071a88f8232526c37d` | `23044fbde5f00d0a081be886de0f657f791aba39d2308f1904f8b80a7def704b` |
| `tests/component-integration/fixtures/t198_s18_native_dta_search_fixture.cc` | `90934f35db0e247c46f100cb6734f4d215dccf33` | `df3899458699cbe3e129066b83306236a234bfebe9d2f0ee17b78f1cca0b064b` |
| `tests/component-integration/fixtures/t198_s20_native_dem_load_dos_fixture.cc` | `94a64b7dd92badd5494fc9e67cdb9993c4a8451b` | `9088743c8ab1cb7b75cbc703011e2fbfa933b946c9e3df45ae0dec3659131bc1` |
| `tests/component-integration/fixtures/t198_s23_ntio_preentry_request_fixture.cc` | `181247580486126c43f9add512cea9f234287473` | `ad63116e63ac4ddf257d74aa61ed4de4bb815e8e638307aa3e75703f10fcf26f` |
| `tests/legacy/opennt-bop/dpmi/t257_s3_dpmi_startup_session_fixture.cc` | `bdb159dee6b899e793cdc57d8f6547da7a2a2c91` | `5200329f13d8f5a5d15350a01254fc24edeb1b70273961a11d7fe507bc5e0bc0` |
| `artifacts/mvdm-audit/shard-3/forcedos-context-probe.cpp` | `1ff025d17b42a51efcc126a5491e904b1d80a52b` | `030602a205746ee0cc867a87fc537791c23942e8d3ae16e627e15e0b4102d873` |
| `src/non-mvdm/public/internal/base/inc/byteordr.hxx` | `009f56c5e0998f8a1aa331bb90bf937ad47be8ec` | `65c4e21cd6afbad1110847bc8398916b373e0fe9e848846dda9f4fe39d9d7589` |
| `src/non-mvdm/public/internal/base/inc/propmac.hxx` | `30facf4a91f293b4a4a765c4f5d4885a577dcc16` | `59cdc637b1f9d7db0706d0292b2e5a9146ba270ea20b3014f201a1f50a1c2a41` |

After removal, the nine indexed Git objects still matched their recorded
SHA-256 (`p12/archive-check.process`, exit 0). The tracked working tree had
no remaining `.cpp`, `.cc`, `.cxx`, `.c++`, `.hpp`, `.hh`, `.hxx`, `.ipp`
or `.tpp` files (`p12/source-scan.process`, exit 0); this scan does not
classify untracked build outputs or scripts that can generate C++ source.
The product graphs retained 971 C-only compiler commands and the five product
maps rejected Detours (`p12/selected-c.process`, `p12/product-maps.process`,
exit 0). The 443-command SoftPC profile audit remained clean
(`p12/profile.process`, exit 0).

The x86/x64 product build and focused checks passed (`p12/build.process`);
staging checked 63 immutable guest files and 274 exports (`p12/stage.process`)
and Control passed (`p12/control.process`). Ten local release image hashes
and the license match package `3ae63090-eba4-445c-b264-edf54f09b56f`.
Full and DOS4GW/NESticle were not run. Historical scripts capable of emitting
C++ or referring to retired C++ inputs, and late-hook concurrency, remain
open; no full-codebase completion is claimed.

## Independent pre-replacement comparison

The P2 package `b8fbc836-9f8c-482d-80c7-7861110808ed`, before product
selection of the new backend, provided isolated four-sample WinMINE, SOL and
WRITE baselines (`full-validation/baseline.process`, exit 0). The isolated P12
package `3ae63090-eba4-445c-b264-edf54f09b56f` passed Full selected gates
against those baseline signatures (`full-validation/full.process`, exit 0):
three Win16 startup frontiers, both 17-case frontend command matrices, RPC,
GUI, lifecycle and nested handoff checks. The test stages use fresh package
identity where requested. Window signatures do not prove gameplay or editing,
and these selected gates do not prove every application's equivalence.
DOS4GW/NESticle was skipped because its verified external fixture was not
supplied; full retirement deadlines were not requested.

## P13 — historical generator C outputs

The two T198 byte-array generators now produce `.c` files with the same array
identifiers and bytes, using C linkage. Historical C++ linkage is not claimed;
the old consumers are absent. The NTDOS generator rejects non-C output paths;
the S93 output manifest names the C files. The original locked input size and
SHA-256 checks remain. All three locked historical binaries are absent locally, so
an end-to-end test using those original binaries was not possible. With
synthetic data only, the tests first demonstrated that the real identity
checks reject it, then substituted hashes inside the isolated test process to
exercise output formatting and compiled/ran byte-by-byte checks on x86 and
x64 (`p13/arrays-r2.process`, exit 0). No synthetic binary is guest media.

The T290 disabled-ingress generator previously selected a retired C++ file,
but that file, its C companion and its fixture are all absent. The generator
now fails explicitly rather than emitting an unusable C++ build graph. This
is not a passing replacement for that historical fixture; the T290 matrix
also references six other missing generators. The original script remains
in commit `09b58f5c6`, blob `086c1c4407a0bac84eebd672558845e2915e1d14`;
the two previous T198 generator blobs are
`11c6ec6c19b97d55eb35e431b238ba3c65a4db4c` and
`728d13584d9dc2315170f57be306b1f724a314fd`.

The x86/x64 product build and focused checks passed (`p13/build.process`);
the 443-command SoftPC profile and 971-command C-only selected graph checks
passed (`p13/profile.process`, `p13/selected-c.process`). Staging verified 63
immutable guest files and 274 ordinal exports (`p13/stage.process`), and
Control passed (`p13/control.process`). Ten release image hashes and the
license match local package `f4f0567d-2924-4971-bdc3-a5a4d73559ce`.
Archived historical scripts and the late-hook concurrency limit remain open.

## P14 archived C++ build-script index

Three inactive pre-T260 scripts that selected historical Bochs C++ sources
were removed from the working tree. Their required Bochs inputs and retained
base build artifacts are absent, and no active `src`, `tests`, or `tools`
reference to their script names was found. This retirement is not a passing
replacement for those historical probes. Exact source blobs remain reachable
from commit `09b58f5c6`; retrieve them with `git cat-file blob <blob-id>`.
`p13/historical-script-audit.process` verified normalized working content
against those historical objects before removal.

| Former path | Git blob | Blob SHA-256 |
| --- | --- | --- |
| `tools/historical/pre-t260/build/New-T187S3PassiveListenerEngineDerivative.ps1` | `5574a90a6b342b27351d1a3a817f9c2f3f929931` | `8838d55564ac938bd657042908272ef8d9406fa1b95094b5e143bbf9f9e5e349` |
| `tools/historical/pre-t260/build/New-T190S3CommandBootstrapDerivative.ps1` | `65bc38282b03a9e1518618ea2d3fc4e176c55c7b` | `7d5418b5abe2db88f1f6959cec6a42e97470987455c13e40675daf59cc3639b4` |
| `tools/historical/pre-t260/probe/Invoke-T212S5EngineCancellationProbe.ps1` | `d3961ca8b16c80e479280c49d5fe9b0901f27867` | `53e0fe9df5376566ae303e60267ec0131590b00d79c51f9f6845db739be42390` |

The three indexed historical script blobs remained byte-retrievable and
matched their SHA-256 after retirement (`p14/archive-check.process`, exit 0).
The top-level CMake project and presets now enable C only; the obsolete C++
compiler launcher setting was removed. On fresh x86 and x64 Ninja trees,
CMake configured successfully, did not create a C++ compiler cache entry or
detection file, and built the C observation peer (`p14/cmake-check.process`,
exit 0). Cache ON/OFF reconfiguration and C peer builds also passed for both
architectures in those same trees (`p14/toggle.process`, exit 0). The build-
hygiene test was updated to check the C launcher and reject C++ compile modes;
its parser and a negative expression control passed
(`p14/hygiene-check.log`). The complete build-hygiene suite was not run.

The imported-library identity check confirmed 22 unchanged upstream files and
one recorded local helper fix (`p14/import-identity.process`, exit 0).

Synthetic C-array and input identity-negative tests passed (`p14-arrays.process`,
exit 0). The x86/x64 product build and focused checks passed (`p14/build.process`);
the 443-command SoftPC profile audit, 971-command selected C-only graph scan,
and tracked-source C++ extension scan passed (`p14/profile.process`,
`p14/selected-c.process`, `p14/source-scan.process`). Staging checked 63
immutable guest files and 274 exports (`p14/stage.process`), and Control passed
(`p14/control.process`). Ten local release image hashes and the license match
package `8e4a9c29-e13d-4bd3-979a-4e0ed94e1d52`. The earlier P12 Full result
remains selected-gate evidence; the external DOS4GW/NESticle fixture was not
available. The historical T290 matrix remains incomplete, and new-thread
races and removal are not proven for the experimental late-hook primitive.

## Final P14 package retest

The P14 package `8e4a9c29-e13d-4bd3-979a-4e0ed94e1d52` passed Full selected
gates against the retained P2 pre-replacement WOW baseline
(`final-audit/full-r2.process`, exit 0). These include Win16 startup frontiers,
the Console and Window command matrices, GUI and protocol paths, lifecycle,
frontend loss and nested handoff. The ten package images matched the build
inputs. DOS4GW/NESticle was skipped because the external fixtures were not
supplied. The first attempt could not allocate the required short physical
test path under sandbox restrictions; the supervised retry used its owned
short-path stage and completed with cleanup.

The MSVC payload-only test and the actual installer missing-DLL test each
passed all four x86/x64 caller-target pairs on retest. Product link-map checks
again rejected Detours; the identity audit confirmed 22 byte-exact imported
files and the one documented local helper fix; and all 971 selected compiler
commands were C-only. These checks do not prove every application's behavior,
Win16 editing or save, or general late-hook new-thread and removal safety.
