# M0 T379 S1 — Original VDMREDIR product admission

## Question

What is the smallest complete original `VDMREDIR.dll` product boundary that
can be admitted after T372's source/binding closure, without converting the
Redirector into a statically linked `nt_bop` provider or treating an arbitrary
`57:xx` trace as the implementation plan?

## Inputs

* Selected original `mvdm-host/vdmredir/sources`, `vdmredir.def`, all sixteen
  listed source/resource units, and `softpc.new/host/src/nt_bop.c`.
* T372 S3 P2 parent-import closure and T372 S5 P6 complete network-owner
  boundary closure.
* Current CPU40 formal graph at
  `build/M0-T371/S2/formal-x86/build.ninja` and the formal runtime staging
  contract in `tools/build/Stage-OriginalSoftpcRuntime.mjs`.
* Source-built immutable DOS workload `build/output/dos/REDIR.EXE`, SHA-256
  `b6e9fad30a5423ead9ecb45c8e28197ea62a39187d36241f9018db82facac3a7`;
  its established source-build identity is recorded in
  `docs/etc/evidence/t235-s5-guest-bundle-result-001.md`.

## Procedure

1. Read the original package manifest, DLL entry and export definition;
   compare them with the parent loader rather than inferring a product from
   the existing static archive.
2. Reconcile every export with `LoadVdmRedir` and with the established parent
   `ntvdm.lib` import boundary.
3. Re-read the whole-archive residual analysis, grouping unresolved imports
   by their original package owner rather than by individual symbol.
4. Verify the formal graph's selected Redirector artifacts and the fixed
   runtime workload/provenance.  No product execution occurs in this S.

## Original product topology

The original `sources` file defines one `DYNLINK` product named `vdmredir`,
with `VrDllInitialize` as its entry and these sixteen original translation or
resource inputs:

```
vdmredir.rc vrmslot.c vrnmpipe.c vrdisp.c vrinit.c vrmisc.c vrnetapi.c
vrnetb.c vrputil.c vrremote.c vrdlc5c.c vrdlcpst.c vrdlcbuf.c vrdlcdbg.c
vrdebug.c vrdll.c
```

`vdmredir.def` exports exactly ten public names:

```
VrDispatch                 VrInitialized
VrReadNamedPipe            VrWriteNamedPipe
VrIsNamedPipeName          VrIsNamedPipeHandle
VrAddOpenNamedPipeInfo     VrConvertLocalNtPipeName
VrRemoveOpenNamedPipeInfo  VrCancelPipeIo
```

The parent product must remain dynamic.  `MS_bop_7` preserves the original
three-state load rule: it loads `VDMREDIR`, resolves nine named imports, then
calls `VrDispatch` with the original one-byte service number; a failed first
load becomes the retained `CF=1`, `AX=ERROR_INVALID_FUNCTION` result and no
further load is tried in that session.  `VrCancelPipeIo` is an original DLL
export but is not among those nine parent `GetProcAddress` calls.  It remains
part of the DLL product contract and must not be silently omitted.

T372 already established the parent side: `original-softpc-process.exe`
emits `original-softpc-process-import.lib`, so a DLL imports the existing
SoftPC instance rather than linking a second executor.  The current final
EXE intentionally does not consume `original-mvdm-redir.lib`, and no
`VDMREDIR.dll` target presently exists.  This is the exact product gap that
S2 owns.

## Whole import disposition

The original local resource and machine-facing imports already have a bounded
owner path: `adapter-mvdm-host-out/redir` supplies same-shaped handle,
mailslot, checked guest-copy and asynchronous named-pipe boundaries; `session`
owns opaque identities and leases; the parent import library supplies original
SoftPC exports.  They are not a reason to add a new Redirector provider.

The remaining complete blocking owner is the **remote RAP transaction and
conversion cohort**, not twelve independent missing functions:

* `RxpTransactSmb`, `RxNetUserPasswordSet` and `GetLanmanSessionKey` belong
  to the original RpcXlate (`RxCommon`/`RxApi`) route.  Their reached bodies
  require the private Lanman redirector FSCTL/RAP contract.
* `XsNetMessageBufferSend`, `XsNetServerEnum2`, `XsNetServiceControl`,
  `XsNetUseAdd`, `XsNetUseDel`, `XsNetUseEnum`, `XsNetUseGetInfo` and
  `XsNetWkstaGetInfo` belong to the original 23-source `XACTSRV` DLL.  Its
  RAP layout/conversion and private Browser/service paths cannot be replaced
  by name-matched public NetAPI calls.

T372 S5 P6 proves no NetBIOS/DLC residual remains in this set.  The cohort
therefore has one S2 disposition: preserve the original callers and their
failure branches, but provide the complete product's explicit
source-shaped-unavailable import boundary for the non-invasive local profile.
S2 must not fabricate network success, import a selective XACTSRV fragment,
or build an SMB/RAP client.

## Selected workload and S2 cohort

The immutable source-built `REDIR.EXE` is the only declared workload for this
task.  It is staged at `system32/REDIR.EXE` by the standard runtime stager and
is distinct from the retained historical `redir/obj/redir.exe` intermediate;
the latter is not selected as runtime media.  The S2 product cohort is:

1. one original `VDMREDIR.dll`, linked from the entire original source list
   through its original `.def` and `VrDllInitialize`;
2. the already-established original parent import library and the existing
   same-shaped local bindings; and
3. one package-level, explicit unavailable disposition for the complete
   RpcXlate/XACTSRV remote cohort.

This preserves the original dynamic loader and makes it possible for S4 to
prove loader success plus both local and unavailable directions without
claiming remote `57:xx` compatibility.

## Observations

* The original package is a DLL product, not a static BOP dispatch library.
* Its ten-export surface is larger than the nine imports currently resolved
  by `LoadVdmRedir`; the formal product must retain all ten.
* Current formal CPU40 selection preserves the source archive/bindings but
  has no DLL target, so its existing `ERROR_INVALID_FUNCTION` loader result
  is a correct T372 baseline, not Redirector activation.
* The sole unresolved import owner group is remote RAP/XACTSRV.  Local
  resources do not justify a second product owner or a trace-selected leaf
  implementation.

## Interpretation and confidence

High confidence.  The original manifest, `.def`, parent loader and T372
whole-archive residual evidence agree on one finite product boundary.  S2 may
now compose the original product only if its remote cohort remains explicitly
unavailable with the original caller error paths intact.  This is product
activation, not a claim that all fifty `57:00..31` services are implemented.

## Follow-up

S2 composes the original DLL and its parent loader closure.  S3 then examines
the selected local lifecycle group; S4/S5 respectively make the formal
matrix and one frozen `REDIR.EXE` observation.  A future remote/RAP package
is required before any claim of LANman/RPC/XACTSRV compatibility.
