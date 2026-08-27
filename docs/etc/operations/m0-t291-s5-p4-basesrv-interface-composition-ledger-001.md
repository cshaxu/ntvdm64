# M0 T291 S5 P4 — BaseSrv interface-composition ledger

## Scope

This ledger covers the selected `opennt-host/base/win32` VDM package imported
by P3. It is source-first: the original `srvvdm.c` and `vdm.c` bodies remain
authoritative. No adapter body, server replacement, selector route or build
target is enabled by this ledger.

## Interface groups

1. **CSR client capture and dispatch** — `CsrAllocateCaptureBuffer`,
   `CsrAllocateMessagePointer`, `CsrClientCallServer`, and
   `CsrFreeCaptureBuffer` are called by the original `GetNextVDMCommand` and
   Base client operations. Modern Windows has no composable public CSR client
   API. Owner: `adapter-opennt-host`. Required result: a same-shaped bounded
   message/capture transaction preserving `VDMINFO` capacity, retry, wait and
   failure order; no raw pointer crosses into `broker` or guest state.
2. **CSR server request and process context** — `PCSR_API_MSG`,
   `PCSR_REPLY_STATUS`, `CsrLockProcessByClientId`, `CsrUnlockProcess`,
   `CSR_SERVER_QUERYCLIENTTHREAD`, process sequence and console context are
   used by the original server record lifecycle. They are NT4 product-private.
   Owner: `adapter-opennt-host`. Required result: source-shaped cooperative
   registration and bounded current-caller context; no CSRSS clone or process
   enumeration.
3. **Event-pair and duplicated-resource lifecycle** — `NtCreateEvent`,
   `NtSetEvent`, `NtResetEvent`, `NtWaitForSingleObject`,
   `NtDuplicateObject` and `NtClose` implement the original server/client
   wait/wake and standard-handle cleanup order. Owner: `adapter-opennt-host`.
   Public Win32 event/duplicate operations may implement the private adapter
   body, but only after a fixed source-order/error ledger for each reached
   operation.
4. **NT RTL allocation, copying, critical sections and strings** — reached
   `Rtl*` primitives carry the original record/list algorithm. Owner:
   `adapter-mvdm-host-out/win32` only when its existing façade already has the
   exact operation; otherwise `adapter-opennt-host` owns the BaseSrv-private
   binding. No source body is to be rewritten for convenience.
5. **WOWEXEC notification and security/PIF branches** — dynamic USER export
   lookup, `PostMessageA`, process-token checks, PIF information and shared
   WOW registration are original source branches. They retain original source
   identity but are profile-disabled until the WOW/PIF owner packages are
   separately admitted. They may not be treated as a reason to enable a
   partial BaseSrv command path.
6. **`srvinit.c` full Base-server dispatch table** — it records where the VDM
   handlers belonged in the original Base server but also references unrelated
   Base API server routines. It remains byte-exact mirror-only until a
   complete selected Base server composition is audited; it is not a hidden
   command dispatcher input.

## Required P4 output

For each reached operation, create a child row in the shared interface tracker
with original declaration, caller, source ordering, owner, modern binding or
source failure, x86/x64 ABI treatment and focused verification. Only after the
client capture group and the one-session server event group have all such rows
may a formal source-shaped `GetNextVDMCommand` composition fixture be admitted.
