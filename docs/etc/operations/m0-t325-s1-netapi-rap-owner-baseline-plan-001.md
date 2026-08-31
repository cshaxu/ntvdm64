# M0 T325 S1 — NetAPI/RAP Redirector Original-Owner Baseline Plan

## Objective

Turn the complete original network-management cohort reached from VDMREDIR
into a finite source/ABI/lifetime/failure ledger. This is an owner-package
baseline, not a trace-selected implementation of a `57:xx` leaf.

## Selected Source Cohort

- `mvdm-host/vdmredir/vrnetapi.c` — original workstation, user, computer and
  downlevel dispatch/control flow.
- `mvdm-host/vdmredir/vrremote.c` — original remote/RAP request flow.
- `mvdm-host/dos/command/cmdredir.c` and `mvdm-host/inc/vdmredir.h` — original
  callers, declarations and request layouts.
- `opennt-host/netapi/netlib/ntstatus.c` — byte-identical original status-map
  algorithms; it remains the selected status owner.

## Required Ledger Fields

For each physical original call site, record the caller/function and source
line; declaration and requested semantics; every guest, session, NetAPI or
host identity/buffer and its acquire/release duration; original result/status
mapping; and exactly one disposition:

1. direct public modern Win32/NetAPI binding through the existing
   source-shaped facade;
2. original source plus a bounded same-shaped binding;
3. original source-shaped unavailable/failure result with a named cause; or
4. an explicit transfer to NetBIOS, DLC, VDD, broker, WOW or another named
   later owner package.

The ledger must distinguish an API whose name is public from an API whose
input layout, allocation, status map or lifetime is actually equivalent.

## Exclusions

No project SMB/RAP/RPC implementation; no private service, CSRSS, kernel or
VDD import; no native pointer/HANDLE in MVDM/guest state; no new mapping
manager; no remote guest success claim; and no change to the completed local
Redirector cohort.

## Exit

S1 closes only after the selected cohort has one source-first disposition for
every reached edge, with the direct/same-shaped/unavailable/transfer reasoning
captured. Its evidence then names the smallest possible next binding recovery
without expanding the product perimeter.
