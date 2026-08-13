# T198 S8 Mechanical Action Boundary Witness 001

## Question

Can the pending read/write work required by the selected finite boot-namespace
family cross from `bx-vdm` to the native mantle without reversing the declared
dependency direction or attaching any provider?

## Inputs

- `src/bx-vdm/bx_ntvdm_pending_action_v1.{h,c}` at `551608f`, inspected as an
  adapter-local descriptor draft.
- New `src/bx-mantle/bx_ntvdm_mechanical_action_v1.{h,cc}`, registered first
  as `BX-MANTLE-068`.
- Existing CPU5/Pentium-MMX/non-x86-64 seed command:
  `tools/Invoke-T197S6MinimalMachineLinkProbe.ps1 -WholeCpu5Core`.
- `tools/Invoke-T198S3FiniteNativeRunProbe.ps1 -MechanicalActionProbe` and
  `tests/bx-vdm/Test-MantleMechanicalActionBoundary.ps1`.
- i686 MinGW GCC 16.1.0 and
  `tests/bx-vdm/bx_ntvdm_mechanical_action_v1_abi_test.c`.

## Procedure

1. Compared the first adapter-local descriptor with the architecture direction
   `bx-vdm -> bx-mantle -> bx-core`.  The draft imports VDM result, range and
   multi-write contracts, so a mantle consumer of it would create the
   prohibited reverse dependency.
2. Defined a mantle-owned v1 record containing only an action id, an operation
   kind, physical ranges, offsets and a copied bounded byte array.  The record
   contains no service identity, provider choice, host object, callback or
   retained pointer.
3. Implemented the sole consumer in the mantle.  It validates all layout and
   arithmetic constraints, preflights every range with the existing ordinary
   RAM primitive, then copies all declared ranges.  A read copies into the
   record; a write consumes its copied bytes.
4. Built the existing whole CPU5 seed plus only the new mantle object.  The
   generated direct fixture performed a valid write at `0x800`, attempted a
   two-range write whose second range began at `0x100000`, then read `0x800`
   back.  It exits nonzero unless the invalid operation fails and all four
   original bytes remain unchanged.
5. Compiled and ran the C layout witness with i686 MinGW.  It proves the C
   consumer sees a 16-byte range, `ranges` at offset 32, `payload` at offset
   1056 and a 66,592-byte v1 record.

## Observations

The native run at
`artifacts/build/t198-s8-mechanical-action-r1/` completed with link exit 0 and
run exit 0.  Its manifest identifies x86, CPU5/Pentium-MMX and excludes
`main.cc`, `config.cc`, GUI, `bochs.exe`, device archives, OpenNT and CLI.
`link.map` resolves all three new action symbols from `mechanical_action.obj`.

| Artifact | SHA-256 |
| --- | --- |
| `mechanical_action.obj` | `01D358AD9FFD080FCDA77CAAC712429C0C0C7F2B19AAD59E6656734859AB3F42` |
| native action fixture | `E1E1EE5C201D5ED34D5FDEF1E341905AED9C1C59B5C1127A211E836F10CDC6C5` |

The source boundary scan passed.  The i686 executable is `PE32` Intel i386 and
exited 0.  The previous `bx_ntvdm_pending_action_v1` remains an adapter-local
descriptor validation aid; it is not an input to the mantle and does not
define the cross-layer ABI.

## Interpretation And Confidence

High confidence for the mechanical boundary: the native Bochs owner performs
only preflighted ordinary-RAM copies, and the action crosses only from
`bx-vdm` toward the mantle.  This is not a boot-provider, BOP, CLI, host
filesystem or continuous-guest-execution result.  The provider must later
translate its source-owned operation/result objects into this record and
interpret the copied read result before returning a typed generic outcome.

## Follow-up

Add the adapter-side conversion/completion layer only after it can be shown to
remain synchronous, fixed-width and provider-neutral.  Then compose the
whole finite boot namespace as one provider family; do not attach isolated
services to the action consumer.
