# M0 T291 S5 P5 — Base VDM external-file final disposition

## Question

What is the final product disposition of each current external OpenNT
evidence-import file, once the Base VDM service package is bounded above CSR
and below selected MVDM consumers?

## Inputs

- complete-package boundary ledger `OPENNT-BOUNDARY-0001..0008`;
- Base VDM vertical-slice audit;
- exact current external file rows; and
- original `base/win32` and SDK source/declaration files.

## File conclusions

- `base/win32/inc/base.h`, `basevdm.h` and `basemsg.h` are exact
  declaration/layout carriers for the selected VDM protocol. They are
  **binding-only**, do not add a host provider and remain at their current
  original paths.
- `base/win32/server/basesrv.h`, `srvvdm.h` and
  `client/basedll.h` are exact package-private declaration carriers required
  by the selected server/client bodies. They are **binding-only**. Unreached
  declarations do not constitute an enabled BaseSrv/BaseClient service.
- `base/win32/server/srvvdm.c` is the selected original server-side VDM
  state-machine body. Its final form is a **true original subset,
  adapter-backed** by `adapter-opennt-host`; its CsrSrv process/context and
  event operations stop at the bounded same-shaped adapter. The original
  business/control ordering remains in the mirror subset.
- `base/win32/client/vdm.c` is the selected original VDM client body. Its
  final form is a **true original subset, adapter-backed** by
  `adapter-opennt-host`, retaining the reached `GetNextVDMCommand`, `ExitVDM`,
  `SetVDMCurrentDirectories` and admitted `RegisterWowExec` forms. CSR capture
  and dispatch remain adapter boundaries; unrelated BaseClient functions are
  not selected.
- `base/win32/server/srvinit.c` is **not-host-runtime**. Its broader BaseSrv
  dispatch is useful package-boundary evidence only and must leave the
  production `opennt-host` mirror before the static-closure task admits a
  runtime source list.
- `public/sdk/inc/ntcsrsrv.h` is **binding-only** in
  `mvdm-platform-abi`: it is the exact declaration carrier for the former CSR
  boundary, not an approval to link NTDLL CSR transport.

## Finite closure

The selected VDM client/server bodies reach only their named
`adapter-opennt-host` same-shaped CSR/context/event boundary, public modern
process/event primitives where their historical contract permits, session and
broker records with copied fixed-width data, and the existing
`adapter-mvdm-host-out` facade where a selected MVDM caller already owns the
other side. They do not call into a new CSRSS, NTDLL CSR transport, kernel VDM
or a whole BaseClient/BaseSrv product.

## Reversion implication

`app/command_source.c` and
`adapter-mvdm-host-out/monitor/source/vdm_command.c` remain **not-yet-
eligible** reversion candidates. The next Base VDM recovery task must prove the
original producer/consumer transaction before deleting either current route.

## Follow-up

The canonical static-closure task must physically select the two original
source subsets with `DIVERGENCE:` crop markers and component README entries,
and move `srvinit.c` to retained evidence. This audit does not edit or enable
those bodies.
