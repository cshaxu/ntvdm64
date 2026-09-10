# M0 T265 S7 — Bochs-core retained-diff consolidation

## Question

Can repeated default-off observer scaffolding and the non-product keyboard
creation body leave the adopted Bochs mirror without changing admitted CPU,
opaque-ingress, or headless-8042 mechanics?

## Inputs and procedure

- Compared `src/bochs-core` with pinned Bochs 2.6 at
  `O:\repos.external\bochs-2.6-compat\bochs-2.6`.
- Moved five default-off observer gates/no-op macros into
  `bochs-core-overlay/cpu/observation_gates.h`; adopted paths retain only
  their native mechanical observation calls.
- Moved headless keyboard creation beside the existing private teardown body.
  The original-named `libkeyboard_LTX_plugin_init/fini` remain minimal
  delegations in `iodev/keyboard.cc`.

## Results

- Six adopted files no longer contain duplicate observer gate definitions:
  `access32.cc`, `ctrl_xfer16.cc`, `ctrl_xfer32.cc`, `instr.h`, `soft_int.cc`,
  and `memory.cc`; the shared physical-write macro also leaves `cpu.h`.
- `keyboard.cc` loses the non-product allocation/init body; overlay owns both
  creation and teardown.
- Fresh formal MSVC x64 `/MT` Ninja graph
  `build/M0-T265-S7/r002` built `bochs-core.lib`, `adapter-bochs.lib`,
  `t261-s5-bop-ingress-fixture.exe`, and
  `t244-s2-headless-8042-lifecycle-fixture.exe`.
- Both focused fixtures exited zero. The latter exercised two native 8042
  create/destroy rounds and preserved the `FA`, `AB`, `41` identification
  response sequence. The earlier `r001` build directory is retained only as a failed overlapping-invocation diagnostic; it is not acceptance evidence.

## Interpretation and follow-up

The extraction changes no reachability or enabled feature. Real/V86 CPU
checks, `advance_ip`, #UD hook, checked RAM, PIC and valid product-shell
subsets remain in the adopted mirror because they are actual Bochs decision
points or source subsets, not removable scaffolding.
