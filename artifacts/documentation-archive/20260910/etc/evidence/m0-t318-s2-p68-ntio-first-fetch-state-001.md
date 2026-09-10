# M0 T318 S2 P68 — NTIO First-Fetch State

## Question

Does original CPU40 CCPU enter the original `NTIO.SYS` image with the real-mode
16-bit state that its first instructions require, or is the pre-BOP stall an
incorrect CPU entry-state problem?

## Source Route

Original `InitialiseDosEmulation` loads `NTIO.SYS` at `0070:0000`, then calls
the original `setCS(0x70)` and `setIP(0)`.  The original CCPU loop reaches
`SETUP_HOST_IP` before its generic `NEXT_INST` dispatch.  NTIO begins with a
16-bit relative jump; its first original `0x50:0x11` BOP is later in the image
and therefore cannot be a prerequisite of this state check.

## Procedure

The existing registered DIV-164 diagnostic seam records, once and only after
the original `SETUP_HOST_IP`, the already-observed `CS:EIP`, `MSW`, and code
segment default-operand-size bit (`CSX`).  It does not alter the CPU, guest
memory, BOP route, session, or stop behavior.

The same fixed, console-owning, non-debug CPU40 container was used.  Its media
manifest remained immutable; only the rebuilt formal x86 product executable
was substituted.

## Observation

The capture reports:

```text
MVDM-STARTUP-MILESTONE NTIO-LOADED
MVDM-STARTUP-MILESTONE CPU-SIMULATE
MVDM-CPU-STATE 0070:00000000 MSW=0010 CSX=0
```

`CS=0070`, `EIP=0`, and `CSX=0` establish the expected 16-bit NTIO entry.
`MSW=0010` retains the normal real-mode coprocessor bit; it is not protected
mode.  The probe deliberately records no BOP conclusion.
The raw records are:

- `artifacts/research/m0-t318-s2-p68-ntio-first-fetch-state.txt`
- `artifacts/research/m0-t318-s2-p68-ntio-first-fetch-state.txt.console.txt`
- `artifacts/research/m0-t318-s2-p68-ntio-first-fetch-state.txt.json`

## Interpretation

The evidence rejects an incorrect segment, instruction-pointer, protected-mode
or default-operand-size entry explanation.  It is not evidence against DEM or
a particular BOP service, and admits no BOP implementation.  The hot-path
state reporter was removed after this capture because later fixed-console
experiments showed that additional writes can perturb the observable startup
trace; P68 remains valid only for the state it actually records.
