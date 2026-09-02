# M0 T377 S4 — Formal workload-matrix closure

## Formal source closure

The CPU40/x86 formal manifest already selects the complete original owner
cohorts admitted in S1:

- DEM's original file, handle and IOCTL sources, including `demfile.c`,
  `demhndl.c` and `demioctl.c`;
- keyboard/PPI/mouse sources; original system timer, ICA and queued-event
  sources; and the original host timer/EOI bindings;
- the complete original `base/video` and generated `base/cvidc` groups; and
- the original host graph/presentation owner with its narrowly registered
  public Console/session binding.

S2's formal Ninja action rebuilt those selected sources and linked
`original-softpc-process.exe` successfully.  The graph therefore contains the
same original owner bodies that S2/S3 audited; it does not substitute a
project-defined keyboard, PIC, timer, VGA, file or IOCTL controller.

## Immutable-media staging closure

`Stage-OriginalSoftpcRuntime.mjs` now stages the five S1 matrix inputs beside
the already selected DOS and firmware package:

- `MEM.EXE`;
- `KB16.COM` and `KEYBOARD.SYS`;
- `GRAPHICS.COM` and `GRAPHICS.PRO`.

Every guest and firmware asset remains hash-checked and mismatch-refusing.
The stage tool deliberately treats the formally linked product as the sole
replaceable input, and only when the caller supplies both `--update` and
`--replace-product`.  A plain `--update` correctly refused to overwrite a
different staged product.  This is a product deployment operation, not
permission to alter any original DOS or firmware byte.

The current formal product and selected original media were then staged at
`O:\\ntvdm64`; the staged matrix hashes match S1's immutable inventory.

## Frozen S5 contract

S5 may make exactly the following bounded, console-owning observations from
that fixed stage, each with the same formal product/media identities and **no
`--child-environment` diagnostic variable**:

1. declared DOS command `MEM.EXE`;
2. declared DOS command `KB16.COM US`;
3. declared DOS command `GRAPHICS.COM`.

The observer may report only the fixed container identity, console transcript,
bounded result and first source-owned marker already emitted by the product.
It must not enable a DEM-open, session-dispose or command-continuation
diagnostic.  Earlier exploratory runs with optional diagnostics remain
non-acceptance research only.

## Disposition

S4 is closed.  S5 owns all integration observations and any classification of
an original program return.  No S4 result claims that keyboard installation,
video presentation or general DOS graphics compatibility has run successfully.
