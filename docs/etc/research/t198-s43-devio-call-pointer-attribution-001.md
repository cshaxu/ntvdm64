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

The null far-call target is **not** a missing Bochs CPU, port-space, BIOS,
firmware, BOP, DEM, or CLI capability. The CPU has executed the original
far-call mechanism faithfully enough to expose an upstream guest-state
problem.

The current fixture reaches original `DEVIOCALL2`, but it has not established
the original prepared device chain that must supply the header at `DS:SI`.
The later generic-stop state retains `DS:SI = BF1F:1482`; the address is
`0xC0672`. S42 proves the call consulted a null `CALLDEVAD` value, but the
existing evidence does not directly capture whether the preceding header
field was already zero, whether the slot write missed the expected relocated
DOSDATA instance, or whether a subsequent guest instruction changed it.

Therefore the precise current classification is **guest device-chain
prepared-entry / DOSDATA-lifecycle gap, unclassified within that owner**.
It is not permission to prewrite `CALLDEVAD`, install a fake device header,
attach a host console, or skip `DEVIOCALL2`.

## Successor

S44 may use the existing opaque terminal ordinary-RAM snapshot once to copy
the four bytes at the source-derived final `SS:[CALLDEVAD]` location
`0x00A7:037A` (`0x0DEA`) in the exact fixture. It must not change any Bochs,
mantle, adapter, guest, or profile implementation, and must distinguish a
post-stop snapshot from a direct pre-call observation. A still-zero value
would corroborate the guest prepared-entry gap, not identify its writer;
only then may a separately admitted observation assess the header/write
sequence.
