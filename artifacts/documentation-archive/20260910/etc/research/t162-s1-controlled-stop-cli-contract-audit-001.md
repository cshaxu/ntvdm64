# T162 S1 controlled-stop CLI contract audit 001

## Result

T161's `FE` is the declared guest target completion event, but it is not yet a
complete CLI process-result contract.

`QUIT.COM` has SHA-256
`06A37DFF559DF7325DE8B003F4DF53C188F733E0CA312AAD961C34DAE48D7B83` and
exactly `C4 C4 FE`. `quit.asm` says it terminates a VDM; `bop.h` names `FE`
`BOP_UNSIMULATE`, “end execution of code in a vdm.” The adapter accepts only
real-mode vector 6 plus these copied bytes and returns a zero-effect typed
STOP; its focused test rejects `FD` and protected mode. Bochs then uses only
its generic typed stop mechanism.

The guest terminal contract is therefore satisfied. Bochs exit code one is
not a guest failure classification. The retained FE audit proves the outer
Win32 Bochs wrapper can wait on `fgets(stdin)` after CPU-loop return. The
hidden observer's inherited stdin explains the cleanup limitation.

## Disposition

Do not change FE, guest behavior, adapter stop behavior, or Bochs CPU state.
The cohesive successor is a CLI-owned child-stdin and normal-result contract:
noninteractive launch must close/redirect child stdin, await normal return,
and map this proven controlled stop to a CLI result. No host handle crosses
the CLI/adapter ABI.
