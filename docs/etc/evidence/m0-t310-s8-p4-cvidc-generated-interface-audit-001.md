# M0 T310 S8 P4 — CVIDC generated-interface ABI audit

## Question

Can the selected original `softpc.new/base/cvidc` generated C-video backend
be enabled safely on both supported host architectures, or do its generated
dispatch tables retain an x86-only calling-contract assumption?

## Inputs

- Exact `mvdm-host/softpc.new/base/cvidc` sources, notably `evidgen.h`,
  `vglfunc.c`, `evidfunc.c`, `SINIT011.c` through `SINIT013.c`, and
  `SEVID000.c` through `SEVID030.c`.
- Exact CCPU/video declarations in `softpc.new/base/inc/cpu_vid.h` and
  `softpc.new/base/ccpu386/evidgen.h`.
- Original configuration carrier `softpc.new/obj.vdm/cdefine.inc`.
- Formal MSVC `/MT` Ninja graphs below `build/M0-T310/S8/p1-machine-source/`.

## Procedure

1. Restored the original `C_VID` profile selection in the formal graph.
   `cdefine.inc` selects `-DC_VID` for both historical x86 and non-x86 CCPU
   paths; omitting it compiled out `ev_glue.c` providers and created false
   same-package unresolved forms.
2. Rebuilt the complete 39-translation-unit original `cvidc` package on x86
   and x64.
3. Compared the generated `VideoVector` declaration to the `C_Video`
   initializer and inspected the actual generated `S_*` definitions.
4. Counted compiler-reported incompatible function-pointer initializers by
   source unit.

## Observations

- `cvidc/evidgen.h` declares **81** `VideoVector` slots. `vglfunc.c`
  initializes **78** and deliberately leaves the final
  `setWritePointers`, `setReadPointers`, and `setMarkPointers` slots zero.
  That tail is original data, not a missing project implementation.
- Each `vglfunc.c` `S_*` declaration is the historical generic form:
  `IHPE S_... IFN0()` or `void S_... IFN1(IHPE, value)`.
  The actual generated body, for example
  `S_0363_CiGetVideolatches_00000000` in `SINIT011.c`, is a four-argument
  `IUH` rule dispatcher.
- `evidfunc.c` has **692** direct generated `S_*` table entries. The
  `EVID_WRT_POINTERS` and related table declarations retain typed source
  interfaces such as `(IU8, IU32)`; they are not interchangeable with the
  generic dispatcher signature.
- With the original profile restored, both formal x86 and x64 CVideo archives
  compile. Their relevant diagnostics are nevertheless real ABI evidence:
  x86 reports 167 incompatible function-pointer initializers and x64 reports
  188. On x64, 110 originate in `evidfunc.c` and 78 in `vglfunc.c`.
- The difference is not merely a missing `C_VID` definition or a linker
  ordering issue. C function-pointer type compatibility is not restored by
  widening `IHPE`; a caller using the typed vector cannot safely invoke a
  generic four-register dispatcher through a mismatched C function pointer.
- `IHPE` is already correctly bound to `UINT_PTR` for native private SoftPC
  addresses. `IUH` remains 32-bit despite its original "host register sized"
  meaning, so the generated dispatcher itself needs a separate x86/x64 width
  audit before it can carry a native pointer argument.
- There are two different original generated headers named `evidgen.h`:
  `base/cvidc/evidgen.h` declares the 81-slot C-video vector, while
  `base/ccpu386/evidgen.h` declares a five-slot vector.  They cannot be made
  interchangeable merely by global include-directory order.  The actual
  original build selected generated headers per translation-unit/package;
  the modern graph must recover that selection before any thunk carrier is
  admitted.

## Decision

`cvidc` is selected original source and may remain in the formal static graph,
but it is **not runtime-closed** on either host architecture. This is a
single P4 display/backend issue, not a reason to replace CCPU/video with
Bochs or to create display semantics in an adapter.

The approved recovery shape is:

1. Recover the original per-package generated-header selection and prove the
   CCPU/video visible vector layouts before changing a table definition.
2. Audit every generated table slot from the original `evidgen.h` and
   initializer input, recording exact return/argument kinds and its target
   `S_*` body.  The tracked ledger now contains 773 such rows: 81 vector
   slots and 692 EVID entries, with no missing generated definition.
3. Determine, from original `IUH` use, which generic arguments carry a guest
   scalar, a private native SoftPC pointer, or an actual guest physical
   address. Only the latter uses the existing mapping-manager physical
   binding; private controller pointers stay private native-width values.
4. Generate one private `mvdm-host-overlay` typed-thunk source from those
   original inputs. It supplies exact C function types, performs only the
   proven scalar/pointer conversion, and initializes the tables in the same
   original order. It is selected instead of the original table-definition
   unit for the affected product configuration; original sources remain
   byte-exact evidence.
5. Keep the original table bodies on x86 only if the generated audit proves
   their ABI is valid under the chosen compiler. The target is one identical
   source-derived thunk selection for x86 and x64, not an x86-only bypass.
6. Compile the generated carrier with diagnostics clean for C4113/C4133/C4047
   in this interface family, then run one source-shaped video memory/port
   operation. No new fixture is justified before the whole generated ABI is
   selected.

## Exclusions

- Do not hand-write dozens or hundreds of wrappers.
- Do not suppress the diagnostics, cast function pointers, or make `IUH`
  pointer-sized globally before its complete CCPU usage audit.
- Do not map all `IUH` values through the session mapping manager: most are
  CPU/SoftPC numeric data, and doing so would change original arithmetic.
- Do not use Bochs, MONITOR, kernel VDM, `src.old`, or a display substitute.

## Confidence and follow-up

## P4 implementation progress

The formal graph now selects the generated CVIDC carrier in place of the two
original table-definition translation units only.  It retains the original
generated rule bodies and exact original initializer ordering, and compiles the
whole `cvidc` package with the target-local `CVIDC_RULE_WORD` definition.  The
same graph builds `original-softpc-cvidc.lib` on both MSVC x86 and x64 without
`C4113`, `C4133`, or `C4047` from the replaced `vglfunc`/`evidfunc` table
carrier.

That resolves the 773-table CVIDC contract, but it exposes the next, distinct
part of the same P4 family: the reached `base/video` units still contain 40
x64 `C4113` assignments (12 `egwrtm12.c`, 10 `gfx_updt.c`, 9 `ega_writ.c`, 8
`egawrtm0.c`, and 1 `vga_mode.c`).  x86 emits none for this set.  They are
original handler-table/callback assignments, not residual CVIDC table entries.
P4 therefore remains open: it must derive the corresponding source-shaped
`base/video` typed carriers before video memory execution is enabled.

The accompanying callback ledger separated those 40 sites into three recovery
shapes.  The 20 same-file delayed prototypes (`egawrtm0.c` and `egwrtm12.c`),
10 polymorphic marking callbacks (`gfx_updt.c`), eight BIOS chain-write
declarations, and the `vga_mode.c` update declaration are now selected through
source-derived declaration/callback carriers.  The identical selection builds
on x86 and x64, reducing the live x64 diagnostic set from 40 to **one**.

That remaining CVIDC EVID-to-`MEM_HANDLERS` bridge is not a pointer-width
conversion: it crosses from a guest-address-oriented EVID ABI to the original
machine memory-handler ABI.  `ccpusas4.c` stores the broader GMI handler
registry as `void (*)()` forms, so a local video cast would merely hide the
same error.  P4 must restore that registry's typed dispatch contract and use
the existing checked physical binding at the bridge before video execution can
be enabled.

The full `base/video + cvidc` archives currently compile on both architectures;
this is compile closure only, not a claim that runtime video I/O is enabled.
