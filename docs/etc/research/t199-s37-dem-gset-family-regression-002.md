# T199 S37 DEM GSET Family Regression 002

## Question

Do the complete GSET drive/volume/DPB providers preserve one immutable,
CLI-admitted drive inventory and the original register/memory contracts,
including the distinct full-DPB and DPB-list forms?

## Evidence

The source-built MSVC x64 `/MT` all-DEM fixture at
`artifacts/build/t199-s37-dem-package-r87-gset-boundaries/` exits zero.

It proves, within one DEM package session:

- `50:0D` preserves AH and returns OpenNT's no-registry C: fallback in AL.
- `50:0E` returns the immutable snapshot's DOS-reduced geometry in
  AX/BX/CX/DX/SI with CF clear.
- `50:0F` counts the admitted A:/C: inventory according to the original
  contiguous-drive rule.
- `50:25` writes the immutable fake-DPB branch at checked `DS:DI`, including
  the source-defined assigned bytes and the terminal AX=5/CF result for an
  aperture-crossing destination.
- `50:46` writes two 35-byte packed entries for the A:/C: inventory, sets
  each Drive/Unit pair and link field, and leaves the byte after the two-entry
  output untouched.
- `50:0E` with an unadmitted drive reaches the contained AX=5/CF terminal
  result without a live volume query; `50:10` leaves its caller buffer
  unchanged on the same boundary.
- `50:19` and `50:1C` preserve the original setter failure shape: only AL is
  changed to `FF`; no carry flag is synthesized and the ambient host clock is
  never written.

The test also retains the existing `50:10`, `50:13`, `50:18`, `50:1A`,
`50:41`, and no-device IOCTL assertions, so the GSET component is checked as
one family rather than as a trace-selected endpoint.

## ABI correction

The two-entry `50:46` regression exposed that
`bx_ntvdm_dem_dpb_service_v1_prepare` wrote its final offset to fixed-width
GPR delta slot 6.  The Bochs CPU ABI order is AX, CX, DX, BX, SP, BP, SI, DI;
slot 6 is SI.  `demGetDPBList` advances BP.  The provider now writes slot 5,
and the regression asserts that exact BP delta.  No Bochs selector behavior,
machine capability, or BOP routing changed.

## Boundary

This closes the stated GSET family regression gap for admitted state, invalid
drive and bad guest range.  It does not create a BIOS BPB/BDS, raw drive, host
CWD, live machine name, or ambient host-volume query.  It is not a claim that
the whole DEM package, COMMAND, XMS/DPMI, or native CLI guest execution is
closed.
