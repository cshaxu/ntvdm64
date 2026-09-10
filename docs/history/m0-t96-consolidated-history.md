# m0-t96 historical record

Consolidated task record. Raw contemporaneous records are retained in the external documentation archive.

## m0-t96-closure-20260812.md

# M0 T96 Closure — OpenNT BOP Host-Service Plane Recovery

## Closed Outcome

T96 closes the holistic BOP architecture package, not the NTVDM CLI runtime.
It established generic adapter ingress, source-derived OpenNT selector/service
ownership, provider precedence, module-scale DEM/COMMAND/XMS-DPMI/system
planes, SoftPC BIOS ownership, legacy endpoint gating, and full inventory
regression. `nt_bop.c` remains source-contract evidence, not a linked
historical host composition.

S10/P1 linked the current r49 native fixture with all T96 plane and gate
objects, then observed a real NTIO/NTDOS path. It reaches `50:42` at
`9346:6351`; OpenNT's DEM table maps that service to
`demNotYetImplemented`, so the adapter correctly does not fabricate a result.
The full procedure and retained trace are in
`etc/research/t96-s10-real-path-checkpoint-001.md`.

## Transfer Decision

The owner authorized dependency- and ROI-based queue selection. T97 therefore
selects the read-only DOS namespace, file, and search capability rather than
first attempting a repository-wide replacement of every frozen legacy
provider. It is the direct observed capability boundary and has a coherent
original OpenNT component family (`demfile`, `demhndl`, `demdir`, `demsrch`,
and shared FCB contracts). The broad legacy-provider replacement remains a
later candidate and must consume T97's component/ABI evidence.

T96 must not be interpreted as a runnable CLI result: it is a closed BOP
plane and ownership boundary that enables the next capability package.


## m0-t96-s1-p1-20260811.md

# M0 T96 S1 P1 — Complete BOP Provider and Module Map

## Scope

This delivery closes the static mapping subtask only. It does not enable a BOP
provider, change a guest register, or compile/link OpenNT host code.

## Inputs and procedure

- Immutable definition inventory:
  artifacts/analysis/opennt-bop-inventory-005-20260811-001/.
- Reproducible mapper:
  tools/governance/Export-OpenNtBopProviderMap.ps1.
- Pinned OpenNT dispatcher sources:
  demdisp.c, cmddisp.c, xmsdisp.c, dpmi32.c, vrdisp.c, and dbg.c; their exact
  function mappings are emitted in the result JSON.

The mapper was invoked against the immutable inventory and wrote the
non-overwritten evidence root
artifacts/analysis/opennt-bop-provider-module-map-001-20260811-003/.

## Observed result

- All 16 top-level selectors have an original route and one present
  disposition.
- All 194 source-definition service rows map to a function-table handler, the
  debugger switch dispatcher, or an explicit dispatcher-bound sentinel.
- DOS and COMMAND each contain one documented LASTSVC sentinel; those are
  marked non-callable rather than falsely reported as missing handlers.
- The evidence retains all 41 historical BIOS selector definitions and all
  349 annotated BIOS[] table rows without claiming a selected historical
  configuration.
- The map distinguishes original routing from current provider availability:
  ordinary rows are mapped-not-enabled; no row claims guest-visible success.

## Boundary findings

nt_bop.c is source evidence for top-level routing, not a linkable adapter
object. DEM, COMMAND, and XMS dispatchers bind to historical softpc.h
register/SAS contracts; DPMI binds to mode and VDM-pointer semantics;
Redirector binds to NT/Win32/network APIs; and Debugger binds to VDM stack
frames. These facts justify the next subtask's shared ingress/provider ABI
rather than direct dispatcher linkage.

## Verification and delivery limitation

- Export-OpenNtBopProviderMap.ps1 completed successfully.
- The output self-check reported: 16 selectors, 194 service rows, 41 BIOS
  definitions, 349 BIOS rows, and zero selector routes missing.
- Documentation governance verification and git diff --check passed before
  delivery formation.

This P is a local commit pending delivery because this repository has no
configured Git remote. It is not evidence that any BOP provider is accepted
or runnable.


## m0-t96-s2-p1-20260811.md

# M0 T96 S2 P1 — Shared BOP Ingress ABI

## Delivery

This delivery adds the adapter-local, fixed-width BOP ingress classifier and
typed pass-through dispatch gate:

- src/bx-ntvdm-adapter/bx_ntvdm_bop_ingress_v1.h
- src/bx-ntvdm-adapter/bx_ntvdm_bop_ingress_v1.c
- tests/bx-ntvdm-adapter/bx_ntvdm_bop_ingress_v1_test.c

The ingress accepts only copied exception, CPU, and instruction-window values.
It routes every inventoried selector/service form to one classification and
returns a checked CPU-result-v2 pass-through result. It does not call a
provider, read guest memory, expose a pointer, advance IP, change a register,
or link OpenNT or Bochs source.

## Evidence and verification

The route taxonomy and source basis are recorded in
etc/research/t96-s2-bop-ingress-abi-001.md. The focused Clang build and test
completed with exit code zero under -Wall -Wextra -Werror. It exercises every
callable service index in all six service families, both dispatcher-bound
sentinels, all top-level selectors, invalid forms, and the non-#UD gate.

Documentation governance verification and git diff --check passed.

## Explicit limitations

This is not S2 closure. No provider registry or provider implementation is
installed yet; all source-known service routes remain deferred or explicitly
unavailable and retain pass-through CPU behavior. The existing historical
endpoint chain in adapter_runtime_v2_dispatch is not migrated by this
delivery. Its migration and no-bypass enforcement remain the dedicated S8
scope after the service planes are admitted.

The standard MinGW validation remains unavailable here because MSYS2 cannot
create temporary files in its fixed C:\msys64\tmp directory. The successful
Clang test verifies the standalone adapter ABI only; it does not assert
cross-island integration.


## m0-t96-s2-p2-20260811.md

# M0 T96 S2 P2 — Provider Selection Registry

## Delivery

This delivery adds
src/bx-ntvdm-adapter/bx_ntvdm_bop_provider_registry_v1.{h,c} and extends the
focused ingress test. The registry converts a classified ingress record into
a fixed-width provider-family, precedence, and disposition record.

Mapped BOP routes select their original OpenNT owner family but remain
deferred. Explicit top-level unavailable routes select the original
failure-or-deferred precedence. Everything else is not applicable. No
provider function, module pointer, dynamic loader, guest pointer, host
pointer, or CPU mutation crosses this registry.

## Exit facts

S2 now has one shared selector/service classifier, one provider-selection
record format, and one typed pass-through result gate. Subsequent provider
planes can attach to stable provider-family IDs without changing Bochs event
decoding or the ingress ABI.

The previous isolated endpoint chain is frozen: S2 adds no new service
recognizer to it. Its migration to the common registry and the final
no-bypass assertion remain the planned S8 work, after the provider planes
exist.

## Verification

The standalone focused Clang build passed with -Wall -Wextra -Werror, and the
test executable exited zero. It covers all selectors and service ranges plus
deferred, unavailable, and not-applicable provider selections. Documentation
governance and git diff --check passed before delivery formation.

The normal MinGW probe remains environment-blocked by MSYS2 temporary-directory
permissions; this record does not treat that as a successful MinGW build.


## m0-t96-s3-closure-20260811.md

# M0 T96 S3 Closure — DOS/DEM Host-Service Plane

## Outcome

S3 closed its component-level DEM host-service recovery scope. It did not
claim that historical DEM code is directly linkable or that the old runtime
chain is migrated; both would be false. It established the reusable, audited
provider boundary required before S8 migration.

## Closure facts

- The pinned `demdisp.c` table is completely mapped: 73 callable services,
  eight original `demNotYetImplemented` slots, and one non-callable sentinel.
- `dem_plane_v1` classifies every callable service by original component and
  disposition: namespace 24, FCB 8, GSET 13, raw media 3, misc 13,
  error/lock 3, V86 bridge 1, and original no-op 8.
- The original no-op, profile boot-drive, and immutable drive-inventory
  provider slices all require the same ingress, registry selection, and DEM
  plane record. No admitted slice can independently turn a deferred service
  into a success result.
- Full IOCTL and absolute-media source contracts prove that AL=0D/11 and DEM
  services 41/42 are a single raw-media family, deferred without a physical
  host-disk shortcut. AL=08/09 remain paired projected-drive metadata for a
  later migration.
- Existing pre-T96 runtime endpoint recognizers are preserved as evidence and
  frozen. Their migration/no-bypass integration is expressly S8 work.

## Evidence and verification

Primary records: `t96-s3-opennt-dem-provider-map-001.md`,
`t96-s3-dem-component-boundaries-001.md`,
`t96-s3-demgset-disposition-001.md`,
`t96-s3-demioctl-source-contract-001.md`,
`t96-s3-demdasd-source-contract-001.md`, and
`t96-s3-dem-plane-abi-001.md`.

Strict focused Clang C11 tests passed for the full plane, original-noop slice,
boot-drive slice, and drive-inventory slice. Documentation governance and
`git diff --check` passed at each P. The default MinGW probe remains an
environmental limitation because MSYS2 cannot create temporary files in
`C:\\msys64\\tmp`; it was not treated as a passing build. No Git remote is
configured, so local commits remain pending push.

## Handoff

S4 begins the same recovery process for the original COMMAND service plane.
It must not consume DEM internals, reintroduce `nt_bop.c`, or bypass the
common ingress/selection structure.


## m0-t96-s3-p1-20260811.md

# M0 T96 S3 P1 — Complete DEM Source-Closure Map

## Delivery

The reproducible DEM provider-map generator and immutable output establish
the complete original OpenNT service-to-handler-to-definition closure for the
DOS/DEM BOP family:

- tools/governance/Export-OpenNtDemProviderMap.ps1
- artifacts/analysis/opennt-dem-provider-map-001-20260811-002/

## Facts

All 74 DEM definitions have one static disposition: 65 source-pending
composition handlers, 8 original demNotYetImplemented handlers, one LASTSVC
sentinel, and zero unresolved handler definitions.

The eight original unavailable services are recorded by numeric service and
symbol in t96-s3-opennt-dem-provider-map-001.md. No existing adapter endpoint
changes this original classification.

## Limitation and next step

This is source closure, not a DEM provider implementation. The 65 handlers
still bind to the historical CCPU/SAS/NT-host composition. S3 next groups
those dependencies and compares every existing adapter endpoint with the
matching original handler before any provider is admitted.


## m0-t96-s3-p10-20260811.md

# M0 T96 S3 P10 — DEM No-op Plane Gate

## Delivery

The admitted `demNotYetImplemented` provider now requires the common DEM
plane record and accepts execution only when that record identifies the same
service as the original no-op component.

## Boundary and verification

This is a classification gate only: no new BOP service, legacy runtime branch,
OpenNT call, guest-memory access, or host capability was added. The strict
focused C11 test covers all eight no-op slots, a non-no-op rejection, and a
tampered component-record rejection. Documentation governance and diff checks
were run; the local commit is pending push because no Git remote is configured.


## m0-t96-s3-p11-20260811.md

# M0 T96 S3 P11 — Unified DEM Slice Gates

## Delivery

The profile boot-drive and immutable drive-inventory DEM providers now require
the same unified plane record as the original no-op provider. Each accepts
only its exact original GSET service/component classification.

## Boundary and verification

No provider gained a new service or capability: boot drive remains a
profile-selected DOS index and drive inventory remains a CLI-created immutable
snapshot. Strict focused C11 compile/test runs passed for both providers with
the plane/ingress/registry closure. Documentation governance and diff checks
were run; the local commit is pending push because no Git remote is configured.


## m0-t96-s3-p2-20260811.md

# M0 T96 S3 P2 — DEM Endpoint Reconciliation

## Audit result

The existing adapter has twelve DEM-specific recognizers. Each is now mapped
to its exact OpenNT service number and original handler in
etc/research/t96-s3-opennt-dem-provider-map-001.md.

Eleven are only source-derived candidates. They remain unadmitted until their
original handler contract, composition blocker, and contained replacement ABI
are reviewed as part of the DEM provider module.

One endpoint is a hard conflict: the legacy fast-read recognizer handles
service 66, but the OpenNT demdisp table maps it to demNotYetImplemented.
That endpoint is frozen for migration purposes and must not be treated as a
successful DEM provider.

## Consequence

The next S3 step is a provider-level disposition table and explicit rejection
for original unavailable entries, followed by review of the 65 original
handler dependency groups. No new service recognizer is authorized.


## m0-t96-s3-p3-20260811.md

# M0 T96 S3 P3 — Original DEM No-Implementation Contract

## Correction

The prior endpoint audit correctly found that FASTREAD service 66 cannot retain
its legacy read-path. Follow-up source inspection established that
demNotYetImplemented is a no-implementation handler, not a carry-set error
handler: it optionally logs and then clears CF.

## Delivery

The new DEM provider slice owns exactly the eight original table entries that
select demNotYetImplemented. It resumes after the BOP and clears CF, with no
file operation, guest-memory access, general-register mutation, or provider
callback. A focused test covers all eight values and proves that DEMREAD is
not accepted by this slice.

The slice is not yet connected to the legacy runtime chain. That connection
must occur through the common ingress/provider registry during the planned
endpoint migration, not through a new recognizer.


## m0-t96-s3-p5-20260811.md

# M0 T96 S3 P5 — DEM GETDRIVES Projection Provider

## Delivery

The drive-provider slice owns only DEM service 15, demGetDrives. It takes the
immutable CLI-filtered drive snapshot and exactly reproduces the original
counting rule: A/B form the two-drive baseline and contiguous admitted
removable/fixed/CD/RAM entries from C onward increase AX. It resumes the BOP
and clears CF.

## Boundary

The provider does not invoke GetLogicalDrives or GetDriveType during a guest
request. Those observations remain a one-time CLI/session capability input.
It does not own boot-drive choice, free-space, DPB, current-directory, or
other demgset services.

## Verification

Focused Clang compilation with strict warnings and test execution passed. The
test covers a contiguous C/D/E projection and a gap that stops the original
count. The remainder of the demgset component is retained in the disposition
record, including the explicit GETBOOTDRIVE policy blocker.


## m0-t96-s3-p6-20260811.md

# M0 T96 S3 P6 — Profile-Drive DEM Boot Provider

## Delivery

The boot-provider slice owns DEM service 13 through the common ingress and
provider-selection records. It receives a profile-selected guest DOS drive
index and returns the original one-based AL result while preserving AH.

The selected index is already established by the profile's common placement
of COMMAND, CONFIG.SYS, AUTOEXEC.BAT, and target. This replaces the legacy
hard-coded C fallback with a declared guest-composition fact.

## Boundary and verification

The provider has no registry, CWD, host path, drive-enumeration, or host
handle input. It rejects a drive index outside A through Z. Strict focused
compilation and test execution passed: index 2 produces AL 3 and invalid
index 26 is rejected.

Runtime migration remains deferred to the single common registry path; this
delivery does not add another adapter recognizer.


## m0-t96-s3-p7-20260811.md

# M0 T96 S3 P7 — DEMIOCTL Source-Contract Closure

## Delivery

Recorded the entire original `demioctl.c` AL selector range and cross-checked
it against the DOS kernel's actual BOP call sites. The resulting contract
keeps the original component whole: AL=08/09 are one future immutable
projected-drive metadata slice, while AL=0D/11 remain one deferred raw
block-media slice.

## Boundary and verification

The record identifies the pinned historical files and their hashes, source
register/guest-memory contracts, host-composition blockers, and negative
disposition. It corrects the older trace-only AL=08 interpretation without
rewriting that historical evidence. No runtime code, Bochs behavior, host
filesystem access, or new legacy recognizer was added.

The documentation inventory and governance verifier were run after this
delivery; the local commit is pending push because this repository has no
configured Git remote.


## m0-t96-s3-p8-20260811.md

# M0 T96 S3 P8 — DEM Raw-Media Component Disposition

## Delivery

Recorded the complete source boundary for services 41/42 (`DEMABSDRD` and
`DEMABSDWRT`). They remain one deferred raw block-media provider family,
alongside generic IOCTL AL=0D/11, rather than becoming a read-only namespace
convenience path.

## Boundary and verification

The evidence covers original registers, guest structures, BDS lifecycle,
physical media APIs, INT 13h/SAS/`host_simulate` behavior, and exact profile
exclusions. No runtime service, Bochs behavior, or host-device access was
added. Documentation inventory/governance verification and diff checks were
run; the local commit is pending push because no Git remote is configured.


## m0-t96-s3-p9-20260811.md

# M0 T96 S3 P9 — Unified DEM Plane Classification

## Delivery

Added the adapter-local `dem_plane_v1` classifier. It gives every callable
DEM BOP one original component group and one disposition before any provider
is allowed to act: eight source-proven no-ops and 65 deferred operations.

## Boundary and verification

The module is data-only. It neither calls OpenNT nor contains a provider
callback, host/guest pointer, CPU result, or Bochs dependency. The focused
strict C11 test covers all 73 callable services and rejects the original
dispatcher sentinel. Documentation governance and diff checks were run; the
local commit is pending push because no Git remote is configured.


## m0-t96-s4-closure-20260811.md

# M0 T96 S4 Closure — COMMAND Host-Service Plane

S4 closes the COMMAND source/provider boundary, not full historical COMMAND
composition. The complete 17-service dispatcher map and data-only COMMAND
plane preserve source component ownership. `CMDSAVEWORLD` is the sole
source-proven executable no-op and is plane-gated. Execution services are
explicitly unavailable; all other nontrivial components remain deferred.

The CONFIG/AUTOEXEC namespace replacement, target launch fixture, and COMSPEC
bootstrap are retained as legacy endpoint evidence only. Their runtime routing
and exact semantic reconciliation are S8; no direct recognizer expansion is
admitted. Evidence is in the S4 provider map, plane ABI, boot-file
reconciliation, start-info contract, and launch-composition blocker records.

S5 receives no COMMAND internals. It begins XMS/DPMI source dispatch and
machine-owner disposition review.


## m0-t96-s4-p1-20260811.md

# M0 T96 S4 P1 — COMMAND Dispatcher and Component Map

## Delivery

Mapped all 17 `cmddisp.c` entries plus its sentinel to their original handler
modules, guest call sites, host dependencies, and current CLI disposition.
The result preserves the source table's component shape: launch, config pair,
environment, execution, handles/redirection, console/VDD, and lifecycle are
not reduced to trace-driven individual handlers.

## Boundary and verification

No COMMAND runtime service, guest-memory reader, host handle, process launch,
VDD dependency, or historical source linkage was added. The provider map
records why direct compilation would retain SoftPC/Win32 composition and which
future services are only candidates. Documentation governance and diff checks
were run; the local commit is pending push because no Git remote is configured.


## m0-t96-s4-p2-20260811.md

# M0 T96 S4 P2 — Unified COMMAND Plane

Added a complete data-only COMMAND plane classification for all callable
services. It preserves original components and makes execution services 8/10
explicitly unavailable rather than accidentally successful. Strict focused C11
test and documentation governance passed; no Git remote is configured.


## m0-t96-s4-p3-20260811.md

# M0 T96 S4 P3 — COMMAND Boot-File Reconciliation

Recorded the exact difference between the source-derived CONFIG/AUTOEXEC
replacement and its old runtime routing. The namespace-path semantics are
retained; only common-plane migration is deferred to S8. No code or runtime
behavior changed. Documentation governance and diff checks were run; no Git
remote is configured.


## m0-t96-s4-p4-20260811.md

# M0 T96 S4 P4 — CMDSAVEWORLD Original No-op Provider

Added the sole source-proven COMMAND no-op provider for service 03. It requires
common ingress, registry selection, and COMMAND-plane original-noop records,
then resumes without any state change. The disabled historical save-world
implementation remains disabled. Strict focused C11 test, documentation
governance, and diff checks were run; no Git remote is configured.


## m0-t96-s4-p5-20260811.md

# M0 T96 S4 P5 — COMMAND Launch Composition Blocker

Recorded why the existing target-only `cmd_get_next_service` cannot represent
the original `cmdGetNextCmd` provider. It is frozen as a legacy S8 migration
object; a future profile must declare a copied launch protocol rather than
add special cases. No runtime behavior changed.


## m0-t96-s5-closure-20260811.md

# M0 T96 S5 Closure — XMS/DPMI Provider and Machine-Owner Disposition

## Result

S5 is complete. The complete OpenNT XMS (12 services) and DPMI (25 services)
dispatcher tables are mapped in
`etc/research/t96-s5-opennt-xms-dpmi-provider-map-001.md`. No historical
dispatcher is linked. The new `bx_ntvdm_xms_dpmi_plane_v1` is a fixed-width
classification record only: it has no callback, guest pointer, Bochs API, or
CPU-result transition.

All XMS services are deferred. DPMI service 12 (`DpmiVcdPmSvcCall32`) is
explicitly unavailable in the first CLI profile because the original handler
enumerates Windows registry serial-port state. All other DPMI services are
deferred pending their Bochs-native memory, CPU, interrupt, or separately
bounded composition prerequisites. No service acquires fabricated success.

## Verification

On 2026-08-11 the following focused closure compiled and ran successfully
with `C:\Program Files\LLVM\bin\clang.exe` using `-std=c11 -Wall -Wextra
-Werror`:

```text
instruction-window + CPU-delta + exception + CPU-state + CPU-result
+ BOP-ingress + provider-registry + XMS/DPMI-plane
+ bx_ntvdm_xms_dpmi_plane_v1_test
```

The test exercises every callable XMS and DPMI service and rejects DPMI
service 25. Documentation inventory/export, governance verification, and
`git diff --check` also passed before closure.

## Handoff

S6 may now audit the remaining Redirector, debugger, notification, VDD and
mouse host-provider families with the same rule: original source is first
evidence; historical system bindings are not adapter imports; unresolved work
is deferred or source-proven unavailable.


## m0-t96-s6-closure-20260811.md

# M0 T96 S6 Closure — System Provider Disposition

## Result

S6 is complete. The remaining host-provider families are closed as source
classification in `etc/research/t96-s6-opennt-system-provider-map-001.md`.
The result covers all 49 Redirector services, all 16 debugger services, and
all residual host-composition selectors identified in `nt_bop.c`, including
the otherwise unnamed historical 55h and ISV 58h entries.

Redirector is explicitly unavailable in the first CLI profile. This preserves
the original `MS_bop_7` no-VDMREDIR failure direction instead of silently
providing networking, host pipes, mailslots, NetAPI, DLC or host identity.
Debugger remains deferred because its source has no isolated, source-proven
CLI failure/resume component. Mouse, keyboard, video, CPU lifecycle and BIOS
forms remain Bochs-native owners, never adapter implementations.

`bx_ntvdm_system_plane_v1` is only a copied classification record. It accepts
registry-selected 56:00--0F and 57:00--30 identities; it holds no callback,
guest pointer, host handle or CPU result.

## Verification

On 2026-08-11 `C:\Program Files\LLVM\bin\clang.exe` compiled and ran the
strict (`-std=c11 -Wall -Wextra -Werror`) focused closure consisting of the
common ingress/registry dependencies, `bx_ntvdm_system_plane_v1`, and its
test. The test covers every Redirector and debugger value and rejects 57:31.
Documentation inventory/export, governance verification and `git diff --check`
are required before commit.

## Handoff

S7 is now the only active packet. It must reconcile the historic SoftPC BIOS
selector/table evidence with Bochs-native ownership, not import the SoftPC
BIOS dispatcher or turn the adapter catalogue into a machine implementation.


## m0-t96-s7-closure-20260811.md

# M0 T96 S7 Closure — SoftPC BIOS Ownership

## Result

S7 is complete. All 41 named SoftPC BIOS selector definitions and all 349
annotated historical `BIOS[]` table forms have a reproducible disposition in
`etc/research/t96-s7-softpc-bios-owner-matrix-001.md`:

- 227 `illegal_bop` forms are historical unavailable;
- 25 forms are OpenNT host-selector collisions; and
- 97 are Bochs-native candidates, not enabled features.

The matrix explicitly refuses the false inference that the three BOP bytes
can select one branch of the historical `BIOS[]` table. No SoftPC BIOS source,
firmware, device logic, table dispatcher or pointer is copied into the
adapter. Existing adapter BIOS selector records remain legacy migration
objects for S8, not a widening permission.

## Verification

`tests/bx-ntvdm-adapter/Test-SoftpcBiosOwnerMatrix.ps1` ran successfully on
2026-08-11. It consumes the immutable inventory directly, verifies 41
definitions, 349 rows, 227 unavailable forms, 25 collision forms and 97
native candidates, and rejects a missing collision or unclassified row.

## Handoff

S8 is now the only active packet. It must inventory every existing adapter
runtime/end-point bypass and either place it behind the closed DEM/COMMAND/
XMS-DPMI/system planes with a recorded contract, or freeze/reject it. It may
not add an independent runtime recognizer.


## m0-t96-s8-closure-20260811.md

# M0 T96 S8 Closure — Legacy Endpoint Migration and No-Bypass Verification

## Result

S8 is complete. Every discovered legacy direct BOP recognizer is now recorded
in `etc/research/t96-s8-legacy-endpoint-register-001.md` with its original
owner, target plane, and plane-gated/frozen/rejected disposition.

The shared host-service catalogue now consumes common ingress rather than
decoding BOP bytes again. A generic `legacy_plane_gate_v1` verifies
ingress/registry/DEM-or-COMMAND-plane ownership before the runtime invokes an
identity-known endpoint. All current DEM and COMMAND endpoint entry stages
with a proven service identity are gated. The conflicting old `50:42`
FASTREAD runtime call was removed because S3 proves the original slot is
`demNotYetImplemented`.

Top-level, BIOS, mouse, VDD, EMM, printer and CPU-lifecycle forms retain
their S6/S7 non-adapter disposition. They do not become plane providers just
because a legacy helper remains useful as evidence.

## Verification

- `Test-BopLegacyEndpointRegister.ps1` passed; it verifies 15 frozen direct
  recognizers, 6 permitted architecture-core recognizers, and rejects a new
  unregistered direct BOP recognizer.
- The generic gate compiled and ran under strict Clang flags.
- `bx_ntvdm_adapter_runtime.c` passed Clang syntax checking with the existing
  CLI include surface and `_CRT_SECURE_NO_WARNINGS` (the latter suppresses
  pre-existing UCRT `wcscpy` deprecation diagnostics only).
- Documentation inventory/governance and `git diff --check` passed.

## Handoff

S9 is now the only active packet. It must prove the complete immutable BOP
inventory, ingress routes, provider dispositions, plane coverage, legacy
register and profile outcomes agree without relying on a reached trace.


## m0-t96-s9-closure-20260812.md

# M0 T96 S9 Closure — Full Inventory/Provider/Capability Regression

S9 is complete. The profile regression combines immutable inventory validation
with a compiled all-callable-family chain test. It proves all 192 callable
service identities traverse common ingress, provider registry and their
closed family plane; the two defined sentinels reject. It also validates all
16 top-level selectors and all 349 BIOS table forms as inventory facts.

The result is deliberately not a claim that deferred BOP providers execute.
It proves they have a non-success classification, while original no-op and
explicit-unavailable rules remain auditable. S10 may now concentrate on the
real reachable NTIO path and must report only an admitted runtime result,
not classification success.

