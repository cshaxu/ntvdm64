# M0 T302 S2 P2 — BaseClient local facade

## Question

Can the reached BaseClient entry retain its original caller-facing
`GetNextVDMCommand(PVDMINFO)` request/result boundary without importing the
whole private CSR/Base DLL product shell?

## Inputs

- `src/opennt-host/base/win32/client/vdm.c:340-727`;
- `src/opennt-host/base/win32/server/srvvdm.c:130-410`;
- `basedll.h`, `basemsg.h`, `ntcsrdll.h` and the T302 P1 local record;
- T302 P1 focused x86/x64 fixture.

## Direct-composition probe

The selected original client translation unit was compiled using the T302
MSVC x86/x64 environment and its reached BaseClient/BaseSrv/ABI include roots.
The first unblocked original-header failure is
`src/opennt-abi/source/public/sdk/inc/nt.h:41`, which requires unavailable
`ntkeapi.h`; before supplying the historical architecture macro, the modern
SDK `ntdef.h` rejected the old header set for lacking a target architecture.
The selected function itself requires private `basedll.h`, CSR capture
allocation, `CsrClientCallServer`, `NtCurrentPeb`, `BaseSetLastNTError` and
the Base API message product shell. Directly compiling the full file would
therefore import unrelated Base DLL API bodies and private CSR transport.

## Selected recovery

`source/base_vdm_client.c` owns the historical public spelling. It takes a
caller-local `VDMINFO` snapshot, calls the named local BaseSrv dispatcher, and
copies all size/result fields back in the source-shaped request/result order.
`source/base_vdm_local.c` owns only the reached one-session DOS record
selection/copy/capacity/reentry behavior. The client and server retain no
caller `VDMINFO` pointer after return.

## Verification

```text
powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T302BaseVdmLocalNinja.ps1 -Architecture x64
ninja -C build/M0-T302/S2/x64 test
powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T302BaseVdmLocalNinja.ps1 -Architecture x86
ninja -C build/M0-T302/S2/x86 test
```

Both commands report `PASS: local VDM command transport`.

## Boundary

This is a smallest source-shaped client facade, not a CSR client replacement.
First-VDM system query, CSR capture transport, BaseSrv wait/wake object pairs,
CSRSS process context, multi-process routing, WOW/PIF and child lifecycle are
explicitly deferred to their queue owners.
