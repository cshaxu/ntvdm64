# T229 S2 P12 — DEM handle cursor and EOF Direct/Readonly conformance

## Scope

This owner-group regression covers the shared `demhndl.c` normal-handle
contract, rather than selecting a new BOP from a runtime trace:

- `50:00` (`demChgFilePtr`) receives `AX:BP`, `CX:DX`, and `BL`, then returns
  the resulting 32-bit position in `DX:AX` with CF clear.
- An unknown `AX:BP` token returns `ERROR_INVALID_HANDLE` (`AX=6`, CF set)
  through the current `demClientError`-shaped error result.
- `50:16` (`demRead`) reads fixture bytes through the checked guest-RAM path.
  A subsequent request with ZF set deliberately retains the current cursor;
  at EOF it returns `AX=0`, CF clear and creates no guest-RAM write.

## Original source basis

`src/opennt/base/mvdm/dos/dem/demhndl.c` defines `demChgFilePtr` with the
`CX:DX` / `BL` to `DX:AX` contract.  Its `demRead` only calls
`SetFilePointer` when ZF is clear; a successful `ReadFile` transfers its
actual byte count and clears CF.  A zero byte result at EOF is therefore a
successful result, not an adapter-specific error.

## Verification

The formal Ninja target was rebuilt and both profile runs passed:

```text
ninja -C build/M0-T229-S2/formal-direct-readonly-20260818b \
  bin/t225-s19-dem-package-lifecycle-fixture.exe
...fixture.exe direct pdb-lifecycle    => 0
...fixture.exe readonly pdb-lifecycle  => 0
```

The test lives in
`tests/bx-vdm/bx_ntvdm_dem_package_family_v1_test.c`, under the installed
whole-provider lifecycle.  It does not claim host-read failure, nonzero or
relative seek, nor the complete file/handle/FCB/search package.