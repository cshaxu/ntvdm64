# M0 T372 S5 P1 — Redirector network-owner import disposition

S3's whole-archive link audit left nineteen network-owner imports.  They are
not nineteen independent Redirector services and must not become nineteen
adapter leaf implementations.  The source-first dispositions are:

* **Existing original host source:** `NetpNtStatusToApiStatus` is already the
  retained `opennt-host/netapi/netlib/ntstatus.c` implementation.
* **Public modern system binding:** `SystemFunction006`, `SystemFunction012`
  and `SystemFunction016` are the historical spellings used by `crypt.h` for
  the LM password helpers.  The x86 `Advapi32.dll` still exports all three
  (forwarded to `CRYPTSP`); the final image can satisfy these through the
  normal public Advapi32 import library, with no copied crypto body.
* **Header-carried primitives:** `InsertTailList` and `RemoveHeadList` are
  historical list primitives used by `vrnetb.c`.  They are not network
  providers; the appropriate Win32 list carrier must expose their original
  macro contract so no external function is emitted.
* **Original remote-API client package:** `GetLanmanSessionKey` and
  `RxNetUserPasswordSet` belong to OpenNT
  `ds/netapi/rpcxlate/rxapi/rxuser.c`; `RxpTransactSmb` belongs to the
  cooperating `ds/netapi/rpcxlate/rxcommon` transaction package.  This is one
  non-MVDM package decision, not a set of local definitions.  Its later
  composition must retain the original remote/transaction ordering or report
  the original unavailable direction where public modern transport cannot
  preserve it.
* **Original transaction-server package:** `XsNetMessageBufferSend`,
  `XsNetServerEnum2`, `XsNetServiceControl`, `XsNetUseAdd`, `XsNetUseDel`,
  `XsNetUseEnum`, `XsNetUseGetInfo` and `XsNetWkstaGetInfo` are implemented in
  OpenNT `ds/netapi/xactsrv` (`apimsg.c`, `apisrv.c`, `apisvc.c`, `apiuse.c`
  and `apiwksta.c`).  `xactsrv` is the historical server-side RAP product
  shell, not a directly linkable user-mode VDM client library.  S5 therefore
  treats it as source/contract evidence: a later same-shaped Redirector
  boundary may call public NetAPI only where that preserves the original
  result and conversion contract; otherwise it must return the source-proven
  unavailable result.  It must not embed a private server/RAP broker.

This collapses the S3 residual list into four owner-contract groups.  It also
identifies the only additional OpenNT source package with a plausible direct
runtime role (`rpcxlate`); importing it requires a separate full package
closure, rather than selectively copying `rxuser.c` into Redirector.

This is an import/source disposition only.  It neither activates a `57:xx`
selector nor claims a live remote, NetBIOS or DLC operation.
