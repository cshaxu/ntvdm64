# M0 T335 S3 P2 — WOW pointer-access family audit

## Finding

The selected original `wow32/sources` manifest contains 67 C files that use
the historical VDM-pointer macro family. It is not safe to alter only
`wcall16.c`:

- 763 `GETARGPTR` uses;
- 402 `FREEVDMPTR` uses;
- 273 `GETVDMPTR` uses;
- 178 `FLUSHVDMPTR` uses;
- 164 `GETPSZPTR` uses;
- 148 `GETMISCPTR` uses;
- 95 `GETOPTPTR` uses;
- 36 `GETFRAMEPTR` uses;
- 20 `ALLOCVDMPTR` uses; and
- 3 `GETSTRPTR` uses.

The original macros derive raw aliases from `FlatAddress` and use no explicit
lease token. A callback-only macro change would make two incompatible memory
lifetime systems inside the same original WOW32 package. It is rejected.

## Consequence

P1 remains valid as the narrow implementation of a *single scoped frame
lease*, but it is not yet the integration interface for `wcall16.c`. The
proper next implementation is a package-local, same-named macro binding that
uses the one session guest-memory mapping instance, tracks nested temporary
aliases, commits exactly at `FLUSHVDMPTR`, and rejects unknown/stale aliases.
It must be designed for the full selected macro family, not only for the
callback source.

`GETARGPTR` is specifically not an independent mapping operation: it is
pointer arithmetic within a parent `VDMFRAME` lease. The full binding must
preserve that parent lifetime instead of attempting to map the derived value
again. Variable-size and string forms also require bounded extent/termination
rules before their first source body is enabled.

## Scope effect

No original WOW provider is enabled by this audit. The S3 next step is a
source-wide pointer-plane contract and implementation, followed by selection
of only the original non-GUI callback/frame cohort. This prevents a
trace-driven callback special case while retaining P1's already-verified
mechanical building block.

The macro counts and individual dispositions are recorded in
[`m0-t335-s3-p2-wow-pointer-access-family-ledger.tsv`](../operations/m0-t335-s3-p2-wow-pointer-access-family-ledger.tsv).
