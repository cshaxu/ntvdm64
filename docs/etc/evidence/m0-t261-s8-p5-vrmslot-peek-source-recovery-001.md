# M0 T261 S8 P5 — `vrmslot.c` Peek source recovery

## Question

Can the first reached VDMREDIR mailslot service replace its source-derived
implementation with the original OpenNT provider body without adding a second
CCPU frame, guest mapper or handle manager?

## Inputs

- Original body: `refs/opennt/base/mvdm/vdmredir/vrmslot.c`,
  `VrPeekMailslot`.
- Original interface contract: `refs/opennt/base/mvdm/inc/vdmredir.h`,
  `SET_ERROR(err)`.
- Existing bounded same-shaped machine boundary:
  `src/adapter-softpc/opennt_ccpu_sas_facade.h`.
- Existing Redirector session composition:
  `src/opennt-bop/ingress/redir_native_session.c`.

## Procedure

1. Re-root the original `VrPeekMailslot` body under
   `src/opennt-bop/vdmredir/vrmslot.c` and retain its register/error call
   shape.
2. Replace only its unavailable NT4 product-header closure with the declared
   adapter-softpc CCPU/SAS facade; register this statement-local change as
   `BOP-DIV-057`.
3. Add a BOP-owned scoped-body caller that constructs the existing copied
   CCPU/SAS call record from the generic event.  It owns no selector-blind
   machine implementation, handle table or guest mapper.
4. Remove the former `57:0C` cached-peek route.  The fixture now asserts the
   original `ERROR_NOT_SUPPORTED` result and then reads the still-unconsumed
   message through `57:0D`.

## Observation

Formal Ninja root `build/t261/s8-r010` compiled the new `vrmslot.c`, linked
`opennt-bop.lib` and linked `bin/t251-s3-redir-ingress-fixture.exe`.  The
focused executable printed:

```text
T251 S4 Redirector: typed selector-57 lifecycle and mailslot owner group pass
EXIT=0
```

## Interpretation

The first source body is connected through the one existing mechanical frame
and preserves OpenNT's explicit non-peek failure.  This is a source-fidelity
correction, not a new capability: Windows mailslots provide no non-destructive
peek and the old cached-message simulation is no longer a provider route.

This result does **not** close `vrmslot.c`.  The self-authored fixed mailslot
array and its private 16-bit allocator still serve Make/Delete/Info/Read/
Write/Terminate.  The next P5 work must replace them with the original
`VR_MAILSLOT_INFO` lifecycle while mapping only native `HANDLE` values through
the existing session-owned host-handle manager.
