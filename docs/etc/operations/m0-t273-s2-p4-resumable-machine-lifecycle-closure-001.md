# M0 T273 S2 P4: resumable minimal-machine lifecycle closure

## Objective

Close the P3 gap without importing the old general runtime stage: retain one
minimal Bochs machine across bounded runs, then destroy it deterministically.
The interface remains wholly mechanical and does not express a guest service,
host provider, selector, process, session or historical SoftPC contract.

## Candidate audit and disposition

`src.old/adapter-softpc/machine_stage.cc` proved that the prior implementation
already used a retained machine lifecycle, but its body also owns cancellation,
physical IRQ polling, instruction observations, compatibility state and
mechanical-action policy.  Those are not construction/resume prerequisites and
were not copied.

The retained subset is instead represented by the new project-owned
`adapter-bochs/machine_lifecycle.{h,cc}` API:

- `create` owns one opaque 1 MiB-capable native machine and initializes its
  native timing base.
- `load_realmode` copies opaque instruction bytes to ordinary RAM and applies
  a real-mode CS:EIP entry.
- `run_budget` owns a one-shot native timer.  Its callback requests a typed
  CPU stop; before a subsequent run it clears only that adapter-owned stop
  latch, preserving the existing machine, CPU and RAM.
- `destroy` releases the same machine.  No native Bochs type crosses the C
  boundary.

`machine_facade_prepare_cpu_resume` is the one additional private operation.
It changes only `kill_bochs_request`; the native CPU event loop clears the
asynchronous stop indication when the next loop begins.  It does not rewrite
registers, memory, devices or pending interrupts.

## Verification

The focused fixture places `EB FE` at `0000:0000`, creates one 1 MiB machine,
runs for 64 timer ticks, confirms the machine remains active, runs a second
64-tick slice without reload/reconstruction, then destroys it.

- MSVC x64 `/MT`: incremental compilation of the new lifecycle, changed
  facade and fixture, link into the existing 149-object P3 closure, and native
  execution all succeeded (`exit 0`).
- MSVC Win32/x86 `/MT`: the same incremental compile/link/run sequence
  succeeded (`exit 0`).

Both outputs remain disposable below `build/M0-T273-S2/r005*`.  Neither probe
loads firmware or a guest image, decodes a BOP, imports OpenNT, or activates a
device/product shell.

## Result and next boundary

The S2 lifecycle exit condition now has direct construction/reset/bounded-stop
and retained-resume evidence on both supported host widths.  The remaining
T273 work is S3/S4: an app-owned opaque machine handle and a whole-boundary
audit.  Those later steps may consume this API but must not add host-service
semantics to it.
