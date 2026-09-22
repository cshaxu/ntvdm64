# M0 T421 S4 — DLL component-boundary closure

## Audit result

The S1--S3 source/object/DLL ledger was rechecked after the component moves.
Only two directories intentionally remain beneath `ntvdm-exe`:

- `redir/`: the unique worker guest-memory copy provider and asynchronous
  completion provider.  They are required to retain worker address-space and
  lease ownership; the DLL calls them through its bounded import ABI.
- `wow/`: `wow_user_runtime` and `wow_user_session_binding`, the sole worker
  TLS/session binding.  WOW32 imports only the narrow parent functions and
  does not own a second state domain.

Everything else previously under those directories was either moved to the
corresponding late-loaded target in S2/S3 or proven absent from the product.

## Removed and corrected residue

- Removed `New-T291S3WowCallbackTransactionNinja.ps1`: no caller referenced
  it and its only target source no longer exists.
- Repaired two retained redirector fixture generators to compile the moved
  target-side `vdmredir-dll/source/mvdm_redirector_guest_copy.c`; they now add
  the corresponding target include root rather than producing a dead path.
- Moved the WOW32-only `nt_vdd.h` declaration wrapper to
  `wow32-dll/include`.  The byte-exact original carrier stays under
  `opennt-abi/source/public/sdk/inc`; ordinary MVDM users still include it
  directly.

The generic historical CRT compatibility unit remains under `ntvdm-exe/win32`:
it is selected by both the formal worker product and the DLL recipe, carries
no worker state, and is not a WOW32 target binding.  It is therefore not
mislabelled as either target-local implementation.

## Verification

- Both repaired historical redirector generators produced valid Ninja graphs
  in `build/M0-T421/S4/t324-compat-r1` and `t334-compat-r1`; their target
  inventories contain no missing source path.
- Fresh `wow32.dll` x86 link passed in
  `build/M0-T421/S4/wow32-ntvdd-r1` (77 original bodies, 21 target bindings).
- Fresh full x86 formal product in `build/M0-T421/S4/formal-x86-r1` linked
  `run16.exe`, `basesrv.exe`, `dtmgr.exe`, `ntvdm.exe`, and `VDMREDIR.dll`.
  The DLL reports machine `14C`.
- Existing worker boundary fixtures passed:
  `WOW_PAGE_DOMAIN_LIFECYCLE_OK` and
  `WOW_ORIGINAL_USER_CLIENT_LAYOUT_OK wnd=176 desktop=100`.
- The immediately preceding S3 17-route deployed COMMAND/MEM/EDIT regression
  remains applicable: S4 does not alter runtime source or deploy any binary.
  It changes only source ownership, build-path correctness and a WOW32-only
  declaration wrapper; no WOW16 acceptance is claimed.

## Result

T421 is complete.  It establishes `vdmredir-dll` and `wow32-dll` as explicit
target components, while retaining only the demonstrably worker-local state
and guest-memory boundaries in `ntvdm-exe`.  No original mirror body was moved,
and no generic common component was introduced.
