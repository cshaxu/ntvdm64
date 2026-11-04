# T475 S3 — Worker Base and NTVDM dependency removal design

## Scope and source baseline

The owner admits one S for the illegal Worker Base and NTVDM dependencies in
the [S2 inventory](../evidence/m0-t475-s2-component-dependency-inventory.md).
The two workers' dependency on worker-base remains the owner's explicit
exception. NTHook dependencies are outside this S. Source baseline `5bc6eb897`
and local package `2b1325bd-17fb-4435-b2be-55995de325f6` retain the current
ABI, guest behavior and ten-image release. No guest media or mirrors are to be
rewritten. Common code is project-specific shared substrate; lib-win32 contains
mechanisms with no common or executable-private dependency.

WOW32's and VDMREDIR's PE images currently import the NTVDM executable ABI.
Whether this S also removes that runtime import is a pending scope decision;
the design and P sequence below first address the independently proved source,
header and static-link violations. No implementation should treat an unresolved
runtime-scope decision as permission to weaken a requested no-dependency goal.

## Target ownership and invariants

| Mechanism | Proposed owner | Boundary and retained authority |
| --- | --- | --- |
| VDMREDIR's unused `worker-base.lib` input | No new owner | Delete only after direct negative-link and runtime checks. NTVDM and NTVWM retain their allowed Worker Base selection. |
| Historical CRT spelling/UCRT adapter currently in NTVDM | `lib-win32/crt-compat` | Shared source owns the ABI adapter; original `sprintf`, `sscanf`, `_swprintf`, calling convention, link selection and legacy unbounded semantics are unchanged. Avoid duplicate symbols/CRT instances. Verify actual callers and the x86 WOW graph. |
| Guest-memory lease contract and generic bounded lease mechanic | `common/guest-memory` for the product contract; assess product-independent allocation/lease mechanics for lib only if genuinely separable | The shared contract exposes a borrowed opaque lease, access flags and checked operations. The single guest mapping, lease context, generation, commit and teardown remain tied to the existing worker session. Do not compile a second authoritative session or leak native backing memory. |
| WOW page-domain, USER/GDI and guest-copy interfaces | Bounded contracts under `common/vdm-memory`, `common/wow` and `common/vdm-redir` (final names subject to existing-owner inspection) | Split consumer-facing declarations from worker-private initialization, session structures and implementation. Keep the actual worker's one page-domain, native object lifetimes, guest-copy authority, calling conventions and ordinals. Delete cross-component private include paths, including indirect and forced includes. |
| Historical thread-start calling-convention bridge | `lib-win32/thread` for generic Win32 thunk; `common` for explicitly supplied product thread/session hooks | Generic code must not include session or Worker Base. The sole existing session authority supplies bind/unbind/exit callbacks; design initialization and cross-DLL sharing before implementation so static copies cannot create separate state. Preserve cdecl/WINAPI stack behavior, TLS binding, normal return, `ExitThread`, SEH cleanup and fail-fast rules. |

Public header extraction is not a mechanical directory move: split the eight
currently reached NTVDM headers along actual consumer use, remove the WOW32
need for `session.h`, and verify transitive dependencies using the compiler's
dependency database. Preserve the original mirror and registered divergences.
Where a provider remains genuinely NTVDM execution state, define a narrow
shared contract and one authoritative provider; do not create a common copy of
the state or move guest execution policy into a platform library.

## Dependency-ordered P deliveries

1. **P1 — remove unused Worker Base link input.** Eliminate it from VDMREDIR,
   prove a real link without that archive, verify no actual members previously
   entered the DLL, and exercise redirector startup, thread behavior and guest
   network paths with available fixtures. Check other non-worker links for the
   same archive; preserve the two authorized worker links.
2. **P2 — shared CRT adapter.** Relocate the proven common CRT compatibility
   provider into `lib-win32/crt-compat`, change WOW32/NTVDM build selection,
   and verify symbol resolution, calling convention, failure and formatting
   behavior without adding duplicate exports.
3. **P3 — shared guest-memory and redirector contracts.** Split consumer-only
   lease/copy declarations into common; remove private NTVDM includes from
   VDMREDIR and WOW32. Preserve synchronous copy order, aliasing, failure and
   lease commit/release. Put the generic lease mechanism in a shared owner only
   if it preserves the existing single owning context and failure semantics.
4. **P4 — WOW/USER/GDI boundary.** Split the page-domain, GDI alias, user
   runtime and session-binding headers into shared consumed contracts and
   worker-private interfaces. Update all 40 affected object dependencies and
   prove there is no NTVDM-private header in the WOW32 production closure.
   Preserve page publication, callbacks, borrowing, retire, aliases and cleanup.
5. **P5 — thread ABI and session integration.** Separate generic native thread
   call conversion into lib from product session binding in common using an
   explicit, lifetime-safe hook interface. Demonstrate actual VDMREDIR no
   longer selects the NTVDM-private thread object, and retain one worker
   session owner. Check normal, cancellation, exception and thread-exit paths.
6. **P6 — full cross-configuration closure.** Audit all supported source,
   private-header, archive-member and binary-import graphs and reconcile each
   original S2 violation. Include a runtime-ABI removal design and delivery
   here only if it is part of the owner's chosen scope. A no-code P6 can be
   evidence-only; an implementation P remains subject to the full local gate.

The P boundaries can be combined or reordered if a complete dependency chain
requires it, but a P must not leave a broken build or a second state owner.
For every production P: build affected x86/x64 products, run focused positive,
negative and lifecycle fixtures plus all selected Control gates, stage the
coherent ten-image package, publish to this worktree's `assets/release`, verify
all hashes, record evidence and commit locally. No push or main-worktree
runtime deployment is part of the established workflow.

## Runtime ABI decision

The current WOW32 and VDMREDIR binary imports are 184 and 38 NTVDM thunks in
the audited profile. Relocating source and declarations into common does not
remove those PE import-table dependencies. If the owner requires no runtime
import either, first design a common-owned provider/binding with one runtime
state instance and an initialization path that works before any imported
function or DLL initialization is reached. Compare a shared runtime module
with an explicitly passed function table: include loader/CRT initialization,
all 184/38 symbol and ordinal contracts, x86 calling conventions, callback
reentrancy, worker restart and error cleanup. A new common DLL affects package
composition and deployment; a function table affects startup and binding.
Neither alternative is accepted solely by this design, and PE import auditing
must prove whichever final boundary is chosen. Original NTVDM-required exports
for external/original consumers cannot be removed without separate proof.
