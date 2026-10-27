# CCPU SUB and XOR: selected bodies versus native x86

`sub.c` and `xor.c` are byte-identical to the frozen OpenNT source; selected
worker `c_main.c` opcode families 28–2D dispatch SUB and 30–35 dispatch XOR
through the existing operand decoding; worker map attributes `_SUB` and
`_XOR` to `original-ccpu386:sub.obj` and `xor.obj` respectively. This is a
static call-chain observation; no real guest decoder execution was performed.

Offline 32-bit fixture `sub-xor-native-oracle.c` directly links copies of the
selected object files, supplies CCPU flag storage and a separately computed
parity table, then compares actual native 32-bit x86 SUB/XOR with each body.
Its bounded coverage is all 65,536 operand pairs at 8-bit width for each
instruction; each 16- and 32-bit operation also receives all 169 ordered pairs
from a 13-value boundary set and 100,000 deterministic pseudo-random pairs.
Total: 531,748 cases. The oracle masks both inputs to operand width, as the
body contract expects; it checks result, CF, PF, ZF, SF and OF, plus AF for
SUB. XOR AF is undefined and was not compared. Result:

```text
PASS SUB/XOR 531748 cases: exhaustive 8-bit pairs, 16/32 edge and seeded samples; defined flags only
```

Host reports AMD64 Family 26 Model 36 Stepping 0, AuthenticAMD. The random
sequence is deterministic and not an exhaustive 16/32-bit pair sweep. The
fixture does not test production `pf_table` initialization or register
bindings, non-normalized operands, actual c_main decoding, memory operands,
LOCK prefixes, exceptions, real/protected mode or session lifecycle. It does
not establish behavior on other host CPU vendors. Source identity and this body
witness do not justify an integrated/full classification; keep both pending.
No NTSRV, NTVDM, or guest was run.

Selected object hashes SUB
`CBC293C23C2A7B82B9EF27C922191A88103A4915A9D128B27CAB721BEF0094A8`, XOR
`A6BE343982BC93377F62274D186E18B2F457AF8DE45EF0BF9453530476FC19F0`;
link copies remove only `.drectve`, hashes respectively
`1932A90AC7DBDB5058D072817DB79CC7A6788EF2372C74EA8EFEC9671DC6955E`,
`9B6738D40652EAC95BE69EE7777B866053A559EFB98E2C05663CAE12E9D0AAA7`.
Fixture executable SHA-256
`A35EE146D894334704FBFD56A68DD315C5CDA4638F13E9FA11A7FF3CE9451944`;
source SHA-256
`672760178C169FBC064A3954D3A01418E94C6EE6BA08931696E4126C73511A26`.
