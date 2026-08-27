# M0 T291 S3 — WOW frame/callback foundation closure

## Scope

S3 admitted only the bounded pointer, numeric task/frame and non-fast
`CallBack16` interface foundation. It did not admit a WOW provider body,
selector, Win16 guest load, fast monitor, BaseSrv/CSR recreation or a WOW
broker.

## Closed interfaces

- P1: source-shaped `GETVDMPTR` / `FLUSHVDMPTR` / `FREEVDMPTR` acquire,
  flush, release and stale-lease handling use only the existing session
  `guest_memory` lease.
- P2: the reached `CURRENTPTD()` fields are a thread-bound numeric projection
  with the original first/later callback stack-selection branch.
- P4: the packed 38-byte numeric `CBVDMFRAME` is created and reread through
  bounded guest leases, preserving field order and AX:DX result location.
- P5: the non-fast `SETVDMSTACK` / `getIP` / `host_simulate` / `setIP` /
  `VDMSTACK` interval uses the same-shaped SoftPC facade and typed Bochs
  mechanics. It is no longer routed through session command/control dispatch.

## Verification

Formal MSVC `/MT` Ninja fixtures passed on x86 and x64 for all four layers:

1. WOW pointer scope;
2. WOW task/frame projection;
3. callback-frame construction and synchronous result reread; and
4. numeric VDM stack plus `host_simulate` call order.

The current source tree contains no `MVDM_WOW_CALLBACK_CONTROL_OPERATION` or
WOW callback registration in the session control dispatcher. The retained P3
record is explicitly superseded for callback execution, while its neutral
multi-operation facility remains the carrier for `GetNextVDMCommand`-style
control requests.

## Transfer

S4 owns the distinct command-broker, monitor-control and owner-specific
postcondition contracts identified after this foundation. It must not claim a
complete BaseSrv/CSR, monitor TEB or WOW runtime merely because S3's bounded
mechanics close.
