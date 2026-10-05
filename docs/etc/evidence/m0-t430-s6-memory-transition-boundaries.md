# T430 S6 memory and transition boundaries

Sequential admission after S5 a143894f1. Reconcile current selected inputs
with existing non-WOW DPMI/XMS/EMS/IRQ/lease proof, then supplement only named
unproved boundaries. The lease correction is delivered as526f73c1c; the
subsequent boundary supplement changes tests only. S6 closure is recorded below.

The owner provenance gate remains controlling: immutable guest defects stay;
original host defects adopt only an existing matching SoftPC repair; otherwise
TODO and preserve. Demonstrated project adaptation defects are corrected,
regardless of whether a hook resides in a mirror. WOW gaps retain their
existing queued owners.

## Initial selected cases

- Invalid DOSX IDT: current pm_entry_contract_fixture and actual PM provider
  ordering; distinguish controlled dependency proof from CPU/guest execution.
- PM-stack/exception return: actual descriptor/CCPU profile and retained
  instruction/guest witnesses; source identity before reuse.
- EMS AH56/BOP68: existing ems_mapping_probe and original map/return owner;
  prove guest stack/mapping restoration, not only an EMS installation marker.
- XMS/lease: existing memory and guest_memory_lease tests; failure/data
  preservation and teardown ownership, not a recreated DOS allocator.
- IRQ sentinel/concurrent notification: actual selected notification adapter;
  no relocation of original scheduler or fabricated guest interrupt success.

## Project lease isolation correction — delivered526f73c1c

`src/ntvdm-exe/session/guest_memory_lease.c` is project-owned adaptation,
not an original OpenNT/MVDM algorithm. Its release checked active/epoch but
not the pointer's ownership. Two active contexts at the same epoch could
release each other's lease: the wrong provider received the write and the
other context's bounce was freed. This is a controlled API-boundary defect;
no real guest cross-context misuse or observed product crash is claimed.

The fix checks pointer equality against the context's eight owned slots,
before dereferencing the supplied lease. It adds no token, global registry,
protocol or scheduler. Existing commit/rollback semantics remain. Callers
still must not retain a released lease pointer across slot reuse or epochs;
this is not a concurrent lease API or stale-pointer generation scheme.

`tests/session/guest_memory_lease_boundary_test.c` fails before the repair
at line49/check5 (`build/M0-T430/S6/r001-lease-before/lease.exe`). Afterward,
45 assertions pass: matching epochs, cross-context rejection with both
providers unchanged, valid owner commit, forged/inaccessible pointers,
range/access rejection, read/write failure, read-only no commit, exhausted
slots, rollback/reuse, teardown/no commit, repeated end and epoch rollover.
The existing session-level lease test also passes unchanged.

Reproduce using
`tests/observation/verify-guest-memory-lease.ps1 -BuildRoot build/M0-T430/S6/<fresh>`.
`r006-lease-replay` retains build/result/source+binary identity for both tests.
`r005-lease-replay` is a failed runner quoting attempt, not a product failure.

Formal `product-programs` x86 /MT rebuild passes in the selected
`build/M0-T427/S2/r001` cache. Current actual-provider
`original-external-memory-test.exe` exits0: original EMS cross-window lease
copy, SAS reverse pages, allocation resize preservation, GDI aliases and
mapping removal pass. These are provider fixtures, not a guest AH56/BOP68
stack-restoration witness.

Fresh PM entry extraction at `r003-pm-entry` passes65,538 cases, explicitly
mock-boundary, not CPU execution. modesw SHA256 is
4BBFF6CB6021A4839B6616169529BCCA76510F98B82CA9665F03637E4654ED22.
Actual `cpu40-descriptor-domain-fixture.exe` exits0 (including absent/short
DOSX IDT); actual CCPU stack transition passes36 cases, thread lifecycle
normal/abnormal passes, and HALT/reset/debug/fault fixture exits0. The build
retains existing controlled fixture seam overrides; no original algorithm
replacement is introduced.

Candidate `r007-runtime` contains all eight formal outputs; WOW32 lives under
the cache's `wow32/` directory, not its top level. The first copy attempt
reported that path error; the retained candidate WOW32 already matches the
formal BC7906DEB736524D472270D03EBDCBC1F085F09BCCF756F8BD057EB4AE866102
hash. Do not treat the copy warning as a missing DLL or silently suppress it.
Product gate `r008-product` passes Console17/Window17 and three independent
WOW frontiers against S4, total197,908ms (WOW67,477; Console62,808;
Window61,620). `r009-xms/x86` rebuilds the original XMS/suballocator and
project lease provider: both no-bounce/no-zero reuse and bounded move/cancel
markers pass. This is controlled provider evidence, not real guest exhaustion.

`r010-publication` preserves S4 recovery and confirms all eight published
hashes match `r007-runtime`. Only NTVDM changes, to
C977A61D68F9C0F7D5744EBA95471B2E6FE764E33A8F47DA063BDD8E1BAC4788.
`r011-published-smoke` fails before launch because its ad-hoc command resolved
Z before subst. `r012-published-smoke` retains a native-zero watchdog failure:
observer inherited53x15 rather than the matrix's explicit disposable Console
geometry, input-ready=no. No product-failure causality is asserted or waived.
Using the matrix's SHORT_HISTORY setup and ordinary frontend in
`r013-published-smoke`, actual published Console/Window native-zero and MEM
pass their exit/output assertions. All owned processes, environment and Z
are cleaned. The failed setup/result remains distinct from passing evidence.

`r014-handoff` passes native exit23 but retains nested Console timeout under
an inherited53-column/14-row viewport. `r015-handoff` uses the product
matrix's explicit disposable Console configuration: native, nested Console
and nested Window all pass actual input, DOS/MEM output, parent recovery and
exit23, with unchanged assertions/deadlines. This preserves the S4 deepest
frontier under declared comparable conditions; arbitrary tiny-host geometry
is not newly certified. The probe configuration dependency is explicit,
not retries until success. Governance, relative links and diff checks pass.

This bounded lease correction is the S6 P1 production delivery, not S6
closure. Source review confirms only a pre-dereference bounded ownership
check changes production behavior; no API layout, mirror, original allocator,
worker lifecycle or shared transport changes. Other-session proposal/TODO
edits remain excluded.

## Selected boundary supplement and closure

The original owner remains in place. No production file changes after the
lease delivery. Current c_main's DIV214 atomic notification adapter and
DIV221 negative INTACK guard are project additions, not newly discovered
original defects. Original PIC/CCPU, DOSX, HIMEM, EMS driver and allocator
algorithms are not rewritten. No new SoftPC import is warranted by this proof.

| Obligation | Current proof and actual boundary |
| --- | --- |
| Invalid DOSX IDT/no partial mutation | Fresh PM-entry extraction65,538 controlled cases and actual descriptor fixture above; not a real invalid-media DOSX launch. Original DOSX is immutable. |
| PM-stack/exception return | Actual original CCPU stack36 cases, descriptor/debug/task-switch fixture, plus fresh ordinary guest16/32 and CODE32 return witnesses. Each checks SP, flags/registers, a twice-nested divide handler and two actual timer IRQs. |
| EMS AH56/BOP68 | Authored ems_call_return_probe.asm calls actual original INT67 driver: map page1, far-call, RETF through EmmRet/BOP68, restore page0; exact SP, caller/target cell data, callback count and free. Invalid subfunction8F and handle83 leave map/SP/call count unchanged. Not a claim of nested AH56 reentrancy or atomic late-map failure. |
| XMS failure/data | Original HIMEM guest with FORCED_RELOCATION proves actual move, failed huge growthA0 preserving size/data, shrink, locked resizeAB, double unlockAA, double freeA2, oversized allocationA0, restored capacity and A20 nesting. UMB_NO_FREE_BLOCKS is a retained original unavailable outcome, not UMB allocation success. Controlled copy-cancel/provider failures retain r009 proof. |
| Lease teardown | P1's45 assertions and unchanged session fixture; forged/cross-context/range/provider failure, rollback, slot exhaustion, epoch and teardown. No unsupported concurrent lease API claim. |
| IRQ sentinel | Actual linked PIC intack returns-1; actual CCPU receives stale HW notification, executes STI and reaches BEEF instead of dispatching INTFFFF. Same fixture retains HALT/reset and exception/debug tests. |
| Concurrent notification | Extract current complete atomic functions and actual local masks verbatim. Two real Windows producers raise distinct bits for4096 rounds;8192 raises, selective consumption preserves the other bit; timer clear preserves other notifications. This is adapter synchronization proof, not a guest IRQ throughput/SLA measurement. |

Reproduce notification with
`tests/observation/verify-ccpu-notification.ps1 -BuildRoot build/M0-T430/S6/<fresh>`.
`r018-notification-final` passes and records source/extraction/fixture/EXE hashes.
`r016-notification` passed the earlier fixture revision. Actual CPU/PIC
`r022-cpu-replay` passes and retains source/EXE/linked-provider identities.
The first new PIC run crashed because the fixture omitted InitializeIcaLock;
the worker's original initialization was not missing. Only test setup changed.

Reproduce selected guest cases with
`tests/observation/verify-memory-transition-guest.ps1 -RuntimeRoot
build/M0-T430/S6/r007-runtime -Observer
build/M0-T427/S4/r049/console-startup-observer.exe -BuildRoot
build/M0-T430/S6/<fresh> -Case <case>`.
Cases are interrupt16, interrupt32, code32, task-cleanup, ems-call and
xms-failure. They use a fresh build-only package clone, exact process-owned
cleanup, serialized BaseSrv use and Z: removed afterward. Guest probes are
NASM-authored tests, not altered original media. MSVC x86/MT builds the EMS
test-only PIF builder; all intermediates remain below the run root.
Each run requires actual output markers and real zero completion, not only
observer exit. DOSX stays
C5AF29A29ABF167B243DAABF877459E8278B8C9A339BF8E1E2576EAD5F6CEEFF.

Retained unsuccessful attempts are not waived passes:
r017-ems-call printed the complete success marker but its old CloseOnExit0
profile correctly stayed Inactive and timed out. The builder now accepts an
explicit test-only --close-on-exit, leaving its old default unchanged.
r019-dpmi16 passed all guest assertions but bare outer COMMAND EXIT returned
its established1; the verifier refused it. The revised direct /c route
propagates actual probe completion instead of weakening the zero assertion.
r021-task-cleanup failed NASM before launch due to a scalar/null splat;
the definitions are now always an array. Fresh final runs below supersede
these setup attempts, not retry-to-success product failures.

Final source-identical `r024-final-<case>` passes all six cases with actual
zero exits and expected markers. Runtime manifests, original media, source,
observer and guest hashes are retained; task-cleanup also hashes both freshly
assembled children. Product sources have no diff from526f73c1c. All eight
O:/winnt/system32 hashes match r007, Z: is absent after cleanup, and
documentation governance, relative links and diff checks pass. Test review
confirms no changed production algorithm, masked assertion or original-media
mutation. Other-session proposal/TODO changes remain excluded.

Source review leaves original guest limitations, external-device prerequisites
and unknown unselected boundaries explicitly unproved; no new repair owner is
invented. S6's selected obligations are proved at their stated layer or retain
their original disposition. Current eight-file publication is the S6 P1 set,
not S4; product runtime inputs are unchanged by this supplement. S7 owns the
integrated final ledger/gates; T430 still requires owner acceptance.
