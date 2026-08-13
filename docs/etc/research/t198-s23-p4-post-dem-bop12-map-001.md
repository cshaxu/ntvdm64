# T198 S23 P4 Post-DEM BOP12 Map 001

## Question

What is the first unimplemented operation after exact NTIO completes DEM
`50:11`, and which component owns its semantics?

## Inputs And Procedure

The S23 native test bridge recorded the copied instruction window at its first
unhandled `#UD`. The exact NTIO run reported `next=12:b1`. Review then used
`sysinit1.asm:822-828`, `bios.c:153-160`, `bios.h:39-40`, and the retained
historical evidence ledger.

## Observations

The actual BOP encoding is three bytes: `C4 C4 12`. The recorded fourth byte
`B1` is the immediately following `mov cl,6`, not a BOP service. `sysinit1`
identifies `BOP 12h` as the substitute for BIOS `int 12h`, then shifts returned
AX KiB by six to form a 16-byte-block segment value. The historical BIOS table
maps selector `12h` to `memory_size`.

## Interpretation And Follow-Up

This is a top-level machine BIOS selector, not a DEM service and not a DOS or
host namespace responsibility. Its result contract must be re-admitted as a
bounded `bx-vdm` machine-composition provider using an evidenced conventional
memory value; Bochs remains selector-blind. No provider is enabled by this
map.
