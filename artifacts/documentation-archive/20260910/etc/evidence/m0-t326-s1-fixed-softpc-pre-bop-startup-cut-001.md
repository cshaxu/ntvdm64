# M0 T326 S1 — Fixed SoftPC pre-BOP startup cut

## Result

T326 reaches its required original pre-BOP boundary. The unchanged selected
x86 product, run once in the fixed normal container, crosses the original
`NTIO` `50h:11h` route. Static original source resolves that marker to:

```text
ntvdm.c::main
  -> base/support/main.c::host_main
    -> nt_msscs.c::InitialiseDosEmulation
      -> nt_msscs.c::scs_init
      -> selected NTIO.SYS load and CS:IP = 0070:0000
    -> nt_cpu.c::host_start_cpu -> cpu_simulate
      -> nt_bop.c::MS_bop_0
        -> demdisp.c::DemDispatch(0x11)
          -> demmisc.c::demLoadDos
```

`demdisp.c` places `demLoadDos` at service `0x11`; the observed marker is
emitted by `MS_bop_0` immediately before its unchanged `DemDispatch` call.
The subsequent observed BOP markers prove that this dispatch returned to the
original CPU40 continuation. This is startup-continuity evidence only: it
does not claim NTDOS ordinary-program execution, COMMAND completion, `EXEC`,
or parent return.

## Fixed container

- Observer: `tools/observation/ObserveSoftpcStartup.mjs` with the compiled
  `build/tools/console-startup-observer.exe`.
- Container: `console-owning-nondebug`; no debugger, breakpoint, guest-memory
  probe, altered console ownership or changed product argument.
- Timeout: 8,000 ms.
- Product arguments: `-f -o --ordinary-child`.
- Immutable media manifest SHA-256:
  `b46ad6c47e692ba3b1213dd11b9c1a9b3c686410028c3d70e172086b26b62ce2`.
- Selected refreshed x86 product SHA-256:
  `5413790dfade48e8e0168f4cfdf631a53c1d2cfe83e857beefe0942ce5aef419`.

The staged manifest records the DOS and firmware inputs. The product EXE is
the only mutable container member by design; staging copied the rebuilt x86
product into `original-softpc-process.exe` beside the verified inputs.

## Observation

The single fixed observation exited rather than timed out:

```text
container=console-owning-nondebug
result=exited
exit=0xc0000005
timeout-ms=8000
MVDM-BOP-DISPATCH 50:11
MVDM-BOP-DISPATCH 50:3B
MVDM-BOP-DISPATCH 50:0F
MVDM-BOP-DISPATCH 50:1B
... 54:05 ...
MVDM-ORIGINAL-EXCEPTION code=0xc0000005 address=0x00000000 ip=0x00000000
```

The terminal exception is after the named BOP crossing. It is therefore a
successor owner for the minimal DOS guest-execution/controlled-return package,
not a license to repair a BOP leaf or a pre-BOP SoftPC device path.

## Paired source graphs

Both formal product graphs rebuilt and linked `original-softpc-process.exe`:

- x86: `build/M0-T319/S3/termination-x86`, 418 scheduled actions;
- x64: `build/M0-T319/S3/termination-x64`, 418 scheduled actions.

The original fixed-disk source exposed one shared modern-header omission:
`FSCTL_QUERY_FAT_BPB_BUFFER`. The current SDK retains the control code but not
the NT4 response carrier. `adapter-mvdm-host-out/win32/include/ntioapi.h` now
restores the byte-identical original 0x24-byte declaration under existing
`ADAPTER-WIN32-016`; `nt_fdisk.c` remains unchanged. This is a declaration
closure, not fixed-disk success evidence.

## Disposition

T326 is closed. Its planned S2--S4 are not admitted: their purpose was to
recover a pre-BOP owner only if S1 had stopped before the first original
crossing. The direct observation instead reaches that crossing. Queue
candidate 2, **Minimal DOS guest execution and controlled return**, owns the
post-crossing ordinary guest control flow and the reported null-address
exception attribution.
