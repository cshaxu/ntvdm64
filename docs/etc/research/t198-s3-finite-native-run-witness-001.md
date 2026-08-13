# T198 S3 Finite Native Run Witness 001

## Result

The new CPU5/Pentium-MMX finite mantle helper source-builds and runs a neutral
bare-byte fixture to the native PC-time stop latch, then cleans up normally.
This closes only the mechanical run contract required before BOP composition.
It is not evidence of a booting guest, BIOS, DOS, OpenNT provider or BOP call.

## Exact Witness

- Build root:
  `artifacts/build/t198-s3-finite-native-run-probe-r8`.
- Manifest:
  `t198-s3-finite-native-run-probe.json`, `passed: true`, link and run exit
  codes both zero.
- Configuration: pinned MSVC x86 CPU5/Pentium-MMX projection, non-x86-64.
- Source closure: the T197 whole-CPU5 core/mantle source list plus only
  `src/bx-mantle/bx_ntvdm_finite_run.cc` and the generated fixture main.
- Fixture: one `HLT` byte at physical `0x1000`, entered as real-mode
  `0100:0000`, at one million IPS with a 64 PC-time instruction-tick budget.

The captured run log records CPU reset, Pentium-MMX CPUID initialization and
ordinary-RAM block allocation.  The zero exit result denotes
`BX_NTVDM_FINITE_RUN_COMPLETED_BUDGET`: the helper's one-shot callback set
only the native `kill_bochs_request` latch, `cpu_loop` returned through its
existing asynchronous-event path, the timer was unregistered, and machine
cleanup completed.

## Repair Found During The Witness

The first linked execution attempt (`r6`) returned `rejected_input` before
entering `cpu_loop`.  The checked ordinary-RAM copier correctly rejected the
write because T197's direct CPU reset had not initialized the native PC A20
mapping state.  `bx_ntvdm_minimal_machine_c::initialize` now performs:

```text
CPU initialize -> PC A20 enable -> CPU hardware reset
```

This is a mantle lifecycle repair using the existing native PC/memory owner;
it enables no device, firmware, GUI, BOP or adapter behavior.  r7 then proved
the checked RAM allocation/write path.  Its `NOP; JMP $` trace shape exposed
a stack-overflow in the adopted trace path, which is not a contract requirement
and remains unclaimed.  The accepted r8 witness uses native `HLT` waiting to
let PC-time drive the bounded stop without relying on that loop trace shape.

## Boundary Checks

- `tests/bx-ntvdm-minimal-machine-boundary.ps1` verifies the A20 lifecycle
  ordering along with the existing finite machine ownership constraints.
- `tests/bx-ntvdm-finite-run-boundary.ps1` verifies fixed-width private
  fields, checked RAM copy, real-mode entry, PC-time callback/stop/unregister,
  and rejects adapter/OpenNT/DOS/WOW/BOP/GUI/product inputs.
- The r8 manifest forbids `main.cc`, product configuration, GUI, device
  archives, adapter, OpenNT, BOP and CLI inputs.

## Limitation And Follow-Up

The 64-unit value is a native PC-time tick budget, not a guarantee of exactly
64 architecturally retired instructions.  The accepted helper owns opaque
bytes only.  T198 S4 must inspect the existing generic #UD event seam and its
object/link dependencies before any `bx-vdm` ingress is composed with this
run mechanism.
