# M0 T198 S121: DEM Lifecycle Correlation Ledger

S121 adds a fixture-only, 16-entry ledger for canonical DEM `50:12`, `50:02`
and `50:42` events at the existing composition return boundary.  Each entry
contains only copied service/outcome, `AX:BP` token and a five-scalar namespace
diagnostic: version, open flag, file index, generation and offset.  It holds
no path, guest byte/address, image pointer, host pointer or provider selection
state, and is not used by BOP routing.

The production-side accessor copies those five scalars from an explicit
composition object; the test bridge binds that object only in the fixture.
No Bochs source changes.  No guest run occurred.

`artifacts/build/t198-s121-dem-lifecycle-ledger-compile-r1` records:

- focused C11 ledger test exit `0`;
- CPU5/x64 compilation of the composition accessor;
- CPU5/x64 compilation of the native bridge and fixture.

The registered one-run harness now links the fixture ledger, but executing it
requires a separate admission.
