# T475 S3 — Worker Base and NTVDM dependency removal design

## Scope and source baseline

The owner admits one S for the illegal Worker Base and NTVDM dependencies in
the [S2 inventory](../evidence/m0-t475-s2-component-dependency-inventory.md).
The two workers' dependency on worker-base remains the owner's explicit
exception. NTHook dependencies are outside this S. Source baseline `5bc6eb897`
and local package `2b1325bd-17fb-4435-b2be-55995de325f6` retain the current
ABI, guest behavior and ten-image release. No guest media or mirrors are to be
rewritten. Redir-only conversion and redirector decisions belong in VDMREDIR. Common code
contains project-specific mechanisms and contracts actually shared by multiple
components; lib-win32 contains independent Win32 mechanisms with no common or
executable-private dependency. A consumer must never require another private
component as a result of the move.

WOW32's and VDMREDIR's PE images currently import the NTVDM executable ABI.
Whether this S also removes that runtime import is a pending scope decision;
the design and P sequence below first address the independently proved source,
header and static-link violations. No implementation should treat an unresolved
runtime-scope decision as permission to weaken a requested no-dependency goal.

## Target ownership and invariants

| Mechanism | Target owner | Boundary |
| --- | --- | --- |
| Redir-specific network/result conversion and copy workflow | `vdmredir-dll` | Keep Redir's data interpretation, ordering, API results and errors with Redir. Assess the current NTVDM `mvdm_redirector_worker_copy_from/to` bodies and move only the Redir-specific orchestration that can operate through the shared guest-memory contract. Do not make the worker depend on Redir. |
| Guest address validation, bounded read/write and lease contract used by NTVDM, WOW32 and Redir | `common/guest-memory` (working name) | Shared product contract, not a Redir interface. Preserve one existing worker-owned guest address space, lease context, generation and lifetime; do not duplicate session state or expose native backing pointers. Put an implementation in common only when its production consumers and ownership justify it. |
| NTVDM session, guest mapping and WOW page-domain state | Existing worker-owned instance | Consumers receive bounded access through common contracts. The provider remains singular even if source code or declarations are reorganized; preserve calling conventions, ordinal compatibility, teardown, callback and error ordering. |
| Generic Windows CRT compatibility adapter | `lib-win32/crt-compat` | Shared mechanism without Redir, common or worker-private policy. Preserve legacy `sprintf`, `sscanf`, `_swprintf` and ABI/link behavior. |
| Historical thread-start calling-convention bridge | `lib-win32/thread` for generic thunk; common for product session hooks | Redir and the worker consume the generic mechanism. The worker owns actual session binding; avoid per-module copies of session state and retain cdecl/WINAPI, TLS, SEH, `ExitThread` and fail-fast semantics. |
| WOW USER/GDI/page-domain consumer contracts | Bounded `common/wow` contract (working name) | Shared only where consumed across component boundaries; leave WOW32-specific logic in WOW32 and worker-private lifecycle in the worker. Split declarations rather than mechanically relocating the entire session or page-domain header. |
| Unused VDMREDIR `worker-base.lib` input | None | Remove after proving the real production link and required behavior. The two workers retain their explicitly authorized dependency. |

These are ownership decisions, not permission to create a new copy of guest,
session or page-domain state. Names are working names; inspect current owners
before adding interfaces. Runtime exports required by existing original callers
remain until their consumers and compatibility are fully accounted for.

## Dependency-ordered P deliveries

1. **P1 — remove Redir's unused Worker Base link.** Remove the explicit
   `worker-base.lib` input from the VDMREDIR production closure, verify no
   actual member was supplying a symbol, then build and exercise the real
   redirector including startup and available network/thread fixtures. Confirm
   NTVDM and NTVWM still select Worker Base.
2. **P2 — shared guest-memory contract.** Extract the minimum bounded guest
   address, read/write and lease contract genuinely shared by the worker,
   WOW32 and Redir into common. Separate consumer-facing declarations from
   `session.h` and other worker-private interfaces. Verify bounds, aliasing,
   lease ownership, commit, failure, teardown and single-session behavior.
3. **P3 — return Redir-only copy logic to Redir.** Inspect the two current
   worker-side redirector copy functions and move only Redir-specific conversion
   and orchestration into `vdmredir-dll`, using P2's shared contract for guest
   access. Keep worker execution state with its existing owner; account for
   existing NTVDM exports and do not create a worker-to-Redir dependency.
   Verify segment/offset addressing, sequential aliased fields, copy failures,
   error mapping and actual network redirector behavior.
4. **P4 — extract shared CRT compatibility.** Move the common Win32/UCRT
   spelling adapter into `lib-win32/crt-compat`, update both NTVDM and WOW32
   source selections, and prove symbol/ABI and formatting behavior, including
   missing and duplicate provider negative checks.
5. **P5 — separate WOW/USER/GDI contracts.** Split common-consumed declarations
   from eight reached NTVDM-private headers; keep WOW-specific logic in WOW32
   and worker state/lifecycle with the worker. Verify the 40 affected WOW32
   objects and transitive/forced includes, page publication, callback, borrow,
   retirement, aliases and cleanup on the actual production path.
6. **P6 — extract thread mechanism and bind session once.** Place generic
   Win32 thread/calling-convention mechanics in `lib-win32/thread`; use common
   contracts for the product-specific binding. Replace VDMREDIR's selection
   of NTVDM-private `thread_start_compat.c` without moving session state into
   Redir or creating a second owner. Verify x86 stack, current-thread binding,
   normal exit, explicit `ExitThread`, exceptions, cancellation and cleanup.
7. **P7 — final dependency closure.** Re-audit all component source, link and
   compiler-recorded header closures, actual archive selection and runtime
   imports. Verify each S2 Worker Base/NTVDM finding is resolved under the
   owner's chosen runtime-ABI scope. An evidence-only closure needs no build;
   a production change retains the full gate.

These are sequential reviewable P boundaries. If analysis shows a boundary
cannot build safely on its own, revise the order or combine the inseparable
parts and record why; never introduce a temporary duplicate authority. Each
production P requires the affected x86/x64 build, focused positive, negative
and lifecycle tests, all selected Control gates, coherent local ten-image
publication and hash checks, evidence and a separate local commit. No push or
main-worktree runtime publication is part of this workflow.
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
