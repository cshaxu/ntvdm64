# M0 T242 S3 INT 15h / AH=88h Source-Mirror Increment

## Scope

The bounded source-built observation recorded in S2 reaches `C4 C4 15` after
the conventional-memory bootstrap.  This increment restores only the exact
original non-PM `INT15_EMS_DETERMINE` branch required by that path:

```c
case INT15_EMS_DETERMINE:
#ifdef PM
    /* historical CMOS path */
#else
    setAX(0);
#endif
    break;
```

The active CPU5/P-MMX first profile has no adopted historical protected-mode
product shell, CMOS/RTC, timer, keyboard, DMA, or interrupt-controller
composition.  The source mirror therefore contains the complete observable
non-PM body, `setAX(0)`, and leaves all other `cassette_io` cases out of the
admitted fragment.

## Boundary

`src/bx-vdm/bop/opennt/softpc/tape_io.c` contains the source-shaped fragment.
The named `softpc_tape_io_shim` only carries AX into and out of the synchronous
fixed-width generic-UD event.  It retains no guest pointer, host handle, or
Bochs object.  `bx-core` and `bx-mantle` remain selector-blind: the adapter
recognizes the real-mode `C4 C4 15` window and AH=`88h`, then returns AX and
`RIP+3` through the pre-existing typed outcome.

## Verification

Formal r4 builds 260 graph edges, links `ntdos64-native.exe`, and runs
`t242-s3-softpc-tape-io-fixture.exe` successfully.  The fixture proves:

- real-mode `C4 C4 15`, AH=`88h` is accepted;
- the source non-PM result is AX=`0000`;
- the typed result resumes at `RIP+3` without modifying flags;
- other AH values and protected mode decline.

The follow-on bounded source-built run accepts `15/AH=88h`, then reaches
existing accepted routes `50:0D`, `50:21`, `54:0C`, `50:12`, `50:00`, `50:42`
and `50:02`.  It subsequently consumes the 100,000-tick budget at
`0800:0000`.  That result is neither a guest EXEC/parent-return claim nor an
authorization to patch the next trace point.  Its owner must be established
by the next coherent S3 source/ABI sweep.
