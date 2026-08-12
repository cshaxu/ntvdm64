# T95 S7 DEMFASTREAD Host-Composition Gap

## Question

Why does the contained x86 profile reach `50:42`, and is it safe to substitute
the existing `DEMREAD` service?

## Evidence

- Trace 009 reaches `50:42` with AX=4001. The corresponding BP half is the
  private read-only token's high word, so `BP:AX` reconstructs
  `0x4e534001`; token/SFT ordering is not reversed.
- `handle.asm` calls `FastOrSlow`. It takes the fast BOP path when
  `MIPS_BIT_MASK` is clear; `nt_msscs.c` explicitly clears that bit on i386.
- `demdisp.c` maps `SVC_DEMFASTREAD` to `demNotYetImplemented`, and that
  helper clears CF without performing I/O. `handle.asm` only falls back to
  `DEMREAD` when the fast BOP returns CF set.

## Interpretation

The OpenNT sources prove an x86 fast-I/O composition dependency that is not
implemented by the visible DEM dispatcher. Returning CF=1 or executing the
adapter's `DEMREAD` implementation at `50:42` would alter the historical
success/fallback contract. The token is correctly shaped for the slow DEM ABI;
it is not evidence of an adapter SFT-layout bug.

## Decision

Keep `50:42` pass-through. Treat native x86 fast-BOP ownership as a separate
missing historical host-composition capability. Do not alter MIPS state, add
fast read, or reinterpret the BOP as a Bochs/device requirement without an
independent source/trace contract.
