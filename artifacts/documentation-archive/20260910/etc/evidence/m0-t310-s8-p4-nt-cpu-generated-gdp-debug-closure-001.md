# M0 T310 S8 P4: `nt_cpu` generated-GDP debug closure

## Scope

This record closes the historical x86/x64 ABI worklist rows for
`src/mvdm-host/softpc.new/host/src/nt_cpu.c`.  It covers only private SoftPC
GDP state; it does not add a guest-address or external-handle mapping route.

## Finding

The former non-`PROD` branch compared two selected GDP fields with
`GdpOffsetFromName`.  The helper has no definition or declaration in any of
the three retained OpenNT source baselines.  It was meaningful only for the
historical fixed-width packed GDP.  The selected build instead uses the
generated native-width GDP carrier registered by `MVDM-HOST-DIV-048`, so the
numeric packed-offset assertion no longer has a valid representation to
inspect.

## Resolution

`nt_cpu.c` retains the original source-named GDP fields and removes only the
invalid non-`PROD` packed-offset check.  The file records the divergence at
the exact removed checks; `src/mvdm-host/README.md` expands the existing
`MVDM-HOST-DIV-048` entry.  No adapter, mapping-manager identity, warning
suppression, or changed product CPU control flow was added.

## Verification

- Formal Ninja rebuilt `obj/host/nt_cpu.obj` for both x64 and x86 product
  graphs.
- The exact Ninja compile command was then rerun for both architectures with
  `PROD` removed.  Both non-product diagnostic builds succeeded without
  `C4311`, `C4312`, `C4057`, `C4115`, `C4133`, or `C4013` from `nt_cpu.c`.
- The remaining `C4005` macro redefinitions originate in the original
  `host_def.h` product-configuration spelling.  They are outside this
  pointer/call-ABI closure and remain visible.

## Ledger disposition

All 81 `nt_cpu.c` rows in
`m0-t310-s8-p4-whole-tree-warning-x86-x64-classification-worklist.tsv` are
now `closed-generated-private-gdp-slot` with the verification basis above.
