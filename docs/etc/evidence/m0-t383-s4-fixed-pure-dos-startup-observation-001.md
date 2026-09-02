# M0 T383 S4 — fixed pure-DOS startup observation

## Question

Does the current CPU40/SoftPC product reach the first original `54:01`
COMMAND return for the built-in DOS request `EXIT`?

## Fixed inputs

- Product: `build/M0-T371/S2/formal-x86/original-softpc-process.exe`,
  SHA-256 `6df5e9767fa7e383275bf63eab3f8398634ff94a344cb19e7a80e12140722271`.
- Immutable staged media: `O:\ntvdm64`, manifest SHA-256
  `91ade2abe857e876c97336e789370aadc8b614dcd8c783ce37c9288c78a6f011`.
- Existing console-owning, non-debug observer and its fixed eight-second
  timeout.
- One forwarded app request: `--command EXIT`.

## Observation

The observer ended with its controlled timeout (`0x53504354`).  The trace
reached original boot, DEM reads, permanent COMMAND setup and both expected
`54:0F` returns, but no `54:01`.

Unlike the earlier port-frame interpretation, the timed-out primary stack has
an exact earlier original source chain after the second `54:0F`:

```
SoftPC illegal_op_int
  -> host_error
     -> ErrorDialogBox
        -> Win32 wait
```

Rebasing the captured product frames from image base `0x00c80000` to the
formal preferred base maps them respectively to:

- `softpc.new/base/system/illegalp.c::illegal_op_int`;
- `softpc.new/host/src/nt_error.c::host_error`; and
- `softpc.new/host/src/nt_error.c::ErrorDialogBox`.

The original error string captures the fault as
`CS:03f4 IP:2128 OP:63 6f 64 65 78`.  The attempted opcode is `0x63`, followed
by ASCII bytes `codex`; this is not a normal `54:01` delivery failure and the
modal original error path prevents COMMAND from continuing.

## Disposition

T383 is closed: its complete COMMAND/Base VDM startup cohort compiled and
passed its local original-caller proof, while its one allowed product run
identified an earlier CPU40 control-flow/illegal-operation owner.  No
COMMAND, Base VDM, printer, guest-media or `cmdExec32` source was changed.

The next dependency-first package is a full CPU40 illegal-operation and
error-path audit.  It must establish why guest execution reaches
`03f4:2128`, whether it is a valid original control path, a missing CPU40
instruction/interrupt contract, or preceding guest-state corruption.  It may
not turn the original error dialog into an unconditional continue/ignore.
