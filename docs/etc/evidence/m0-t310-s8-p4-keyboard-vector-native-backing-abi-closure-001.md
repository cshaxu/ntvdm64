# M0 T310 S8 P4 — Keyboard-vector native-backing ABI closure

## Source finding

The original `kb_setup_vectors` BOP-5F path receives an NTIO keyboard table as
fixed-width guest physical values.  Its NT4 x86 implementation converted the
results of `effective_addr` directly into `word *` and `half_word *`, and later
converted the resulting table pointer back to `sys_addr` for `sas_loadw`.
Those conversions truncate native pointers on x64.

The function already uses the selected SoftPC SAS backing pointer
`Start_of_M_area` for the same table's idle-counter cells.  Therefore its
correct source-shaped native-width representation is the existing backing
pointer plus an unchanged fixed-width physical offset.

## Recovery

`kb_setup_vectors` now:

1. retains every original `effective_addr` calculation as a guest physical
   number;
2. obtains table, stream-buffer and dirty-count host views only through
   `Start_of_M_area + physical_offset`; and
3. passes `phy_base + word_offset` to `sas_loadw`, rather than truncating a
   table host pointer back to a physical address.

This is `MVDM-HOST-DIV-059`.  It preserves the original BOP table order,
original SAS owner and all downstream controller calls.  It creates no new
mapping manager: `Start_of_M_area` is selected internal SoftPC SAS backing,
not an external host object crossing a component boundary.

## Verification boundary

The formal selected original-SoftPC candidate rebuilt on both architectures:

- `build/M0-T310/S8/p1-machine-source/x64/keyboard-ram-x64-build.log`
- `build/M0-T310/S8/p1-machine-source/x86/keyboard-ram-x86-build.log`

Neither log contains the former direct table-pointer or pointer-to-physical
conversions at the reached `kb_setup_vectors` sites.  This closes only their
native-width representation.  Full BOP-5F keyboard/IVT/timer/video/printer
behavior remains the original owner-package composition work; no headless
subset is presented as complete device execution.
