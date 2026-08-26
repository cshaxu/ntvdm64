# M0 T280 S5 — reached VDM directory-control evidence

The focused fixture validates four source-derived outcomes:

1. no bound session returns `STATUS_NOT_IMPLEMENTED`;
2. a bound session with no registered control provider returns
   `STATUS_NOT_IMPLEMENTED`;
3. `VdmStartExecution` remains explicitly unavailable even if a handler is
   registered; and
4. only `VdmQueryDir` passes the original `VDMQUERYDIRINFO` object to the
   registered synchronous session control handler.

The fixture is not a directory-query implementation. Its later provider is a
DEM namespace capability that preserves the original query/reset semantics
using public host APIs. The control object contains host-local pointers and
handles, so it intentionally never uses the guest-memory mapping manager.

## Formal runs — 2026-08-26

```text
VsDevCmd -arch=x64 -host_arch=x64
ninja -C build/M0-T280/x64-vdm-control test
PASS: VdmQueryDir adapter contract

VsDevCmd -arch=x86 -host_arch=x64
ninja -C build/M0-T280/x86-vdm-control test
PASS: VdmQueryDir adapter contract
```

Both graphs use `/MT /W4` and Ninja `deps = msvc`; neither includes a DEM body,
Bochs source, a kernel VDM call, or a guest-memory mapping operation.
