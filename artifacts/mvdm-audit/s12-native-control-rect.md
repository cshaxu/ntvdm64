# S12: original USER WOW rectangle-layout branches

The observed WOW32 x86 build compiles the original USER `_GetClientRect`
implementation in `windows/core/ntuser/rtl/wow.c` without defining `WOW`.
No project/generated C/header, installed Windows SDK or Visual Studio header
definition or undefinition of that label was found. This `WOW` conditional
selects a packed 16-bit rectangle representation; it is not the switch that
enables the project's WOW32 runtime. The selected host controls use native
`RECT` objects with four `LONG` fields, for example `SLPaint`'s `rcEdit` and
the native `DRAWITEMSTRUCT` rectangle.

The two unselected 16-bit-layout bodies were replaced by markers, retaining
their conditionals and native alternatives. Win16's external
`WU32GetClientRect` path remains: it obtains a native `RECT` and converts it
through `PUTRECT16`. The companion JSONL records the original ranges and body
hashes. No guest source or binary was changed; unobserved build profiles are
not covered.

Independent reconstruction from `470e8960e` verified both hashes and the
complete file, and checked that the RECT16 thunk remains. Cached x86/x64
product builds and documentation governance passed. All ten product images
matched the previous release exactly or differed only by four PE linker
timestamp bytes; runtime tests and publication were not repeated for
equivalent images.
