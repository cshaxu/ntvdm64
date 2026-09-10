# M0 T363 S3 — Conventional-device formal closure

## Procedure

```text
ninja -C build/M0-T362/S2/x86 original-softpc-process.exe
ninja: no work to do.

ninja -C build/M0-T362/S2/x86 -t query original-softpc-process.exe
```

The target is up to date. The query shows the product links
`original-ccpu386.lib`, `original-softpc-bios.lib`,
`original-softpc-support.lib`, `original-softpc-comms.lib`,
`original-softpc-host-roots.lib`, the selected original MVDM libraries, and
the existing same-shaped host/session bindings in one Win32/x86 process.

Together with S2's source/graph verifier, this proves the immediate CPU40
`c_IOVirtualised -> IOS -> INT 17/LPT` cohort is neither an orphaned archive
member nor a monitor-only import. It is formally present in the selected
product without a new shim or device semantic change.

## Limitation

Formal closure does not claim that a print byte was produced or that optional
dongle/direct-port/host-printer branches are available. Those branches were
not reached and preserve their original failure responsibility. The sole
permitted unchanged workload observation is S4.
