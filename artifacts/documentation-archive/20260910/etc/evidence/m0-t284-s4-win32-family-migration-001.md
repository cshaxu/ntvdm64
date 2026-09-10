# M0 T284 S4 — Win32-family migration

## Result

The complete S1-assigned Win32 family is now owned by
`src/adapter-mvdm-host-out/win32/`. The move preserves the existing public
spellings and source-facing facade contracts; it does not enable a provider or
change a historical call result.

## Moved audited inputs

- Headers: `nt.h`, `ntioapi.h`, `ntrtl.h`, `nturtl.h`, `vdmapi.h`,
  `winbasep.h`, and `winconp.h` now live in `win32/include/`.
- Facades: `ntioapi_facade.c` and `opennt_support_rtl.c` now live in
  `win32/source/`.
- Fixtures: the declaration and NT-I/O facade fixtures now live in
  `tests/adapter-mvdm-host-out/win32/`.
- The previous `src/adapter-win32/README.md` divergence register is merged
  into `src/adapter-mvdm-host-out/win32/README.md`; its records retain their
  original IDs and file scope.

## Binding repairs

- The two forwarding declaration carriers received only the additional `../`
  path segment required by the new family nesting.
- Formal graph module identity is `adapter-mvdm-host-out-win32`; family paths
  remain `adapter-mvdm-host-out/win32`. This prevents Ninja from treating a
  directory separator as part of a static-library output name.
- The focused NT-I/O graph now declares the pre-existing direct include roots
  for `adapter-softpc` and `src`, required by the reached original `dem.h`
  guest-location declaration. No source body was changed.

## Verification

- An active-source/build/test scan found no remaining `adapter-win32` root.
- Formal MSVC/Ninja rebuilds passed on x86 and x64:
  - T283 original COMMAND and DEM declaration-carrier archives;
  - T279 RTL support archive (and x64 pointer-width fixture);
  - T280 NT-I/O facade fixture.
- The compiler emitted only retained historical-source warnings in the
  original carrier builds; no migration error remains.

This closes S4. The next packet is the separately bounded SoftPC/CCPU/SAS
family migration; it must preserve the `adapter-bochs` typed machine seam.
