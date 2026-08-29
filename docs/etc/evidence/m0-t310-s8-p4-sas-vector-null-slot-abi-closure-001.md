# M0 T310 S8 P4 — SAS vector null-slot ABI closure

## Source contract

The selected original CCPU `sasCdef.c` initializes `cSasPtrs` in the order of
the selected generated `struct SasVector` in
`mvdm-softpc-patch/x86/prod/sas4gen.h`. Every provided entry has a matching
`TYPE_*` declaration. The only unchecked initializer was the entry after
`c_SasRegisterVirtualSelectors`: source used a generic `(void (*)()) 0`.

That exact slot is `Sas_overwrite_memory`, whose selected generated contract is
`TYPE_sas_overwrite_memory`: `void (IU32, IU32)`. The original profile leaves
that capability unavailable; there is no selected implementation to bind.

## Recovery

The mirror retains the same null slot and initializer order as
`(TYPE_sas_overwrite_memory *)0`. This is a type-only source correction: it
preserves the original unavailable behavior; it does not manufacture an
overwrite-memory provider; it neither exposes a host pointer nor crosses a
guest-memory boundary; and it does not require the session mapping manager or
an adapter.

`MVDM-HOST-DIV-060` records the one changed expression.

## Verification

The formal `original-softpc-candidate` graph is rebuilt on both MSVC `/MT`
architectures after this correction. The reached `sasCdef.c` C4113 assignment
must be absent from both resulting logs. This only closes the generated SAS
vector's null-slot representation; it does not claim all SAS runtime,
virtualisation, or controller paths are complete.
