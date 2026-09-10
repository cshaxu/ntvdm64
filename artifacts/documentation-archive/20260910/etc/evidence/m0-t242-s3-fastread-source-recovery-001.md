# M0 T242 S3: FastRead Source Recovery

## Question

After the admitted `BIOS[15]/AH=88h` prerequisite, why does the source-built
bundle reach `50:42` and subsequently enter an unproductive guest loop; can
that reached local-DOS read edge be recovered without moving DOS file or
process semantics into the machine layers?

## Inputs

- `src/opennt/base/mvdm/dos/v86/doskrnl/dos/handle.asm:447-579`.
- `src/opennt/base/mvdm/dos/dem/demdisp.c:165,230-238` and
  `src/opennt/base/mvdm/dos/dem/demhndl.c:93-224`.
- The active direct OpenNT import and CCPU/SAS replacement at
  `src/bx-vdm/bop/{opennt/dem/demhndl.c,shim/demhndl_shim.c}`.
- The bounded source-built observation recorded in
  `m0-t242-s3-int15-ems-source-mirror-001.md`.

## Source And ABI Map

`FastOrSlow` selects the fast branch for a non-pipe SFT when the i386
`MIPS_BIT_MASK` is clear.  That branch supplies the normal file-read ABI
`AX:BP` opaque handle, `BX:SI` position, `CX` count, `DS:DX` buffer and ZF,
then executes `SVC_DEMFASTREAD` (`50:42`).  On carry clear, the guest-owned
`handle.asm` continuation advances the SFT position by the returned `AX`.

The imported `demdisp.c` deliberately maps `42h` to
`demNotYetImplemented`, which clears carry and leaves `AX` intact.  An exact
symbol sweep of the admitted OpenNT and SoftPC sources finds no fast-read
worker.  Thus the original table body is source-complete as an unavailable
historical endpoint, but it is insufficient for the selected x86 caller: an
unchanged stale `AX` is not a valid read count.

The directly imported `demRead` body has the same input and output ABI.  It
performs the host read through the existing opaque-handle/checked-guest-RAM
seam, returns the actual byte count in `AX`, preserves the source error
algorithm, and never changes guest SFT position.  The guest retains selection,
position update and slow-path behavior.

## Decision

Use the smallest source-derived composition seam:

- `dem_ingress_shim.c` recognizes only an otherwise valid `C4 C4 50 42`;
- it calls `bx_ntvdm_demhndl_invoke_fast_read`;
- that helper accepts only service `42h` and invokes the imported OpenNT
  `demRead` body through the existing scoped CCPU/SAS replacement;
- all other DEM services, including `50:43`, preserve original
  `DemDispatch` table ordering and disposition.

This is not a dispatcher-table edit, a synthetic carry fallback, a MIPS-flag
change, a new filesystem provider, or a Bochs/mantle change.  It is a
contract-preserving replacement for the unavailable historical fast worker.

## Verification

Fresh external formal Ninja root `build/M0-T242-S3/formal-r5` compiled the
active `bx-vdm` archive and
`t230-s2-demhndl-direct-import-fixture.exe`.  The fixture opens a disposable
local file through the current opaque handle context, verifies the ordinary
`demRead` route, resets the file position, then sends exactly `C4 C4 50 42`.
It proves copied `hello` bytes, `AX=5`, carry clear and `RIP+4`.  The prior
original-table no-op control remains covered independently.

The same formal root then linked `ntdos64-native.exe` and ran exactly one
100,000-tick source-built observation with the frozen DOS/WOW roots and
`share.exe` input.  It accepted the same pre-FastRead sequence through
`50:12`, `50:00`, `50:42`, and `50:02`; there were no faults.  Unlike the
preceding AH=88h-only run, the budget terminal was `CS:EIP=8e08:2f57`, not
`0800:0000`.  The observer records ingress-state rather than post-provider
register/memory deltas, so this is progression evidence, not a claim of a
specific transferred count, `$Exec`, child entry, or parent return.

## Interpretation And Follow-Up

The recovered edge is a local file-transfer prerequisite, not proof of
`INT 21h/AH=4Bh`, PSP/arena/JFN construction, child entry or parent return.
One bounded source-built observation may now validate the coherent `AH=88h`
plus FastRead group.  Any residual after that observation must first be added
to the tracker with its source owner; it is not admitted as a trace-led leaf
implementation.
