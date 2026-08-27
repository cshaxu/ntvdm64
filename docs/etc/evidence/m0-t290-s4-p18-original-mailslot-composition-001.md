# M0 T290 S4 P18 — original mailslot composition

## Result

The selected original `mvdm-host/vdmredir/vrmslot.c` mailslot body now runs on
formal external MSVC/Ninja x86 and x64 fixtures.  This is a local owner-package
proof only; no `57:xx` selector ingress is enabled.

## Preserved original behaviour

- `VrMakeMailslot` uses the original namespace conversion, native public
  `CreateMailslot` call, original record/list population and AX/CF result.
- `VrGetMailslotInfo`, `VrWriteMailslot`, `VrReadMailslot`,
  `VrDeleteMailslot` and `VrTerminateMailslots` retain their original source
  bodies, public Win32 calls and register/error sequence.
- `VrPeekMailslot` is proven to retain its original explicit
  `ERROR_NOT_SUPPORTED` result; no NT-level peek substitute was invented.

## Required bindings

- `MVDM-HOST-DIV-021` replaces only the historical private 16-bit bitmap
  allocator with `adapter-mvdm-host-out/redir` access to the current session's
  existing host-resource mapping manager.  The exported guest value remains a
  nonzero `WORD`; native record pointers and HANDLEs remain host-private.
- `MVDM-SUPPORT-DIV-003` preserves the packed four-byte guest far-address in
  `DosWriteMailslotStruct`; the source still decodes it with
  `READ_FAR_POINTER` under the existing synchronous Redirector pointer scope.
- The fixture declares exact read/write spans for the name, request structure,
  write buffer and read buffer.  Each lease ends before returning from the
  original source body.

## Formal matrix

For each architecture, run:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T290S4VrMailslotNinja.ps1 -Architecture <x86|x64> -RepositoryRoot .
ninja -C build/M0-T290/S4/vrmslot-<x86|x64> -v
build/M0-T290/S4/vrmslot-<x86|x64>/bin/t290-s4-vrmslot-fixture.exe
```

Both `x86` and `x64` completed successfully on 2026-08-27.  The fixture
proves make → info → write → read → delete, original peek decline, stale
identity rejection after delete, and PDB termination cleanup followed by
stale rejection.

## Transfer

The asynchronous named-pipe completion group remains S4 work.  It is not
implied by this synchronous mailslot proof: its source retains guest
destinations and ANR data beyond the call, and ANR delivery still requires the
explicitly excluded physical IRQ mechanical request.
