# M0 T317 Closure — DEM/COMMAND SoftPC vertical slice

## Result

T317 is closed. Its three source-first cohorts established a bounded original
DEM/COMMAND-to-SoftPC vertical slice under the production Win32/x86 CCPU40
profile:

- S1 recorded the complete reached DEM/COMMAND cohorts and their ordered
  original/adapter/explicit-unavailable boundaries.
- S2 formally composed the original COMMAND bootstrap units and recovered the
  copied local Base VDM request/capacity/wake/retry/re-entry contract.
- S3 formally composed `cmdexec.c`, `cmdexit.c`, `cmdredir.c` and `cmdpif.c`.
  It preserves the original child-process order, child-only standard streams,
  session-mapped redirection records, detached cdecl worker context, and
  balanced Base VDM re-entry count.

## Evidence

- [S1 boundary ledger](../etc/evidence/m0-t317-s1-dem-command-softpc-boundary-ledger-001.md)
- [S2 closure](../etc/evidence/m0-t317-s2-command-bootstrap-cohort-closure-001.md)
- [S3 P1 child streams](../etc/evidence/m0-t317-s3-p1-command-child-stream-boundary-001.md)
- [S3 P2 worker context](../etc/evidence/m0-t317-s3-p2-command-worker-base-vdm-context-001.md)
- [S3 P3 exit/redirection](../etc/evidence/m0-t317-s3-p3-command-exit-redirection-disposition-001.md)

## Deliberate limits and transfers

This is not guest execution completion. The following remain explicitly
unclaimed and transfer by original owner:

- DOS `EXEC`, PSP creation, parent restoration and ordinary guest return;
- PIF/ForceDos/console product behavior;
- remote or named-pipe Redirector behavior;
- WOW child/broker behavior;
- CSRSS/BaseSrv server transport and multi-session broker behavior.

No CPU30/V86, Bochs, alternate executor, raw guest pointer, native handle in
MVDM state, or COMMAND-private dispatcher was admitted.
