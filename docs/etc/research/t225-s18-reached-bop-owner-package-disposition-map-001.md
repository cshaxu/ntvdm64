# M0 T225 S18 Reached BOP Owner-Package Disposition Map 001

## Question

Which original owner package owns every distinct BOP endpoint reached by the
bounded S17 primary run, what route actually accepts it today, and which
Direct/Readonly/Overlay disposition follows without treating trace order as
an implementation backlog?

## Inputs and method

- S17's immutable source-built Direct and Readonly observations,
  [external generic-UD bridge composition result](../evidence/t225-s17-external-generic-ud-bridge-composition-result-001.md):
  33 identical accepted BOP records per mode.
- Original top-level dispatch:
  `src/opennt/overlay/base/mvdm/softpc.new/host/src/nt_bop.c`.
- Original service tables:
  `src/opennt/base/mvdm/dos/dem/demdisp.c` and
  `src/opennt/base/mvdm/dos/command/cmddisp.c`.
- Current generic ingress/registry and bound composition:
  `src/bx-vdm/bx_ntvdm_bop_ingress_v1.c`,
  `src/bx-vdm/bx_ntvdm_bop_provider_registry_v1.c`,
  `src/bx-vdm/bx_ntvdm_adapter_runtime.c`, and package-session sources.
- The complete DEM, COMMAND and current-route ledgers:
  [T199 S37 DEM matrix](t199-s37-dem-package-provider-matrix-001.md),
  [T199 S43 COMMAND ledger](t199-s43-command-family-owner-disposition-ledger-001.md),
  and [T221 S3 P29 current-route audit](t221-s3-t200-current-bound-route-audit-001.md).

This is a source-only map. It neither reruns the trace nor changes a provider,
Bochs machine configuration, guest byte, profile, or BOP result.

## Normalized reached set

The following counts are the 33-record S17 sequence collapsed by exact
selector/service identity. `R` means the observed Direct and Readonly route
accepted a typed resume; it is **not** a claim that the complete historical
component is recovered.

| Owner package | Reached identity and count | Original owner | Current bound route | Direct / Readonly / Overlay posture |
| --- | --- | --- | --- | --- |
| DEM bootstrap/misc | `50:11` x1 | `demLoadDos`, `demmisc.c` | bounded NTDOS input transaction | immutable guest input / same / ABI retained but unselected |
| DEM bootstrap/misc | `50:3B` x1 | `demIsDebug`, `demmisc.c` | no-debug source-derived false branch | contained / contained / deferred |
| DEM GSET/state | `50:0D` x1 | `demGetBootDrive`, `demgset.c` | immutable captured-drive provider | captured policy / captured policy / deferred |
| DEM GSET/state | `50:0F` x1 | `demGetDrives`, `demgset.c` | immutable captured-drive provider | captured policy / captured policy / deferred |
| DEM GSET/state | `50:1B` x1 | `demSetDTALocation`, `demgset.c` | checked locator registration | adapter-private state / same / deferred |
| DEM GSET/state | `50:21` x3 | table's `demNotYetImplemented` `SVC_GETDRIVEINFO` slot | original no-op provider | no host mutation / same / same |
| DEM GSET/state | `50:46` x1 | `demGetDPBList`, `demgset.c` | captured-drive DPB transaction | captured policy / captured policy / deferred |
| DEM error/lock | `50:32` x1 | `demSetHardErrorInfo`, `demerror.c` | checked hard-error locator registration | adapter-private state / same / deferred |
| DEM namespace/handle | `50:12` x4 | `demOpen`, `demfile.c` | package-owned boot/whole-provider open seam | admitted host view / readonly host view / ABI retained, unselected |
| DEM namespace/handle | `50:00` x2 | `demChgFilePtr`, `demhndl.c` | package-owned seek seam | admitted host view / readonly host view / deferred |
| DEM namespace/handle | `50:16` x3 | `demRead`, `demhndl.c` | checked bulk-read transaction | admitted host view / readonly host view / deferred |
| DEM namespace/handle | `50:02` x3 | `demClose`, `demhndl.c` | package-owned close/session seam | admitted host view / readonly host view / deferred |
| DEM misc/lifecycle | `50:3E` x1 | `demWOWFiles`, `demmisc.c` | no-WOW contained misc disposition | no-WOW resume / same / deferred |
| DEM misc/lifecycle | `50:36` x1 | `demEntryDosApp`, `demmisc.c` | no-VDD void lifecycle provider | void notification / same / deferred |
| COMMAND bootstrap/session | `54:05` x1 | `cmdSetInfo`, `cmdmisc.c` | copied CMD/DOS locator registration | adapter-private registration / same / deferred |
| COMMAND configuration | `54:0C` x1 | `cmdGetConfigSys`, `cmdconf.c` | immutable boot-file transaction | declared guest input / same / deferred |
| SoftPC BIOS/memory | `12` x1 | `BIOS[12] -> memory_size`, `mem_size.c` | profile-bound conventional-memory result | fixed machine profile / same / not a host-mutation mode |
| SoftPC BIOS/memory | `15` x1, `AH=88` | historical `cassette_io`, but reached caller is `sysinit1.asm` extended-memory query | bounded AH=88 profile result only | fixed machine profile / same / not a general INT 15 implementation |
| SoftPC BIOS/interrupt composition | `5F` x1 | `MS_bop_F -> kb_setup_vectors` | bounded stream-IO completion in existing composition | machine-composition shim / same / not profile-owned |
| SoftPC BIOS/EMS profile | `66` x1 | `InitSpcEmm` no-EMS branch | explicit no-EMS result (`BX=0`) | machine profile terminal / same / not profile-owned |
| SoftPC BIOS/mouse composition | `C8` x1 | `nt_mouse.c -> mouse_install1` | checked mouse-table to INT 33h mapping transaction | headless mapping only / same / not profile-owned |
| SoftPC BIOS/printer profile | `17` x1 | `printer_io` | compiled-out-printer continuation | no-device continuation / same / not profile-owned |
| NT host top level | `5E` x1 | `MS_bop_E` notification (`AL=0`) | config-complete continuation | no host mutation / same / deferred |

All 23 identities are accounted for: 14 DEM identities account for 24
observations, two COMMAND identities account for two, and seven machine or
top-level identities account for seven; together with the repeated DEM calls
they total 33. The S17 per-mode equality proves only that the same selected
Direct/Readonly routes were reached.

## Current-route assessment

The ingress and provider registry classify a family before a package chooses
its provider. That is the correct durable boundary. The historical direct
runtime still contains several older exact-window entry points in
`bx_ntvdm_adapter_runtime.c` (`50:11`, GSET registrations, the COMMAND
registration/configuration pair, and machine selectors). They remain
source-visible compatibility routes, but they are not a license to add more
leaf recognizers.

The required treatment is therefore:

- retain a route only when the current package session or a documented narrow
  source-derived terminal contract proves its layout, order, and failure rule;
- migrate retained legacy route code into its original owner package when that
  package is next reconciled;
- replace a synthetic/profile-shaped fallback with the source-owned
  Direct/Readonly capability path where one exists; and
- delete a fallback only after its package replacement has a regression.

In particular, `50:36` is a void DEM notification, not program launch; `5F`
is a machine-composition operation, not a DEM service; and `15/AH=88` is a
bounded memory query, not activation of the historical cassette handler.

## Package result and next admission

No reached service becomes the next task merely because it appears first or
repeats most often. The next owner-scoped implementation package is the
**whole DEM host-service package**, beginning with a source/ABI reconciliation
of its existing namespace/FCB, GSET/state, error/lock, raw-media and misc
subproviders into the single package session. It must cover all 73 callable
`50:00..48` slots with one of: reusable original provider, source-derived
rehost, original no-op/failure, or explicitly deferred capability. Direct and
Readonly must use the shared capability ABI; Overlay stays extension-only and
must not fall through to Direct.

COMMAND remains a distinct complete 17-service package after DEM; XMS, DPMI,
Redirector, debugger, and the SoftPC BIOS/machine set remain separately owned
packages. The latter must be recovered under bx-core/bx-mantle ownership or a
bounded bx-vdm machine-composition contract, never by moving their semantics
into DEM or COMMAND.

## Confidence and limits

High confidence applies to source-table ownership, exact S17 identity/count,
and the listed current source routes. Moderate confidence applies to the
long-term migration shape of legacy shims because it needs the next package's
full regression before deletion. Nothing here establishes a runnable DOS
session, whole DEM closure, whole COMMAND closure, general BIOS emulation, or
Overlay implementation.