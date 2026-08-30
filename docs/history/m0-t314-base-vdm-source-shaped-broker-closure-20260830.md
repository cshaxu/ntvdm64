# M0 T314 closure — Base VDM source-shaped broker recovery

## Result

`M0 T314 S1-S2` is closed for the admitted, one-session DOS subset of the
original BaseClient/BaseSrv VDM command broker.

- Original BaseClient/BaseSrv sources supplied the public `VDMINFO` ABI,
  copied-record, capacity, environment and no-command/retry contracts.
- The selected adapter retains one copied DOS record and one session-local
  event. A producer wake causes BaseClient's second request with
  `ASKING_FOR_SECOND_TIME`; it never retains a caller, guest or cross-process
  resource.
- First-VDM, reentry, copied current directories and `ExitVDM(FALSE, ...)`
  remain bound-session behavior. `ExitVDM` completes the session rather than
  terminating the host process.
- A fresh x86 Ninja graph passes the focused Base VDM broker fixture.

## Evidence

- [S1 immediate-boundary audit](../etc/evidence/m0-t314-s1-base-vdm-immediate-boundary-audit-001.md)
- [S2 local wake/retry closure](../etc/evidence/m0-t314-s2-local-base-vdm-wake-retry-closure-001.md)

## Explicit limits

This closure does not recreate CSR/CSRSS or kernel VDM transport, a generic
application queue, cross-process BaseSrv records, parent/child wait-pairs,
PIF/startup/title/desktop/standard-handle records, WOWEXEC notifications, or
WOW task lifecycle. Those source branches remain owned by their respective
broker, COMMAND/DEM, Redirector and WOW packages.

## Next admission

The queue's next candidate is **Monitor / `NtVdmControl` user-mode contract
package recovery**. It requires its own Status admission and must not be
inferred from this local Base VDM closure.
