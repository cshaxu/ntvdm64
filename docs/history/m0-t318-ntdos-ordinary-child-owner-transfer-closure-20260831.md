# M0 T318 — NTDOS Ordinary-Child Owner-Transfer Closure

## Result

T318 is closed by its declared transfer exit, not by a claim that the ordinary
child ran.  The original `NTIO -> SVC_DEMLOADDOS -> NTDOS` route, the guest
`EXEC`/parent-return ownership, and the reached DEM/COMMAND contracts were
preserved and evidenced.  No app loader, replacement executor, BOP route, or
guest-image modification was introduced.

The first fixed-container native failure is outside T318's admitted scope:
the original SoftPC floppy wait performs a recursive CPU40 `host_simulate()`
and transfers to address zero.  It is a machine-owner prerequisite before
the declared child, `EXEC`, or parent return can be observed.

## Closed evidence

- [S1 guest/host contract freeze](../etc/evidence/m0-t318-s1-ntdos-exec-parent-return-contract-freeze-001.md)
- [P75 post-command guest-bootstrap owner cohort](../etc/evidence/m0-t318-s2-p75-post-cmdsetinfo-guest-device-bootstrap-owner-cohort-001.md)
- [P76 floppy recursive-simulation transfer](../etc/evidence/m0-t318-s2-p76-floppy-recursive-simulation-owner-transfer-001.md)

## Transfer

`M0 T319` owns the smallest original SoftPC machine cohort required to
continue the frozen slice: FDC wait/interrupt completion, the reached
`int15`/`wait_int` guest-vector initialization, and recursive CPU40
simulation/return.  It must retain original failure behavior and may not
replace the guest device call, add BOP semantics, or synthesize a successful
floppy completion.  A later result returns to DOS-child continuity only after
this cohort has source-shaped evidence.
