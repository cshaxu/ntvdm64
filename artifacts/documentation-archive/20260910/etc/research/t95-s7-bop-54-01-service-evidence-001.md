# T95 S7 BOP 54:01 Service Evidence

## Implemented Scope

`bx_ntvdm_cmd_get_next_service` is now the sole adapter-side implementation
of the reached `BOP_CMD:01` first-command delivery. It is selected only from
the existing runtime v4 generic gather route. It first gathers exactly the
36-byte copied `CMDINFO`; completion stages one ordinary-RAM multi-write
transaction and a fixed resume result.

The transaction writes only the source-contract fields: `UCOMBUF`, canonical
selected executable path, current drive/count, zero handle/redirector/batch
fields, executable size/type, and prior `CMDSETINFO`'s SCS-sync/DOS-binary
bytes. It clears CF and resumes after four BOP bytes. It requires the
canonical 128/273 COMMAND buffers, a valid `CMDSETINFO` registration, a valid
frozen drive snapshot, selected target namespace entry, and an unconsumed
launch declaration. A second call cannot prepare a read after a queued
delivery.

The path remains path-free: it is reconstructed as DOS `C:\\TARGET.COM` or
`C:\\TARGET.EXE` from adapter-private namespace/launch data. No host path,
file handle, pointer, Bochs API, device or OpenNT host component enters the
service. The declaration maximum is narrowed to 118 tail bytes, the actual
source-derived `UCOMBUF` limit for a `TARGET` command.

## Verification

Fresh root `artifacts/build/t95-s7-cmdgetnext-c11-001`, CMake 3.30 and GCC
16.1.0 (with `C:\\msys64\\ucrt64\\bin` on `PATH`) built and ran only:

```
bx-ntvdm-cmd-get-next-service-test
bx-ntvdm-adapter-runtime-test
```

Both passed:

```
bx-ntvdm CMDGETNEXTCMD: bounded first-command transaction verified
bx-ntvdm-adapter-runtime-test: identity-to-pending-transaction path verified
```

The focused fixture proves the exact 36-byte gather, 12 bounded staged writes,
COM extension type, CF-clear result, and one-shot rejection after commit. The
runtime regression proves the newly linked service preserves the existing
identity-to-pending-transaction lifecycle.

## Remaining Evidence Gate

This is not a guest-runtime success claim. The next action is exactly one
watchdog-bounded native trace with the same profile/target to prove that the
actual `95AB:03C1` observation queues and commits this transaction, and to
classify the next stop. No additional BOP or device is authorized before that
trace.
