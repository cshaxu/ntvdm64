# T198 S43 DEVIOCALL2 pointer attribution

## Question and method

S43 is a read-only correlation of the S42 copied control-transfer facts with
the locked source-built NTDOS/NTIO image ledgers and original OpenNT source.
It adds no runtime observation or code.

## Exact ownership chain

| Observed or derived fact | Primary evidence | Owner and meaning |
| --- | --- | --- |
| `9346:4D53` bytes are `36 FF 1E 7A 03` | S42 copied predecessor window | It is a guest far indirect call through `SS:[037A]`. |
| Exact NTDOS byte site | `docs/research/ccpu-bios-closure/S5-NTDOS-IMAGE-ADDRESS-MAP.md` | The locked image maps `DEVIOCALL2` to `0000:4D47`; its bytes at the matching call site are `36 FF 1E 7A 03`. |
| `037A` slot | same map and `src/opennt/base/mvdm/dos/v86/doskrnl/dos/dev.asm:525-542` | `CALLDEVAD` is DOSDATA's temporary far-call slot, not a BIOS table or an adapter input. It begins zero by design. |
| Immediate writers | `dev.asm:534-537` | `DEVIOCALL2` reads `DS:[SI.SDEVSTRAT]`, stores that offset in `SS:[CALLDEVAD]`, stores `DS` as its segment, then makes the far call. |
| Required source of those fields | `src/opennt/base/mvdm/dos/v86/doskrnl/bios/msbio1.asm:76-84`; `S5-NTIO-CON-DEVICE-INIT-TRACE.md` | The first original NTIO resident device header is `CON`; it carries `strategy` and `con_entry` pointers. For the locked initial NTIO image they are `0070:01EC` and `0070:01F7`. |
| Lifecycle that makes the pointer live | `src/opennt/base/mvdm/dos/v86/doskrnl/bios/sysinit1.asm:903-928`; `S5-SYSINIT-TEMPORARY-DOS-STATE-GATE.md` | Original `sysinit` selects a runtime temporary location, moves NTDOS, then `DosInit` relocates DOSDATA. Neither its final segment nor its device-chain state may be synthesized from the initial NTDOS load segment. |

S42's return pair `9346:4D58` is exactly the post-call location in that
locked `DEVIOCALL2` byte sequence. This confirms the code identity rather
than merely a matching opcode.

## Classification

The call is **not** a missing BOP, DEM, or CLI capability. S44 corrects the
remaining attribution: its terminal raw snapshot at `00A7:037A` is
`FF FF 1F BF`, i.e. `BF1F:FFFF`, rather than zero. The immediate source writer
is therefore consistent with the observed `DS=BF1F` and an original device
header strategy offset of `FFFF`.

The snapshot is post-stop, so it does not prove the exact call-time value.
It does prove that S42 did not observe a null operand and removes the prior
"missing device header" classification. The remaining owner is original
guest execution continuity at the source-derived `BF1F:FFFF` target,
including the retained prefetch diagnostic `EIP [00010000] > CS.limit
[0000ffff]`. It is not permission to prewrite `CALLDEVAD`, install a fake
device header, attach a host console, or skip `DEVIOCALL2`.

## Successor

S44 completed the terminal snapshot and disproved its conditional null case.
The successor is a read-only audit of the original split-at-`FFFF` strategy
layout and retained CPU real-mode instruction-pointer behavior before any
core change is admitted.
