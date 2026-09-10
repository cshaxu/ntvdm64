# OpenNT KB16 Source-Closure Record

## Scope

This record covers the keyboard-control command in
`base/mvdm/dos/v86/cmd/keyb`, whose makefile declares `kb16.com`. It is a
compatibility-build result only; it does not prove that the resulting command
loads or handles real keyboard input correctly.

## Completed Inputs and Objects

The disposable `toolchain-probe/kb16-stage-v1` copies the tracked command,
V86 include tree, `base/mvdm/inc` monitor headers, and messages tree. The local source-built message tool
successfully generated `KEYB.CL1`, `KEYB.CL2`, and `KEYB.CLA` from the tracked
EN-US `usa-ms.msg` input.

JWasm built all nine original objects with zero warnings and zero errors:

```text
keyb  parser  keybcmd  keybtbbl  commsubs  keybi2f  keybcpsd  keybi9  keybi9c
```

Two narrow stage-only compatibility transformations are recorded in the stage
manifest:

1. Four `REPE MOVSB` spellings become `REP MOVSB`. For MOVSB both spellings
   encode `F3 A4`; ZF is not consumed by the move operation. `REPE CMPSB`
   remains unchanged.
2. The unused local byte-emitting `OPTION` and `GOTO` macro definitions become
   `KEY_OPTION` and `KEY_GOTO`, avoiding assembler reserved-word collisions.

## Monitor ABI Inputs

`keybi9c.asm`, the ninth original object, explicitly includes `bop.inc` and
`vint.inc`, then uses `BOP`, `FSTI`, `FCLI`, and `FIRET`. The initial stage
copy omitted the tracked shared headers; they are present at OpenNT blobs
`d1cd72a61233bbfa4101afa92c16440321abcdc4` (`BOP.INC`) and
`fecc99951d04e2424be35731fd6dc50dda0a03f7` (`VINT.INC`) and were copied
unchanged to the original relative include path.

`BOP.INC` defines BOP as `C4 C4 <selector>` and gives `BOP_WAITIFIDLE` value
`5Ah`. `VINT.INC` defines the fixed NTVDM state location and virtual-interrupt
semantics used by `FCLI`, `FSTI`, and `FIRET`. No local monitor shim is
introduced.

## Link and Layout Result

The original `keyb.lnk` object order was preserved. WLink requires a public
entry symbol whereas historical LINK16 accepts `END START`; the stage exports
the pre-existing `START` label only, preserving entry `0000:0100` without
changing code or data. The link emits the expected warning that no stack
segment exists.

| Artifact | Result |
| --- | --- |
| linked `KB16.v1.exc` | 15,035 bytes |
| EXC SHA-256 | `829146E03DFF5DB92A83E0B4A276808271DC394B0A441CF6063BA63518F01E7C` |
| raw relocation helper output `KB16.v1.com` | 15,003 bytes |
| post-`ORG 100h` layout candidate | 14,747 bytes |
| post-`ORG 100h` SHA-256 | `DC94F226963E2308D70FF9483446D1A56B4A3DB1C9D4CBBBB8138FFBEF004BDA` |

WLink emits a two-paragraph MZ header while the tracked historical EXC uses
32 paragraphs. Removing the verified 256-byte all-zero `ORG 100h` prefix gives
the post-ORG candidate above. It still differs from the 14,710-byte tracked
`KB16.COM`: it is 37 bytes longer, its first jump displacement is `2230h`
rather than `2210h`, and 9,076 bytes differ in their common range. The output
is therefore a source-build candidate, not a retail-identical or runtime-ready
replacement. Applying generic trailing-zero stripping is invalid here because
it removes data needed by the image.

## Consequence

KB16 is direct evidence that the DOS keyboard command depends on the VDM
monitor ABI, not merely on BIOS keyboard services. Runtime validation must
exercise command installation, `INT 9h`, `INT 2Fh`, BOP idle behavior, keyboard
LED updates, and interaction with `KEYBOARD.SYS` under a fixed research
profile.
