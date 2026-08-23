# M0 T242 S3: NTIO Config Progression And Keyboard Transfer

## Question

After the recovered local `50:42` read, does the source-built guest remain in
an unclassified loop, or does its next reached boundary identify the owner
that must precede guest DOS EXEC recovery?

## Inputs And Procedure

- Staged original-toolchain `NTIO.SYS` (33,792 bytes), `NTDOS.SYS` (27,858
  bytes) and `COMMAND.COM` (50,384 bytes), as frozen by T242 S1.
- `refs/opennt/base/mvdm/dos/v86/doskrnl/bios/ntio.map` and
  `src/opennt/base/mvdm/dos/v86/doskrnl/bios/{sysinit2.asm,spckbd.asm}`.
- `src/opennt/base/mvdm/softpc.new/base/{bios/bios.c,keymouse/keybd_io.c}`.
- One existing default-off, selector-blind provenance build at
  `build/M0-T242-S3/formal-r6-provenance`, followed by bounded 100,000 and
  1,000,000 tick runs.  No source, BOP provider, device or guest byte changed
  between the two runs.

## Observations

At 100,000 ticks, the terminal was `8e08:2f57`.  Provenance copied the
15-byte instruction window:

```text
aa 3c 22 74 3e 3c 20 77 ef 2e 80 3e 5f 04 55
```

The exact sequence occurs at staged `NTIO.SYS` offset `0x3927`.  The original
NTIO map makes that `SYSINITSEG` address `009d:2f57`; its public range places
it inside `sysinit2.asm:organize`.  The corresponding source is `org51`:
`stosb`, quote test, space test, and the ordinary CONFIG.SYS line parser.
Thus the 100,000-tick result is normal original startup work, not an EXEC
failure and not a candidate BOP repair.

At 1,000,000 ticks, the same input crossed that parser and reached one new
accepted ingress event:

```text
CS:EIP=8e08:45a6  C4 C4 5F  AX=BEEF  disposition=decline
```

`spckbd.asm` builds the resident keyboard table and documents this exact
`BOP 5F` handoff.  The NTVDM SoftPC table in `bios.c` maps selector `5F` to
`MS_bop_F`; `keybd_io.c` documents the resulting
`MS_bop_F -> kb_setup_vectors()` path.  The default non-NTVDM alternative
table is `illegal_bop`, so bare selector bytes do not select a safe generic
adapter behavior.

## Disposition

`BOP-TOP-5F` is the first remaining prerequisite on this source-built path.
It belongs to the selector-blind PC machine keyboard/IVT composition package:
the package must audit the original table, table layout, IVT writes and native
keyboard lifecycle before any typed machine request is offered.  `bx-vdm`
continues to recognize/route BOPs but cannot implement `kb_setup_vectors`,
keyboard device state, or IVT/PIC policy.

The observed decline is retained as an explicit boundary; it is not changed
to Ignore, CF fallback, a synthetic vector table, or a new guest/host service.
The one-million-tick terminal at `0000:0301` is consequently not attributed
to a guest EXEC defect.

## T242 Outcome

T242 S3 has exhausted its admitted machine-or-existing-DEM prerequisite
scope: it recovered the available AH=88h and FastRead group, then classified
the first remaining external owner with source, ABI and lifecycle evidence.
The unexecuted guest `EXEC`/parent-return source remains guest-owned and is
returned to the dependency-ordered queue after the BOP-5F machine package.
