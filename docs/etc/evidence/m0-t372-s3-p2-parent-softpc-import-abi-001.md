# M0 T372 S3 P2 — Parent SoftPC import ABI closure

## Scope

Recover the original parent-process `ntvdm.lib` import contract required by
the owner `VDMREDIR.dll`.  This is not a Redirector-provider replacement and
does not activate a `57:xx` selector.

## Original contract

`src/mvdm-host/softpc.new/obj.vdm/obj/i386/ntvdm.def` is the original
`ntvdm.exe` export definition.  It exports the CCPU register forms,
`Sim32pGetVDMPointer`, `host_simulate`, `WaitIfIdle`, `SoftPcEoi`, ICA,
VDD and debugger entrypoints used by late-loaded owner DLLs.  In particular,
`nt_bop.c::LoadVdmRedir` keeps the original dynamic `LoadLibrary` and
`GetProcAddress` composition rather than linking Redirector into the host
image.

The formal generator now links the existing parent executable with that
unchanged definition and writes `original-softpc-process-import.lib`.  A
Redirector DLL can therefore import the one already-running SoftPC instance;
it never receives a second static CCPU/SoftPC link.

## Minimal completion needed for the complete original export table

The first formal link exposed only two missing exports:

- `DBGNotifyNewTask`;
- `DBGNotifyRemoteThreadAddress`.

Their full original bodies are private NT4 VDM debugger transport.  The
ordinary non-debug source result is no event.  The adapter consequently
retains both original exported names and parameter shapes; non-debug calls
remain no-event, while an actually debugged call takes the existing typed
session-unavailable result.  This is registered as
`ADAPTER-MVDM-DEBUGGER-004`; it neither recreates the private transport nor
terminates the application process.

## Formal evidence

From an external MSVC x86 environment:

```text
New-T310OriginalSoftpcNinja.ps1 -Architecture x86 \
  -BuildRoot O:\repos.hobby\ntvdm64\build\M0-T371\S2\formal-x86 \
  -NodeExecutable O:\.nvm\versions\node\v22.22.1\bin\node.exe
ninja -C O:\repos.hobby\ntvdm64\build\M0-T371\S2\formal-x86 \
  original-softpc-process.exe
```

The final parent link succeeded and emitted
`original-softpc-process-import.lib`.  `dumpbin /exports` confirms, among
others, these original symbols: `Sim32pGetVDMPointer`, `SoftPcEoi`,
`WaitIfIdle`, `call_ica_hw_interrupt`, `DBGNotifyNewTask`, and
`DBGNotifyRemoteThreadAddress`.

A disposable whole-archive `VDMREDIR` link using the new import library plus
the existing Redirector, SoftPC, Win32 and session binding libraries leaves
only 19 unresolved external names.  They all belong to the later public
network / NetBIOS / RAP group:

- `XsNet*`, `RxpTransactSmb`, `RxNetUserPasswordSet`;
- `NetpCopyWStrToStr`, `NetpNtStatusToApiStatus`,
  `GetLanmanSessionKey`;
- `SystemFunction006/012/016`;
- list/heap helpers used only by the NetBIOS body.

No CCPU register, `Sim32*`, SoftPC, VDD, session, guest-memory, named-pipe,
thread-start or Redirector-local adapter import remains unresolved.  The
disposable image is not a product DLL because `/force:unresolved` was used
solely to disclose S5 ownership; it is not staged or enabled.

## Disposition

The source-shaped parent import ABI is closed.  The actual `VDMREDIR.dll`
image remains an S3/S4/S5 combined deliverable: its source-defined entry and
ten exports will be linked only after local lifecycle and the remaining
network-owner dispositions no longer require forced unresolved imports.
