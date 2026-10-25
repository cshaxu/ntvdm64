# M0 T454 S1 — WOW32 resource owner matrix

## Conclusion and scope

S1 closes the static W05 admission boundary only. It identifies the selected
original resource call chain, its state owners and finite S2/S3 work; it does
not claim that resource loading has been exercised in the current product.
No product source, guest media, build artifact or release asset changed.

The two pinned original editions agree on the selected core: OpenNT
`5c5b979ec08c17d3ca2eb70e8aad62d26515d01c` and OpenNT-4.5
`98bd914d250dd72a85e351da1ec3aa88433aac35` have byte-identical
`wow32/wres32.c`, `wow32/wres16.c` and `wow16/kernel31/wow16cal.asm` content
(aside from recorded line endings). Their relevant SHA-256 values are,
respectively, `1217c5fa87b808329826d4ef2e785a5eb574de9cb1e9691de99680844f5bd13a`,
`24324d4b95b7c5ac102e9751872af1db779efc0cff223b32aa066034cb874ae9`, and
`441fce7b100a5e4e1caac173f190a02ce33cee08340ea047037b0fad3a681147c`.

## Fixed call and ownership boundary

| Operation | Exact original route | Authority and representation | Normal/failure/retirement rule | S2/S3 disposition |
| --- | --- | --- | --- | --- |
| Find | USER `W32FindResource` → `FindResource16` → `RET_FINDRESOURCE` → `WOW16FindResource` → guest KERNEL `FINDRESOURCE` | Guest KERNEL owns name/type lookup and NE resource identity. `FindResource16` owns temporary guest copies for string name/type and creates one host `RES` record only after a nonzero guest resource-info result. Numeric IDs remain numeric. | Temporary copies release after callback; a successful find inserts a fresh bridge record, not a deduplicated global identity. Guest-module route intentionally ignores `wLang`; native route alone uses `FindResourceEx`. | S2 must test numeric and string name/type, missing resource, original type/name ordering and duplicate find records. |
| Load | `W32LoadResource` → `LoadResource16` → `RET_LOADRESOURCE` → `WOW16LoadResource` → guest `LOADRESOURCE` | Guest KERNEL owns NE load/reload and `rn_usage`; `RES.hresdata16` is the bridge's returned guest data identity. | Nonzero guest data writes `hresdata16`. A failed guest load leaves the existing `RES` record for later original cleanup; it is not an excuse to invent eager PE-style release. | S2 must prove success and partial/missing-load retention; S3 must prove its precise later retirement. |
| Lock | `W32LockResource` → `LockResource16` → `RET_LOCKRESOURCE` → `WOW16LockResource` → guest `LOCKRESOURCE` plus `GLOBALSIZE` | Guest far pointer and guest allocation size are separate 16-bit callback outputs. `RES` owns either a host conversion buffer (MENU/DIALOG) or a scoped guest-memory alias marked `RES_ALIASPTR`; W03 remains the allocation/alias provider. | Lock first enters guest. MENU/DIALOG use a converted host buffer; ordinary types use a guest alias; ACCELERATOR is explicitly a different owner and returns NULL here. A successful lock with host allocation failure has no local balancing unlock in the original body. | S2 must preserve the fixed-width far-pointer/size channel, alias/copy distinction and lock failure. It must not put a native pointer in Guest ABI or implement accelerators as a generic resource fall-through. |
| Unlock | `W32UnlockResource` → `UnlockResource16` → `RET_UNLOCKRESOURCE` → `WOW16GlobalUnlock` → guest `GLOBALUNLOCK` | Guest heap owns lock count. `RES` owns only its host conversion buffer or scoped alias flag. | Guest unlock happens before non-alias host-buffer release; then `pbResData` and `RES_ALIASPTR` clear. The callback maps guest zero/nonzero into a native BOOL, so raw forwarding is wrong. | S2 must test alias versus conversion copy and exact callback-first ordering; repeated locking is a stated failure/coverage obligation, not assumed safe. |
| Free (host-to-guest) | `W32FreeResource` → `FreeResource16` → `RET_FREERESOURCE` → `WOW16FreeResource` → guest `FREERESOURCE`, then `FreeRes16` | Guest KERNEL owns NE count/discard/reload semantics. `RES` owns bridge-record removal. | `WOW16FreeResource` maps guest zero to native TRUE and nonzero to FALSE. `FreeResource16` removes its `RES` record regardless of that result; `FreeRes16` unlinks before optional unlock and record free. | S3 must prove both direction-specific result normalization and bridge unlink/conditional-unlock order. A modern `FreeResource` result is not a substitute. |
| Free (guest-to-host) | guest `IFreeResource` selects `WK32FreeResource` for the GA_WOWHANDLE partition; `WK32FreeResource` evaluates `HCURSOR32` before native `FreeResource` | Guest `IFreeResource` owns resource-table accounting. Cursor/icon aliases have their own provider; native FreeResource is compatibility-only. | The low-bit mask is a partition, not a generic handle-validity flag. Existing cursor/icon alias lookup can be observable before native FreeResource. | S3 must retain this separate route and test existing alias/null/native/ordinary guest partitions. No global cursor/icon destruction substitution. |
| Task retirement | `W32FreeTask` → `DestroyRes16(ptd->htask16)` | `RES.hmod16` preserves the raw producer key. Task, instance and module identities are separate original values. | `DestroyRes16` matches its supplied value exactly; inspected module retirement has no direct `DestroyRes16` call. This is not proof of universal module/DLL cleanup. | S3 must preserve raw-key equality and separately record any created-task/resolved-module metadata. It may not silently normalize keys or bulk-retire on module unload. |

## Current source and consumer census

The selected original hosts are present in the product at `src/mvdm/wow32/`:
`wres32.c` is the USER-facing split dispatcher, `wres16.c` owns `presFirst` and
the six callback operations, `wres16.h` defines the bridge record, `wcall16.c`
owns `RealLockResource16`, and `wkman.c` supplies the current task-retirement
call. `src/mvdm/wow16/kernel31/wow16cal.asm` is the selected guest callback
shim. No replacement resource manager was found.

The complete direct named `FindResource16` producer census in this source
slice is two paths: `W32FindResource` and `wcommdlg.c::GetTemplate16(FALSE)`.
The latter has six named template consumers (ChooseColor, ChooseFont, print,
print setup, file name and Find/Replace). Its Find/Replace instance retains the
record across API return until destruction; the other five normally unlock then
free. `UseHandle(TRUE)` is a different conversion-buffer path and does not make
a `RES` record. MENU/DIALOG conversion and complete common-dialog behavior are
later consuming scopes; this T owns only the foundation they require.

## Proven limits and bounded work

The original body visibly lacks a local repeated-lock guard and does not
balance a guest lock if MENU/DIALOG host-buffer allocation fails. These are
source facts, not proof of a product-visible bug. S2 must first exercise the
selected reachable cases through the actual provider. If a host correction is
proved, it must follow source policy: retain the original owner, use the
smallest source-shaped change, register one divergence, and prove normal,
negative and retirement paths. If no matching approved correction exists for an
inherited original-host defect, retain it as a bounded limitation rather than
inventing a replacement.

Shared-module retirement, callback reentry after a `RES` pointer is passed,
full resource-handler (`rt_proc`) lifetime and cursor/icon materialization are
not silently absorbed. Their finite prerequisite is: an internal record may
store raw producer key, resolved module, creating task and its own identity as
separate fields; the raw key remains authoritative for original exact-match
cleanup. Any new cross-callback carrier requires a specific later admission;
S2 does not introduce one.

## Verification and handoff

S1 performed source/caller/ABI review against the selected current tree and
read-only audit. `tools/governance/Verify-DocumentationGovernance.ps1` ran as
owned background PID 2460 on 2026-10-24 and passed. `git diff --check` was
clean at review time.

S2 is now bounded to actual find/load/lock/unlock behavior and W03 reuse; S3
owns free/rollback/exact-key retirement. Required later evidence is real
guest/provider execution plus bounded fixtures for valid/missing numeric and
string lookups, partial load, alias/copy locking, conversion failure, repeated
load/release and task/module-associated retirement. A compile, symbol match or
fixture-only result cannot close W05.
