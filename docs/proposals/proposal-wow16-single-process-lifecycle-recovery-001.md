# Single-process WOW16 application lifecycle recovery

## Objective

Recover the immediate original owner that prevents the selected `WRITE.EXE`
Win16 workload from starting, running, and returning in one Win32/x86 worker.

## Boundary

Following package-completion closure, this candidate owns actual Win16
application behavior within one x86 worker, not a requirement to retain the
former combined launcher/server executable. Broker completion does not prove
this workload; it cannot be used to bypass incomplete package contracts.

Preserve the source-owned low-memory dialog and guest error route. Do not add a
private USER/CSRSS shell, broker transport, CPU tracing policy, or substitute
guest/UI failure behavior.

## Exit evidence

This package verifies only integrated application behavior. DPMI/CPU40
lifecycle, WOW32, debugger, callback, lease, wait and resource owners must
already have package-completion dispositions and their local acceptance before
WRITE is used as integration evidence. A missing package closure is a stop,
not authority to repair one function opportunistically from a runtime trace.

Prove one bounded Win16 start/return workload through its original-owner path,
or record the exact source-proven unavailable boundary and its receiver.
