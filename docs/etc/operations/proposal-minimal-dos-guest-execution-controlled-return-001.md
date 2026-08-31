# M0 T320 — Minimal DOS guest execution and controlled return

## Purpose

Execute one declared DOS program through the selected original CPU40 SoftPC,
original Base VDM/DEM/COMMAND path, and immutable DOS guest media; then record
its controlled result-return path.  This is the first queue package after
T319.  It does not redefine a reachable startup marker as successful guest
execution.

## Predecessor evidence

- T318 froze the original NTDOS `EXEC -> PSP/arena/JFN/environment -> parent
  restore -> 54:0B` contract and the selected original host route.
- T319 restored original ROM residency and established that the current fixed
  x86 container reaches `50:11`, `50:3B`, `50:0F`, `50:1B` and `54:05`, then
  exits `0xc0000005` outside the original main and known-thread exception
  paths.

Thus the task begins with the source-defined CPU40 raw-worker/execution
prerequisite.  It must not turn a new trace hit into an ad hoc BOP provider.

## S plan

### S1 — CPU40-to-guest execution prerequisite contract

Combine the reached original CPU40 entry and worker paths with the already
frozen NTDOS/COMMAND return contract.  Establish the exact source order from
`host_start_cpu` / `c_cpu_simulate`, raw host thread creation, ROM/BIOS
callbacks and BOP `54:05` to the first guest load/EXEC transition.  Classify
each possible terminal path as original composable code, same-shaped existing
adapter, missing original package, or explicit later owner.  No production
behavior changes occur in S1.

### S2 — Earliest source-shaped prerequisite recovery

Recover only the earliest proven original CPU40 machine/worker prerequisite
that prevents S1's declared DOS program from reaching the frozen guest load
contract.  Prefer an existing original SoftPC call shape and bounded adapter
binding.  Preserve failure behavior; no BOP/guest provider or synthetic device
success is permitted.

### S3 — Declared DOS program and controlled return

Run the fixed console-owning x86 container with unchanged media and one
declared DOS program.  Prove the original guest load and its result/return
boundary, or transfer one exact remaining original owner.  Rebuild/link the
affected CPU40 product under x86 and x64.

## Exit

The task closes only with either:

1. fixed-container evidence of one declared DOS program reaching its original
   controlled return boundary; or
2. a source-defined, narrow original prerequisite transfer that excludes
   invented guest, BOP, or device success behavior.

In both cases x86 and x64 formal graphs must compile/link normally; x64
compile/link is not a claim of x64 guest execution.
