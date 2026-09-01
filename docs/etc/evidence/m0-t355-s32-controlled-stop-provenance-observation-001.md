# M0 T355 S32 — controlled-stop provenance observation

## Scope

S31 established that the fixed `0x00000078` is a non-local controlled session
stop, but the common completion number could still originate in four selected
unavailable boundaries.  S32 adds no route, guest, media, CPU, timer, BOP or
provider behavior.  It only supplies a default-off copied source-entry label
at the existing `session_terminate_current` boundary.

The label is thread-local, static source text.  It is written only when
`MVDM_SESSION_TERMINATION_REPORT_PATH` is set, immediately before the existing
`longjmp` termination path.  With that variable absent, the product neither
opens nor writes a report and its completion code is unchanged.

## Source result

The original SoftPC BOP carrier is unambiguous:

```text
softpc.new/host/src/nt_bop.c::MS_bop_6
    -> DBGDispatch()
```

The original `mvdm-host/dbg/dbg.c::DBGDispatch` consumes a debugger operation
frame from the VDM stack and dispatches its debugger-product modes.  The
ordinary non-debug profile deliberately does not supply the historical NT4
debugger/CSR transport.  Its same-shaped adapter therefore records
`BACKEND_UNAVAILABLE` and uses the existing controlled-stop contract.  S32
does not reinterpret that contract.

## Formal build and one fixed observation

The formal CPU40/x86 target was rebuilt with the configured MSVC x86
environment and Ninja:

```text
ninja -C build/M0-T355/S20/formal-x86 -j 8 original-softpc-process.exe
```

The rebuilt product was staged in the fixed `O:\ntvdm` runtime root.  The
existing console-owning, non-debug observer ran unchanged with only the
default-off report path supplied:

```text
container=console-owning-nondebug
result=exited
exit=0x00000078
```

Its separate provenance record is:

```text
MVDM-SESSION-TERMINATION origin=debugger:DBGDispatch code=0x00000078
```

The durable observation files remain in the fixed owner-selected runtime
root as `m0-t355-s32-fixed-container-refined.txt` and
`m0-t355-s32-termination-refined.txt`; they are runtime evidence, not source
inputs.

## Disposition

This selects the complete original debugger dispatch cohort as the next
source-first recovery/audit owner.  It does not authorize an unconditional
success, a synthetic debugger reply, an arbitrary `DBGDispatch` no-op, or a
trace-driven one-service patch.  The known missing private debugger/CSR
transport must be classified package-wide before any recovery is admitted.
