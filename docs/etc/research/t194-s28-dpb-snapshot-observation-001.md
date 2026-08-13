# M0 T194 S28: DPB Snapshot Observation

Packet: M0 T194 S28  
Disposition: one invocation retained; DPB commit observed; controller watchdog closure is permission-blocked.

## Inputs And Procedure

The existing v6 controller was invoked exactly once with the S15 runner/shim,
S27 bundle, and the pinned v6 one-slot profile/root/target. Its fresh evidence
directory is `artifacts/analysis/t194-s28-dpb-snapshot-observation-001`; the
input record pins the S26/S27 engine hash
`4A562EBDCD50E3FD16062013B1A9891FBEE84AB602B120925D46C756D7B01531`.
The requested watchdog was 15 seconds and the controller retry policy was
`none`.

## Observation

The retained stderr log is 11,285 bytes, SHA-256
`DCE4A166A3455F93C4254F4536037310DAE4C582B5B849B498FBA6C4E81B1845`.
It shows the previous startup sequence through `50:0F`, then:

```text
... selector=50 ... service=46
... adapter multi-write transaction committed writes=14 bytes=2a
... selector=15
... selector=50 ... service=0d
... selector=50 ... service=21
... boundary passed through ... bytes=c4c45021
```

Thus the real current derivative commits the `50:46` DPB result and resumes;
the next reached host-service boundary is `50:21`, not the former repeated
`0000:0A84` pattern. This proves only the observed transaction/resume order.
It does not prove a DPB device, filesystem, successful `50:21`, normal COMMAND
return, or continuous execution.

## Watchdog Limitation

After the 15-second deadline, the controller's `taskkill /T /F` received
`ERROR: Access denied` in the current execution environment. The controller
therefore did not finish its normal outcome/exit-code record. A subsequent
process check found no matching runner, shim, engine, Bochs, or wrapper process,
and only the controller's input/wrapper/stdout/stderr files remain. Stdout is
389 bytes, SHA-256
`2D6323D6DBD6FE4B6C63E3F4D6B1AB032ED4CC3F5C93ED5D236323EDFA271DA2`.

This is a controller-permission limitation, not a guest result and not an
authorization to retry. No second invocation was made.

## Follow-up

Before any new run, audit original OpenNT `50:21` (`demIoctl`) and the existing
provider disposition to determine whether it is a source-backed contained
drive-type service or a real device/filesystem requirement. The watchdog
permission issue must also be addressed by a separately admitted controller
mechanics packet; it must not be hidden with an unrecorded retry.
