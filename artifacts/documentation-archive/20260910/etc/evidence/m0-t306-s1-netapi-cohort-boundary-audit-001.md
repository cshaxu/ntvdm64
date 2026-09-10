# M0 T306 S1 P4 — NetAPI cohort boundary audit

## Question

For the sixteen signature-proven NetAPI calls from `mvdm-host/vdmredir`, which
original non-MVDM code is worth preserving as a small mirror slice, which
calls stop at current public Windows APIs, and which branches still require a
separate Redirector protocol decision?

## Inputs

- The sixteen physical `netapi-package-candidate` rows in
  [the signature-seed package ledger](../operations/m0-t306-s1-signature-seed-package-ledger.tsv).
- Original definitions:
  `ds/netapi/netlib/ntstatus.c:57`,
  `ds/netapi/svcdlls/wkssvc/client/wksstub.c:79,295`, and
  `ds/netapi/rpcxlate/rxapi/rxuser.c:1180`.
- The selected `mvdm-host/vdmredir/vrremote.c` and `vrnetapi.c` callers.
- Current Windows SDK `lmwksta.h`, which still declares
  `NetWkstaGetInfo` and `NetWkstaUserGetInfo` with their public API contracts.
- The retained Redirector source disposition matrix.

## Procedure

Kept all physical call counts and source identities. Read each selected
definition through the first source boundary: public API, small closed
original algorithm, or private RPC/RAP/SMB transport. Inspected the current
SDK declarations only to establish that the reached workstation calls are
publicly supported contracts; no behavior was enabled and no network operation
was performed.

## Observations

- Eleven calls reach `NetpNtStatusToApiStatus`. Its original body is a finite
  status switch followed by `RtlNtStatusToDosError`; it holds useful original
  MVDM-compatible error semantics and has no network-session/product-shell
  dependency. The exact `ntstatus.c` slice is therefore a candidate for a
  small `opennt-host` mirror, with its status primitive supplied by the
  existing Win32/NT declaration boundary.
- One `NetWkstaGetInfo` and three `NetWkstaUserGetInfo` calls have same-named
  current public Windows API contracts. Their NT4 `wksstub.c` bodies instead
  start `Netr*` RPC and then try a downlevel `Rx*` protocol. Importing that
  file would enlarge the runtime toward a Workstation/RPC product shell while
  adding no required MVDM algorithm.
- One `RxNetUserPasswordSet` call reaches a genuine historical downlevel
  account transaction. The body validates legacy lengths, converts/encrypts
  legacy password forms, and invokes `RxRemoteApi`; it cannot be represented
  as a transparent direct link to the workstation calls above.

## Decision

Select only `netlib/ntstatus.c` as a future, exact small OpenNT mirror slice.
The two workstation information forms stop at public Win32 NetAPI with a
named Redirector/Win32 width-and-buffer adapter; native returned buffers must
remain adapter-local and be copied into source-defined bounded MVDM storage.
`RxNetUserPasswordSet` remains a named Redirector protocol hard boundary: its
source stays evidence, and no substitute success result is authorized.

This does not admit source copying or adapter implementation in T306. The
complete per-form result is in the companion
[NetAPI cohort ledger](../operations/m0-t306-s1-netapi-cohort-ledger.tsv).

## Confidence and follow-up

High confidence in the source/path classification and the public workstation
API boundary. Medium confidence in any eventual modern password-change mapping:
it must first demonstrate original failure semantics and required permission/
credential behavior. Continue T306 with the remaining OLE and CSR cohorts;
the later Redirector package consumes the two public workstation bindings and
the retained protocol hard boundary.
