# M0 T310 S8 P4 — CCPU vector-tail default closure

## Question

How can the selected original `SasVector` retain its complete initializer on
both host widths when two tail entries have no selected x86 SoftPC definition?

## Source findings

- `mvdm-host/softpc.new/base/ccpu386/sascdef.c` declares and installs
  `c_sas_touch` and `c_VirtualiseInstruction` in the original final two
  `SasVector` slots.
- The only original generated declarations and call macros found in the
  imported OpenNT source family are the MIPS/PPC `host/inc/*/{pig,prod}/sas4gen.h`
  carriers.  No selected x86 CCPU source calls either macro or function.
- `ccpusas4.c` owns the selected x86 CCPU alternatives for physical and
  linear byte access (`c_GetPhyAdd`, `c_GetLinAdd`) and for BIOS instruction
  virtualization (`biosDoInst -> host_simulate`).  Fabricating a successful
  result from either missing vector tail would therefore alter a distinct
  CCPU/BIOS contract.

## Disposition

The original NTVDMx64 `fmstubs.c` fallback is the correct limited disposition:
an unexpected vector-tail call immediately triggers `__debugbreak()` and
returns only the source patch's unreachable fallback value.  It is not an
adapter implementation and it has no mapping-manager or Bochs route.

`MVDM_SOFTPC_PATCH_CCPU_VECTOR_DEFAULTS_ONLY` compiles exactly three required
symbols: `EDL_fast_bop`, `c_sas_touch`, and `c_VirtualiseInstruction`.  It
excludes the rest of the imported patch's unrelated placeholders.  The latter
two definitions use the generated `IU32, IUH, IU32, IU32` and `IU32, IU32`
forms rather than the patch's accidental 32-bit `UINT` spelling, so the
vector's native-word size argument has one x86/x64 ABI.

## Verification

After regenerating the formal Ninja graph, both commands completed:

```text
ninja -C build/M0-T310/S8/p1-machine-source/x64 \
  ntvdmx64-softpc-ccpu-vector-defaults.lib
ninja -C build/M0-T310/S8/p1-machine-source/x86 \
  ntvdmx64-softpc-ccpu-vector-defaults.lib
```

Both archives compiled the three-symbol restricted object.  The only emitted
diagnostics were pre-existing `host_def.h` macro redefinitions and intentional
unreferenced parameters in debug-break defaults; neither is a pointer-width,
function-pointer, parameter-width, or calling-convention mismatch.  A later
S8 forced-link run must prove these three symbols disappear from its unresolved
list; that remains separate from functional recovery of a vector-tail call.

## Scope limit

This closes only the complete-vector linkage/default contract.  It does not
claim a reachable `c_sas_touch` or `c_VirtualiseInstruction` operation has been
recovered.  If a selected x86 profile reaches either entry, the immediate
debug-break is the required evidence stop; recovery needs a separately proven
CCPU physical-memory or `biosDoInst -> host_simulate` transaction.
