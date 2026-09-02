# M0 T372 — Redirector and host-resource execution closure

## Delivered scope

T372 completed the source-first whole-package recovery audit for original
OpenNT `vdmredir`: the original provider source bodies, parent import ABI,
local lifecycle/named-pipe/mailslot/async groups, NetLib helper subset and
public/private network owner boundaries were reconciled without inventing a
replacement Redirector provider.

The formal CPU40/x86 candidate compiles the selected original provider archive.
The selected runnable profile intentionally does not package `VDMREDIR.dll`;
the original `MS_bop_7` dynamic loader therefore retains its source-defined
`ERROR_INVALID_FUNCTION` result if a guest later reaches `57:xx`.

## Evidence

* S1 source/topology: `docs/etc/evidence/m0-t372-s1-redirector-runtime-topology-001.md`.
* S2 source-first interface closure: `docs/etc/evidence/m0-t372-s2-redirector-source-first-interface-closure-001.md`.
* S3 parent/DLL ABI: `docs/etc/evidence/m0-t372-s3-p1-redirector-dll-import-topology-001.md` and
  `docs/etc/evidence/m0-t372-s3-p2-parent-softpc-import-abi-001.md`.
* S4 local completion groups: `docs/etc/evidence/m0-t372-s4-p1-local-completion-source-build-001.md`,
  `m0-t372-s4-p2-formal-cpu40-regression-001.md`, and
  `m0-t372-s4-p3-original-termination-cleanup-001.md`.
* S5 network owner boundaries: `docs/etc/evidence/m0-t372-s5-p1-network-owner-import-disposition-001.md`
  through `m0-t372-s5-p6-network-owner-package-boundary-closure-001.md`.
* S6 whole-family dynamic disposition: `docs/etc/evidence/m0-t372-s6-p1-original-dynamic-load-dispatch-reconciliation-001.md`.
* S6 one frozen observation: `docs/etc/evidence/m0-t372-s6-p2-frozen-redirector-observation-001.md`.

## Verified limits

* `RpcXlate` RAP/FSCTL and XACTSRV remain explicit source-proven unavailable
  owner packages, not hand-written network shims.
* The sole S6 fixed `LOADFIX.COM` observation never reached `57:xx`; it ended
  at an earlier original COMMAND/Base VDM continuation timeout.
* No conclusion is made about named-pipe, mailslot, NetBIOS, DLC, remote
  network, COMMAND child, DOS child or WOW runtime execution.

## Next work

Future work may select a different dependency-ordered owner package from
`docs/QUEUE.md`. A later Redirector activation requires a new admitted packet
that provides a complete source-defined DLL product disposition; it may not
silently static-link `VrDispatch` into `nt_bop.c` or treat this closure as a
network-runtime claim.
