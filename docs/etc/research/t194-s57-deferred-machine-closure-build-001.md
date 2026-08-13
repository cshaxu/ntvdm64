# M0 T194 S57: Deferred Native-POST Machine Closure Build

The S56 `nmake /f ntdos64-t98-current-adapter.mak
ntdos64-t98-current-adapter.exe` target completed once in the frozen root.
It compiled 68 adapter and five CLI objects, `main.o`, `cpu/exception.o`, and
the existing machine-composition object; it linked the pre-existing original
`unexp_nt.c.obj` and `illegalp.c.obj`. No archive or device target ran, and
the executable was not run.

Hashes: engine `C53E3A0A62E055A7B5ABA4BC363CF72DF4E84AD9F6DB9BE628813BA7524CDB35`;
`main.o` `04ED98486DD46FA4C3B2A6E4271783C157DFD433283793F2169F7F5B6544696C`;
`cpu/exception.o` `5537C6BB8341C6901AB8A7AFCCEE3A33D89C88BFBC392ACF6CB54E4F525CAFD2`;
machine bridge object `82B78B56B109CF6A0D81DCC848BB173B4CBA12D14FF0D5B229C50A88577F5148`.

The map resolves `illegal_op_int` from `illegalp.c.obj`, `unexpected_int` from
`unexp_nt.c.obj`, and the machine bridge/probe from its one existing bridge
object. Warnings are retained C4244 in `exception.cc` and the known historical
CRT-model LNK4098/LNK4217 diagnostics. This is build closure only.
