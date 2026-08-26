# opennt-mvdm-firmware

Canonical original MVDM firmware-input mirror. Its final retained
upstream-relative package set is `softpc.new/base/bios`, `softpc.new/bios`,
`softpc.new/roms` and `softpc.new/data`.

The current T276 audit keeps the byte-exact source union temporarily under
`src/opennt-mvdm-host`; no source is copied, changed, compiled, linked or
deleted by this skeleton. A dedicated hash-verified `git mv` packet must
establish this component. It is an immutable input carrier: it has no host
runtime library edge and never enables a parallel SoftPC machine. Only an
admitted `adapter-bochs` manifest may select a firmware input later.

## Divergence register

None. No upstream source has entered this root yet.
