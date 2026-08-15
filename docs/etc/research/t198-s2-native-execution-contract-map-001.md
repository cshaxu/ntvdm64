# T198 S2 Native Execution Contract Map 001

## Question

What is the smallest `bx-mantle`-owned execution contract that can run fixed
bare bytes after the T197 CPU5 reset lifecycle and return in a controlled,
selector-blind way, without restoring the Bochs product shell?

## Inputs And Procedure

- T197 r12 CPU5 native lifecycle witness and its finite machine sources.
- `src/bx-core/cpu/cpu.cc`, `event.cc`, and `init.cc`.
- `src/bx-core/memory/memory.h` and `misc_mem.cc`.
- `src/bx-mantle/pc_system.h` and `pc_system.cc`.
- The rejected historical execution owner, `refs/bochs/main.cc`.

The review traced the native reset, entry, time delivery and loop-return
paths.  It deliberately did not execute guest bytes, link a new object, or
enable a device, firmware image, BOP ingress, adapter object or CLI surface.

## Source Facts

| Need | Native source fact | Contract consequence |
| --- | --- | --- |
| Reset baseline | `bx_ntvdm_minimal_machine_c::initialize` creates finite SIM, RAM and default port space, then calls `BX_CPU_C::initialize` and hardware `reset`. | Reuse this single machine lifecycle; a run cannot begin before successful initialization. |
| Input placement | `BX_MEM_C::ordinary_ram_writable` validates the complete span before `copy_to_ordinary_ram` copies it; it never falls back to device writes. | Copy a fixed byte span only after whole-span ordinary-RAM validation. |
| Entry | `BX_CPU_C::apply_real_mode_entry(Bit16u, Bit32u)` changes only checked real-mode CS:RIP after reset. | Entry is an explicit `CS:EIP` value, not a BIOS/ROM reset vector or a provider continuation. |
| Time source | Single-CPU `BX_SYNC_TIME_IF_SINGLE_PROCESSOR` invokes `BX_TICK1` after execution; `BX_TICKN` also occurs for repeated/string work. PC-time dispatches registered tick callbacks through `countdownEvent`. | The finite bound is a **native PC-time instruction-tick budget**, not a claim of an exact retired-instruction count. |
| Loop stop | `BX_CPU_C::handleAsyncEvent` returns to `cpu_loop`'s caller when `bx_pc_system.kill_bochs_request` is set. | A mantle-owned one-shot tick callback may set only this existing machine stop latch. |
| Product alternative | `main.cc` wraps `cpu_loop` in the full product startup/GUI/device/configuration lifetime. `benchmarkTimer` also sets `bx_user_quit` alongside the stop latch. | Do not use `main.cc` or `benchmarkTimer`; use a dedicated mantle-private callback that has no GUI/product effect. |

## Proposed Private Contract

The next implementation may place a C++-only declaration beside
`bx_ntvdm_minimal_machine_c`.  It is not an adapter, CLI or OpenNT ABI.

| Field / result | Type | Meaning | Prohibited meaning |
| --- | --- | --- | --- |
| `entry_bytes`, `entry_byte_count` | `const Bit8u *`, `Bit64u` | Caller-owned fixed bare image, copied synchronously during the call. | Guest pointer retained after return, host-capability object, BOP bytes by interpretation. |
| `entry_physical_address` | `bx_phy_address` | Destination ordinary-RAM address. | ROM, device aperture or BIOS selection. |
| `entry_cs`, `entry_eip` | `Bit16u`, `Bit32u` | Explicit real-mode entry after reset. | DOS/NTVDM continuation or selector/service identity. |
| `instruction_tick_budget` | `Bit64u` | Positive one-shot PC-time tick budget. | Exact retired-instruction count, wall-clock timeout or unbounded run. |
| `ips` | `Bit32u` | Positive PC-time initialization rate needed by native timer accounting. | General Bochs configuration. |
| result enum | fixed C++ enum | `completed_budget`, `rejected_input`, `machine_error`, or `unexpected_loop_return`. | Adapter result, provider status or host error propagation. |

No field denotes a BOP selector/service, OpenNT provider, DOS object, host
handle, adapter pointer, firmware, device or GUI choice.  The byte content is
opaque to mantle: any future BOP interpretation remains solely the already
separate `bx-vdm` plane.

## State Model

```text
fresh
  -> initialized
  -> input-preflight
  -> ordinary-RAM-copy
  -> real-mode-entry
  -> pc-time-initialized
  -> one-shot-stop-armed
  -> cpu-loop
  -> completed-budget | unexpected-loop-return
  -> cleanup

Any preflight, copy, timer-registration, or lifecycle failure -> machine_error
```

The run is one-shot because the current finite machine records its first
initialization attempt and owns its RAM/port-space cleanup.  The callback
must be deactivated or unregistered before cleanup.  A stopped loop is a
mechanical outcome only; it makes no guest boot, BIOS, DOS, BOP or continuous
execution claim.

## Build Placement And Rejections

The helper, its private request/result types and its one-shot callback belong
in `src/bx-mantle`; it links only to the already closed whole CPU5 core and
finite mantle.  It must not change CPU decode/exception source, add an
adapter dependency, or use `refs/bochs/main.cc`.

The following alternatives are rejected:

- passing a numeric budget into `BX_CPU_C::cpu_loop` (would alter adopted CPU
  semantics);
- using `benchmarkTimer` (couples the bare run to `bx_user_quit` product
  state);
- using a GUI/watchdog, BIOS, PIC, ROM or device to stop execution;
- treating a tick budget as an exact instruction-retirement guarantee;
- placing this run contract in `bx-vdm` or interpreting its bytes there.

## Conclusion And Follow-Up

The native source closes the contract design without a new Bochs-core
intrusion.  The precise next slice is a small mantle-only implementation and
fresh CPU5 link witness for the state model above.  It may demonstrate a
bare finite run such as a fixed `NOP; JMP $` image, but it must not attach the
BOP plane until that neutral mechanic is independently verified.
