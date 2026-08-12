# T121 S1 deferred machine-composition build 001

Fresh derivative `artifacts/build/current/t121-deferred-machine-composition-r1`
links successfully. Its only Bochs rebuild targets are `main.o` and
`cpu/exception.o`; the only new component object is
`machine/bx_ntvdm_machine_bop_v1.obj`. No archive is a make prerequisite.

The one exception command includes deferred startup, BOP catalogue listener
and machine-composition macros simultaneously. The map contains the unchanged
`unexpected_int`, the machine probe and the listener. Boundary tests pass.
The executable SHA-256 is
`DB2BF25C2AA5BDA4B482B0968D668FCFD050F394FD7BC278E2642E725DC53E1E`.

The retained `LNK4098` CRT warning remains a runtime limitation. No run occurs
in T121; T122 admits one deferred observation only.
