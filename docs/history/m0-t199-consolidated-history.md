# m0-t199 historical record

Consolidated task record. Raw contemporaneous records are retained in the external documentation archive.

## m0-t199-closure-20260814.md

# M0 T199 Closure — OpenNT BOP Owner-Package Recovery

T199 closed the complete admitted OpenNT BOP surface as owner packages rather
than trace-discovered individual handlers.  DEM, COMMAND, XMS, DPMI,
Redirector, Debugger, top-level host services, and machine/BIOS forms each
have a current owner, disposition and source-built evidence.  Selected
unavailable/deferred BOPs now return typed outcomes rather than raw `#UD`.

The closure is structural and compositional, not a claim that every historical
NT host capability exists.  Fresh all-package evidence is in
`artifacts/build/t199-s50-boot-all-families-r1/` and
`artifacts/build/t199-s50-native-xms-dpmi-r1/`; the complete audit is
`etc/research/t199-s50-global-bop-package-closure-audit-001.md`.


## m0-t199-s1-closure-20260813.md

# M0 T199 S1 Closure: Global BOP State and Package Plan

T199 S1 reconciles the full inventory with current source, composition and
evidence. It establishes that classification coverage is not implementation
closure, retains the partial COMMAND endpoint facts accurately, and orders
complete component packages for subsequent work.

Evidence: [T199 global BOP state audit](../../artifacts/documentation-archive/20260910/etc/research/t199-global-bop-state-audit-001.md).


## m0-t199-s12-closure-20260813.md

# M0 T199 S12 Closure — 2026-08-13

S12 is complete. The original Redirector package is mapped as 50 callable
members, `57:00` through `57:31`, rather than the previously recorded 49.
The source exporter, ingress, catalog, system plane and regressions now retain
`SVC_NETBIOSCHECK` (`57:31`), while `57:32` remains out of range. Its system
component is async/device rather than assignment/macro.

No Redirector host capability, historical dispatcher link, guest trace,
Bochs/core/mantle change, or per-service shim was added. S13 is admitted to
implement one source-derived explicit-unavailable provider for the entire
package.

Primary evidence: [S12 matrix](../../artifacts/documentation-archive/20260910/etc/research/t199-s12-redirector-source-abi-failure-matrix-001.md).


## m0-t199-s13-closure-20260813.md

# M0 T199 S13 Closure — 2026-08-13

S13 is complete. One `bx-vdm` source-derived unavailable facade is attached
to every defined Redirector BOP member, `57:00` through `57:31`. It reproduces
the original unavailable result through the common four-byte ingress:
`RIP+4`, `CF=1`, and `AX=1`. `57:32` remains unhandled.

The package gained no Redirector capability and no guest trace. It was
source-built and run both as a 50-member focused regression and through the
bound MSVC x64 `/MT` adapter composition.

Primary evidence: [S13 regression](../../artifacts/documentation-archive/20260910/etc/research/t199-s13-redirector-unavailable-package-regression-001.md).


## m0-t199-s14-closure-20260813.md

# M0 T199 S14 Closure — 2026-08-13

S14 is complete. It establishes that `BOP_DEBUGGER` is a three-byte selector
whose mode/arguments reside on the guest stack; the 16 header constants are
not a current fourth-byte dispatch ABI. The CLI has no debugger host plane,
so its selected whole-package disposition is deferred controlled stop.

S15 is admitted to correct the adapter's selector-only ingress and attach one
package facade. No Debugger capability or trace is part of that work.

Primary evidence: [S14 matrix](../../artifacts/documentation-archive/20260910/etc/research/t199-s14-debugger-source-abi-failure-matrix-001.md).


## m0-t199-s15-closure-20260813.md

# M0 T199 S15 Closure — 2026-08-13

S15 is complete. Debugger BOP ingress is corrected to the source-proven
three-byte selector form. One package facade converts the unsupported CLI
Debugger plane into typed controlled stop without reading guest stack memory
or consuming byte four. Focused and bound-composition MSVC x64 `/MT` tests
pass for both three- and four-byte observation windows.

Primary evidence: [S15 regression](../../artifacts/documentation-archive/20260910/etc/research/t199-s15-debugger-deferred-package-regression-001.md).


## m0-t199-s16-closure-20260813.md

# M0 T199 S16 Closure — 2026-08-13

S16 is complete. Every remaining non-family top-level selector now has an
original source path and an owner/disposition basis. The record separates CLI
host composition, Debugger, adapter config, mantle machine lifecycle and
DPMI/monitor transition ownership, and records the `5C/5D` historical variant
conflict.

S17 is admitted for grouped top-level disposition and facade design; no
selector behavior is implemented by this mapping task.

Primary evidence: [S16 matrix](../../artifacts/documentation-archive/20260910/etc/research/t199-s16-top-level-bop-source-owner-matrix-001.md).


## m0-t199-s17-closure-20260813.md

# M0 T199 S17 Closure — 2026-08-13

S17 is complete. All top-level non-family selectors now have a single owner
and disposition group. Existing idle/config/keyboard/terminal helpers are
explicitly designated migration inputs for one bx-vdm facade; monitor/DPMI
and historical-variant selectors remain outside it.

Primary evidence: [S17 disposition](../../artifacts/documentation-archive/20260910/etc/research/t199-s17-top-level-grouped-disposition-001.md).


## m0-t199-s18-closure-20260813.md

# M0 T199 S18 Closure — 2026-08-13

S18 is complete. One `bx-vdm` facade now owns the admitted top-level routing
group `51,59,5A,5B,5E,5F,FE` after common BOP ingress. The existing limited
config and machine-action providers remain behind that route; no host feature
or Bochs semantics were added.

Focused and bound-composition MSVC x64 `/MT` regressions pass. This is not a
claim of full WOW, UI, debugger, scheduler, CONFIG, keyboard, PIC, or BIOS
closure, and it does not include `5C`, `5D`, or `FD`.

Primary evidence: [S18 regression](../../artifacts/documentation-archive/20260910/etc/research/t199-s18-top-level-package-facade-regression-001.md).


## m0-t199-s19-closure-20260813.md

# M0 T199 S19 Closure — 2026-08-13

S19 is complete. The machine/BIOS package now has one source-backed owner
map: all 41 BIOS definitions and 349 conditional table rows are accounted
for, while `12`, `15:88`, `5F`, `5C/5D`, `FD`, and C1--C6 have distinct,
non-overlapping boundaries.

The result does not enable any new Bochs feature or choose a SoftPC product
branch. S20 is admitted to route this known package through common ingress.

Primary evidence: [S19 machine/BIOS map](../../artifacts/documentation-archive/20260910/etc/research/t199-s19-machine-bios-source-abi-owner-map-001.md).


## m0-t199-s2-closure-20260813.md

# M0 T199 S2 Closure: Complete DEM Source and Failure Matrix

All 73 callable DEM services and the dispatcher sentinel are reconciled to
original source, ABI direction, failure class and current evidence level.
The matrix does not admit individual implementation; it supplies the input to
one whole-DEM disposition decision.

Evidence: [T199 S2 DEM matrix](../../artifacts/documentation-archive/20260910/etc/research/t199-s2-dem-source-abi-failure-matrix-001.md).


## m0-t199-s20-closure-20260813.md

# M0 T199 S20 Closure — 2026-08-13

S20 is complete. The admitted machine BOP group now crosses common ingress
and one `bx-vdm` facade. Existing bounded memory and machine-action providers
remain intact; unresolved variant and monitor forms stop explicitly.

No SoftPC BIOS branch, Bochs feature, or machine semantics were added.

Primary evidence: [S20 regression](../../artifacts/documentation-archive/20260910/etc/research/t199-s20-machine-bop-facade-regression-001.md).


## m0-t199-s21-closure-20260813.md

# M0 T199 S21 Closure — 2026-08-13

S21 is complete. It establishes C1 A20's existing native owner, lifecycle
preconditions, fixed selector-blind wrapper ABI, negative cases, and the
required mantle exception register entry. No bx-core patch or BOP behavior is
admitted.

Primary evidence: [S21 admission](../../artifacts/documentation-archive/20260910/etc/research/t199-s21-c1-a20-capability-admission-001.md).


## m0-t199-s22-closure-20260813.md

# M0 T199 S22 Closure — 2026-08-13

S22 implements C1 as a selector-blind native mantle A20 wrapper. It is
source-built and executed with the full x64 CPU5 closure; invalid lifecycle
and value cases leave native state unchanged. It remains deliberately
unconnected to any BOP provider.

Primary evidence: [S22 regression](../../artifacts/documentation-archive/20260910/etc/research/t199-s22-c1-a20-capability-regression-001.md).


## m0-t199-s23-closure-20260813.md

# M0 T199 S23 Closure — 2026-08-13

S23 establishes the C2 extended-memory capability owner and ABI boundary.
OpenNT's historical SA allocator is not linked; C2 is admitted as a bounded,
source-derived mantle allocator over the native Bochs RAM aperture.

Primary evidence: [S23 admission](../../artifacts/documentation-archive/20260910/etc/research/t199-s23-c2-extended-memory-capability-admission-001.md).


## m0-t199-s24-closure-20260813.md

# M0 T199 S24 Closure -- C2 Bounded Aperture Allocator

## Outcome

Closed on 2026-08-13.  C2 is now a registered, selector-blind native
extended-aperture capability in `src/bx-mantle`, with query, allocate, free,
resize and overlap-safe move operations.

## Boundary

The capability begins at 1 MiB and uses only native RAM-length and checked
ordinary-RAM copy primitives.  It exposes opaque numeric handles and physical
offsets, never host pointers.  It neither recognizes nor implements a BOP,
XMS, DPMI, OpenNT service, device, BIOS, interrupt or adapter policy.

## Evidence

- Admission: [T199 S23 C2 admission](../../artifacts/documentation-archive/20260910/etc/research/t199-s23-c2-extended-memory-capability-admission-001.md).
- Registration: [BX-MANTLE-079](../../artifacts/documentation-archive/20260910/etc/research/adapter-external-intrusion-exceptions.md).
- Regression: [T199 S24 C2 regression](../../artifacts/documentation-archive/20260910/etc/research/t199-s24-c2-extended-memory-regression-001.md).
- Native artifact: `artifacts/build/t199-s24-extended-memory-r1/t199-s24-extended-memory.exe`, MSVC x64 `/MT`, CPU5/Pentium-MMX.

## Deferred

This does not implement the XMS provider.  C3 UMB admission, C5 INT15
semantics and the whole XMS package remain separate, whole-package work.


## m0-t199-s25-closure-20260813.md

# M0 T199 S25 Closure -- C3 UMB Owner and ABI Admission

## Outcome

Closed on 2026-08-13 with a deferred disposition.  C3 is not implementable in
the current no-device minimal machine without inventing a UMB layout or
importing the historical NT/SoftPC product host.

## Preserved boundary

No Bochs, mantle, adapter or OpenNT source changed.  XMS services `52:06..08`
remain a deferred complete subgroup.  No synthetic low-ROM hole, firmware,
VGA, option ROM, VDD or historical host allocation behavior was enabled.

## Evidence

[C3 UMB owner/ABI/failure map](../../artifacts/documentation-archive/20260910/etc/research/t199-s25-c3-umb-owner-abi-failure-map-001.md).


## m0-t199-s26-closure-20260813.md

# M0 T199 S26 Closure -- C5 INT15 and Fault-Delivery Admission

## Outcome

Closed on 2026-08-13 with a deferred disposition. C5 cannot be admitted from the no-device machine because the reached XMS path is a SoftPC keyboard and firmware callout contract, not a generic CPU interrupt API.

## Preserved boundary

No Bochs, mantle, adapter or OpenNT source changed. XMS `52:09` remains deferred. No synthetic IVT, PIC, keyboard, BIOS table, callback or device was enabled.

## Evidence

[C5 map](../../artifacts/documentation-archive/20260910/etc/research/t199-s26-c5-int15-owner-abi-failure-map-001.md).


## m0-t199-s27-closure-20260813.md

# M0 T199 S27 Closure -- C4 Protected-Mode State Admission

## Outcome

Closed on 2026-08-13 with a deferred disposition. OpenNT DPMI couples descriptor and mode changes to historical process LDT, application and stack lifetime; native Bochs CPU ownership does not make those semantics a mantle or adapter API.

## Preserved boundary

No Bochs, mantle, adapter or OpenNT source changed. No direct CR0, LDT, descriptor, stack or exception wrapper was introduced. The complete DPMI package remains deferred except explicit VCD unavailability.

## Evidence

[C4 map](../../artifacts/documentation-archive/20260910/etc/research/t199-s27-c4-protected-mode-owner-abi-failure-map-001.md).


## m0-t199-s28-closure-20260813.md

# M0 T199 S28 Closure -- XMS/DPMI Whole-Package Reconciliation

## Outcome

Closed on 2026-08-13. All 37 callable XMS/DPMI identities have one current
package-level disposition: 36 deferred and DPMI `53:12` explicitly unavailable.

## Evidence

[Whole-package reconciliation](../../artifacts/documentation-archive/20260910/etc/research/t199-s28-xms-dpmi-whole-package-reconciliation-001.md) and the retained x64 `/MT` fixture artifact.

## Boundary

No BOP gained a synthetic success/failure result. No machine capability was
enabled, and no trace was used to choose service work.


## m0-t199-s29-closure-20260813.md

# M0 T199 S29 Closure — COMMAND Package Session

S29 is closed.  It replaced the scattered bounded COMMAND execution helpers
with one `bx-vdm` COMMAND package session and one package dispatch entry.
All 17 callable `54:00..10` identities have a verified package disposition:
eight bounded source-derived provider paths, one original no-op, three common
unavailable stops, and five deferred members.  The source-built MSVC x64
family fixture passed at
`artifacts/build/t199-s29-command-package-r7/`.

This closes package composition only.  It does not assert full original
COMMAND parity, does not close `54:04` or `54:0E` as independent components,
and does not include a native trace run.


## m0-t199-s3-closure-20260813.md

# M0 T199 S3 Closure: Whole DEM Provider Disposition

T199 S3 selects one source-derived plane layout for every DEM service group, preserves original no-op and sentinel behavior, and identifies `50:42` as a bounded compatibility exception rather than an original handler result. S4 may now implement one DEM facade and all-service regression boundary.

Evidence: [T199 S3 DEM disposition](../../artifacts/documentation-archive/20260910/etc/research/t199-s3-dem-whole-package-disposition-001.md).


## m0-t199-s30-closure-20260813.md

# M0 T199 S30 Closure — DEM Package Session

S30 is closed.  `bx_ntvdm_dem_package_session_v1` now owns the DEM package
router and copied DEM session state.  All 73 callable DEM identities are
verified to enter that one router; existing namespace/search, GSET, IOCTL,
error/lock, DPB/DTA, session and compatibility providers remain behind it.
`50:36` no longer bypasses the package.

The MSVC x64 `/MT` fixture passed at
`artifacts/build/t199-s30-dem-package-r6/`.  This is source-built package
composition evidence only; it makes no native trace or full historical DEM
parity claim.


## m0-t199-s31-closure-20260813.md

# M0 T199 S31 Closure — 2026-08-13

S31 completed the XMS package provider without trace-led service selection.
All `52:00..0B` identities enter one `bx-vdm` session/router. Five
source-derived operations (`00`, `02`, `03`, `05`, `0B`) use already admitted
selector-blind mantle capabilities; the other seven have one documented
package-deferred result. `52:0C` remains rejected.

The native regression exposed and corrected one real boundary error: C2 had
used guest-A20-applied ordinary-RAM checks, despite OpenNT XMS allocating
host/SAS backing while A20 wrapping is enabled. Exception BX-MANTLE-079 was
amended before the selector-blind physical-backing primitive was added. The
result is source-built and native-lifecycle verified; it is not a guest trace
or a claim of complete XMS guest-runtime closure.


## m0-t199-s32-closure-20260813.md

# M0 T199 S32 Closure — 2026-08-13

S32 implemented one all-family DPMI router rather than a service-by-service
patch. All 25 callable `53` identities have a mechanically verified outcome:
the VCD/VDD service `12` is explicit unavailable; every remaining service is
package-deferred until the separately governed protected-mode/LDT/session
component exists. No trace selected the work and no DPMI semantics entered
Bochs, mantle, or the adapter outside `bx-vdm`.


## m0-t199-s33-closure-20260813.md

# M0 T199 S33 Closure — 2026-08-13

S33 reverified the already implemented whole-package Redirector unavailable
route against the current source tree and bound composition. All fifty
callable identities remain one source-derived failure contract, with no
per-service shim or host capability. The source-built composition fixture
exited zero. No trace was used.


## m0-t199-s34-closure-20260813.md

# M0 T199 S34 Closure — 2026-08-13

S34 reverified the current selector-only Debugger package in bound
composition. The fourth byte remains guest code and both accepted window forms
produce the sole controlled deferred stop. No debugger semantics were added.

The next non-duplicative integration work is to compose the already verified
XMS/DPMI package routers with the native mantle lifecycle through a new
composition boundary, rather than to modify this C-only boot-namespace
fixture or fabricate DPMI capabilities.


## m0-t199-s37-closure-20260814.md

# M0 T199 S37 Closure — DEM Whole-Package Provider Implementation

S37 is closed as a DEM package result.  OpenNT `demdisp.c`'s 73 callable
`50:00..48` identities were grouped by original component owner and assigned
one bounded provider, source-derived terminal result, or original no-op.
The pre-trace ledger mechanically confirms 73 entries, 73 distinct entries,
with no missing or duplicated identity.

Fresh MSVC x64 `/MT` all-DEM regression r87 passed, including namespace/FCB,
GSET, error/lock, MISC, DASD/IOCTL and no-op family boundaries.  The one
bounded r94 native integration build linked the current composition and ran
the guest exactly once.  It completed recorded DEM lifecycle transactions and
stopped at the fixture's existing COMMAND `54:01` boundary.  That observation
is integration evidence only; it neither claims continuous CLI execution nor
creates an endpoint implementation task.

The current-manifest native recipe was repaired to initialize MSVC once per
build, and its explicit object list was extended for the already admitted
checked path, A20, extended-memory and backing-RAM owners.  No Bochs BOP
selector knowledge, host filesystem access, or new device semantics was
introduced.

Evidence: [pre-trace ledger](../../artifacts/documentation-archive/20260910/etc/research/t199-s37-dem-pretrace-evidence-ledger-001.md),
[native observation](../../artifacts/documentation-archive/20260910/etc/research/t199-s37-dem-native-integration-observation-001.md),
and `artifacts/build/t199-s37-dem-integration-native-r94/`.


## m0-t199-s38-closure-20260814.md

# M0 T199 S38 Closure — Global BOP Owner and Composition Audit

S38 closes the whole-table audit, not a BOP implementation endpoint.  It
accounts for every ingress identity: DEM `50:00..48`, XMS `52:00..0B`, DPMI
`53:00..18`, COMMAND `54:00..10`, Redirector `57:00..31`, selector-only WOW,
Debugger, top-level and machine forms, plus all admitted sentinels and
unknown-service boundaries.  Each is assigned exactly one package owner and
one explicit provider/failure/deferred disposition.

The audit found a real lifecycle gap: the generic bridge can consult native
XMS/DPMI after the boot namespace declines, but the installed CLI runtime had
never bound that native session.  The runtime now initializes/binds both
sessions and releases both on reset, including a partial-install rejection.

Fresh witness `artifacts/build/t199-s38-global-bop-r5/` source-builds, links
and runs an MSVC x64 `/MT` fixture.  It binds both sessions and demonstrates
that `52:00` reaches the native XMS provider through the one generic #UD
bridge.  Test mantle capabilities are deterministic ABI doubles only; no
Bochs device, DOS semantic or native guest trace was used.

Evidence: [global ledger](../../artifacts/documentation-archive/20260910/etc/research/t199-s38-global-bop-owner-composition-ledger-001.md).


## m0-t199-s39-closure-20260814.md

# M0 T199 S39 Closure — Package-Complete Native Integration Baseline

S39 made one bounded source-built guest execution after all currently admitted
BOP package sessions were composed.  The run reached and completed existing
DEM package transactions, then stopped at the first unhandled `54:01` BOP.
The exact source-built witness is
`artifacts/build/t199-s39-global-native-r1/`; its provenance reports
`runs=1`.

The outcome is not a `54:01` implementation task.  It assigns the frontier to
the COMMAND bootstrap/launch/lifecycle package.  `cmdGetNextCmd` depends on
the lifecycle registration made by `cmdSetInfo`; the current package declines
when that copied registration is not established.  Any repair must therefore
reassess the complete COMMAND entry lifecycle and its original failure
contract.

The native fixture and its observation bridge were adjusted to bind and query
both existing package sessions in production order.  A separate S93 fixture
preparation run compiled the revised native entry fixture without a second
guest execution.

Evidence: [S39 baseline](../../artifacts/documentation-archive/20260910/etc/research/t199-s39-package-complete-native-integration-baseline-001.md).


## m0-t199-s40-closure-20260814.md

# M0 T199 S40 Closure — COMMAND Entry Lifecycle Package Recovery

S40 repaired an adapter-invented registration gate in the whole COMMAND entry
lifecycle. Original `cmdGetNextCmd` reads CMDINFO directly; `cmdSetInfo`
records separate SCS/DOSDATA locators. The adapter now permits `54:01`
before `54:05`, omitting only registration-owned writes until a validated
copied registration exists. It never creates a zero-address guest write or
declines the callable BOP to the CPU.

Fresh MSVC x64 `/MT` source-built witnesses are
`artifacts/build/t199-s40-command-entry-r3/` (helper transaction) and
`artifacts/build/t199-s40-command-session-r2/` (bound ingress/composition
session before registration, followed by preserved registered lifecycle).
No native trace was run.

Evidence: [source/ABI map](../../artifacts/documentation-archive/20260910/etc/research/t199-s40-command-entry-lifecycle-source-abi-map-001.md).


## m0-t199-s41-closure-20260814.md

# M0 T199 S41 Closure — COMMAND native integration observation

S41 performed its one authorized source-built CPU5 native guest execution.
The process reached the controlled fixture stop (exit `2`) with `next=54:01`.
It therefore did not prove COMMAND handling of the actual startup CMDINFO
transaction.  It did prove the current composed manifest builds, links and
executes through established DEM traffic before the COMMAND boundary.

The result is retained as a bounded integration observation, not a component
closure or a trace-created endpoint task.  The follow-on is S42's complete
COMMAND CMDINFO ABI recovery, grounded in OpenNT source and whole-package
source-built regression before another native run.

Evidence: [S41 observation](../../artifacts/documentation-archive/20260910/etc/research/t199-s41-command-native-integration-observation-001.md).


## m0-t199-s42-closure-20260814.md

# M0 T199 S42 Closure — COMMAND CMDINFO ABI Package Recovery

S42 recovered the original 36-byte CMDINFO caller/callee map from matching
OpenNT header, 16-bit COMMAND and host service sources.  The contained
provider now preserves the `54:0F → 54:01` environment-capacity hand-off:
when CMDINFO capacity is too small it resumes with the original CF/AX retry
shape before guest mutation, and otherwise validates the declared environment
range separately from command/executable output ranges.

The implementation was verified by fresh MSVC x64 `/MT` helper and bound
composition regressions.  No native guest run was authorized or performed.
The initial S41 `54:01` observation remains unexplained at live-register
level, so this closure does not claim that COMMAND or the frontier is closed.

Evidence: [CMDINFO source/ABI map](../../artifacts/documentation-archive/20260910/etc/research/t199-s42-command-cmdinfo-source-abi-map-001.md).


## m0-t199-s43-closure-20260814.md

# M0 T199 S43 Closure — COMMAND Full Service-Family Composition

S43 reconciled every callable `54:00..10` slot in OpenNT `cmddisp.c` with the
bound bx-vdm COMMAND package.  The resulting ledger distinguishes contained
CLI profile responses, checked source-derived providers, the original no-op,
and explicit unavailable stops.  A fresh MSVC x64 `/MT` bound-composition
fixture passed across the complete family.

No native guest trace ran.  This proves package-level static routing and
family behavior, not recovery of historical process/redirection/VDD/console
capabilities or closure of the S41 native frontier.

Evidence: [COMMAND family ledger](../../artifacts/documentation-archive/20260910/etc/research/t199-s43-command-family-owner-disposition-ledger-001.md).


## m0-t199-s44-closure-20260814.md

# M0 T199 S44 Closure — XMS Whole-Package Disposition

S44 mapped all twelve OpenNT XMS slots to their mechanical owner and CLI
disposition. The adapter retains only copied register/KiB-handle translation;
bx-mantle retains A20 and extended-RAM mechanics. Unadmitted move, page-size,
UMB, INT15-hook and free-query paths now stop explicitly instead of leaking
back as raw #UD.

Fresh MSVC x64 `/MT` global composition evidence is in
`artifacts/build/t199-s44-xms-family-r2/`. No native guest trace ran.


## m0-t199-s45-closure-20260814.md

# M0 T199 S45 Closure — DPMI Whole-Package Disposition

S45 mapped all twenty-five OpenNT DPMI BOP identities from `53:00` through
`53:18` to their original protected-mode, interrupt/fault, memory, session,
DOS-bridge, debug, or VDD owner.  The current CLI profile admits none of the
complete DPMI composition, so every member now stops explicitly at the typed
boundary rather than returning to the CPU as an unhandled `#UD`.

Fresh MSVC x64 `/MT` global-composition evidence is retained in
`artifacts/build/t199-s45-dpmi-family-r1/`.  No native guest trace ran.


## m0-t199-s46-closure-20260814.md

# M0 T199 S46 Closure — Redirector Whole-Package Revalidation

S46 revalidated the complete OpenNT `BOP_REDIR` family against the current
bound CLI composition.  All fifty original `57:00..31` identities retain the
single source-derived unavailable result (`RIP+4`, `CF=1`, `AX=1`); undefined
`57:32` remains rejected.

Fresh MSVC x64 `/MT` evidence is retained in
`artifacts/build/t199-s46-redir-package-r1/`.  No Redirector host capability
and no native guest trace was introduced.


## m0-t199-s47-closure-20260814.md

# M0 T199 S47 Closure — Debugger Whole-Package Revalidation

S47 verified that `BOP_DEBUGGER` is the three-byte `C4 C4 56` selector with a
guest-stack mode ABI, not a four-byte service family.  The current CLI profile
therefore preserves both three- and four-byte observation windows as a single
deferred typed stop, with no stack read and no register/flags change.

Fresh MSVC x64 `/MT` bound-composition evidence is retained in
`artifacts/build/t199-s47-debugger-package-r1/`.  No debugger host capability
and no native guest trace was introduced.


## m0-t199-s48-closure-20260814.md

# M0 T199 S48 Closure — Top-Level Host-Service Package

S48 revalidated all non-machine top-level host-service selectors and repaired
the `5E` nonzero-notification continuation to match original `MS_bop_E`:
ignore and resume at `RIP+3`.  The implementation keeps all other unsupported
host functions as explicit typed stops or bounded continuation as documented.

Focused and bound MSVC x64 `/MT` evidence is retained in
`artifacts/build/t199-s48-top-level-facade-r1/` and
`artifacts/build/t199-s48-top-level-package-r1/`.  No native trace or host
capability was introduced.


## m0-t199-s49-closure-20260814.md

# M0 T199 S49 Closure — Machine and BIOS BOP Package

S49 revalidated the machine/BIOS BOP package through both focused and bound
MSVC x64 `/MT` fixtures.  `12` and `15:88` retain declared profile values,
`5F` retains its checked-action machine handoff, and `5C`, `5D`, `FD` remain
typed deferred stops.  bx-core and bx-mantle remain selector-blind.

Evidence is retained in `artifacts/build/t199-s49-machine-facade-r1/` and
`artifacts/build/t199-s49-machine-package-r1/`; no native guest trace ran.

