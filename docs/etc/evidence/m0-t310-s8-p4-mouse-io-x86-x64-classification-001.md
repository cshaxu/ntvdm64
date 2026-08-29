# M0 T310 S8 P4: mouse_io.c x86/x64 Classification

## Question

Does the current selected mouse controller body contain a host x86/x64 ABI defect that requires a mirror change?

## Inputs and procedure

- Force-rebuilt the formal mouse_io object under x64 and x86.
- Read each diagnosed same-unit helper, the host mouse provider, the selected host_simulate declaration, and the MOUSE_SCALAR callback slots.

## Observations

- EGA helper default-int warnings and private cursor-mask declarations do not transport host pointers, handles, or function-table values.
- MOUSE_SCALAR is signed 16-bit and IU16 is unsigned 16-bit; the two range diagnostics are guest-coordinate signedness, not a 32/64-bit transition.
- nt_mouse.c provides host_os_mouse_pointer as a VOID function with three native pointers. The missing declaration carrier is visible, but no pointer is narrowed and the source-level repair is not required by the x86/x64-only S8 rule.
- The existing MVDM-HOST-DIV-057 remains the source-backed void/no-argument host_simulate contract.
- host_memset argument-order semantics and unprovided graphics/mouse product behavior remain functional owner-package work, deliberately not hidden by this ABI audit.

## Result

All 16 tracked records are classified. No source change or warning suppression was introduced.
