# DPMI software-interrupt selected-source dispatch

The formal test compiles the current `dpmiint.c` with the selected x86 worker
command plus `/Gy` into a test-only object. It calls the real
`DpmiSwIntHandler`, `ReflectV86Int` and `BuildStackFrame` implementations;
unrelated unresolved service boundaries terminate the fixture if called.
The test supplies controlled registers, LDT entries and a memory mapping.

The absent-handler path returns false without mapping or target reporting.
The real virtual-8086 path builds its original three-word frame, dispatches
through the simulated IVT and performs exactly one required stack mapping.
The protected-mode underflow path returns false from the original stack
limit check, without an extra diagnostic mapping. These three cases passed.
The selected production object and test-only object have identical extracted
instruction bytes for all three tested functions, with relocation placeholders
still unresolved; the link map and hashes are retained. This comparison does
not prove linked addresses or every dependency is identical.

The fixture uses synthetic CPU/memory boundaries and is not a guest execution,
full protected-mode exception test, or proof of every DPMI handler. The
separate last-error observer tests cover enabled and disabled reports. No
product or guest code changed in this verification.
