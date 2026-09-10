# M0 T310 S18 P1 — host system-interrupt formal assembly

## Recovery result

The selected host-system interrupt boundary preserves original call shapes and
source order. Heartbeat creation, event initialization, timing and alertable
teardown remain original public Win32/NTDLL forms. `NtAlertThread` is the sole
private NT4 call; its existing same-shaped adapter queues an APC only to wake
the original alertable wait. `nt_inthk.c` retains the original local CCPU
registration tables rather than forwarding callbacks to BaseSrv or Bochs.

Thread handles and controller-local counters are not guest fixed-width ABI
values, so no mapping-manager token is required here. Actual guest/resource
crossings remain with their own machine or endpoint package.

## Formal selected x86 assembly

The current formal CCPU40 graph compiled the exact selected original host
timer, EOI, interrupt-hook and reset sources with the thread-alert adapter:

```text
ninja -C build/M0-T310/S16/original-softpc-ccpu40-s16p5 original-softpc-candidate
# 369/369 successful
```

The forced-link audit contains no unowned S18 implementation gap. Its later
platform/UI, BaseVDM/BOP, device endpoint, VDD and Redirector edges are not
runtime evidence and stay with their named owners.

## Deferred integration

This closes source bindings only. S49 must validate the selected lifecycle
without MONITOR, kernel VDM or Bochs fallback.
