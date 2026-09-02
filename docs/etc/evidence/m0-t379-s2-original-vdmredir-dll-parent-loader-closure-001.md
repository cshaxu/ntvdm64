# M0 T379 S2 — Original VDMREDIR DLL and parent-loader closure

## Question

Can the full original `vdmredir` source package become a real CPU40/x86 DLL
with its original entry and export surface, while importing the one active
SoftPC process and retaining source-shaped unavailable results for the
uncomposable RAP owner package?

## Inputs

* S1 admission ledger and the original `vdmredir/sources` and `vdmredir.def`.
* Existing T372 parent import library and same-shaped local Redirector,
  SoftPC, Win32 and session binding libraries.
* `tools/build/New-T310OriginalSoftpcNinja.ps1`, formal root
  `build/M0-T379/S2/formal-x86`, and original parent `nt_bop.c` loader.

## Implementation

The formal graph now has a `VDMREDIR.dll` target which:

1. compiles every source member named by the original manifest plus the
   original `vdmredir.rc` resource;
2. links through the unmodified original `vdmredir.def`, therefore retaining
   `VrDllInitialize` and all ten original exports;
3. consumes `original-softpc-process-import.lib`, which is emitted as an
   implicit result of the original parent EXE link; and
4. links already-existing same-shaped local bindings and original NetLib/API
   helpers as ordinary DLL inputs.

The one new adapter source is a **single package boundary**, not an alternate
Redirector provider.  It provides the exact signatures imported by the
complete RpcXlate/XACTSRV remote cohort.  `RxpTransactSmb`,
`RxNetUserPasswordSet` and `GetLanmanSessionKey` return
`ERROR_CALL_NOT_IMPLEMENTED`; the XACTSRV handler forms return
`STATUS_NOT_IMPLEMENTED`.  Original `vrremote.c`/`vrnetapi.c` remain the
callers and retain their original status mapping, DOS-register failure paths
and all local-resource behavior.  No SMB/RAP request, name-matched NetAPI
success result, or selective XACTSRV body is introduced.

During the first full link, 73 unresolved externals appeared.  They reduced
to ten after the link rule correctly consumed its declared libraries.  Those
ten were all pre-existing owner inputs omitted from the DLL command, not new
product semantics: session identity/location mechanics live in
`softpc-bindings.lib`, while original NetLib Unicode conversions are public
`ntdll` imports.  Adding these inputs completed the product without changing
the original VDMREDIR body.

## Verification

From an external MSVC x86 environment:

```text
New-T310OriginalSoftpcNinja.ps1 -Architecture x86 \
  -RepositoryRoot O:\repos.hobby\ntvdm64 \
  -BuildRoot O:\repos.hobby\ntvdm64\build\M0-T379\S2\formal-x86 \
  -NodeExecutable O:\.nvm\versions\node\v22.22.1\bin\node.exe \
  -ParallelJobs 8
build\M0-T379\S2\formal-x86\run-ninja-parallel.cmd VDMREDIR.dll
build\M0-T379\S2\formal-x86\run-ninja-parallel.cmd -n VDMREDIR.dll
```

The DLL link completed and the second command reported `ninja: no work to do`.
A PE export-table read of the produced DLL reported exactly these ten names:

```text
VrAddOpenNamedPipeInfo     VrCancelPipeIo
VrConvertLocalNtPipeName   VrDispatch
VrInitialized              VrIsNamedPipeHandle
VrIsNamedPipeName          VrReadNamedPipe
VrRemoveOpenNamedPipeInfo  VrWriteNamedPipe
```

The source inspection remains the loader proof in this S: `MS_bop_7` retains
the original lazy `SafeLoadLibrary("VDMREDIR")`, the original nine lookup
sequence and its three-state failed-load result.  This S deliberately does
not load the DLL into an unrelated process: it imports the selected parent
EXE's export library and must be exercised only beside that parent in S4/S5.

## Observations

* The resulting DLL is a genuine original-product-shaped PE, not a forced
  unresolved archive and not a static `VrDispatch` substitute.
* It exports all ten original names even though the current parent resolves
  nine.  In particular, `VrCancelPipeIo` remains present.
* The parent remains the only SoftPC executor.  The DLL imports it through
  its generated original export library.
* Remote RAP/XACTSRV remains explicit unavailable behavior.  Local pipe,
  mailslot, identity and guest-copy routes still have their existing bounded
  owners.

## Interpretation and confidence

High confidence for source/link product closure.  The original full source
list, resource, entry/export definition and parent import ABI now compose in
one formal CPU40/x86 graph.  Runtime loading has not yet been claimed; S3
must complete the local lifecycle owner review before S4 stages the DLL beside
the parent and verifies its normal/failed loader directions.

## Follow-up

S3 audits and closes the selected local resource/lifecycle group, including
cancel and stale/disconnect directions.  S4 then packages this exact DLL with
the formal parent for the activation matrix; S5 makes the single frozen
`REDIR.EXE` observation.
