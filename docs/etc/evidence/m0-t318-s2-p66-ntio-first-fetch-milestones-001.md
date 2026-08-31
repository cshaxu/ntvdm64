# M0 T318 S2 P66 — NTIO Load And CCPU First-Fetch Milestones

## Question

Does the immutable, non-debug, console-owning CPU40 runtime container reach
the original NTIO load boundary and begin original CCPU instruction execution,
rather than merely remaining alive in host initialization?

## Inputs

- Selected formal CPU40 product staged by the fixed `runtime-r33-config-system-media`
  container.
- Existing console-owning launcher and eight-second timeout.
- Original `softpc.new/base/support/main.c::InitialiseDosEmulation` and
  `softpc.new/base/ccpu386/c_main.c::{c_cpu_simulate,ccpu}`.

## Procedure

Three registered diagnostic-only milestones write to the existing standard-error
sink after the original `InitialiseDosEmulation` returns, immediately before
the original `ccpu(FALSE)` call, and after the first original `SETUP_HOST_IP`.
They do not read or modify guest memory, CPU state, session state, BOP routing,
or any original return/failure value.

The selected x86 product was rebuilt, staged without changing the fixed media
or observer contract, and observed for eight seconds. The same source change
then completed the x64 formal CPU40 compile/link row; x64 is not a runtime
claim.

## Observation

The x86 console capture records, in order:

```text
MVDM-STARTUP-MILESTONE NTIO-LOADED
MVDM-STARTUP-MILESTONE CPU-SIMULATE
MVDM-STARTUP-MILESTONE CCPU-FIRST-FETCH
```

The bounded observation timed out normally after 8000 ms with the existing
controlled timeout result `0x53504354`. Its stage manifest records product
SHA-256 `5ea9656277eec211018d288a0c614c865722df3c5f56ac931f8b56a201626098`.
The fresh x64 formal Ninja product target also linked successfully (12
incremental actions, ending in `original-softpc-process.exe`).
The retained raw observer records are:

- `artifacts/research/m0-t318-s2-p66-ntio-first-fetch-milestones.txt`
- `artifacts/research/m0-t318-s2-p66-ntio-first-fetch-milestones.txt.console.txt`
- `artifacts/research/m0-t318-s2-p66-ntio-first-fetch-milestones.txt.json`

## Interpretation

This proves the selected original `InitialiseDosEmulation` path completed its
NTIO setup/load decision, `host_start_cpu` reached the original CCPU simulator,
and CCPU performed a first instruction fetch. It does **not** prove a BOP,
NTDOS, `EXEC`, child completion, or parent-return transition.

The next investigation follows the already-selected original BOP/illegal-opcode
path; it must not introduce an app-owned dispatcher.
