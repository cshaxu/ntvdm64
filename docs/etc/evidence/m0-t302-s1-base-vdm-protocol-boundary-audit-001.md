# M0 T302 S1 — Base VDM command-protocol boundary audit

## Question

Can the first shared MVDM host command protocol be recovered from original
BaseClient/BaseSrv source without recreating the NT4 CSRSS/CSR product shell or
leaving the app/monitor callback path as a parallel protocol owner?

## Inputs

- Exact mirror comparisons for `base/win32/client/vdm.c`,
  `base/win32/server/srvvdm.c`, `srvvdm.h` and `basemsg.h` against the selected
  OpenNT source. All four hashes matched their selected original paths.
- Original `GetNextVDMCommand` at `vdm.c:340` and
  `BaseSrvGetNextVDMCommand` at `srvvdm.c:130`.
- The existing app command source and monitor facade.
- [T301 closure](../../history/m0-t301-closure-20260828.md).

## Procedure

Read only the direct client/server command-protocol bodies, their request
records and their named direct helper declarations/definitions. Classified the
protocol as client capture, local server record selection/copy, capacity retry,
wait/wake, completion, WOW/PIF and product-shell transport. Did not inspect
unrelated MVDM one-degree candidates, enable a BOP or make a machine change.

## Observation

- The selected client and server files are byte-exact mirrors, so no existing
  mirror diff must be retained for this package.
- `GetNextVDMCommand` already contains the correct observable request sizing,
  captured-copy, wait/retry and capacity-result order. Its obstruction is only
  the historical CSR transport and 32-bit pointer-bearing capture layout.
- `BaseSrvGetNextVDMCommand` contains the matching source algorithm for DOS
  record selection, no-command wait, environment-only copy and result copy.
  It is coupled to CSRSS console/DOS/WOW record shells and duplicated-handle
  transport, not to a missing command algorithm.
- The existing app/monitor implementation repeats a limited subset of this
  logic through a one-callback binding. It omits server record/wait semantics
  and is therefore a migration source, not a permanent protocol owner.

## Decision

Use the original client body through a same-shaped local capture/dispatch
facade. Rehost only the reached server record algorithm in the named
`adapter-mvdm-host-out/basesrv` family, retaining its field order, status,
capacity, retry and no-delete semantics. Public Win32 event/wait mechanics are
behind the existing `win32` adapter family; a session owns bounded local record
state. The `app` component supplies a declared command as a producer only.

The full dispositions are in:

- [protocol boundary ledger](../operations/m0-t302-s1-base-vdm-protocol-boundary-ledger.tsv)
- [existing seam ledger](../operations/m0-t302-s1-base-vdm-existing-seam-disposition-ledger.tsv)

## Explicit limits

CSR/CSRSS LPC, remote handle duplication, global first-VDM discovery,
cross-process broker behavior, WOW task wakeup and PIF console behavior remain
outside this S. The next implementation S may implement only the local
single-session DOS path plus original-shaped capacity/retry/no-command and
reentry contracts. It may not create a generic CSR facade, a callback-only
command dispatcher or a new BOP route.
