# T225 S30 native segment-access Direct/Readonly result

## Inputs and procedure

The formal MSVC x64 `/MT`, CPU5/Pentium-MMX Ninja root was
`build/M0-T225-S30/segment-access-r3`.  It rebuilt and linked
`bin/ntdos64-native.exe` from the manifest-owned bx-core, bx-mantle, bx-vdm,
opennt-host and CLI libraries.

The immutable S10 primary root, its paired startup configuration/autoexec
sources, and `--instruction-tick-budget 1000000 --observe-first-fault` were
used once each in Direct and Readonly.  No BOP or generic-UD observation was
selected.

## Observations

| Fact | Direct | Readonly |
| --- | --- | --- |
| Exit | `1` | `1` |
| First fault | `#GP(13)`, error `0`, `CS:EIP=0C41:2357` | identical |
| Segment-access tuple | read-word, direct-limit, index `3`, selector `0C41`, offset `FFFF`, width `2`, limit `FFFF`, valid `00000007` | identical |
| Terminal | machine failure, detail `2` first-fault stop | identical |
| stdout SHA-256 | `6426B7A50580DC2584834021D65021FF87FEFD40DB445B9766AB38DF29853243` | identical |
| stderr SHA-256 | `CB8526DC115A44AB0B3511EBA6D1C0892FE0534836E1DB9AA61BAB0198021BC6` | identical |

Both stderr records retain the original Bochs
`read_virtual_word_32(): segment limit violation` message.

## Interpretation

S30 resolves the mechanical branch: the failure is the original direct-limit
branch of a two-byte virtual word read at the end of a real-mode segment.  The
new tuple does not establish which guest instruction or original NTDOS source
routine formed the effective `DS:FFFF` address; the first-fault EIP is not an
instruction-boundary proof.  The identical Direct/Readonly records reject
host mutation capability as a cause.

No CPU wrap rule, descriptor change, guest image patch, BOP/provider, BIOS,
PIC, FPU, timer, or adapter workaround is authorized by this result.  The
only successor is a read-only original NTDOS control-flow/data-pointer audit
from the reached DOSCODE region to the word-read producer.
