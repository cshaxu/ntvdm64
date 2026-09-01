# M0 T352 — CPU40/COMMAND post-`SVC_CMDSETINFO` continuity closure

T352 closes P06B's bounded continuation objective.  The selected original
`54:05` path now has source, formal and durable-runtime evidence:

- original `CmdDispatch(05)`, `cmdSetInfo` and numeric scalar registration
  return in their original call order;
- original `MS_bop_4` executes its original service-byte `setIP(+1)`;
- CPU40 reaches the next original COMMAND ingress, `54:0C`, and that handler
  also returns;
- the accepted run preserves the existing fixed 39-character system-root,
  non-debug console-owning child and eight-second watchdog.

The closure is intentionally narrow.  It does not claim a DOS workload,
NTDOS `EXEC`, a child program, graphics output, or parent return.  The durable
return records and the invalid preliminary report-path observations are
recorded in [T352 S5 evidence](../etc/evidence/m0-t352-s5-command-call-cohort-attribution-001.md).

The successor is the queue-head complete post-`54:0C` COMMAND/NTDOS
continuation package (P06C).  It must begin with source ownership and the
reached guest continuation; it may not treat the `54:0C` trace hit as
authorization for a standalone BOP-provider change.
