# M0 T372 S6 P1 — Original Redirector dynamic-load and dispatch reconciliation

## Question

What does the selected CPU40/x86 product do when guest code reaches the
original `57:xx` BOP family, after S1--S5 have completed the source/link
dispositions for the whole original `VDMREDIR` provider package?

This is a source, selected-build and staged-image reconciliation. It does not
enable an individual service, create a substitute dispatcher, or claim that a
Redirector protocol was observed in a guest.

## Fixed inputs

* `src/mvdm-host/softpc.new/host/src/nt_bop.c`: the original parent BOP
  loader and `MS_bop_7` state machine.
* `src/mvdm-host/vdmredir/vdmredir.def`: the original DLL export contract.
* `src/mvdm-host/vdmredir/vrdisp.c`: the original `57:00..31` dispatch
  table.
* `docs/etc/operations/m0-t290-s1-redirector-service-abi-ledger.tsv`: the
  row-level original body, owner group and external-contract ledger.
* `tools/build/New-T310OriginalSoftpcNinja.ps1`: the selected formal CPU40/x86
  product graph.
* S3--S5 evidence, especially the explicit RpcXlate/RAP and XACTSRV
  unavailable dispositions in
  `m0-t372-s5-p6-network-owner-package-boundary-closure-001.md`.

## Original parent/DLL contract

`vdmredir.def` declares ten DLL exports: `VrDispatch`, `VrInitialized`,
`VrReadNamedPipe`, `VrWriteNamedPipe`, `VrIsNamedPipeName`,
`VrIsNamedPipeHandle`, `VrAddOpenNamedPipeInfo`,
`VrConvertLocalNtPipeName`, `VrRemoveOpenNamedPipeInfo`, and `VrCancelPipeIo`.

The original parent `LoadVdmRedir()` dynamically resolves the first nine in
that order. `VrCancelPipeIo` remains a valid original DLL export used by other
original callers, but it is not one of this parent's nine load-time
`GetProcAddress` requirements. A failed DLL load or any missing one of those
nine exports closes the module and returns `FALSE`.

`MS_bop_7()` retains the original tri-state behavior: state `0` attempts the
load once; success changes state to `1` and calls `VrDispatch` with the service
byte; failure changes state to `2`, sets `CF`, sets `AX` to
`ERROR_INVALID_FUNCTION`, and advances guest IP past the BOP byte; state `2`
repeats that same error/advance result without another loader attempt.

The source diff against the selected OpenNT parent contains only registered
observation, MSVC-cast and configuration-completion divergences outside the
`MS_bop_7` / `LoadVdmRedir` regions. No static replacement links `VrDispatch`
into the parent.

## Selected profile disposition

The formal target `original-softpc-candidate` compiles the complete original
provider archive, including `original-mvdm-redir.lib`; that establishes source
and static archive closure only. The runnable `original-softpc-process.exe`
link line deliberately does **not** include `original-mvdm-redir.lib` or
`redirector-bindings.lib`. The selected build tree and fixed `O:\ntvdm64`
stage contain no `VDMREDIR.dll`, import library, or export file.

Consequently the selected profile's source-defined dynamic result is not an
unresolved image and not a synthetic provider: `SafeLoadLibrary("VDMREDIR")`
fails, then the original parent exposes `CF=1`, `AX=ERROR_INVALID_FUNCTION`,
and `IP+1`. This global gate applies before the original provider dispatcher
sees a service byte. It preserves the existing S5 rule that no unavailable
RAP/XACTSRV or unobserved NetBIOS/DLC protocol is reported as executed.

## Complete `57:xx` entry reconciliation

Every row below has its original source body and detailed owner/external
contract in the linked S1 ABI ledger. Its common selected-profile disposition
is **original dynamic DLL unavailable before provider dispatch**; the final
column identifies the closed owner group whose source/link result supplies
that disposition.

| Selector | Original entry | Owner group | Selected-profile disposition source |
| --- | --- | --- | --- |
| `57:00` | `VrInitialize` | lifecycle | S3; global loader gate |
| `57:01` | `VrUninitialize` | lifecycle | S3; global loader gate |
| `57:02` | `VrGetNamedPipeInfo` | named-pipe | S3; global loader gate |
| `57:03` | `VrGetNamedPipeHandleState` | named-pipe | S3; global loader gate |
| `57:04` | `VrSetNamedPipeHandleState` | named-pipe | S3; global loader gate |
| `57:05` | `VrPeekNamedPipe` | named-pipe | S3; global loader gate |
| `57:06` | `VrTransactNamedPipe` | named-pipe | S3; global loader gate |
| `57:07` | `VrCallNamedPipe` | named-pipe | S3; global loader gate |
| `57:08` | `VrWaitNamedPipe` | named-pipe | S3; global loader gate |
| `57:09` | `VrDeleteMailslot` | mailslot | S4; global loader gate |
| `57:0A` | `VrGetMailslotInfo` | mailslot | S4; global loader gate |
| `57:0B` | `VrMakeMailslot` | mailslot | S4; global loader gate |
| `57:0C` | `VrPeekMailslot` | mailslot | S4; global loader gate |
| `57:0D` | `VrReadMailslot` | mailslot | S4; global loader gate |
| `57:0E` | `VrWriteMailslot` | mailslot | S4; global loader gate |
| `57:0F` | `VrTerminateDosProcess` | lifecycle | S4; global loader gate |
| `57:10` | `VrNetTransactApi` | NetAPI | S5; global loader gate |
| `57:11` | `VrNetRemoteApi` | NetAPI | S5; global loader gate |
| `57:12` | `VrNetNullTransactApi` | NetAPI | S5; global loader gate |
| `57:13` | `VrNetServerEnum` | NetAPI | S5; global loader gate |
| `57:14` | `VrNetUseAdd` | NetAPI | S5; global loader gate |
| `57:15` | `VrNetUseDel` | NetAPI | S5; global loader gate |
| `57:16` | `VrNetUseEnum` | NetAPI | S5; global loader gate |
| `57:17` | `VrNetUseGetInfo` | NetAPI | S5; global loader gate |
| `57:18` | `VrNetWkstaGetInfo` | NetAPI | S5; global loader gate |
| `57:19` | `VrNetWkstaSetInfo` | NetAPI | S5; global loader gate |
| `57:1A` | `VrNetMessageBufferSend` | NetAPI | S5; global loader gate |
| `57:1B` | `VrGetCDNames` | NetAPI | S5; global loader gate |
| `57:1C` | `VrGetComputerName` | NetAPI | S5; global loader gate |
| `57:1D` | `VrGetUserName` | NetAPI | S5; global loader gate |
| `57:1E` | `VrGetDomainName` | NetAPI | S5; global loader gate |
| `57:1F` | `VrGetLogonServer` | NetAPI | S5; global loader gate |
| `57:20` | `VrNetHandleGetInfo` | NetAPI | S5; global loader gate |
| `57:21` | `VrNetHandleSetInfo` | NetAPI | S5; global loader gate |
| `57:22` | `VrNetGetDCName` | NetAPI | S5; global loader gate |
| `57:23` | `VrReadWriteAsyncNmPipe` | async-pipe | S4; global loader gate |
| `57:24` | `VrReadWriteAsyncNmPipe` | async-pipe | S4; global loader gate |
| `57:25` | `VrNetbios5c` | NetBIOS | S5; global loader gate |
| `57:26` | `VrHandleAsyncCompletion` | async completion | S4; global loader gate |
| `57:27` | `VrDlc5cHandler` | DLC/LLC | S5; global loader gate |
| `57:28` | `VrVdmWindowInit` | DLC/window | S5; global loader gate |
| `57:29` | `VrReturnAssignMode` | mode | S5; global loader gate |
| `57:2A` | `VrSetAssignMode` | mode | S5; global loader gate |
| `57:2B` | `VrGetAssignListEntry` | mode | S5; global loader gate |
| `57:2C` | `VrDefineMacro` | mode | S5; global loader gate |
| `57:2D` | `VrBreakMacro` | mode | S5; global loader gate |
| `57:2E` | `VrNetServiceControl` | NetAPI | S5; global loader gate |
| `57:2F` | `VrDismissInterrupt` | async completion | S4; global loader gate |
| `57:30` | `VrEoiAndDismissInterrupt` | async completion | S4; global loader gate |
| `57:31` | `VrCheckPmNetbiosAnr` | NetBIOS completion | S5; global loader gate |

Thus all 50 table slots and all ten original exports have one selected-profile
outcome. This statement deliberately distinguishes three levels:

1. **Static source/archive closure:** selected original provider sources
   compile into `original-mvdm-redir.lib`.
2. **Dynamic availability:** no `VDMREDIR.dll` is built or staged for the
   current product, so the original parent takes its documented unavailable
   path.
3. **Guest observation:** none has yet been made by this record. In
   particular, it does not prove any named-pipe, mailslot, network, NetBIOS,
   DLC, COMMAND, DOS-child, or WOW execution.

## Verification

* Re-read the original `vdmredir.def`, `MS_bop_7`, `LoadVdmRedir`, and
  `VrDispatchTable` source.
* Confirmed the formal candidate graph contains `original-mvdm-redir.lib`.
* Confirmed the selected process link omits it and `redirector-bindings.lib`.
* Searched the formal build root and fixed stage for `VDMREDIR.dll`, `.lib`,
  and `.exp`; no candidate exists.
* Confirmed the detailed row source/owner evidence remains in the S1 ABI
  ledger and the S5 unavailable owner-package results remain explicit.

## Follow-up

S6 may now evaluate the identity of the declared fixed container and decide
whether one immutable observation can reach an already-selected `57:xx`
attempt. No observation may be used to select a new provider or to alter this
dynamic disposition.
