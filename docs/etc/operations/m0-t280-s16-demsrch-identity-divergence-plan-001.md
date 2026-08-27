# M0 T280 S16 — `demsrch.c` host-identity divergence plan

Apply the smallest source-local divergence to original `dos/dem/demsrch.c`:
replace only persistence and validation of `PFFINDLIST` in the 32-bit DTA/FCB
fields with `mvdm_host_identity_*` calls. `FFindId` remains untouched numeric
data. Local native `PFFINDLIST` variables and the original search/control flow
remain unchanged.

The original source proves that an exhausted find-list allocation follows the
existing `STATUS_NO_MEMORY` / `RtlNtStatusToDosError` resource-failure path
inside `SearchFile`. A facade publication failure must therefore release the
new native entry before exposing it, clear the persisted pair, set the same
source-shaped out-of-resource result and take the existing caller failure
branch. It must not report a successful first result without a valid next
search identity.

The exact patch points are original stores at lines 316, 354 and 557;
validations at 436--475 and 601--650; and `FreeFFindEntry` at 2384--2390.
The final point reverse-looks-up and releases the surrogate before freeing the
native object. Every modified point receives a local `DIVERGENCE` comment and
an `opennt-mvdm-host` README register row. No unrelated pointer arithmetic or
provider route may change.

Verification requires x86/x64 syntax composition of original `demsrch.c`
with the facade, a focused identity lifecycle fixture covering path and FCB
stores/validation/cleanup, and a diff review proving no raw `PFFINDLIST` is
persisted through `STOREDWORD`.
