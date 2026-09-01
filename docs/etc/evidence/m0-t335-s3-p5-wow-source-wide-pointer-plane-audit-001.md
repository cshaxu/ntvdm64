# M0 T335 S3 P5 — source-wide WOW32 temporary-pointer plane audit

## Complete selected-source inventory

`tools/research/Generate-WowPointerPlaneLedger.mjs` reads the original
`mvdm-host/wow32/sources` manifest and regenerates the companion ledger.  The
current selected set contains **77 original C translation units**.  The
ledger records every source use of each temporary-pointer form, grouped by
form and source file; it intentionally excludes macro definitions in headers.

This supersedes P2's 67-file convenience sample as the source-wide input for
S3.  It does not imply that all 77 units are enabled or that their GUI,
multimedia, printing, communication, OLE and broker dependencies have been
admitted.

## Source-derived partition

1. **Directly bounded forms** — `GETVDMPTR`, `ALLOCVDMPTR`, `GETFRAMEPTR`,
   `CBFRAMEPTR`, and explicit-length `GETSTRPTR` already carry a fixed span.
   They can use one active temporary-alias record: numeric 16:16 address,
   resolved CPU40 linear address, bounded read/write lease, explicit flush
   and exact release.
2. **Derived forms** — `GETARGPTR`/`FREEARGPTR` are offsets into an existing
   `VDMFRAME`; they never acquire or release a second lease.  The old
   `(ULONG)p` arithmetic is a distinct x64 mirror divergence and must become
   byte-pointer arithmetic only when the full source-level binding is enabled.
3. **Optional forms** — `GETOPTPTR`, `GETPSZPTR`, `GETPSZIDPTR` preserve their
   original zero/null branch.  Their non-null form requires the same bounded
   lease as the resolved pointer class.
4. **Variable/string forms** — `GETMISCPTR`, `GETVARSTRPTR`, terminated
   `GETPSZPTR` and `GETPSZPTRNOLOG` lack a complete span in the macro call.
   They cannot silently be treated as one byte or as an arbitrary 64-KiB
   read.  Each selected owner cohort must supply its original bounded record
   or string limit before its source is enabled.
5. **Arbitrary selector/long-lived forms** — `SEGPTR`, direct
   `GetPModeVDMPointer`, and `GetRModeVDMPointer` require a selector resolver
   beyond the current CPU40 loaded-segment subset.  `wucomm.c` also retains a
   communication alias beyond a synchronous call.  These are not legal
   consumers of a temporary session lease and remain unavailable until their
   original owner contracts are selected.
6. **Commit forms** — `FLUSHVDMPTR` may commit only the exact active write
   alias.  `FLUSHVDMCODEPTR` additionally requires the original CCPU code
   invalidation contract and cannot be replaced with a data-memory commit.
7. **Release forms** — each `FREE*` operation must identify the same active
   base alias, reject stale/derived pointers and never survive `host_simulate`
   or a worker boundary.

## Consequence for implementation

The next binding is one package-local WOW temporary-alias registry in
`adapter-mvdm-host-out/wow`, backed exclusively by the existing
`adapter-mvdm-host-out/softpc` guest-pointer seam.  It will not be a new
mapping manager.  It may enable only the direct bounded/frame subset until an
original source cohort proves the appropriate variable/string/selector
preconditions.

The original `wcall16.c` non-fast callback is a direct-bounded/frame cohort:
it needs `TD`, fixed `VDMFRAME`/`CBVDMFRAME`, its original stack arithmetic,
flush/free ordering and recursive CCPU40 return.  P3/P4 have established two
of those preconditions; P5 establishes that selecting it will not secretly
claim the unrelated 76 source units or their wider pointer requirements.

## Reproduction

```text
node tools/research/Generate-WowPointerPlaneLedger.mjs
```

The generated file is
[`m0-t335-s3-p5-wow-pointer-plane-source-usage-ledger.tsv`](../operations/m0-t335-s3-p5-wow-pointer-plane-source-usage-ledger.tsv).
