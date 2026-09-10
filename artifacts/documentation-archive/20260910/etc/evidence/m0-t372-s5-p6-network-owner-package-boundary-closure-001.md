# M0 T372 S5 P6 — Redirector network owner-package boundary closure

## Question

Can the twelve remaining physical imports of the original `vdmredir` archive
be closed by importing their complete OpenNT owner packages, or do their
original contracts stop at a modern public-API / private-Lanman boundary?

This is a source and formal-link disposition.  It does not activate a
`57:xx` service, claim a usable remote network, or substitute a new
Redirector provider.

## Fixed input and residual surface

P5 rebuilt the original `vdmredir` archive with the selected original NetLib
conversion and allocation bodies.  The post-P5 parent/DLL whole-archive audit
is `build/M0-T371/S2/formal-x86/redir-parent-import-full-bindings-audit.log`.
It has twelve physical import records, comprising eleven unique names:

* `RxpTransactSmb` is referenced by both `vrremote.c:VrTransaction` and
  `vrnetapi.c`; that is one original transaction owner, not two adapters.
* `RxNetUserPasswordSet` and `GetLanmanSessionKey` are reached by the
  `vrnetapi.c` remote/password paths.
* `XsNetMessageBufferSend`, `XsNetServerEnum2`, `XsNetServiceControl`,
  `XsNetUseAdd`, `XsNetUseDel`, `XsNetUseEnum`, `XsNetUseGetInfo`, and
  `XsNetWkstaGetInfo` are the original local NetAPI/RAP-conversion calls.

No NetBIOS or DLC import remains in this post-P5 residual.  Their existing
S4 same-shaped completion/machine bindings therefore remain their owner
disposition; this P does not claim that their guest protocols have executed.

## Original package evidence

### RpcXlate: one downlevel transaction package, not three helpers

The dual-provenance OpenNT and OpenNT-4.5 copies of
`ds/netapi/rpcxlate/rxcommon/sources` name the twelve-source static
`RxCommon` library.  `Transact.c` defines `RxpTransactSmb`; it constructs an
`LMR_TRANSACTION` for `\\server\\IPC$` and calls
`NetpRdrFsControlTree(..., FSCTL_LMR_TRANSACT, ...)`.  The function is thus
not a public `NetUse*` wrapper: its observable contract is a synchronous
Lanman redirector FSCTL/RAP transaction, including IPC-tree selection,
transaction buffer layout, timeout and returned RAP payload.

Likewise the dual-provenance `ds/netapi/rpcxlate/rxapi/sources` names the
fifty-source static `RxApi` library.  `rxuser.c` defines both
`RxNetUserPasswordSet` and `GetLanmanSessionKey`.  The latter opens the
current process token, obtains its logon LUID, and calls
`NetpRdrFsControlTree(..., FSCTL_LMR_GET_CONNECTION_INFO, ...)` to retrieve
the server's Lanman session key.  The former performs the original
downlevel password/session-key sequence before its remote RAP call.

The package manifests themselves name `netlib.lib` and `NetRap.lib`; the
reached bodies additionally require the private Lanman redirector FSCTL
contract.  A public modern NetAPI call can neither transmit this arbitrary
RAP transaction nor retrieve the original Lanman session key.  Replacing
these calls with a locally invented SMB/RAP client, or claiming success from
an unrelated public API, would change the original failure and transport
contract.  That is prohibited by the source policy.

**Disposition:** retain the original `vrremote.c` and `vrnetapi.c` bodies,
their original error branches, and the `rpcxlate` paths as source evidence.
They are source-proven unavailable in the current non-invasive profile.  A
future Remote/RAP package may admit the complete selected `RxCommon` and
`RxApi` source sets only after it supplies a bounded, source-shaped transport
with the exact FSCTL/RAP behavior; that work is already described by
`docs/etc/operations/proposal-netapi-rap-redirector-network-boundary-recovery-001.md`.
This S5 packet neither imports a selective `rxuser.c` fragment nor creates a
private transport adapter.

### XactSrv: original RAP conversion owner, not a leaf NetAPI adapter

The dual-provenance `ds/netapi/xactsrv/sources` declares the 23-source
`XACTSRV` dynamic library.  Its `xactsrv.def` exports every unresolved
`XsNet*` name, proving that the eight imports belong to one original package:

* `apimsg.c` calls `NetMessageBufferSend` after its original message buffer
  conversion;
* `apiuse.c` calls public `NetUseAdd`, `NetUseDel`, `NetUseEnum` and
  `NetUseGetInfo`, but surrounds each call with original RAP descriptor and
  `XsFillEnumBuffer` / `RapConvertSingleEntry` processing;
* `apiwksta.c` calls public `NetWkstaGetInfo` while producing the original
  16-bit return layout;
* `apisrv.c` calls public `NetServerEnum` for one path and private
  `I_BrowserServerEnumForXactsrv` for its transport-aware path;
* `apisvc.c` uses historical `NetServiceControl`, original service-name
  translation, and RAP conversion.

Thus the presence of a public `NetUse*`, `NetWkstaGetInfo`, or
`NetServerEnum` call does not make the corresponding `XsNet*` function a
safe one-function adapter: the original RAP input/output descriptor,
buffer-size, error and conversion behavior belongs to the package.  The
full XACTSRV source manifest also depends on `netlib`, `netrap`, `rxapi`,
`rxcommon`, `browser`, `samlib`, `winspool`, and the historical service
surface.  In particular its Browser call is private and its service/message
branches do not have an equivalent public modern contract.

**Disposition:** do not selectively copy the five reached files or write
eight `XsNet*` shims.  The current profile keeps the original caller bodies
compiled for source/link evidence but leaves the complete XACTSRV group
source-proven unavailable.  The same named NetAPI/RAP boundary proposal owns
the required full-package audit; it may select a finite original XACTSRV
slice only if every retained RAP conversion and outgoing public/private
boundary is proven.  `NetUse*` is a candidate public capability within that
later package, not authorization to bypass the original conversion owner.

## S5 closure effect

All S5 public-network residuals now have one non-overlapping conclusion:

1. NetLib status/alloc/copy helpers are linked as original source (P3/P5).
2. LM crypto import spellings use public Advapi exports (P4).
3. NetBIOS/DLC completion imports are owned by the existing S4 machine and
   session binding surface; their protocols await S6 observation.
4. RpcXlate/RAP and XACTSRV are complete original owner packages with a
   recorded hard boundary, not missing leaf functions.  They retain their
   original unavailable direction until a separately admitted full package
   recovery proves otherwise.

This satisfies S5's source-first disposition requirement without silently
turning an original DLL link failure into fabricated remote-network success.
S6 alone may reconcile the whole original `VDMREDIR.dll` image and record
whether the selected runtime profile loads it or preserves the original
failed-load path.  No claim about actual `57:xx`, NetBIOS, DLC, SMB/RAP,
COMMAND, DOS-child or WOW execution follows from this closure.

## Verification

* Read both original `RxCommon` / `RxApi` manifests and the reached
  `Transact.c` / `rxuser.c` functions.  The reached function bodies are
  byte-identical between the two external source trees; the corresponding
  build manifests are treated as independent corroborating package evidence
  because their historical build-control text is not byte-identical.
* Read the original 23-source `XACTSRV` manifest, its export file, and the
  five reached owner bodies named above.
* Re-read the post-P5 whole-archive audit.  It lists precisely the owner
  imports enumerated in this record and no unresolved NetLib copy/alloc/status
  helper.
* Regenerate the formal CPU40/x86 graph and run:

  ```text
  build\M0-T371\S2\formal-x86\run-ninja-parallel.cmd \
    original-softpc-candidate
  ```

  The command completed all 458 formal graph actions successfully.  A
  subsequent `ninja -t query original-softpc-candidate` named both
  `original-mvdm-redir.lib` and the selected original
  `original-opennt-netlib.lib` / `original-opennt-netapi-api.lib` inputs;
  `ninja -n original-softpc-candidate` reported `no work to do`.
