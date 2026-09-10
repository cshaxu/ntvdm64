# M0 T318 S2 P35 — Host-application worker and SCS-init prerequisite audit

## Scope

This audit follows the fixed r30 non-debug, console-owning baseline in P34.
It reads the complete original `host_applInit` worker/lock cluster and the
first `scs_init` calls that occur before the NTDOS image-load decision. It does
not alter the fixed observer, product arguments, media, firmware, or timeout.

## Original order and concurrency boundary

The admitted `-f` route in `nt_reset.c::host_applInit` selects the original
host input tables, duplicates the main-thread handle, initializes and enters
the original ICA critical section, then calls `init_host_uis` and
`nt_start_event_thread`.

`nt_start_event_thread` creates its keyboard objects and `ConsoleEventThread`
in the original suspended state. The event worker is not resumed by this
startup path: its first normal resume is `nt_init_event_thread`, reached only
after the guest-side keyboard/ROM readiness route. It is therefore not an
active concurrent predecessor of the first `scs_init` call.

The original `scs_init` order is:

1. `GetNextVDMCommand(NULL)` and the first-session `AddSystemFiles` policy;
2. `CMDInit`;
3. `DemInit`;
4. `XMSInit`;
5. `DBGInit`;
6. original `reset`, which invokes `host_reset` and resumes the heartbeat;
7. `SetupInstallableVDD`, followed by the original `ntio.sys` load decision.

Thus the heartbeat is the only selected worker that may run concurrently with
the later load path. The console event worker and its input wait are not a
valid explanation for a failure before this later resume point.

## Contract review

- `InitializeIcaLock`, `host_ica_lock` and `host_ica_unlock` retain their
  original `RtlInitializeCriticalSection`, `RtlEnterCriticalSection` and
  `RtlLeaveCriticalSection` calls. The ordinary DOS path does not take the
  WOW-only event branch.
- The selected `thread_start_compat` boundary preserves the original cdecl
  worker signatures used by `HeartBeatThread` and `ConsoleEventThread`, while
  the public Win32 thunk supplies the required WINAPI callback ABI. It copies
  only the current session binding into a new worker and unbinds on return; it
  does not pass guest pointers, alter SoftPC fields, or create another
  scheduler.
- `CreateDisplayPalette` is self-contained logical-palette allocation and
  creation. The earlier `SelectPalette(sc.DispDC, ...)` has no dereference of
  an app-owned display object in this windowed console path; its NULL-HDC
  public GDI failure result is ignored by the original source and does not
  establish the observed access violation.
- `GetNextVDMCommand`, `CMDInit`, `DemInit`, `XMSInit` and `DBGInit` remain
  distinct original owner boundaries. They must be evaluated as the SCS
  initialization cluster, not as a new console, BOP, or child-lifecycle
  implementation.

## Disposition

No source edit is admitted from this cluster. The static evidence rules out
the previously suspected active console worker at the point immediately before
`scs_init`, and it confirms that the selected thread/ICA compatibility is
source-shaped for the normal DOS route. It does not attribute r30's
`0xC0000005` to any one provider, nor does it prove NTIO, NTDOS, EXEC or
parent-return reachability.

The next static unit is the complete SCS initialization cohort:
`GetNextVDMCommand`/first-session policy, COMMAND, DEM, XMS and DBG init,
including each original terminate/error edge and its modern adapter contract.
Only a concrete broken contract from that review may justify an affected-lib
Ninja rebuild followed by one unchanged-container observation.
