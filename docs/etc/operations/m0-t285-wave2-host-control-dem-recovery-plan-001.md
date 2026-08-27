# M0 T285 — original host control plane and DEM recovery plan

## Scope

T285 is Wave 2.  It recovers the complete original `mvdm-host/dos/dem`
owner package and the reached, non-executor
`mvdm-host/softpc.new/host/src` control surface through the already named
MVDM-facing adapters.  It never revives SoftPC's CPU/device executor and does
not absorb monitor, COMMAND, XMS, Redirector, WOW, VDD or debugger product
owners.

The current workset is
[`m0-t285-s1-wave2-current-workset.tsv`](m0-t285-s1-wave2-current-workset.tsv):
81 source paths, comprising 22 DEM paths and 59 host-control paths.  The
ledger's disposition counts are 49 `adapter-backed`, five `binding-only`, six
`overlay-required`, and 21 `not-host-runtime`.

## Ordered S sequence

1. **S1 — current workset reconciliation (closed):** regenerate a canonical,
   hash-pinned workset from the existing complete source audit; prove every
   selected path still exists and rebuild selector-disabled original DEM and
   `nt_bop.c` static islands on x86/x64.
2. **S2 — original host-control ingress binding:** inventory every reached
   `nt_bop.c` external call and bind only its source-shaped ingress/control
   declarations to `adapter-mvdm-host-in`, named outgoing adapter families,
   session or an explicit later-owner result.  No selector/provider is enabled
   until the complete binding group has local proof.
3. **S3 — DEM dispatch/data closure:** recover the original dispatch/data and
   declaration group (`dem.c`, `demdata.c`, `demmsg.c`, `demdisp.c`, headers)
   through fixed-width frame/register and bounded guest-memory contracts.
4. **S4 — DEM namespace, file, FCB and handle closure:** recover original
   directory/file/search/FCB paths through the unique session mapping manager,
   original-order Win32 facades and the shared host-resource table.
5. **S5 — DEM device/state/error closure:** recover the remaining original
   DASD, IOCTL, lock, label, error and miscellaneous paths; preserve explicit
   unavailable VDD/FDC/Redirector results rather than creating device policy.
6. **S6 — host capability-control closure:** recover only the 26
   `04-host-capability-control` original host-control rows whose named Win32
   and session contracts are available.  A row that proves a specialist
   VDD/debugger/Redirector/WOW owner remains transferred, not locally rewritten.
7. **S7 — session/event and mechanical-overlay disposition:** complete the
   five session/monitor rows and six machine-transport rows to the extent
   their existing same-shaped façade is available; otherwise make the exact
   later monitor/Bochs owner, failure behavior and non-link selection explicit.
8. **S8 — Wave 2 closure:** rebuild every selected direct static/package
   closure on x86/x64, run per-group focused regressions, verify that all 81
   ledger rows have evidence, and record the exact later-owner transfers.

## Non-negotiable boundaries

- Imported MVDM source calls only the original-shaped adapter declarations;
  it never sees Bochs types or globals.
- `adapter-mvdm-host-in` remains selector-blind; it copies machine events and
  completion results only.
- `adapter-mvdm-host-out/softpc` reaches `adapter-bochs` through typed,
  bounded mechanics only.
- `adapter-mvdm-host-out/win32` supplies a historical call shape over public
  Windows APIs; it never supplants an available OpenNT algorithm.
- A guest address uses only the session `guest_memory` mapping-manager
  instance under a synchronous checked lease.  A host resource uses its
  separate session instance; neither uses numeric identity pass-through.
- A static archive establishes source composition, not a runtime BOP or guest
  execution claim.

## T285 exit standard

The package can close only when every row in the S1 workset has a reviewed,
tested source-composition or explicit non-link/later-owner result; the DEM
owner group has no generic rehost bypass; all direct source groups use the
original caller/ordering/failure contracts through named adapters; and the
full selected x86/x64 static/package matrix passes.  A deferred later-owner
row is valid only if it names that owner, the original unavailable behavior,
and the reason Wave 2 cannot implement it without crossing the boundary.
