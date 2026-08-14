# M0 T199 S35 P1: Native XMS/DPMI Composition Closure

`bx_ntvdm_native_bop_composition_v1` is a new `bx-vdm` composition root. It
owns only copied ingress, provider selection, and the existing XMS/DPMI
session records. It has no Bochs include, no native pointer, no machine state,
and no BOP recognition outside `bx-vdm`.

The native fixture initializes `bx_ntvdm_minimal_machine_c` first, then
initializes and binds this root. Through the root it proves:

- `52:00` returns the typed A20 success resume;
- `52:02` uses the real C2 aperture after the source-derived 64 KiB reserve,
  returning `1088 KiB`;
- `53:12` returns the explicit controlled-unavailable stop;
- ordinary deferred DPMI (`53:00`) publishes no outcome and is not mistaken
  for a successful continuation.

The exact native evidence is
`artifacts/build/t199-s31-xms-native-r5-backing/t199-s35-native-bop-composition.exe`.
It is an MSVC x64 `/MT` link against the complete CPU5/Pentium-MMX minimal
machine closure and exited zero.

This is P1 only. It proves the boundary and lifecycle ordering in one process;
it does not yet make native-root bind/unbind part of CLI startup, and it does
not run a guest trace.

## One external bridge

The existing exported `bx_ntvdm_mantle_generic_ud_bridge_v1` remains the only
external #UD bridge. Its `bx-vdm` implementation now tries the legacy
boot-namespace root first and tries the native root only when that root
declines. `artifacts/build/t199-s35-native-bop-bridge-r1/t199-s35-native-bop-bridge.exe`
is a MSVC x64 `/W4 /WX /MT` regression with a declining legacy-root stub. It
binds the native root and proves that `52:02` is then handled through the one
exported bridge with the expected `1088 KiB` result.

The remaining S35 work is lifecycle admission: a production CLI/machine
composition must initialize/bind this root only after the mantle machine is
live, and unbind it before mantle cleanup. That responsibility must not be
hidden in the legacy C-only boot-namespace root.
