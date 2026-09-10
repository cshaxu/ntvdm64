# M0 T374 S2 — Original WOW32 initialization boundary decision

## Question

Can the original `W32Init`/`W32Dispatch` product boundary be directly
composed from the current source tree, and what is the smallest next
source-shaped cohort if it cannot?

## Original boundary evidence

`mvdm-host/wow32/wow32.c:460-764` establishes that initialization is ordered,
not optional.  Before it returns success it registers 21 input callbacks with
`UserRegisterWowHandlers`, accepts a full `PFNWOWHANDLERSOUT` table (20
callback outputs plus `dwBldInfo` metadata), registers
the Base global-memory hook, joins shared WOW state, installs `CURRENTPTD`,
configures task synchronization and invokes original clipboard/GDI/hung-app
initializers.  `W32Dispatch` thereafter consumes the generated `aw32WOW`
table against that initialized state.

The original implementations of the directly external edges were traced:

* `RegisterWowBaseHandlers` is an OpenNT Base client function in
  `base/win32/client/gmem.c:27-35`.  Its registration body is small, but its
  stored hook is consumed by original `GlobalFree` at `gmem.c:603-604`.
  A correct recovery must retain that source-defined lifetime rather than
  report registration success and discard the hook.
* `UserRegisterWowHandlers` is an OpenNT NTUSER client function in
  `windows/core/ntuser/client/client.c:2256-2331`.  It stores all incoming
  callbacks and fills all 20 output callback slots with private USER routines
  before returning `&gSharedInfo`.
* `mvdm-host/inc/sharewow.h` already contains the original public-Win32
  mutex/file-mapping implementation.  `wow32.c` defines `SHAREWOW_MAIN`, so
  it owns the selected original implementation; no new shared-state provider
  is needed for its single-session use.
* `CURRENTPTD()` remains the original spelling in `wow32.h`, backed by the
  existing private-TLS compatibility declaration
  `adapter-mvdm-host-out/win32/include/nt.h`.  It preserves the source
  per-thread TD location without exposing a host identity to guest state.

## Current composition review

The current `adapter-mvdm-host-out/wow` family contains only the bounded
`CallBack16` guest-frame lease.  It deliberately owns no task, dispatcher,
USER registration or shared-state behavior.  Current adapters provide no
`UserRegisterWowHandlers` or `RegisterWowBaseHandlers` definition.

Consequently the direct original routes have the following disposition:

* `sharewow.h`: **direct original** under the selected single-session
  profile, subject to its existing public-Win32 resource cleanup semantics.
* `CURRENTPTD`: **existing same-shaped adapter-backed declaration**; no new
  mapping manager or guest pointer is introduced.
* `RegisterWowBaseHandlers`: **original-source subset candidate**.  The
  original registration and the original hook-consumption site must be
  recovered together as one Base-client lifetime slice.
* `UserRegisterWowHandlers`: **overlay-required whole interface**.  Its
  original function must retain its 20-callback plus metadata table contract,
  initialization order and `gSharedInfo` result shape, while each private
  USER output implementation receives a public/same-shaped or explicit
  unavailable disposition.

## Decision

No direct W32Init/W32Dispatch composition is currently possible, but the
first non-composable edge is narrower and more precise than “WOW32 is
unavailable”: it is the complete original `UserRegisterWowHandlers`
registration contract and its 20 private USER outputs.  Returning a fake
`gSharedInfo`, a partial output table or static dispatch success would violate
the original subsequent call contract.

S3 therefore owns one whole **USER registration contract cohort**: import the
original Base hook lifetime slice, preserve the original registration table
shape, classify all 20 outputs plus metadata as public binding, retained private boundary or
source-shaped unavailable, and select only a complete initialization form.
It must not enable a leaf WOW service.

## Mapping and limits

All WOW task/guest frame fields stay behind the existing session mapping
manager and bounded callback lease.  The USER output table contains host-side
callbacks and resource handles; it cannot use guest-address mappings as a
substitute for host object identity.  This record does not implement any
provider, modify mirror code, select media or run the product.
