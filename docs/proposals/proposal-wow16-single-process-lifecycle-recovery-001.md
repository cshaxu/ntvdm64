# Single-process WOW16 application lifecycle recovery

## Objective

Recover the immediate original owner that prevents the selected `WRITE.EXE`
Win16 workload from starting, running, and returning in one Win32/x86 worker.

## Boundary

Preserve the source-owned low-memory dialog and guest error route. Do not add a
private USER/CSRSS shell, broker transport, CPU tracing policy, or substitute
guest/UI failure behavior.

## Exit evidence

Prove one bounded Win16 start/return workload through its original-owner path,
or record the exact source-proven unavailable boundary and its receiver.
