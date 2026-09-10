# OpenNT MASM386 And Historical LINK Purification Ledger

Status: superseded historical ledger, 2026-08-07.

> The table below records an earlier comparison effort and is not current
> source-to-image evidence. In particular, it incorrectly promotes
> source-built MASM386, an older historical LINK route, and images later shown
> to require fresh BUILDMSG/LINK16 verification. The current authority is
> `HISTORICAL-TOOLCHAIN-REBASE-2026-08-07.md`.

## Rule

A guest artifact is current only when its stage copies manifested upstream
source bytes, its assembly uses the OpenNT-source-built MASM386, and its link
uses the recovered historical `LINK.EXE` through DOSBox. Guest-source rewrites
for JWasm or UASM are retired evidence, not an allowed fallback.

This ledger distinguishes missing source input from toolchain failure. Neither
case is fixed by reusing a modified legacy stage.

## Reproduced Images

| Artifact | Source and toolchain result | Final evidence | Guest transforms |
| --- | --- | --- | --- |
| `COMMAND.COM` | 34 MASM386 objects; historical LINK; `reloc` | 50,384 bytes, `908A77AC617C2D741F0AA1B73F73973DCF29ADC91F092E5BCB02173C8C732C43` | none |
| `HIMEM.SYS` | 6 MASM386 objects; historical LINK; `reloc` + `stripdd` | 4,784 bytes, `995CBF0672E72DBD1041E6025B40BBD382C5D6EB85FFEA9889827266D72D265F` | none |
| `EXE2BIN.EXE` | 2 MASM386 objects; historical LINK | 9,184 bytes, `DD8ED44FC09EEE0F4E9A3BC583F6C24E9456AEC4166775028523AB824E27EF6D` | none |
| `NTIO.SYS` | 12 MASM386 objects; historical LINK; `reloc 70` | 33,792 bytes, `CFC8BE16576BB6ACD16BB8FC9B2D9A080F544BBFDD2D2D2DF07ED908B3AB4937` | none |
| `NTDOS.SYS` | 38 MASM386 objects; historical LINK; `reloc` + `stripz` | 27,858 bytes, `957662320654AD5251C3A8B228A5DADEC28AA65DDDBCBA38C3658A6E7F93BC84` | none |
| `FASTOPEN.EXE` | 1 MASM386 object; original `fastopen.lnk`; historical LINK | 775 bytes, `548734C1BA281A1F2A5ADD69E3017BC7B75A7D7BFF37890ECD6FB4D1E40E1E08` | none |
| `COUNTRY.SYS` | 1 MASM386 object; original `link16 /map country`; `reloc` | 27,097 bytes; historical MZ SHA-256 `38EB2DD215ED366F4AF50B2DB3A2646F0ED00E91C9C264C9B42C455857FA810B` | none |
| `APPEND.EXE` | BUILDMSG generated original message classes; 1 MASM386 object; historical LINK | 12,498 bytes, `1B8B414C123508E7BB5F340A514A77F1F91AD0B8C662963682CDE624CA4938A1` | none |

The fresh NTIO/NTDOS stages are under `artifacts/toolchain-runs/*-v4/`.
The FASTOPEN, COUNTRY, and APPEND stages are respectively under
`fastopen-masm386-pristine-v1`, `country-masm386-pristine-v1`, and
`append-masm386-historical-link-v3` in the same artifact root.

## Removed Legacy Paths

The following were deliberately removed after the listed MASM386/historical
LINK replacements were verified:

- the JWasm, UASM, and OpenWatcom third-party tool trees;
- all JWasm/UASM/WLink driver scripts, overlay staging scripts, and probe
  scaffolding;
- legacy `SETVER`, `KB16`, `KEYBOARD`, and failed `DOSX` stages and their
  guest images;
- superseded JWasm/UASM/WLink candidate stages for the artifacts in the table.

Only the OpenNT source input, source-built MASM386, DOSBox, recovered
historical `LINK.EXE`, the documented host BUILDMSG executable, and the fresh
MASM386/historical-LINK evidence stages remain. There is no compatibility
assembler fallback in the workspace.

## Not Rebuilt

| Former compatibility path | Why it was not rebuilt | Required next input |
| --- | --- | --- |
| `KEYBOARD.SYS` UASM overlay | `src/opennt` has no `dos/v86/dev/keyboard` source directory. The transformed legacy stage was removed. | recover a manifested, unmodified keyboard source closure and its original response/link inputs |
| `SETVER.EXE` JWasm/OpenWatcom stage | `src/opennt` has no `cmd/setver` directory. The mixed C16/JWasm legacy stage was removed. | recover its manifested source closure, then audit C16 and MASM386 contracts separately |

## Host-Side Exceptions

`BUILDMSG` remains a host compatibility executable. Its fixed-size status
buffer repair is documented in `OPENNT-BUILDMSG-HOST-ABI-RECOVERY.md`; it does
not alter any guest source file. Its APPEND run generated `APPEND.CLA`,
`APPEND.CLB`, `APPEND.CL1`, and `APPEND.CL2` from original inputs.

All listed images are source-to-image evidence only. They are not claims of
retail-byte identity, bootability, or NTVDM runtime compatibility.
