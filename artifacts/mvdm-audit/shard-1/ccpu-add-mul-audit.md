# CCPU ADD and unsigned MUL: selected object differential

`add.c`, `mul.c` and `c_mul64.c` are frozen byte-identical to OpenNT.
Worker map attributes `_ADD`, `_MUL8`, `_MUL16`, `_MUL32` and `_mulu64` to
their selected original CCPU objects. `c_main.c` selects ADD for opcode family
00–05; groups F6/F7 select unsigned MUL8/16/32 (extension 4). `MUL32` calls
`mulu64`; signed `mul64` in the same file is a separate path and is **not**
covered by this witness. Static dispatch is not end-to-end guest evidence.

Offline fixture `add-mul-native-oracle.c` links the actual selected production
object copies, provides CCPU register/flag arrays and a separately generated
parity table, and compares against native 32-bit x86 on an AMD64 Family 26
Model 36 Stepping 0 AuthenticAMD host. Both instructions exhaust all 65,536
8-bit operand pairs. Each of the 16/32-bit bodies gets 169 ordered boundary
pairs and 100,000 deterministic seeded pairs per width. The 531,748 total
cases compare the width-masked result, all six defined ADD flags, MUL high
half (AH/DX/EDX), and MUL CF/OF. MUL PF/AF/ZF/SF are not defined by x86 and
are not judged by the native oracle. Output:

```text
PASS ADD/MUL 531748 cases; width results and defined flags match native x86
```

The 16/32 operand space is sampled, not exhaustively covered, and the
pseudo-random pair generator is deterministic. Inputs are normalized to
width. The fixture replaces production register wiring and parity-table
initialization, and does not test native decoding, memory operands, prefixes,
protected-mode faults, guest lifecycle, signed IMUL or signed `mul64`. It
also does not verify other host vendors. Accordingly all three rows remain
pending integrated coverage. No NTSRV, NTVDM, or guest was started.

Selected object SHA-256: ADD
`6E556CE87571D6814482DD4C15C99896AAD39FB26F711122131A820AED4248A7`;
MUL `C64D31EA62439F804B8424C5C9665C0E3F0097A2327AB74AA3DE6869CE1F6297`;
c_mul64 `CBE4E760B2BE91A6ACBDC97C6866387ACC2E75D7E81EC8B141B5487D17E0C03F`;
c_neg64 dependency `16871E8158CD7CE27D92585E348B4E3065B32BADCC86724E7E0979E9F664B538`
was linked but its signed path was not called. Only `.drectve` is removed
from link copies. Fixture exe hash
`4610545F84793ACB582D9A6EB0BCAC865B515F4C28ED62EAA2D845891AE2F102`,
fixture source hash
`E47A2A7C377EBEEE6F668CF82FD1BE70262E3F777F9959DC3A1702D99B0235E0`.
