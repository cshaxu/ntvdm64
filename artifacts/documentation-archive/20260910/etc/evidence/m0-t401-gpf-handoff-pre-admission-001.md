# M0 T401 — Kernel31 GPF handoff pre-admission audit

## Question

Can the ordinary WRITE General Protection Fault be located or repaired through
the already-composed `BOP_DEBUGGER` / `DBG_GPFAULT` path without introducing a
debugger transport, guest mutation, trace, or CPU/DPMI semantic change?

## Procedure

Read the original Kernel31 fault handler, original host debugger dispatcher,
the current debugger adapter, and the ordinary WOW debug-state propagation.
No program, guest media, runtime environment, or running process was changed.

## Original contract

`kernel31/ldint.asm` declares `FAULTSTACKFRAME`: the protected-mode fault
frame retains saved register words, a previous-handler far return, the DPMI
return pair, error code, faulting `IP:CS`, flags, and faulting `SP:SS` (lines
36–53). `GPFault` is one of the fatal `ExceptionHandler` macro entries
(lines 997–1013). `ExceptionHandlerProc` constructs that frame and calls
`FaultFilter` (lines 926–956).

For a ring-3 LDT fault, `FaultFilter` first offers a registered per-API GP
handler. If none claims it, the only debugger transfer is conditional:

```
test DebugWOW,DW_DEBUG
jz   ff_no_wdebug
xor  ax,ax
push DBG_GPFAULT
FBOP BOP_DEBUGGER,,FastBop
```

(`ldint.asm:517–560`). A nonzero AX means the debugger handled the fault;
otherwise the original `HandleFault` formats an Application Error dialog from
the fault frame and re-points the DPMI return to `KillApp`
(`ldint.asm:824–956`).

The original host `dbg.c` shows why the BOP is not a passive observer:
`DBGDispatch` derives an `FFRAME16` from the live VDM stack, and `GPFault`
constructs a mutable `VDMCONTEXT`, calls the private debugger event delivery,
then writes the context register values back into `FFRAME16`
(`dbg.c:46–75`, `753–929`, `1455–1465`). It is a debugger control-and-resume
contract, not an observation ABI.

## Current composition

The current `adapter-mvdm-host-out/debugger` deliberately preserves the
ordinary no-debug direction. `DBGInit` queries the process debug port and
records false when none is present (`source/dbg_init.c:35–55`);
`DBGDispatch` returns AX=0 for `DBG_GPFAULT` in that state
(`source/dbg_dispatch.c:80–100`). The corresponding original WOW handoff only
sets the guest `DebugWOW` bit after it sees an actual debug port and then calls
`DBGNotifyDebugged(TRUE)` (`mvdm-host/wow32/wow32.c:1877–1921`). Kernel16
initializes `DebugWOW` to zero before that notification
(`kernel31/wow16cal.asm:1135–1178`).

The S2 clean run was explicitly non-debug and displayed Kernel31's
`Application Error` GPF dialog. That is consistent with the ordinary source
route: no `BOP_DEBUGGER` is reached; Kernel31 retains its own fault frame,
formats the dialog, and sends the task to `KillApp`.

## Disposition

There is no stable, ordinary fault-frame bridge at this boundary. Enabling the
debug bit, forwarding `DBG_GPFAULT`, or returning a synthetic handled result
would change the original route and require the unavailable private NT4 VDM
debugger transport. Reading an arbitrary fault frame by selector/IP or CPU
trace would repeat the already-rejected observation approach.

Consequently this audit selects **no repair** and specifically rejects a
debugger-BOP, DPMI, CPU, or WRITE-source response. The next admissible frontier
must be a non-debug, source-owned contract before the fault—rather than the
post-fault debugger interface.
