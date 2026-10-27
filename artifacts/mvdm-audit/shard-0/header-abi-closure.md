# Selected leaf declarations: shard 0

The companion CSV covers seven original headers: CWD, CDQ, CLC, STD, ADD,
AND and INC. After removing comments, each complete file contains exactly one
IMPORT VOID declaration and no extra behavior or interface. Current hashes
match the frozen selected OpenNT edition. Both the original leaf object and
`c_main.obj` have exact-path VALID compiler dependencies on each header.

Selected `insignia.h` supplies IMPORT=extern and ANSI IPT typed parameters;
IU32 is unsigned long and IUM8 is unsigned char. The x86 CCPU build uses its
default cdecl ABI. The shared compile-only fixture checks actual types,
parameter pointers and explicit cdecl function-pointer compatibility without
casts or mock declarations. An incompatible ADD reference is rejected.

The complete leaf implementations and published decoder bindings already have
separate sign/flag or ALU real-guest evidence. Combined with these single-file
declaration facts, each header is full/100 for the selected declaration
contract. This is not a new runtime capability, another compiler profile or
proof of the whole decoder, prefix, fault, interrupt or resource lifetime.
No original header, product or guest changed.
