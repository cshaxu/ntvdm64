# WOW16 `WRITE.EXE` workload recovery

## Objective

Using the preceding CCPU40/V86 guest-contract audit, recover the immediate
original owner that prevents the selected `WRITE.EXE` Win16 workload from
starting, running and returning in one existing Win32/x86 `ntvdm.exe` worker.

## Boundary

This candidate owns actual Win16 application behavior within one x86 worker.
It does not describe a single-process product: `run16.exe`, `basesrv.exe` and
the worker remain separate processes, and the existing multi-worker broker
architecture is neither replaced nor reimplemented. Broker availability alone
does not prove the workload; it cannot be used to bypass an incomplete guest
contract.

Preserve the source-owned low-memory dialog and guest error route. Do not add a
private USER/CSRSS shell, broker transport, CPU tracing policy, or substitute
guest/UI failure behavior.

## Exit evidence

This package verifies integrated application behavior after the contract audit
has identified a concrete immediate owner. DPMI/CPU40 lifecycle, WOW32,
debugger, callback, lease, wait and resource owners must have a complete
disposition and local acceptance before WRITE is used as integration evidence.
If the audit identifies a missing complete owner package, that package is
promoted under the dependency-first rule; this workload task does not repair a
random traced function opportunistically.

Prove one bounded Win16 start/return workload through its original-owner path,
or record the exact source-proven unavailable boundary and its receiver.
