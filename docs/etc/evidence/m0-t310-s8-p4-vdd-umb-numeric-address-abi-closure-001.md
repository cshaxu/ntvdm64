# M0 T310 S8 P4 — VDD/UMB numeric-address ABI closure

## Scope

This closure covers the original `nt_msscs.c` UMB and VDD memory-hook call
forms in the active `mvdm-host` tree.  It does not enable an arbitrary VDD,
kernel VDM, or MONITOR page-fault route.

## Original contract

The NT4 source exposes VDD memory APIs with `PVOID` address parameters and
passes `PVOID *` to `ReserveUMB`.  In the selected non-MONITOR SoftPC profile,
those values are Intel-linear/physical numbers for the UMB address range, not
dereferenceable host pointers.  The original x86 product could cast the number
through `DWORD` because both representations were 32 bits.

The reviewed forms are:

- LIM page-frame allocation in `get_lim_page_frames`;
- VDD install/deinstall, commit/decommit, include/exclude UMB operations; and
- the VDD page-fault callback boundary.

## Recovery

`nt_msscs.c` now uses the existing private
`mvdm_umb_address_encode`/`mvdm_umb_address_decode` codec before numeric page
arithmetic and at every original UMB API boundary.  The public original
function names, `PVOID` parameter spellings, UMB owner selection, page
rounding, list ordering, `SetLastError` paths and callback ordering remain.

The original page-fault code subtracted a native `Sim32GetVDMPointer(0,...)`
base supplied by kernel VDM.  That kernel route is outside the selected
user-mode product.  The selected contract therefore accepts `FaultAddr` as the
original Intel linear number directly and emits the original callback's
numeric `PVOID` carrier through the codec.  No host pointer or mapping token
crosses this API.

`MVDM-HOST-DIV-063` registers this necessary mirror divergence.

## Verification

Outside the sandbox, formal MSVC `/MT` Ninja rebuilt
`original-softpc-host-roots.lib` after the change for both architectures:

- `build/M0-T310/S8/p1-machine-source/x64/msscs-umb-x64.log`
- `build/M0-T310/S8/p1-machine-source/x86/msscs-umb-x86.log`

Both builds completed.  Neither log contains `nt_msscs.c` diagnostics in the
width/call-contract set `C4311`, `C4312`, `C4047`, `C4057`, `C4133` or `C4152`.
Unrelated historical warnings remain intentionally visible under P4 policy.

This is x86/x64 source-contract closure for the selected UMB/VDD numeric
address forms, not evidence that a kernel-VDD provider or full VDD package is
runtime complete.
