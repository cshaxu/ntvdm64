# S11 initial foundation candidates

**Scope and version warning:** This is a chronological research record. Its
opening describes an older candidate, not the current deliverable. Following
the owner's boundary-audit clarification, DIV-467/468/469/470/471 internal
repair diffs were removed from the active runtime and held under
`s11-held-cpu/`; their historical tests below do not certify current source.
See the later boundary-only and atomic-retirement evidence for the current
candidate. No 486-internal repair is included by virtue of this record.

These worktree changes are candidates, not delivered fixes. The x86 and x64
components were linked through their selected cached graphs after the recorded
Ninja wait failures. The coherent ten-image candidate passed all 14 selected
[Full gates](s11-foundation-evidence/full-timings.json); this does not resolve
the separately reproduced SOL Options failure or all S11 behavior boundaries.
The first [build summary](s11-foundation-evidence/build-summary.txt) contains
an earlier NTVDM hash. The later x86-other build produced a different NTVDM;
[final inputs](s11-foundation-evidence/final-build-inputs.json) and the final
package manifest bind that later binary, not the earlier hash.

* The CCPU register-index memory bit macros now calculate signed 16/32-bit
  element displacements using defined arithmetic and the existing address-size
  wrapping helper. The immediate and register-only paths remain distinct.
  Original-source exception MVDM-HOST-DIV-469 is registered. The independent
  [formula review](s11-bit-index-independent-review.md) is not an execution
  test. Ten selected-decoder cases are prepared in the existing bounded CCPU
  fixture and [runner](../../tests/observation/verify-ccpu-bit-matrix.py),
  including BT/BTS/BTR/BTC and a 32-bit address/data case. The ten
  [selected-object cases](s11-foundation-evidence/bit-matrix-final-result.json)
  passed after relinking against the final libraries; permissions, fault
  atomicity and guest execution remain separate. Three additional
  [protected-mode cases](s11-foundation-evidence/bit-fault-final-result.json) pass:
  read-only BT succeeds, while BTC, BTR and BTS fault on a read-only page and
  BTC also faults on an absent page, with
  the expected error, fault address, unchanged carry and unchanged backing
  data. They use a test exception hook and do not prove guest fault dispatch,
  segment-boundary ordering or every instruction form.
  An independent x86 hardware oracle did pass negative word BTC, BTR and BTS
  with process exit 0; its source SHA-256 is
  `79A373BA41E224C635F3CDFF493C283FC3DC6B4F8906AD0D7E0E472ED3DFDC87`
  and executable SHA-256 is
  `998E9A0874D3B0EB5A5D92773499EB9D81A893B22B3D68A0166D411F665C42FA`.
  This native comparison is not a passing CCPU or guest test.
* The far-transfer candidate removes the extra descriptor reads and translation
  that could raise a page fault before original validation. Its two now-unused
  project-owned diagnostics and their observer checks are removed; the
  original-value-only validation witness remains. DIV-306 and DIV-308 were
  updated. A selected-library [fixture](../../tests/mvdm-host/ccpu_far_exception_fixture.c)
  and [runner](../../tests/observation/verify-ccpu-far-exception.py) now
  assert the observed fault and trace-mode controls. The four
  [selected-object cases](s11-foundation-evidence/far-exception-final-result.json)
  passed after the fixture was corrected and relinked against final libraries.
  Broader fault-order and guest tests remain required.

  After shared fixture seam objects were rebuilt for the current bit-index
  audit, Ninja marked the far-exception executable out of date. It was
  rebuilt and the existing four trace-off, matching, other-selector and valid
  controls passed again (`build/t473-s11/far-exception-current-audit`). The
  runner checks the linked far-transfer and exception-hook owners and records
  current hashes. Existing `/FORCE` and duplicate-symbol fixture-link
  warnings remain a limitation; this is not a guest exception-delivery test.
* The project-owned WOW entry bridge generator now declares the original
  one-byte `BOOLEAN` return. A new selected-object regression fixture is
  [passes](s11-foundation-evidence/dll-entry-result.json) on the rebuilt bridge.
  A real Windows loader fixture with a controlled provider also passed; it
  does not verify a real WOW guest DLL lifecycle. The baseline bridge also
  passed that loader fixture, so it does not establish a real loader failure.

The NIDDB candidates remain separate: the [VMM retirement review](niddb-vmm-retirement-contract.md)
and [failure review](host-ch-scope/niddb-allocation-rollback-review.md) explain
why live VM identity and failed initialization still need evidence. Do not
infer that the separately observed intermittent SOL Options failure was caused
by NIDDB. The rebuilt selected `virtual.obj` passes the existing 15-check
[object contract](s11-foundation-evidence/niddb-object-result.json), which
does not cover the outstanding stale-ID or no-retry failure boundaries. Guest
binaries remain unchanged.

Mirror diff hygiene: sas.h had been rewritten with CRLF while the repository baseline uses LF, inflating a comment-only change to 1,089 changed lines on each side. Restoring the baseline LF byte convention leaves the intended DIV-092 comment update at 4 additions and 5 deletions; git diff --check passed. This changes no C tokens or product behavior and does not replace the coherent-build acceptance.

The bit matrix now includes nine signed 16-bit word-index boundaries (-32768, -17, -16, -15, -1, 0, 15, 16, 32767). Independent hardware BT and the selected original CCPU execution both passed all nine with target and neighboring storage checks; the existing 10 cases also passed in the same runner. The controlled fixture used actual cached CCPU objects and an independently linked test executable, not the pending coherent product build. Evidence and hashes are in [bit-word-boundaries](s11-foundation-evidence/bit-word-boundaries/result.json). These tests do not cover every instruction, dword extreme, segment fault or guest execution.

The signed dword boundary matrix now adds INT32_MIN, -33, -32, -31, -1, 0, 31, 32 and INT32_MAX. The hardware oracle reserves virtual address space and commits only the pages needed to verify real x86 BT displacement and neighboring values; the selected CCPU fixture independently checks the same signed bit index with 16-bit effective-address wrapping. All nine hardware cases and all 28 selected-object cases passed; [result and input hashes](s11-foundation-evidence/bit-dword-boundaries/result.json). This does not cover full 32-bit effective-address extremes, modification forms at these indices, segment-boundary faults or a full guest run. A first ad-hoc oracle had an uncommitted preceding guard page and was stopped after identifying that fixture bug; only the corrected oracle is counted.

A failure-control comparison rebuilt c_main from the same source using the committed HEAD c_oprnd.h in an isolated include directory, substituted that object into a separate CCPU archive and linked the same 18 new cases. The first attempt accidentally used the worktree header and is explicitly invalid (result.json); compiler include output exposed it. The corrected build2.log confirms the isolated HEAD header, and result2.json shows 7/18 cases failing (word indices -32768, -17, -16, -15, -1; dword indices INT32_MIN, -32), with 11 controls passing. The current candidate passed all 18 in the separate 28-case matrix. This is a controlled selected-object counterexample and does not prove every instruction or guest path. Evidence in [baseline comparison](s11-foundation-evidence/bit-baseline-comparison/result2.json).

The selected CCPU fixture now extends those nine word and nine dword index boundaries from BT to BTS, BTR and BTC, with carry, modified target and neighboring byte checks. All 82 existing and new selected-object cases passed. The independent x86 hardware oracle separately passed all four operations for each of the 18 boundaries, checking carry, target and neighboring words; the same 54 new modification cases linked against the isolated committed-HEAD decoder failed 33 and passed 21. Result, baseline comparison, hardware output and input hashes are in [bit-family boundaries](s11-foundation-evidence/bit-family-boundaries/result.json). These comparisons use the selected CCPU fixture's 16-bit address wrapping and native hardware's normal linear addressing respectively; they do not establish full 32-bit addressing, all fault ordering, or guest execution. The direct link retains the fixture's existing `/force:multiple` and is not a new product build.

The formal matrix now also covers the nine signed dword boundaries for all four operations using a 32-bit address-size prefix. Each case derives a 32-bit base that puts the final operand at mapped address 4000h, including modulo-32-bit base/displacement arithmetic, and verifies carry, target and adjacent bytes. All 118 selected-object cases and the independent 72-case hardware bit-family oracle passed through the formal runner. The same 36 new address-size cases with the isolated committed-HEAD decoder produced 17 non-passes and 19 passes. See [32-bit addressing evidence](s11-foundation-evidence/bit-family32-boundaries/result.json), baseline result and hashes. This covers these selected index/base pairs in the controlled CCPU harness, not every possible 32-bit address, hardware segment setup, fault order or real guest execution. The temporary formal tree contains copies of the tested executable, map, native oracle and exact selected CCPU object; it is not a product rebuild.

A new controlled cross-page BTC probe found a distinct original CCPU boundary: a dword operand beginning at page offset 0xffe that faults on its second page reports CR2 at the second page plus one byte, while a native x86 BTC witness reports the first byte of the second page. For both write-protected and absent second pages the selected CCPU reports 0x7ffe1001, while hardware's Windows exception record reports offset 0 from the second-page start; the initial page's checked bytes remain unchanged. Original `c_page.c::usr_chk_dword` prechecks `lin_addr + 3` on a crossing and `c_tlb.c` records that checked address, explaining the observed +1. The same source pattern exists in supervisor dword checking. The committed mirror has no local diff in c_page.c. Code, actual results and hashes are in `s11-foundation-evidence/bit-cross-page-fault`. The CCPU negative results use the hardware first-byte expectation; the existing five original cases and cross-page read-only/first-page-protection controls passed. This is a controlled x86 host comparison, not proof of a particular original 386/486 processor revision, a guest regression, or a safe generic repair. Do not silently accept the two negative results or alter common paging without auditing all consumers and original behavior.

The cross-page counterexample is now expanded across dword starting offsets ffd, ffe and fff, with both read-only and absent second pages. Native x86 reported the second-page start for all six cases; selected CCPU reported +0 for ffd, +1 for ffe and +2 for fff, exactly matching original `usr_chk_dword` prechecking the last byte (`lin_addr+3`). The six controlled CCPU cases retained the expected fault type/flags and unchanged tested backing, but four intentionally report failure against the native address expectation. The extra cases and exact binaries/source hashes are in `s11-foundation-evidence/bit-cross-page-offsets`. This narrows the root cause to an original generic paging policy, not the signed-bit-index candidate. Hardware evidence is from the current host and Windows exception reporting; it is not an original 386/486 trace or authority to modify all paging consumers without broader tests.

The 80386 Programmer's Reference Manual, section 9.8.14, describes CR2 as the linear address used for the access causing #PF (Intel document reproduced at https://www.ardent-tool.com/CPU/docs/Intel/386/manuals/prref386/s09_08.htm). The current Intel system-programming manual likewise describes CR2 as the linear address generating the exception (https://cdrdv2-public.intel.com/874249/253668-090-sdm-vol-3a.pdf). Neither cited wording explicitly specifies which byte to report for a multi-byte access split across two pages. Thus the current host's first-byte observation is useful evidence but, alone, is not proof that an original 386 must report that same byte rather than the tested failing last byte; both are within the inaccessible page.

The generic source impact is broader than BTC: `usr_chk_dword` is used by normal dword operand decoders, multiword/far operands, indexed bit operations and user stack checks in `c_stack.c`; `spr_chk_dword` is used for supervisor stack checking. Both original checks perform the same end-byte second-page translation. This makes an unqualified adjustment a global exception-address change rather than a localized bit-index repair. No guest-dependent regression has yet been observed. Preserve the original paging implementation while this is only an architecture observation; the selected CCPU bit-index fixes remain independently testable.

The normal selected-CCPU bit-fault fixture and its observation runner now include 13 cases, including the six split-page offset/protection cases and cross-page read/first-page write-protection controls. The tests explicitly assert the existing original last-byte precheck policy, while the separate hardware comparison remains recorded as a difference rather than a passing hardware-equivalence claim. The formal runner passed all 13 with its decoder and exception-hook map checks (`bit-cross-page-offsets/formal-result.json`). No original paging code or guest bytes changed; this selected-object result is not a full guest exception-delivery test.

## S11 window retirement synchronization â€” worktree integration

This extends the existing ADAPTER-WOW-027/063 ownership seam in
src/wow32-dll/source/wow_window_words_binding.c. Original createw.c finalization
ordering and original control/dialog cleanup remain the behavior owners.
Direct NT4 USER locking cannot compose without the unavailable kernel USER
object/desktop engine. The smallest admitted second-rung adapter is the
existing worker USER runtime lock and original-task callback scope, not a new
scheduler or lock manager. No third-rung mirror change or fourth-rung window
behavior implementation is introduced. Existing scope-finally restoration and
native-return fail-fast handle impossible live-owner abandonment; pending
withdrawal keeps the existing collector. Pure property metadata and completed
retirement do not acquire guest execution. All key states remain in the
existing binding, object table and page-domain authorities.

The prior publication lock alone allowed retirement during another task's
locked graph transaction. The repaired entry obtains shared runtime/execution
ownership for copied-state work, releases data locks across original finalizer
callouts, and unwinds locks and execution in one finally. The complete local
evidence, negative controls, and current limitations are linked from the
[performance investigation](s11-dialog-performance/investigation.md) and its
[central failure matrix](s11-dialog-performance/terminal-central-results.json).
This group adds zero mirror diff and no guest bytes; it remains unpublished
until current-source coherent-package and required regression acceptance.

The explicit CBT creation path now acquires the existing original-task callback
scope for TIF_16BIT producers before touching CCPU-backed creation state.
The enclosing USER lock is reacquired for enrollment after callback_enter
releases it; finally releases that acquisition and restores the original task
scope and caller lock before downstream CallNextHookEx. Resource-only native
creation keeps its existing path. This extends ADAPTER-WOW-031/063 without
changing original control algorithms, scheduler policy, mirror code or guest.
Current isolated candidate passes nested create, rejection, CBT veto, table
cleanup and injected unwind (TLS/hooks/references/restoration/retry), with
real-task flags now in the formal native-creation fixture. These selected-object
cases are controlled owners, not a proof of every cross-task interleaving.
Current-source rebuild, registered parked-task regression and live sibling
acceptance remain required. Prior SOL recovery failure and severe latency
remain open; integration does not authorize publication or S11 closure.

## S11 transaction-local graph reuse â€” worktree integration

The project-owned wow_user_borrow_scope adapter now indexes retained samples
only during one synchronous client-graph traversal. The existing scope list
remains the lifetime owner. A relation can reuse a full view only after the
same backing/binding was validated in this traversal; a collision clears that
validation, and all misses preserve the existing full-read/failure path.
No persistent cache, HWND identity authority, skipped native-return boundary,
original behavior replacement, mirror diff or guest change is introduced.
This is an implementation refinement of the existing copied USER view seam.

The same-nine-component-input edge/baseline/baseline/edge comparison completed
eight SOL modal rounds and reduced observed average show/recovery time by about
22%; both remained seconds-long. The existing isolated collision/failure
controls and the newly added nested-publication test support the bounded
change. Formal current-source graph/window/nested/creation regressions are
running; coherent product and cross-task tests must be rebound after integration.
Neither this optimization nor the pending S11 foundation group is published.

## S11 exact captured relation identities â€” unpublished candidate

The output-only relation_backing[4] in the project-owned observation record is
derived from the pinned WND in copy_window_observation. It is not a guest field,
resource owner, native pointer, persistent cache or input to graph projection.
The borrowed-client graph can avoid repeated pins only for a target already
scope-owned and fully validated in that single traversal; misses and the
standalone relation API remain unchanged. This extends the existing copied
view adapter, with zero additional mirror diff and no guest change.

Four formal cases, explicit retired/reused-slot backing checks and the clean-
parent sixteen-round sibling probe pass. SOL round 2 remains a non-pass;
the candidate is unpublished. See the performance investigation and
captured-relation-current-results.json for the source/package binding and
runner failures. Explicit inner Ninja link targets are required after fixture
object updates; an outer 'no work' report alone is not image freshness proof.

Final disposition of the exact captured relation candidate: reverted together
with the subsequent owned-target read experiment under the owner's optimization
criterion. Both added code without demonstrated speedup. No new observation
layout or associated branch remains. Their passing fixtures are retained as
historical evidence; compatible relation-identity and scalar-fallback regression
assertions remain on the existing interface.


The selected WOW DLL entry bridge was linked into a test DLL with a controlled provider that deliberately leaves nonzero upper EAX bits on a BOOLEAN return. A real Windows loader test observed failed process attach for false, successful attach for true, and both cleanup callbacks on explicit unload. The direct loader executable returned zero with marker `WOW_BRIDGE_OS_LOADER attach_false=failed attach_true=loaded unload=1 bitmap=1 registry=1`. This is a real host loader witness but uses a controlled provider, not a WOW guest DLL lifecycle or a coherent product package. The updated selected-bridge runner completed successfully with both the original ABI fixture and this OS-loader fixture (`build/t473-s11/bridge-loader-formal-r3`); its receipts bind the selected object and test binaries. The initial apparent wait was oversized PowerShell JSON serialization, corrected by reading plain strings. This is not a Full product gate. Exact inputs:
- `tests/mvdm-host/wow_dll_loader_provider.c`: SHA-256 `4e36a08aa26ff21d2b2da8d82de8eabe0d91f0d9b0516b52c2528df97c6287a2`
- `tests/mvdm-host/wow_dll_loader_fixture.c`: SHA-256 `249cc6ebd266159b3af9535374798d8b0216d3b72d15cb2436ed297619faef3a`
- `build/.ninja/msvc-x86-release/wow32/obj/wow32-dll-entry-bridge.obj`: SHA-256 `0ff8eb7e3e78e5ccefc44d9e994d3b0db811b4dfc64c0797084467b46e9b6135`
- `build/t473-s11/bridge-os-loader/wow-bridge-test.dll`: SHA-256 `e600a38077304f313178370b2887b7c66f9a1e1f3ca663561e92e445e32bb0f5`
- `build/t473-s11/bridge-os-loader/wow-bridge-loader.exe`: SHA-256 `0f48622c7d66314dfe715484a2f0fe7cc6e61ab5519e692e6484ece4e247ad96`

A real-loader negative control qualifies that result: a bridge generated from the committed declaration (`int` return), linked with the same poisoned-upper-EAX controlled provider, also passed the one-cycle Windows LoadLibrary/FreeLibrary fixture. The loader observation therefore does not establish that the original host loader would misinterpret the wider return. In a separate direct-call control of that same baseline bridge object, the existing ABI fixture printed `attach=a55a0001 false_attach=a55a0000 thread_false=a55a0000 detach_false=a55a0000 cleanups=2` and returned zero only under its explicitly expected legacy-wide-return mode. This proves the C bridge propagated the upper bits at that direct boundary, not that a real Win16 guest loader suffered a failure. Artifacts are under `build/t473-s11/bridge-old-return-control`; the attempted loader-negative expectation returned nonzero because the old loader case passed, and is not counted as a negative witness.
- `build/t473-s11/bridge-old-return-control/bridge-old.c`: SHA-256 `35595ff8be827cc2e8823d6a5c662812318eca6c3298d700697b8ef5888149a8`
- `build/t473-s11/bridge-old-return-control/bridge-old.obj`: SHA-256 `5bad02b67035881cf2a7639f7d67400593a5ac06fa2c33adbc27308b985978f7`
- `build/t473-s11/bridge-old-return-control/wow-bridge-old.dll`: SHA-256 `b6abe8d4b992c833023b5e8f57ec58179f7af83da47cac26752d4c2c9850d650`
- `build/t473-s11/bridge-old-return-control/bridge-direct-old.exe`: SHA-256 `961182da5a339403098d7ee322b47e8a2332af27110c077daff828a217141331`

The selected real `wow32.obj` was also checked against the bridge declaration. Its actual build graph uses `/Gz`, matching the bridge stdcall symbol `_W32DllInitialize@12`; the original source declares `BOOLEAN`. In this compiled object, the false return path at function offset 0x2b5 executes `xor al, al`, while the success path at 0x2e8 executes `mov al, 1`, followed by the compiler security-cookie call and return. These instructions themselves do not clear EAX high bits, so callers cannot infer a 32-bit C return from the one-byte contract. This disassembly does not establish the actual high bits after the subsequent helper call or prove a live guest failure. Selected object SHA-256: `b7b57867ff563557c7e2c3d7435cf451356b3164ea5a16f62612b05e9f63ccc2`.

The expanded eight-cycle runner `bridge-loader-formal-r4` made no progress after its compile output for ten minutes. Read-only process inspection showed its PowerShell wrapper had no compiler or test children; only the verified owned wrapper was stopped. The run is a non-pass. A modern-pwsh rerun `bridge-loader-formal-r5` was started separately and remains pending; it must not be inferred successful from an earlier one-cycle result. The checked-in candidate runner has since replaced its `Start-Process -Wait` compiler and loader invocations with direct child execution; this later edit is not covered by r5 and requires its own completed run after r5 terminates.

The revised direct-child runner completed in separate `bridge-loader-formal-r6`: both the existing direct BOOLEAN bridge check and eight successive false-attach/true-attach/unload cycles passed. Exact object and fixture hashes and outputs are retained in `s11-foundation-evidence/wow-bridge-loader`. This is a controlled provider and host loader check, not real guest behavior or product Full. The earlier r5 process remains separately owned and is not a passing result.

The earlier r5 loader wrapper also exceeded the 10-minute no-progress budget after compiler output; read-only process inspection confirmed that it had no compiler or test children. Only that identified owned wrapper was stopped, and its exit was nonzero. It remains a non-pass; the separate direct-child r6 result is the validated eight-cycle run.

## Selected CCPU x87 segment-span correction

An actual selected CCPU protected-mode fixture executes FNSAVE and FRSTOR
with 16- and 32-bit operand sizes under exact and one-byte-short data-segment
limits. With the original selected checks, 16-bit exact 94-byte operands
raise #GP at instruction IP 8002h, while 32-bit 107-byte limits permit the
108-byte operation. The selected decoder derives operand size from the 66h
prefix; FNSAVE short-fault cases also verify the output buffer was unchanged.
The baseline is a separately compiled original `zfrsrvd` decoder with only the
precheck constants reverted, and its build log confirms the isolated header;
the earlier attempt replaced `c_main` instead and was invalid, not a negative
witness. The baseline failed four of eight expected cases. The two-line
DIV-470 candidate checks 94 and 108 bytes respectively, and all eight cases
passed with the expected #GP/no-fault results. [Comparison, selected owner,
input hashes and formal result](s11-foundation-evidence/x87-segment-span/) are
retained. This is a host-selected execution witness with an exception-hook
seam, not guest exception-delivery, page fault, external hardware or complete
x87-state proof. No guest bytes changed; the candidate remains unpublished.

## Selected CCPU x87 cross-page correction

`usr_chk_word` validates only the starting word, while original NPX byte
transfers can span 8, 10, 14/28 or 94/108 bytes. When the start word is on
one page and the rest crosses into another, the old direct-physical path can
skip the second page translation and permission check. The selected real CCPU
witness uses discontiguous physical pages, a guarded neighboring page, absent
or read-only second pages, and actual read/write x87 instructions. The
source-derived baseline (same segment-span correction, without DIV-471)
passed 12/48 and failed 36/48; the candidate passed 48/48. The cases assert
correct page-fault access/error, no first-page writes on fault, unchanged
physical guard, and mapped second-page writes; FLD m64 additionally stores
its value back and verifies it came from the mapped second physical page.

DIV-471 checks the final byte with the original user read/write permissions
when paging crosses a boundary, and uses the existing virtual-byte path for
non-contiguous pages. Segment-limit, bit-index and paging controls also pass.
[Results, baseline include proof and hashes](s11-foundation-evidence/x87-cross-page/)
are retained. The exception hook is a host seam; actual guest fault delivery,
concurrent page-table mutation, all x87 modes and coherent product regression
remain unproved. No guest bytes changed, and the candidate is unpublished.

The selected CCPU cross-page fixture now also covers a supervisor-only first
page for all four operand families, both directions and operand-size variants.
All 64 selected cases pass; this strengthens first-page CPL=3 permission and
fault-order evidence, not concurrent page-table behavior. The earlier 48-case
baseline comparison remains bound to its earlier fixture and is not a 64-case
baseline run.

Read-only source comparison on 2026-10-29 found the adjacent SoftPC project's
`src/app-softpc/softpc.new/base/ccpu386/c_oprnd.h` still contains the original
94/106 segment precheck and word-only checks for the same NPX sizes. Its
selected CMake lists `fpu.c`, `zfrsrvd.c` and `c_page.c`; its page helpers
retain the analogous direct-physical byte path. This establishes the same
source-level risk in its selected composition, not a reproduced SoftPC runtime
failure. No SoftPC file or 486-specific implementation was modified.

A coherent candidate containing both x87 fixes passed the existing
Console-17 and Window-17 command matrices in a fresh isolated package. Its
[manifest and receipt hashes](s11-foundation-evidence/x87-cross-page/product-matrix/)
bind the evidence to that package. These normal product paths do not exercise
the special x87 segment/page counterexamples and do not establish that any
user application encountered them. Full product and guest x87 exception
acceptance remain separate.

The owner subsequently clarified that this T audits and restores component
boundaries, not speculative internal defects in already-connected components.
DIV-469/470/471 are therefore held outside S11 boundary-recovery work. Their
working-tree changes, tests and evidence are preserved pending disposition;
a package containing them must not be presented as an S11 boundary-only
release. The already-running Control suite on that package completed and
passed; this is retained solely as candidate evidence and does not make the
held changes part of the authorized recovery scope. Continue with actual
missing original connections and their required closure.

DIV-469/470/471 were subsequently separated from the active runtime after
the owner clarified this T's boundary scope. Their complete existing diff and
source/baseline hashes are preserved in `s11-held-cpu/c-oprnd-held.patch` and
`c-oprnd-held.sha256.json`; `c_oprnd.h` was restored to its prior checked HEAD
bytes after confirming that its entire worktree diff contained these held
changes. The selected NTVDM and WOW32 cached targets rebuilt successfully,
and the actual rebuilt NTVDM export ordinal check passed. The prior x87
package/results remain historical candidate evidence and cannot certify this
new runtime. Unrelated worktree changes were retained.

Under the owner's boundary-only clarification, the two NIDDB changes were
reassessed: both amend the internals of an already-selected original
`virtual.c`, while actual VxD control-sequence reachability remains unproved.
DIV-467/468 are saved as `s11-held-cpu/niddb-held.patch` with source and patch
hashes and removed from the active runtime without dropping the separate VxD
connection question. NTVDM and WOW32 rebuilt; the new NTVDM's 271 export
ordinal checks and current mirror coverage checks passed. Previously tested
boundary packages still contain these now-held NIDDB changes and therefore do
not certify the just-rebuilt runtime. A new coherent package and regressions
are required; no product was published.

After separating both CCPU and NIDDB internal-fix patches, a fresh coherent
candidate was staged from the current rebuilt NTVDM/WOW32 and the previously
selected matching native inputs. Its package passed the real-worker
name/ordinal memory-export VDD witness and the hash-verified Win16 shared
idle/common-dialog test, including both task retirements. The [package and
receipts](s11-foundation-evidence/reconciled-boundary/) are retained. This is
not yet Control or Full acceptance on this exact package; previous boundary
package Control is not reused as proof because its NTVDM included NIDDB.

The remaining active S11 runtime changes were reclassified by ownership after
the boundary-scope clarification: the WOW native idle-forwarding, shared idle
intent, CBT execution scope and window-retirement changes are adapter
callback/identity/lifetime boundaries, with observed 5023 or controlled
ownership witnesses. The class/window and transaction-local graph indexes are
project-owned performance work explicitly requested for the multi-window path;
they do not replace original guest behavior. The WOW DLL-entry BOOLEAN change
is an adapter ABI declaration. The original-template export recovery restores
the missing selected provider interface. `c_xfer.c` removes project-added
diagnostic reads that could precede original descriptor validation, rather
than changing the original algorithm; `sas.h` is an explanatory comment-only
correction. These classifications do not prove the intermittent 1400's cause
or authorize release without the remaining behavior and package gates.

The reconciled boundary-only candidate (with both held CPU and NIDDB changes
removed) subsequently passed the complete selected Control suite, including
its 271-entry PE export check, RPC, GUI, version-negative, terminal handoff,
relaunch and frontend-loss tests. [Runtime manifest and timings](s11-foundation-evidence/reconciled-boundary/control/)
record the exact tested package. This remains distinct from the Full product
suite, the unresolved intermittent WOW failure and the final release gate.

The current exact-source native-wait publication control passed no-debt,
deferred continuation, once-only discharge, failure/exception retention and
last-error assertions. [Output and source hashes](s11-foundation-evidence/reconciled-boundary/return-unwind/)
are retained. It substitutes lower services and is not proof of actual guest
scheduling or the separate intermittent Winmine native-return failure.

Reconciled boundary candidate: Console-17 and Window-17 command matrices passed on the staged package; summaries, manifest and hashes are in [product-matrix](s11-foundation-evidence/reconciled-boundary/product-matrix/). These do not replace Full or establish the cause of the separate WOW 1400 failure.

The reconciled candidate also passed three visible-desktop SOL Options open/close/recovery rounds and launcher completion. [Probe output, input hashes and package manifest](s11-foundation-evidence/reconciled-boundary/sol-options/) identify this run. It does not exercise Winmine or prove the cause of its separate 1400 failure.

The selected-object dialog-lifecycle fixture was rebuilt and passed with the current provider/worker objects: original CCPU modal scope, nested dialog, callback rejection, unwind, resource and terminal retirement markers report zero errors. [Logs and hashes](s11-foundation-evidence/reconciled-boundary/dialog-lifecycle/) retain its controlled boundaries. The WOW32 unsealed image hash matches the staged package's input; the sealed package image differs due to package identity. This is not an actual guest failure-injection or complete reentrancy proof.

The reconciled package also passed isolated WRITE normal close, restart and second close, plus SOL stock draw, Deal, second draw and normal close. [WRITE](s11-foundation-evidence/reconciled-boundary/write-lifecycle/) and [SOL](s11-foundation-evidence/reconciled-boundary/sol-gameplay/) results include actual staged input hashes. These cover the observed operations only, not Winmine or the complete Full suite.

A current hash recheck confirms the cached selected `obj/ccpu/c_xfer.obj` still has SHA-256 E0A4B8045C71155D474632F60AAB6DBCFDD0A29B703BE6A357E591CCE8DF9421, matching the earlier `build/t473-s11/far-exception-current-audit/result.json` fixture receipt. This confirms unchanged object identity for those four controlled fault-order cases; it does not independently bind that object to a released image or prove guest exception dispatch.

The current cached selected far-transfer fixture was run again after the candidate identity recheck. All four fault-order and trace-filter cases passed; the [receipt](s11-foundation-evidence/atomic-task-retire/execution-check/far-exception-recheck/result.json) records selected object and executable hashes. This exercises the same controlled fixture, not real guest fault delivery or the entire product gate.

## Current bridge identity cross-check

The cached selected `wow32-dll-entry-bridge.obj` and generated C still match the controlled BOOLEAN fixture's recorded hashes (`0FF8EB7E3E78E5CCEFC44D9E994D3B0DB811B4DFC64C0797084467B46E9B6135` and `740A49A2B0625AAC12CD8FE6B5473AB35F55433767034ACACD4921E651F1DA73`). The current WOW32 map SHA-256 `9F11D6B0DC87416D0CD10E7AEDD6003DDA80EE33F74AA220C7EC37FCF80E995E` attributes `_DllMain@12` to that bridge object. The unsealed WOW32 SHA-256 `098790BCEEC9DDB0376ABC4086463280D7A936463AAC8FE18636AE45DFD04A5C` matches the input hash of the tested, sealed `atomic-execution-retire-package` WOW32; its sealed hash is `D4C9E632F4963EBE8EA62D48FCB18539804AB24BD17C8415816EE7E93A80E46E`. The staged candidate ran the Notepad, Program Manager and Calculator witnesses. This ties the controlled bridge build to those observed package inputs; it does not observe loader callback arguments or prove real provider failure, detach or all guest DLL lifecycle semantics.
