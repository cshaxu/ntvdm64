# M0 T310 S8 P4 — selected cvidc glue ABI closure

## Scope

This closes the four width/call-ABI worklist records emitted from the selected
`softpc.new/base/cvidc/ev_glue.c` translation unit:

- `T310-S8-P4-CLASS-001106` and `...1107`: BIOS byte/word chain-write
  callback assignments;
- `...1108`: C-video global-carrier assignment;
- `...1110`: generated GDP-carrier allocation result.

## Source-shaped resolution

The current mirror already contains the two minimal, registered source-shaped
repairs required by those records:

- `MVDM-HOST-DIV-062` gives `write_bios_byte` and `write_bios_word` the
  original outer `(ULONG, ULONG)` callback form required by the base/video
  chain-write globals. The generated rules continue to receive their original
  narrow `IU8`/`IU16` values only inside the rule body.
- The same divergence declares `EGA_CPU.globals` as the actual generated
  `VGAGLOBALSETTINGS *` carrier when `C_VID` is selected, instead of treating
  it as the sibling `VGA_GLOBALS *` layout.
- `MVDM-HOST-DIV-048` keeps `setup_global_data_ptr`'s original `IHP` result
  shape while allocating the existing source-derived native-width GDP slot
  carrier. GDP is private controller state, not a guest address, host-object
  token or mapping-manager identity.

## Verification

The formal MSVC `/MT` x64 and x86 Ninja recompilation of
`obj/cvidc/ev_glue.obj` emits none of C4113, C4133 or C4312. Remaining
diagnostics in that original generated unit are C4431/C4013/C4244/C4100;
they are not pointer-width or calling-ABI suppressions and remain visible for
their own source review.

No new wrapper, callback trampoline, warning suppression or mapping-manager
route is added by this closure.
