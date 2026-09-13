# Original RTL and character-conversion restoration

## Owner request and status

On 2026-09-12 the owner requested a separate candidate T at the head of
[Queue](../states/QUEUE.md), covering D11-D16 and D26 from the
[original-owner audit](../etc/evidence/opennt-replacement-audit-20260912.md).
This is proposal/queue authorization, not implementation admission or a
numeric T allocation. The current [Status](../states/CURRENT.md) is unchanged.
Reconcile these rows with the existing audit/restoration task at admission
to avoid duplicate implementation ownership. Preserve other candidate order.

## Objective and ownership

Replace duplicated runtime algorithms with their original OpenNT owners and
the smallest finite modern bindings. Original non-MVDM bodies remain under
`opennt-host` with original-relative paths; MVDM callers remain in
`mvdm-host`. Follow the [source policy](../etc/operations/policy/source-policy.md)
and [architecture rules](../rules/ARCHITECTURE.md). Broker deployment, CLI
changes, CPU instruction changes and complete WRITE recovery are non-goals.
The selected runtime remains Win32/x86 CCPU40 with `ntvdm32.exe <binary>`.

## Proposed sequential S plan

These are candidate S scopes, not active briefs or numeric allocations.  They
are intentionally grouped by original owner and failure/ownership contract,
rather than by every individual function.  At admission, establish only S1 in
Status; each later S opens only after the predecessor's committed evidence
proves its exit condition.

| Candidate S | Scope and source owner | Required exit before the next S |
| --- | --- | --- |
| S1 — recovery map and frozen baseline | Reconcile D11–D16/D26 with the selected graph, original OpenNT paths, current mirror/overlay/adapter providers, selected callers and x86 build manifests.  For every target, record the original body, all outbound dependencies, duplicate local implementation, proposed final owner and the smallest possible binding. | Indexed source/caller/disposition matrix; measured per-cohort diff footprint; no product behavior change; explicit go/no-go grouping for S2–S5. |
| S2 — arithmetic and fill cohort | D13–D15: recover `base/ntos/rtl/x86/largeint.asm`, `movemem.asm`, corresponding declarations and any duplicate `copy_fnc.c` loops. | Original or same-contract provider compiles and links in x86; division/multiply/fill positive and failure/edge fixtures pass; selected timer/memory consumers regress; measured deleted/retained diff. |
| S3 — private environment algorithm and U07-E lifetime | D11 plus the environment part of U07: recover `base/ntos/rtl/environ.c` as one create/query/mutate/destroy ownership unit.  Keep the original algorithm in its OpenNT mirror and bind only its finite allocation/query/free boundary. | Create/clone/set/replace/delete/destroy, malformed names, empty blocks, allocation failure and release ordering are proven; snapshot/install/restore succeeds and fails deterministically on launch/teardown; COMMAND environment regression passes. |
| S4 — status conversion cohort | D12: recover `base/ntos/rtl/error.c`, required tables and reached LastStatusValue behavior; do not bury reduced mappings inside the environment adapter. | Known/unknown/special status encodings, LastStatusValue and no-silent-success failure rows pass; x86 link and S3 environment regression remain green. |
| S5 — USER/Redirector encoding cohort | D16/D26: recover `windows/core/ntuser/rtl/chartran.c:MBToWCSEx`, `netapi/netlib/copystr.c:NetpCopyWStrToStr` and the reached RTL Unicode/OEM provider chain.  Guest-copy is a bounded consumer binding, never the conversion algorithm. | ACP/non-ACP, DBCS/unrepresentable input, partial result, zero/short/exact capacity, terminator, allocation/caller-buffer and guest-copy-failure tests pass; selected WOW/Redirector callers regress. |
| S6 — package integration and closure | Rebuild the selected full x86 worker composition from S2–S5; run source/diff review, selected DOS/COMMAND/EDIT regression and the bounded WRITE frontier observation. | Every D11–D16/D26 and U07-E row has a final disposition, runtime evidence and remaining-binding ledger; governance, link checks, commit/push and clean worktree complete. |

### S1 — recovery map and frozen baseline

S1 is deliberately research/design only.  It must not bulk-import RTL/USER
directories or retain a local replacement because its name happens to match an
original symbol.  It records the exact OpenNT revision/path, selected current
caller and ABI/layout/failure contract for each proposed provider.  If a
non-MVDM source body is directly composable, it remains under `opennt-host` at
its original-relative path; otherwise S1 names the finite adapter ABI and why
the original translation unit cannot compose.

### S2 — arithmetic and fill cohort (D13–D15)

Recover unsigned 64/32 division, quotient/remainder, high-bit inputs and
original divide-by-zero behavior; multiplication low-64-bit overflow results;
and byte-count ULONG fill with original untouched trailing bytes.  Signed C
overflow is a contract risk, not an already proved current-machine failure.
Remove obsolete x64 import-library justifications only where x86 source/build
evidence proves they no longer carry another ABI repair.

### S3 — private environment algorithm and U07-E (D11)

The original uses `ZwAllocateVirtualMemory`, `ZwQueryVirtualMemory` and
`ZwFreeVirtualMemory`.  The only permitted modern binding is package-private
and same-shaped over `VirtualAlloc`, `VirtualQuery` and `VirtualFree`, while
the original environment algorithm remains intact.  Prove page-rounded
committed capacity, zero-fill, in/out base and size semantics, allocation
failure, release and returned NTSTATUS.  For `MEM_RELEASE`, public
`VirtualFree` requires allocation base plus zero size; account for every
reached call.  Never mix HeapFree, VirtualFree or environment-string release
ownership, overwrite the host PEB, or replace its lock with unrelated TLS.

### S4 — status conversion (D12)

Recover the original status mapping separately from environment ownership.
No missing operation may silently succeed, and no convenience Win32 error
mapping may become a hidden substitute for original tables.

### S5 — USER and Redirector encoding (D16/D26)

Preserve ACP versus non-ACP branching, partial-conversion results, allocation
and caller-pointer failure behavior.  For OEM conversion prove byte capacity,
terminators, DBCS/unrepresentable input and original error handling rather
than assuming Unicode character count equals encoded byte length.  Keep safe
guest writes distinct from the conversion algorithm; no durable guest pointer.

### S6 — integration and closure

S6 cannot use a passing fixture as a substitute for the selected worker
composition.  It publishes a compact final ledger that separates original
code restored, mirror diff removed, independent implementation deleted and
irreducible modern bindings retained.  WRITE remains a bounded frontier here;
its full lifecycle belongs to the queued WOW16 package.

## Acceptance and accounting

### U07-E environment lifetime

This package also owns the environment slice of U07: snapshot/install/restore
and cleanup around original BaseClient environment projection, not only RTL
block allocation. Verify restoration on success, launch failure and teardown,
including restoration failure before backup release. Preserve error visibility
and ownership; a failed restore must not be silently reported as success.
Broker owns command payload transport, not this process-local environment
algorithm. Shared guest-memory lease mechanics remain with the DPMI/XMS
proposal; encoding consumers verify their own publication failures.

At admission revalidate the source baseline and selected callers. Retain each
original owner, rejected recovery rung, exact outgoing interface, replacement
disposition and x86 build evidence. Test source-proven differences without
claiming they caused existing WRITE failures. Report removed autonomous lines,
reverted mirror diff, added original source and remaining binding lines
separately; the approximately 250-line discussion estimate is not a promised
net deletion or a substitute for measured final diffs.

All three stages require focused positive/negative tests and affected runtime
regressions. Unknown mandatory semantics remain closure blockers. Build and
fixture products stay in `build/`; verified publication uses
`build/output/ntvdm32.exe` and deliberate deployment to
`O:\ntvdm64\ntvdm32.exe`, with runtime logs under `O:\ntvdm64\logs\`.
Review, governance checks, commit and push follow
[Execution](../rules/EXECUTION.md). This proposal imports or repairs no code.

## Public memory API references

- [VirtualAlloc](https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualalloc): reserve/commit and zero-initialized storage.
- [VirtualQuery](https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualquery): region state and capacity information.
- [VirtualFree](https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualfree): decommit/release and allocation-base requirements.
