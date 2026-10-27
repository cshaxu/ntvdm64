# CCPU CMP / NEG direct-object differential

Frozen CMP and NEG source hashes match OpenNT exactly. The worker map selects
`_CMP` from `original-ccpu386:cmp.obj` and `_NEG` from `neg.obj`.
`c_main.c` routes CMP opcodes 38–3D and also CMPS/SCAS paths to CMP; opcode
groups F6/F7 select NEG for ModR/M extension 3. This source tracing is not a
runtime witness of decode, memory access or string repetition.

`cmp-neg-native-oracle.c` directly links copies of the selected compiled
objects with local CCPU flags and independently generated parity table. It
executes native 32-bit x86 CMP and NEG as oracle and checks all six defined
flags CF/PF/AF/ZF/SF/OF; for NEG it also checks the width-masked result. CMP
covers all 65,536 8-bit operand pairs, 169 boundary pairs and 100,000 seeded
pairs each at 16/32 bits. NEG covers all 256 8-bit values and all 65,536
16-bit values, plus 13 boundaries and 100,000 seeded 32-bit values. Result:

```text
PASS CMP/NEG 431679 cases; defined flags and NEG result versus native x86
```

16/32-bit CMP pairs and 32-bit NEG are bounded, not exhaustive; the host is
AMD64 Family 26 Model 36 Stepping 0, AuthenticAMD. The fixture assumes
normalized operands, replaces production parity-table wiring, and does not
cover decode/memory/string repeat, flags preservation outside the six checked
bits, prefixes, faults, modes, reentrancy or guest lifecycle. Keep both files
pending integrated evidence, not fully restored from body results alone.
No product service, guest or existing executable was run or modified.

Original selected object SHA-256: CMP
`7C55B3F513D40900DB14DF0FE709CE6E7DFF5D5B83814FD151B671A32B4D48DA`, NEG
`6C593857D573C58CF2A547ECFF3A40CBCDE28A159E89771DEDFF3E349957111C`.
Copies used by the fixture removed only `.drectve`, hashes respectively
`BDB2D99CA7806A98E168359912607FE73A27FE111B5A56E0F18A111DAB608027`,
`058BDEBA253F6A983599BEA7F5E9DD03F67FC37E6F13FE20B94C36C5610168AF`.
Fixture exe SHA-256
`6AD6CB0D77B29C7AB3C44747286590F5CEB17EF56E943CAC62E0312D775BEBC0`;
fixture source SHA-256
`992DAE5FB65AC799C4B9219BE5B21C2241D6F484017508D3DCC37FD826323724`.
