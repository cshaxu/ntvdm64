# T228 S1 P5 — Generic CS-transition successor provenance result

## Result

The amended, default-off `BX-MANTLE-075` provenance derivative rebuilt the complete 434-target CPU5/Pentium-MMX MSVC x64 `/MT` Ninja closure at `build/M0-T228-S1/transition-successor-provenance-20260818a`.  Its focused `t228-s1-budget-terminal-position-fixture.exe` completed successfully.  A fresh hash-locked Direct/Readonly run reached the same bounded terminal with byte-identical stdout and stderr.

The retained transition is now:

```
previous=1797:3AB8
current =5A5F:5EF9
predecessor=FF9E4A971793109717000000000000
successor  =000000000000000000000000000000
stack      =BC3A9717D20897176E00
```

All three copied windows are valid ordinary-RAM reads.  The new successor window is captured at the generic `current CS:IP` immediately on the retained CS transition, rather than at the later watchdog terminal.

## Source comparison

The immutable source-built `COMMAND.COM` has non-zero bytes at both relevant file offsets:

```
5EF9: 048805268A4401884501A07B9F8845
9F51: 5D706174680D0A1C52656E616D6573
```

Therefore the observed all-zero successor cannot be characterized as ordinary execution of that static COMMAND image merely because its offset falls within its file length.  The null target existed at the time of the `1797:3AB8 -> 5A5F:5EF9` transfer.

## Disposition

This strengthens the same complete runtime guest-memory producer/control-transfer boundary.  It excludes a late-only corruption theory and still does not identify the producer of the far-call target pointer.  The next diagnostic, if needed, must retain only sufficient selector-blind scalar register state to map that original far-call operand; it may not decode/filter in bx-core, create a BOP task, or repair a static image by address.