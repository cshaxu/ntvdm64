# T95 S7 BOP 50:36 VDD Create-User Notification Audit 001

The r20 source-built runtime trace observes `C4 C4 50 36` at `0032:5a70`,
with the adapter generic listener recording selector `50`, DOS-family service
`36`, then passing it through.  `inc/dossvc.h:146` names service `36h` as
`SVC_DEMENTRYDOSAPP`.  NTDOS calls it in `dos/v86/doskrnl/dos/msproc.asm`
immediately before changing to the DOS application's initial stack and
transferring to its entry point.  At the call, `DS:SI` is that entry point,
`AX:DI` is its initial stack, `DX` is its PDB, and `BX` is initial AX.

`dos/dem/demdisp.c:153` dispatches the service to `demEntryDosApp`.
`dos/dem/demmisc.c:297-318` shows that its non-debug action is solely:

```c
if (!IsFirstCall)
    VDDCreateUserHook(PDB);
```

The original host owner in `softpc.new/host/src/nt_msscs.c:1285-1295` walks
`UserHookHead` and calls each registered `ucr_handler`; it has no result,
guest-memory mutation, device action, interrupt or CPU-state action.  With an
empty list the original loop returns without an effect.  The contained CLI
profile admits no VDD registration/loading capability, so its list is
explicitly empty.  This is a profile restriction, not a claim that VDDs are
unsupported forever.

Therefore adapter service `bx_ntvdm_vdd_create_user_notify_service_v1` owns
only the exact real-mode `C4 C4 50 36` event and returns at RIP+4 with an empty
CPU delta.  It does not inspect the PDB or implement user hooks.  A future
declared VDD capability must replace this endpoint with an explicit adapter
registration contract; it must not add VDD semantics to Bochs or NTDOS.

Malformed instructions, another execution mode, and any other `50h` service
remain generic-listener pass-through.  The companion C11 target verifies the
exact hit plus protected-mode and selector/service mismatch rejection.
