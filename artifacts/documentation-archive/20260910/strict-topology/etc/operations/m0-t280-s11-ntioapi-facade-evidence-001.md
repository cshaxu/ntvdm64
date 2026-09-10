# M0 T280 S11 — reached NT I/O facade evidence

## Question

Can the reached OpenNT NT I/O call surface be exposed to original DEM source on
x86 and x64 without including the whole historical `ntioapi.h`, changing a DEM
body, or mapping host-local data as guest identity?

## Inputs and procedure

The exact platform mirror, the S10 eleven-row ledger, original `demsrch.c`,
`demerror.c`, `demgset.c`, `adapter-win32/include/nt.h`, and the new
same-shaped reached facade were reviewed. In an MSVC Developer environment:

```text
VsDevCmd -arch=x64 -host_arch=x64
New-T280S11NtioapiFacadeNinja.ps1 -Architecture x64
ninja -C build/M0-T280/x64-ntioapi-facade

VsDevCmd -arch=x86 -host_arch=x64
New-T280S11NtioapiFacadeNinja.ps1 -Architecture x86
ninja -C build/M0-T280/x86-ntioapi-facade
```

Both graphs compiled and linked `ntioapi_facade.c` with its focused fixture.

## Observations and interpretation

- Both architecture graphs passed.
- Modern `IO_STATUS_BLOCK` has pointer-sized `Information` on x64; historical
  DEM requires its exact 32-bit record. The facade preserves the original
  source spelling through a local adapter type and converts only the
  synchronous host-local status record around the dynamic `ntdll` call.
- The facade owns directory, volume and symbolic-link call binding. It
  resolves the matching export at call time and returns `STATUS_NOT_IMPLEMENTED`
  if unavailable, rather than substituting a different Win32 success result.
- `HANDLE`, temporary records, name buffers and dynamic entry pointers remain
  host-local. There is no mapping-manager call or guest pointer in the facade.

## Limitation and follow-up

This is an ABI/binding closure only. It does not enable original DEM translation
units, select a directory provider, or copy a directory record to a guest DTA.
Those later packages consume the facade through the separate `adapter-softpc`
guest-memory lease boundary.
