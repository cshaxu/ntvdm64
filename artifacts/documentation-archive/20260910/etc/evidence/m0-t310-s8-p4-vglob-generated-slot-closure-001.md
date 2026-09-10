# M0 T310 S8 P4 — VGlob generated GDP-slot closure

## Question

Do the 152 historical x64 `C4311`/`C4312` diagnostics in the original
`ccpu386/vglob.c` source still represent a current pointer-width defect?

## Inputs

- The byte-exact original `mvdm-host/softpc.new/base/ccpu386/vglob.c`.
- The selected generated `gdpvar.h` and the formal generated
  `mvdm_gdp_slots.h` overlay.
- The existing private `mvdm-host-overlay/.../mvdm_gdp_state.c` carrier.

## Procedure and observation

The historical warning baseline compiled `vglob.c` before the generated GDP
slot overlay was selected.  Its direct `GLOBAL_VGAGlobals` form reconstructs
a `VGAGLOBALSETTINGS *` through `IUH`, producing one truncation and one
wide-pointer reconstruction diagnostic for each accessor.

The current generated header undefines that same source macro and recreates
it as a native-width private GDP slot at the original offset.  Forced formal
Ninja rebuilds of `obj/ccpu/vglob.obj` on x86 and x64 completed with neither
`C4311` nor `C4312`; only unrelated macro-redefinition diagnostics remained.

## Interpretation

This is not a guest address or a host-resource identity.  It is private
generated SoftPC controller state, so it correctly remains in the existing
matching `mvdm-host-overlay` GDP carrier rather than the session mapping
manager.  No original VGlob body was changed and no new adapter was added.

All 152 stale VGlob width rows in the whole-tree worklist are therefore
closed as `closed-generated-private-gdp-slot`.  This is a source ABI closure
only; it does not claim video-device runtime validation.

