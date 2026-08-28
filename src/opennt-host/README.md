# opennt-host

This is the original non-MVDM OpenNT host-service mirror. It contains only
complete source-audited packages used by a selected MVDM caller. It is not an
alternate MVDM provider, a compatibility layer, or a general collection of
host utilities.

## Package admission and current package

This root is not limited to BaseSrv/BaseClient. Every separately admitted
non-MVDM OpenNT host package belongs below this root with its original
relative path preserved. A package is admitted only after the shared tracker
records its direct `mvdm-host` caller, complete outgoing boundary, final file
disposition and any required package-private adapter binding. Discovering a
same-named function body is not package admission.

`base/win32` is the first selected package slice. It contains the original
BaseSrv/client VDM command implementation and its direct original declaration
carriers:

- `base/win32/server/srvvdm.c`, `srvvdm.h`, `basesrv.h`, and `srvinit.c`;
- `base/win32/client/vdm.c` and `basedll.h`; and
- `base/win32/inc/base.h`, `basevdm.h`, and `basemsg.h`.

Every file above is byte-identical to its canonical selected OpenNT source.
The source selection, rejected OpenNT-4.5 `vdm.c` candidate and initial
interface dispositions are recorded in
[`m0-t291-s5-p2-basesrv-source-package-audit-001.md`](../../docs/etc/evidence/m0-t291-s5-p2-basesrv-source-package-audit-001.md)
and the shared MVDM/OpenNT host trackers.

The package is a source mirror only at this point. It is not an enabled build
or runtime input until every reached CSR/private-host interface has a named,
same-shaped disposition. A later package-named subfamily under
`adapter-opennt-host` is permitted only for a private interface that the
owning original source proves cannot compose directly; it must not absorb
MVDM or generic Win32 semantics.

## Divergence register

None. No file under this root has been modified.
