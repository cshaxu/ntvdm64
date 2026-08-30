# M0 T313 S2 P4 — formal candidate and retired-backend test cleanup

Date: 2026-08-30  
Packet: M0 T313 S2  
Profile: selected Win32/x86 CCPU40.

## Formal candidate

The disposable external formal root `build/M0-T313/S2/r1` completed:

```text
[189/189] ... lib.exe /nologo /out:mvdm-softpc-effective-address.lib ...
```

This includes the already selected original CCPU40, BIOS, system, host-root,
video/C-VID, DEM, COMMAND, XMS/DPMI and corresponding named binding archives.
The log contains no compiler error, linker error, `LNK4006`, or stopped-build
marker.  Historical warnings remain visible and are not suppressed by this
packet.

## Immediate cleanup

The retired `tests/app/m0_t273_s3_machine_shell_fixture.cc` still attempted to
select a nonexistent Bochs backend and expected a former unavailable machine
state.  It was no longer a valid current product test and had no build-graph
consumer, so it was deleted rather than retained as a broken compatibility
fixture.

`tests/session/backend_selection_test.c` now verifies the actual contract:
an unknown backend value is rejected and the sole accepted selection is
SoftPC.  The obsolete app-shell unavailable enum member was removed because
the shell has exactly one admitted backend and no caller consumes that state.

## Boundary

This records broad formal composition and immediate retired-test cleanup only.
It is not proof that the original process entry can yet run a complete
create-to-reset lifecycle; that bounded behavior remains the S2 exit work.
