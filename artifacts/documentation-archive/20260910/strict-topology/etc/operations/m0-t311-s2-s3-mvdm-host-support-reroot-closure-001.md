# M0 T311 S2/S3 MVDM host support re-root closure

## Question

Can the selected original MVDM `inc`, `oemuni`, `suballoc`, `dirs` and
`makefil0` paths be restored below `mvdm-host` without changing their source
contracts or leaving an obsolete support component in the live build graph?

## Inputs

- S1 source/consumer record:
  `m0-t311-s1-mvdm-support-provenance-closure-001.md`.
- Selected OpenNT and OpenNT-src-2 MVDM baselines.
- The selected Win32/x86 CCPU40 formal Ninja generator.

## Result

- `git mv` preserved every selected original path under
  `src/mvdm-host/{inc,oemuni,suballoc,dirs,makefil0}`.
- The only material support implementation moved to
  `src/mvdm-host-overlay/oemuni/oemuni_pointer_width.c`; its register row is
  `SUPPORT-OVERLAY-001`.
- The re-root provenance ledger records 117 selected paths: 103 are
  byte-identical to OpenNT and 109 to OpenNT-src-2. The seven differing
  declarations remain the registered minimal divergences; OpenNT-src-2-only
  historical declaration/build-product paths remain selected supplement
  evidence.
- Live SoftPC/Redirector includes and the formal Ninja graph use
  `src/mvdm-host/inc`. The support archives are now named
  `original-mvdm-host-suballoc.lib` and `original-mvdm-host-oemuni.lib`.
- The formerly separate support README registers were rehomed. The host and
  host-overlay registers have no duplicate divergence identifier.
- The formal graph at `build/M0-T311/S3/r1` completed its 399 selected
  Win32/x86 CCPU40 edges; its forced link completed with no old-root reference.
  Its known unresolved entries are unrelated LIM, DEM module/debug, startup,
  NT virtual-memory and debugger/hard-error owners.

## Interpretation

The original support paths are package-internal MVDM host topology, not a
separate product component. The migration changes ownership/path and build
labels only; it does not add a provider, adapter or behavior.

## Follow-up

Later package work consumes these paths through `mvdm-host`. Historical task
records may retain their prior names and paths as evidence, but no live
production or selected formal-build input uses the retired roots.
