# M0 T355 S34 — original ordinary debugger dispatch recovery

## Recovered source contract

The original `mvdm-host/dbg/dbg.c::DBGDispatch` was previously represented by
one over-broad unavailable adapter.  S33 proved the reached startup caller is
Win16 `WOWNotifyTHHOOK`, which pushes `DBG_TOOLHELP` and a 16:16 hook frame.

S34 restores the ordinary non-debug source disposition in the debugger
adapter:

- `DBGInit` and `DBGNotifyDebugged` share the original `fDebugged` state;
- `DBGDispatch` reads the original mode first from current CPU40 `SS:SP`;
- mode `DBG_TOOLHELP` reads the original four 16-bit words in source order and
  retains its hook/f386 values;
- source modes whose original non-debug paths deliver no event retain that
  no-event outcome; modes whose original helper returns false retain `AX=0`;
- a genuinely debugged event remains an explicit private debugger/CSR
  transport unavailable terminal.

The only divergence is replacing the historic durable `Sim32GetVDMPointer`
alias with a bounded synchronous session guest-memory lease.  The selected
original CPU40 descriptor resolver supplies the numeric linear address; no
guest pointer survives the call and no host pointer is published.

## Formal verification

The formal CPU40/x86 graph was regenerated and linked successfully:

```text
ninja -C build/M0-T355/S20/formal-x86 -j 8 original-softpc-process.exe
```

The new dispatcher and state units compile without the earlier fixed-width
`uint32_t*`/`IU32*` contract warning.

## One fixed observation

The unchanged console-owning, non-debug fixed container ran the staged
product in `O:\ntvdm` for its established eight-second window.  Its result:

```text
container=console-owning-nondebug
result=timeout
exit=0x53504354
```

No S34 termination provenance report was emitted: the prior
`debugger:DBGDispatch` / `0x00000078` controlled stop did not occur.  The
same bounded BOP observation reached later original COMMAND/DEM activity,
including `54:01`, `54:0E`, `54:04` and subsequent DEM services, before the
watchdog ended the still-live product.  This is continuity evidence only; it
does not claim COMMAND, DOS EXEC or a guest workload has completed.

## Remaining boundary

The historical debugged branch still requires `SendVDMEvent` /
`STATUS_VDM_EVENT` delivery through the NT4 debugger/CSR product.  It remains
explicitly unavailable.  The next runtime investigation must be a whole
COMMAND command-acquisition/waiting cohort, not a trace-triggered individual
BOP patch.
