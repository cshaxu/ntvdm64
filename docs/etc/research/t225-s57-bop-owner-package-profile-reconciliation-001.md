# M0 T225 S57 BOP owner/package and profile reconciliation

## Question

Does every retained OpenNT BOP endpoint have one present owner, current
executable route or explicit absence, source-recovery rung, Direct/Readonly/
Overlay disposition, and a non-ambiguous action for any workaround?

## Inputs and method

- The retained OpenNT dispatch/header sources, exported by
  `Export-T200S31BopSourceInventory.ps1`.
- Current ingress and composition sources: the generic UD bridge, boot and
  native composition, and every DEM, COMMAND, XMS, DPMI, Redirector,
  Debugger, top-level and machine facade/session.
- Prior T200/T219 route records, treated as evidence to be rechecked rather
  than as current authority.
- The reproducible
  `Export-T225S57BopReconciliation.ps1` generator and its 203-row
  [ledger](t225-s57-bop-owner-package-profile-ledger-001.json).

The generator first invokes the frozen source-inventory exporter.  It rejects
any count other than 203, duplicate identity, or a blank owner, route, rung,
mode disposition, workaround action, or implementation-status field.  It
does not execute a guest or infer a provider from a trace.

## Reconciled inventory

| Owner package | Endpoint count | Current conclusion |
| --- | ---: | --- |
| DEM namespace/file/FCB/search/state | 73 | The generic ingress reaches one DEM session.  Its direct whole-provider, drive view, retained bootstrap paths and CLI-unavailable fallback are real code routes, but individual historical ABI/failure closure is unfinished. |
| COMMAND bootstrap/launch/console/environment/lifecycle | 17 | One COMMAND session owns every callable slot.  Its limited and deferred outcomes are package-owned, not evidence that COMMAND is closed. |
| XMS/A20/allocator/UMB/INT15 | 12 | The session binds selected A20 and extended-memory mechanics; UMB/INT15 members stop explicitly.  This is partial mechanical recovery, not an XMS implementation claim. |
| DPMI protected-mode/LDT/exception/memory | 25 | Every member reaches the DPMI session's explicit deferred stop; no protected-mode provider is claimed. |
| Redirector | 50 | One reachable family-wide original-shaped unavailable response is retained; it is not a network implementation. |
| Debugger | 16 | The stack-form selector reaches the deferred non-debug stop. |
| WOW | 1 | The selector reaches the deferred top-level outcome; no WOW host composition is present. |
| Top-level/machine/BIOS | 9 | Individual engine/machine facades have bounded memory, handoff, config, idle or explicit-deferred outcomes.  No general machine provider is inferred. |

## Profile and fallback decision

Direct and Readonly are the present product modes.  The ledger therefore
records host-backed DEM and COMMAND rows as current direct-capability
candidates and requires source-shaped readonly read/refusal behavior to be
proven by their complete owner-package regressions.  Overlay is recorded only
as an ABI-compatible deferred extension; it neither falls through to Direct
nor selects retained virtual-volume behavior.  Device, network, debugger,
WOW, DPMI and machine outcomes are not silently treated as mutation modes.

The audit also fixes the implementation vocabulary: a mapped ingress, a
typed resume, and a typed stop are route facts only.  They do not prove an
original provider has been recovered.  Existing fallbacks are classified as
retain, migrate/reconcile, replace after regression, or delete only when the
replacement package has closed; none may become a new endpoint-local patch.

## Verification

On the current worktree the commands below passed:

```powershell
powershell -ExecutionPolicy Bypass -File tools/governance/Export-T225S57BopReconciliation.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -OutputPath docs\etc\research\t225-s57-bop-owner-package-profile-ledger-001.json

powershell -ExecutionPolicy Bypass -File tests/bx-vdm/Test-BopInventoryProfileRegression.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64
```

The first reports 203 unique reconciliation rows.  The second independently
reports 16 selectors, 195 service identities and 349 historical BIOS forms.
The 203 rows split as DEM 73, COMMAND 17, XMS 12, DPMI 25, Redirector 50,
Debugger 16, WOW 1 and top-level/machine 9.

## Result and next package

S57 is an audit closure, not a BOP implementation closure.  It selects the
whole **DEM process-resource ownership and cleanup package** as the next
work: recover the original PDB/JFT/SFT resource contract used by `demfile.c`
and `demmisc.c`, including `50:3C` termination ordering, under the four-rung
source-recovery rule.  That package must cover its complete resource family
and Direct/Readonly regression before one native trace is used for integration
observation.  It may not select a service from the current BOUND observation.
