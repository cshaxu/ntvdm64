# Complete WOW page-domain ownership review

## S14 P51

Baseline d676f1150. Question: can the remaining F page-domain provider move to
B without machine callbacks or a duplicate session? Read all 2673 lines of
src/ntvdm-exe/softpc/mvdm_softpc_wow_page_domain.c in contiguous chunks; inspect
mvdm_dpmi_binding.c, compat/dpmi_services.c, original modesw.c/dpmi386.c call
sites and nt_bop.c MS_bop_1. Crosswalk pins the exact provider hash. This record
completes body coverage, not an implementation migration or runtime proof.

## Findings and disposition

| Source unit | Actual behavior and destination constraint |
| --- | --- |
| domain creation/enter/leave/reenter (approximately 310-700) | Constructs identity page tables, read-only shared clock mapping, guest TEB and separate WOW GDT; switches CR3/CR0/GDT. Requires A20 unwrapped and protected mode. These are C machine operations, not generic B allocation. Original nt_bop MS_bop_1 calls enter before WOW initialization; modesw/dpmi386 call leave/reenter. F's DPMI installer currently forwards those transitions through B, so moving only the header is not closure. |
| domain owner/admission | session_thread_current, session_valid and teardown registration connect the singleton domain to product admission. Separate F admission and disposal timing from C-owned machine state. Reuse B host_context binding only for owner/resource identity; do not replace ACTIVE admission with an unproved validity check. No additional TLS or whole session relocation. |
| shared clock | Allocation, native updater and stop are already B; guest mapping and page-table publication use C. Stop the updater before unmapping and disposal. Keep this split rather than reintroduce a machine callback table in B. |
| client layout/publication | Fixed USER16 TEB/SHAREDINFO/SERVERINFO/WND/CLS/MENU offsets and server/client delta are guest/original USER contracts. They are not modern HWND pointers or generic serialization. Keep original layout interpretation with C; sample native metrics/colors/window state through independent host mechanisms. Do not move this entire projection to B. |
| cached callback context | Save/restore the exact HWND/backing pair. A retired pinned generation remains a valid callback reader. Exchange snapshots aliased arrays and validates before its no-callout commit. Re-resolving the current HWND would select the wrong generation. |
| WND preparation/adoption | Allocate and pin unpublished backing first, then commit slot identity. Managed adoption of an observed identical window preserves backing; replacement of a dead observed generation retires the old backing without overwriting readers. Observation does not own a native window. |
| menu transaction | Validate all rows/submenu identities, allocate replacement storage and retirement records before publication. Commit contains no allocation/native call/guest callback. Physical-free failure is retained after logical commit and cannot turn completed native success into failure. Native/local menu ownership remains distinct. |
| borrow/reclamation | Exact backing indexes are non-owning accelerators; existing lists own pins, retained terminal owners and failed frees. Release consumes a reference once, but physical reclaim waits for pins and copied incoming links. This code reads guest graph state and calls VdmFreeVirtualMemory: it is not presently a B-independent host record manager. Avoid a second registry or 24 callbacks to disguise those dependencies. |
| classes | CLS storage remains while prepared, published or retired pinned WNDs reference it. Native unregister success marks released; failed physical free retains the existing record. Class-name bytes live with CLS. The module field is deliberately zero because this implementation has no proven 16-bit module conversion; relocation must not claim to fix that boundary. |
| memory fast paths | Owned same-page RAM copies use CCPU translation; stores preserve CPL/VM access flags through xtrn2phy, with SAS fallback. No alias escapes or survives guest/native callout. These access semantics belong C. Do not substitute raw B memcpy on arbitrary guest/device ranges. |
| graph publication | P26's copied graph validation is already B. Backing allocation, full-generation resolution and original-field commit remain C-coupled. Prepare every pin/allocation before replacing edges and withdrawing old identities; failure retires only fresh preparations and keeps existing publication. |
| cleanup | Restore DOSX context before releasing mappings; stop clock before removing it; retire guest resources before allocator-domain teardown. Host-storage disposal is constrained to deallocation after generation unlink, not arbitrary USER/guest reentry. F chooses when execution is quiescent; it does not need to own the algorithms. |

## Concrete design consequence

Do not implement the earlier whole-file B migration with 24 C callbacks.
The machine context and guest-layout projection need a C implementation owner;
B keeps independent clock, graph validation and genuinely host-only resources.
F retains initialization admission and final quiescence/disposal timing. The
current four-operation DPMI service table must be removed or reduced only as
its actual CPU/debug/mode providers acquire inward ownership; a new function
pointer facade is not a solution.

Before editing, identify original C carriers for machine context and USER
projection, measure resulting mirror points/lines and request approval for
any increase. Reuse actual original declarations/layouts when possible;
do not transplant all 2673 lines into a random mirror file. Review callers
of projection and host-storage disposal together: the 3 direct ledger rows
for this file are not its complete API surface. The evidence establishes
ownership constraints, but exact C insertion and ABI patch are not yet ready.

## Coverage, limits and required verification

This body adds one provider/3 symbol records: all 79 direct-symbol provider
files and 345 symbol records now have complete-body review references. The
418 open dependency records remain open, including 60 headers, 12 roots and
one image import. Four-component inventory contains 1494 tracked files;
79-provider coverage is not a claim that all 1494 have a completed migration
ledger. No production change, mirror delta, guest edit or runtime test here.

Later verification must cover initial allocation failure, A20 rejection,
protected/real mode round trips, GDT restoration, same-HWND replacement during
pinned callbacks, failed frees after committed menu/window retirement, aliased
callback-context arrays, class lifetime, observation versus managed ownership,
and teardown with updater/borrowers quiescent. Existing tests are inputs for
that implementation batch, not a new passing run attributed to this review.
