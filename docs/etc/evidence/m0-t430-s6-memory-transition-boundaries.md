# T430 S6 memory and transition boundaries

Sequential admission after S5 a143894f1. Reconcile current selected inputs
with existing non-WOW DPMI/XMS/EMS/IRQ/lease proof, then supplement only named
unproved boundaries. This note admits no new runtime result or production fix.

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

## Project lease isolation correction (in progress)

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

S6 remains active: selected EMS AH56/BOP68, XMS failure and IRQ concurrency
proof reconciliation, complete product/publication gates and review remain.
S4's published eight-file set is unchanged. S7 and final owner acceptance
remain outside this active S.
