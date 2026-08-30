# M0 T310 S25 P1 — C-VID generated-dispatch source boundary

## Question

Can the selected Win32/x86 CCPU40 product use the original C-VID generated
table definitions without preserving the former x64-only typed-wrapper
generator?

## Source relationship

- `softpc.new/base/cvidc/vglfunc.c` owns the original `C_Video` initializer.
  It contains 156 ordered generated `S_*` entries for the `VideoVector`
  slots declared by `cvidc/evidgen.h`.
- `softpc.new/base/cvidc/evidfunc.c` owns 14 ordered EVID read, write and mark
  tables.  Their initializers contain 692 generated `S_*` entries, with the
  declared table shapes in `base/inc/cpu_vid.h` and `ga_mark.h`.
- The corresponding generated rule bodies remain the exact selected C-VID
  manifest members.  No table entry is reimplemented, redirected through a
  BOP, or converted into a guest/host identity.

## Disposition

The product profile is now strictly Win32/x86 CCPU40.  The historical
four-host-word generated calling convention is an x86 ABI relationship, so
`vglfunc.c` and `evidfunc.c` are compiled directly from the original manifest.
The former `Generate-CvidcTypedTables.ps1` produced one hand-authored carrier
per table entry solely for a deferred x64-width problem.  It is removed from
the production build graph and repository.

Two small mirror declaration corrections remain active:

- `MVDM-HOST-DIV-062` makes the selected callback types explicit rather than
  transporting callbacks through generic data pointers.
- `MVDM-HOST-DIV-121` declares the real four-`IUH` generated mark-rule ABI so
  the original `evidfunc.c` tables compile directly.

They preserve original rule bodies, generated table names, initializer order,
caller form and x86 calling convention.  They do not create a mapping-manager
or Bochs/MONITOR route: C-VID private native words stay private, while any
future guest-memory operation remains governed by the existing machine-facing
mapping boundary.

## Explicit non-claims

This is generated dispatch source closure only.  It does not enable VGA/EGA
controller behavior, host presentation, or guest-visible video execution;
those remain S26, S27 and S49 respectively.
