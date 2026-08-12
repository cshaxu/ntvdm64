# T166 S1 full CLI native controlled-stop observation 001

## Inputs and procedure

T165's observer launched the complete retained chain once: T124
`ntdos64-run`, T163 engine shim, T160 native Bochs bundle, and the retained T164
v4 BYOB set/`TARGET.COM`. `inputs.json` records all hashes and arguments.

## Observation

The process exited in 881 ms, without watchdog termination, with exit code 1.
Both stream files and result JSON exist. The native log records the exact
terminal sequence:

1. `50:36` reaches the COMMAND guest transfer;
2. the guest calls `C4 C4 FE` at `0D00:0100`;
3. `adapter controlled stop accepted`;
4. `cpu loop quit, shutting down simulator`; then
5. native Bochs reports `quit_sim called with exit code 1` and its historical
   nogui close prompt.

## Interpretation

This proves the full CLI chain reaches the source-classified controlled stop
and returns with durable terminal evidence. Exit 1 is a distinct native Bochs
outer-process contract issue, not proof of a BOP failure, watchdog, or guest
crash. T166 is consumed and must not be rerun.

## Follow-up

T167 audits original exit-source ownership before any contained CLI policy is
considered.
