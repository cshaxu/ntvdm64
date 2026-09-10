# T225 S30 focused segment-access observation result

## Build and test

The formal manifest-generated Ninja root
`build/M0-T225-S30/segment-access-r1` rebuilt the CPU5/Pentium-MMX MSVC x64
`/MT` bx-core/bx-mantle closure and ran:

```text
bin/t225-s30-segment-access-observation-fixture.exe
fixture-exit=0
```

The test-owned byte sequence is only `A1 FF FF` (`mov ax,[ffffh]`) in real
mode.  Original Bochs emitted `read_virtual_word_32(): segment limit
violation`; the fixture verified the retained first-fault finite stop and one
copied direct-limit tuple: word-read, width 2, offset `0xFFFF`, scaled limit
`0xFFFF`, selector 0.  It also verified disabled decline, duplicate decline,
explicit reset, and that the generic-UD bridge declines.

## Boundary result

The observer captures the original failure fact before `exception()` and does
not request a stop.  The existing S28 first-fault observer remains the sole
finite-stop owner.  No BOP/provider, guest-memory read, selector decode,
descriptor change, or emulation repair was exercised.  Native immutable
Direct/Readonly attribution remains the next P and is not claimed here.
