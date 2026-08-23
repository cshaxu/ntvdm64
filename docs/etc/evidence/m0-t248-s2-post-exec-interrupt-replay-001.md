# M0 T248 S2 — post-EXEC selector-blind interrupt replay

## Build and input

The formal graph was freshly generated as `build/M0-T248-S2/formal-r1` from
the tracked full-module manifest, with only:

```text
SoftwareInterruptDiagnostic=true
InterruptReturnDiagnostic=true
MSVC x64 /MT, CPU5/P-MMX
```

The unchanged source-built T247 request then ran once with the opt-in
`--observe-ntdos-exec-entry`, `--observe-software-interrupts`, and
`--observe-interrupt-returns` flags:

```text
ntdos64-native --dos-root build/output/dos --wow16-root build/output/wow16 \
  --mutation-mode direct --instruction-tick-budget 1000000 \
  --observe-ntdos-exec-entry --observe-software-interrupts \
  --observe-interrupt-returns build/output/dos/share.exe
```

No provider, CPU state, RAM, port, device, or guest image was changed.

## Result

The run retained the T247 PDB checkpoint:

```text
ntdos-exec-entry observed=1 read-failed=0 cs=04ea eip=00005a70 pdb=0bf2
  physical=0000bf20 exit=8e08097e ctrl-c=00a710b8 fatal=8e083348
  parent=8df8 environment=0000
```

The fixed 64-record software-INT ring ended with original real-mode guest
activity, including repeated `INT 21h` at `CS:IP=0BF2:110C`, an `INT 16h` at
`06BB:01AC`, `INT 21h AX=4C00h` at `0BF2:0B5D`, and a final retained
`INT 2Fh` at `04EA:530C` (`AX=1122h`). The replay recorded zero IRET
instructions. It then reached the same terminal:

```text
interrupt-returns count=0
terminal=3 detail=6 lifecycle=4 presentation=4 cancellation=0 budget=1000000
```

## Decision

This establishes that the post-`50:36` path is not stuck at a missing
DEM/COMMAND provider: it advances through ordinary guest interrupt traffic.
The zero IRET result is a copied CPU fact, not an assertion of a guest defect;
the observed selector BOP transfers are not necessarily architectural `IRET`
instructions.

The existing terminal-position observer intentionally records only when the
finite watchdog fires. Here it cannot report the exact final `CS:EIP`, because
the stage classified an unexpected `cpu_loop()` return. S3 is therefore
limited to a selector-blind mantle observation correction: when an enabled
terminal-position observer sees the same no-fault/no-stop/no-pending/no-
cancellation CPU-loop return, copy the final CPU position before classifying
it. It must not alter the result code, CPU state, BOP routing, or guest
semantics. The existing budget path remains unchanged.
