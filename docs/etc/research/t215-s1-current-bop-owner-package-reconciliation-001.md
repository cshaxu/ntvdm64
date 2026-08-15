# T215 S1: Current BOP Owner-Package Reconciliation

## Question

After current-profile composition admission reaches CPU entry, what is the
current, source-backed disposition of every OpenNT BOP identity, and which
whole owner package should be admitted next without trace-led patching?

## Inputs And Procedure

- Re-exported the pinned historical dispatcher/header inventory with
  `tools/Export-T200S31BopSourceInventory.ps1`.
- Inspected current ingress/registry, bound boot namespace composition, native
  XMS/DPMI composition, and DEM/COMMAND/Redirector facades.
- Reconciled prior owner-package closures T202, T205, T206, T210, T211 and
  T214 against those current sources.

The generated artifact is
`artifacts/reports/t215-s1-current-bop-source-inventory.json`. It contains
exactly 203 identities: DEM 73, WOW 1, XMS 12, DPMI 25, COMMAND 17,
Debugger 16, Redirector 50 and top-level/machine 9.  It remains the
per-selector/service coverage set; this record corrects only its stale
current-composition descriptions.

## Family Reconciliation

| Family | Original owner | Current package disposition | Completion meaning / limitation |
| --- | --- | --- | --- |
| DEM `50:00..48` | `demdisp.c` / DEM file, FCB, GSET, error and device owners | Every identity has a DEM package route, original no-op, FASTREAD compatibility, or explicit source-shaped outcome after T202. | Coverage/disposition closure only. The default direct-host file/handle/FCB/search implementation is still incomplete; fixture/readonly legacy paths cannot be credited as real-host DOS filesystem recovery. |
| WOW `51` | `nt_bop.c`, WOW DLL composition | Deferred typed stop. | Correct non-success for a non-WOW profile; no WOW host composition. |
| XMS `52:00..0B` | `xmsdisp.c`, XMS/A20/memory owners | T205 closes all callable identities: `00..05`, `0A`, `0B` have the admitted session; `06..08` UMB and `09` INT15 are explicit typed deferrals. | Full current-profile disposition, not a device/BIOS/continuous-execution claim. |
| DPMI `53:00..18` | `dpmi32` protected-mode/LDT owners | All 25 are deferred through the native package. | Complete deferred disposition, not DPMI. |
| COMMAND `54:00..10` | `cmddisp.c`, COMMAND component owners | T211 closes bootstrap/config, launch/execution, console/keyboard and lifecycle as one package; `10` remains explicit deferred. | Complete package disposition, not interactive console, NT broker or full host-child queue. |
| Debugger `56` | `nt_bop.c` / debugger services | Deferred typed stop. | Correct non-debug safeguard; no debugger profile. |
| Redirector `57:00..31` | `rdrsvc.h`, `VDMREDIR` | All 50 route to the package-owned unavailable facade. | Complete unavailable family disposition; no network/IPC implementation. |
| Top-level/machine | `bop.h`, `nt_bop.c`, machine owners | Named terminal/no-op/config/machine dispositions. | Each remains with its historical engine or machine owner; none implies an adapter-owned machine replacement. |

## Corrections To The T200 Baseline

T200 accurately establishes the 203-identity inventory and owner/profile
admission basis, but its then-current composition table predates later
package closures.  The following statements are superseded for current-state
planning:

- XMS is no longer merely a partial `00/02/03/05/0B` candidate: T205 gives
  every callable `52` identity an implemented or explicit deferred result.
- COMMAND is no longer endpoint-local evidence: T211 provides exactly one
  component owner for all 17 callable entries, including `09/0E` and
  lifecycle.
- T214 removes the former profile-admission blocker before CPU entry. Its
  controlled terminal is observation only and does not select the next BOP.

These corrections do not promote DEM legacy route coverage, DPMI stops or
Redirector unavailability into functional recovery.

## Next Whole Package

Admit **DEM direct-host file/handle/FCB/search recovery** next. Its scope is
the original DEM namespace/content and opaque-handle owners, including the
file, directory, FCB, DTA and search contracts that make a default admitted
real host drive usable by DOS.  It must use the shared mutation profile
(direct/readonly/overlay/virtual), preserve original DOS error/layout
semantics, and replace—not layer beside—fixture/readonly fallback behavior.

This has higher dependency value than interactive COMMAND, Redirector, DPMI
or WOW: those packages cannot provide a normal CLI DOS workload while its
fundamental selected-host filesystem behavior remains incomplete. Before any
implementation, its next S must map original provider sources, copied guest
ABI, host API availability, readonly/overlay/virtual failures and the full
family regression matrix.

## Confidence And Limit

High confidence for identity coverage and family ownership: the generator
enforces the historical array cardinalities and every family is checked
against current facade/session source and closed package records. Medium
confidence for the next package boundary until its file/FCB/handle source and
ABI map is separately admitted; no trace was used to infer that boundary.
