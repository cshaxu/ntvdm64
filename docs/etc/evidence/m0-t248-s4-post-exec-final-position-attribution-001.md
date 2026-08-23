# M0 T248 S4 — post-EXEC final-position attribution

## Result

The source-built Direct checkpoint remains past original NTDOS
`$Exec:Xfer_To_User` / accepted `50:36` and accepted imported `50:1E`. Its
reproducible terminal is the finite execution budget, not a typed BOP result:

```text
terminal-position CS:EIP = 5859:78B0  (physical 0x5FE40)
terminal = 4 detail = 0 lifecycle = 5 presentation = 5
```

At that terminal point the checked, copied 16-byte instruction window is all
zeroes. This is ordinary RAM, not an instruction range in any staged guest
image, a BOP result, or a selector-blind machine provider result.

## Last segment transfer

The existing default-off history and CS-transition observations retain the
last relevant transition without changing CPU state:

```text
previous = 0000:0A87  SS:SP = 00A7:FFC8  sequence = 570736
current  = 5859:5F04  SS:SP = 00A7:FFC4  sequence = 570737
predecessor bytes = FF 9F 26 03 00 76 00 FF FF FF FF 00 00 14 02
successor bytes   = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
stack bytes       = 8B 0A 00 00 08 A7 00 A7 00 A7
```

In 16-bit decoding the predecessor begins:

```text
0000:0A87  FF 9F 26 03     call far [bx+0x326]
```

The four-byte stack movement and leading stack words `0A8B:0000` are the
ordinary far-call return pair. The indirect target is therefore `5859:5F04`;
execution subsequently advances through zero RAM until the budget terminal at
`5859:78B0`.

## Image comparison and ownership

The minimal-machine staging contract initially copies source-built `NTIO.SYS`
to physical `0x0700..0x8AFF`. `0000:0A87` is physical `0x0A87`, inside that
aperture at image offset `0x387`. The current staged source bytes beginning
at offset `0x384` are:

```text
03 FF FD 9E 58 1F EB 03 FA EB F8 33 C0 8E D8 0E
```

Thus the initial source byte at physical `0x0A87` is `9E`, not the observed
runtime `FF`. The far call and its indirect target are a later guest-visible
mutation of the staged low-memory NTIO aperture. No current evidence identifies
the guest writer or the `DS:BX+0x326` pointer value; doing so would require a
separately admitted physical-write/guest-runtime provenance investigation.

## Disposition

This closes T248's selector-blind question. The CPU executed a normal guest
far call; there is no demonstrated missing CPU decode, IRET, PIC, port,
firmware, BOP provider, or Bochs mechanism to repair. `BOP-DEPENDENCY-115`
remains partial only because original `$Exit/$Abort` parent return has not
occurred. Its immediate blocker is now a source-built guest low-memory runtime
mutation/transfer investigation, not a DEM or COMMAND leaf BOP.

The formal graph is `build/M0-T248-S4/formal-r1` (MSVC x64 `/MT`, CPU5/P-MMX)
with existing instruction-history, CS-transition and provenance diagnostics
enabled. The bounded native run exits `4` by its declared instruction budget.
