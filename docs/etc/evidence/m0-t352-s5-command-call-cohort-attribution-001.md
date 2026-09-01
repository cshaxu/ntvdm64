# M0 T352 S5 — complete COMMAND indirect-call attribution

## Diagnostic cohort

The selected original call order is retained exactly:

1. `MS_bop_4` invokes original `CmdDispatch(05)`;
2. `CmdDispatch` invokes `apfnSVCCmd[5]`, original `cmdSetInfo`;
3. `cmdSetInfo` invokes the existing numeric scalar registration facade;
4. `cmdSetInfo` returns, then the original table call returns;
5. `MS_bop_4` performs original `setIP(+1)` and returns to CPU40.

`MVDM-HOST-DIV-177` adds only durable, fixed-width observation records at the
four internal boundaries.  It neither changes a function pointer, result,
register, guest byte, session state, mapping lease, BOP service, or CPU state.

## Observation validity

The first S5 attempt used the old relative child-report path and is invalid:
the child working directory differs from the observer working directory.  The
observer was corrected to pass an absolute report base.  Its first replacement
used a longer system-root path and exited `0x41`, so it is likewise invalid
against the frozen 39-character bootstrap container.  Both attempts are
retained only as diagnostic limitations.

The accepted replacement used a new 39-character fixed system-root path and
the unchanged eight-second observer semantics.  It timed out normally with
`0x53504354`; its durable child-only report contains, in order:

```text
MVDM-CMD-CALL svc=05 stage=0 ax=032E cf=0
MVDM-CMD-CALL svc=05 stage=2 ax=032E cf=0
MVDM-CMD-CALL svc=05 stage=3 ax=032E cf=0
MVDM-CMD-CALL svc=05 stage=1 ax=032E cf=0
MVDM-BOP-RETURN 54:05 cs=9386 ip=74A1 ax=032E cf=0
MVDM-CMD-CALL svc=0C stage=0 ax=0E1A cf=0
MVDM-CMD-CALL svc=0C stage=1 ax=0E1A cf=0
MVDM-BOP-RETURN 54:0C cs=8E08 ip=0736 ax=0E1A cf=0
```

The staged product SHA-256 is
`2147aec913d99ea588be4f02bc55d28c01f5458f26ff1d9461e285505e37833d`;
the accepted manifest/report/return-report SHA-256 values are respectively
`03de5234e20fca0ea0f25af095f26b345ee54c41eea34b6fa13e5c49df195dd4`,
`a971e5cadf5459be5b152f56ecea656f2c80206eaed32d8053ebae7a00daf87e`,
and `e229ee1cf713063352af4944eb83a3d398c509c61b08e1dcd670f61bc7fd91eb`.

## Result and transfer

The complete original `54:05` COMMAND scalar cohort returns through the
original table, `MS_bop_4`, and CPU40 into the next original COMMAND ingress
`54:0C` (`cmdGetConfigSys`), which also returns.  T352 therefore satisfies
P06B's required next-original-ingress criterion.  It makes no claim that the
subsequent CPU40/NTDOS startup has reached a DOS workload, `EXEC`, graphics,
or parent return.

The next candidate is the complete source-defined post-`54:0C` COMMAND/NTDOS
continuation package, not a leaf BOP repair.
