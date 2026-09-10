# M0 T310 S8 P4 — CVIDC native callback contract closure

Date: 2026-08-29

## Scope

This closure covers the remaining reached pointer/callback conversions in the
original CCPU C-video package after its existing source-derived typed table
carrier is selected:

- generated `VideoVector` callback slots in `cvidc/evidgen.h`;
- `setReadPointers`, `setMarkPointers`, BIOS chain-write and generated-global
  binding in `cvidc/ev_glue.c`;
- the `C_VID` field type in `base/inc/egacpu.h`.

## Source reading and disposition

The original `ga_mark.h` and `EVID_*_POINTERS` tables prove four actual
callback shapes: byte read, scalar mark, string mark, and the shared
native-word generated rule transport. NT4 declared their `VideoVector` slots
as generic `IHP`, which converts a function pointer to a data pointer at every
reached `ev_glue.c` call. The existing generated table carrier already calls
the original rules through `CVIDC_RULE_WORD`, where `IUH` is native-width.

`MVDM-HOST-DIV-062` therefore changes only the mirror vector declarations to
the proven callback types. It does not add per-entry wrappers, reinterpret
guest addresses, or use the session mapping manager.

The same source reading proves that `base/video` declares both BIOS chain
writes as `void (ULONG, ULONG)`, and that the generated C-video profile owns
`struct VGAGLOBALSETTINGS`. The glue entrypoints and `EGA_CPU.globals` now
publish those existing contracts. Non-C-video profiles continue to use the
original `VGA_GLOBALS` field type.

## Verification

Both formal candidates completed:

- `build/M0-T310/S8/p1-machine-source/x64/cvidc-callback-candidate-x64.log`
- `build/M0-T310/S8/p1-machine-source/x86/cvidc-callback-candidate-x86.log`

Neither log contains a CVIDC `C4113`, `C4133`, `C4047`, `C4152`, `C4055`,
`C4311`, or `C4312` diagnostic. Other original non-width warnings remain
visible and unsuppressed. This is compile/ABI closure only; it does not claim
runtime video-controller, port, or display validation.
