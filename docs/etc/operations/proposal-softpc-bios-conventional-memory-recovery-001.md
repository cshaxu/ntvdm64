# Proposal: SoftPC BIOS Conventional-Memory Reset/Query Recovery

## Purpose

Recover the first source-proven native boot dependency as one bounded SoftPC
BIOS component: reset-owned conventional-memory state and `BOP 12h`'s
`memory_size()` query.  This is not a general BIOS implementation and does
not change the canonical 203-host-BOP count: `12h` is a reached machine/BIOS
selector dependency of the guest bootstrap.

## Evidence and source authority

The source chain is:

```text
dos/v86/doskrnl/bios/sysinit1.asm:goinit
  -> C4 C4 12
  -> softpc.new/base/bios/bios.c: BIOS[0x12] = memory_size
  -> softpc.new/base/bios/mem_size.c: memory_size()
  -> sas_loadw(MEMORY_VAR) ; setAX(memory_size)
```

`softpc.new/base/bios/reset.c` owns initialization of `MEMORY_VAR`.  The
current native snapshot records `0040:0013 = 0x027f` for the pinned first
profile; see `../research/bop-12-memory-size-contract-r20.md`.  The existing
adapter-local helper is evidence-only legacy composition: it hard-codes a
profile result and also recognizes an unrelated `15h/AH=88h` form.  It must
not be promoted unchanged into this recovery.

## Boundary

- `bx-core` remains CPU, RAM/ROM and exception owner.
- `bx-mantle` may own an opaque, fixed-width conventional-memory profile value
  and its reset lifetime, but contains no BOP, BIOS, DOS, OpenNT or SoftPC
  selector vocabulary.
- `bx-vdm` alone recognizes the exact copied `C4 C4 12` window and translates
  an admitted machine response into the source-shaped low-16-bit `AX` result
  and `RIP + 3` resume.
- The original `mem_size.c` body is the source/layout/ordering authority.  If
  its SAS/CCPU calls cannot link, retain its structure in an imported mirror
  and use only the smallest named shim for the bounded read and AX write.  The
  mirror records every divergence beside the changed call.

No host pointer, Bochs object, selector-specific CPU action or guest-memory
inspection crosses this boundary.

## Admission plan

1. **S1 — source, reset and ABI ledger.** Verify `bios.c`, `mem_size.c`,
   `reset.c`, `bios.h`, `sas.h`, the current mantle reset profile and the
   actual current `BOP 12h` ingress.  Add a stable machine dependency row to
   `docs/etc/bop-list.md`; decide the exact declared first-profile KiB value
   from reset/source/snapshot evidence rather than convenience constants.
2. **S2 — selector-blind machine profile seam.** Give the mantle a reset-owned
   typed query contract with strict lifetime/range/teardown negatives.  It
   may not write a BOP result register or inspect guest instruction bytes.
3. **S3 — original-source-shaped provider.** Import/activate the smallest
   `mem_size` recovery through `bx-vdm`; preserve its low-16-bit AX-only
   outcome and decline non-exact/malformed/missing-profile paths.
4. **S4 — regression and one bounded observation.** Test reset/query values,
   result width, all negative boundaries and selector-blind scans.  Then run
   one source-built NTIO observation only to verify
   `50:11 -> 12h -> next declared boundary`; trace does not authorize a new
   leaf feature.

## Explicit non-goals

- `15h/AH=88h`, unless an independent source/owner audit admits it later.
- Keyboard, INT 15, PIC/IRQ, FPU, CMOS, VGA, block media, FDC/DMA, full BIOS
  ROM, DPMI, XMS UMB allocation, guest EXEC or COMMAND semantics.
- A generic machine configuration language or a Bochs product shell.

## Exit rule

The package closes only when the active route uses a source-shaped
`memory_size` recovery fed by a reset-owned selector-blind machine contract,
with focused local positive/negative tests and one bounded native observation.
It must leave the core/mantle free of BOP/DOS/OpenNT/SoftPC vocabulary and
must record all deferred machine needs in the live tracker.
