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

## Proposed sequential stages

These are candidate stages, not active S briefs. Materialize the working plan
and the sole active brief at admission under the execution rules.

### S1 - Integer arithmetic and memory fill (D13-D15)

Recover the selected routines from `base/ntos/rtl/x86/largeint.asm` and
`movemem.asm`, or prove a same-contract binding before retaining an alternate
provider. Remove obsolete x64 import-library justifications for the current
x86 product. Include mirror declarations and `copy_fnc.c` caller loops where
they duplicate the same original operation.

Verify unsigned 64/32 division, quotient/remainder, high-bit inputs and
original divide-by-zero behavior; multiplication low-64-bit overflow results;
and byte-count ULONG fill with original untouched trailing bytes. Compile/link
the actual providers and regress selected timer and memory consumers. Signed
C overflow is a contract risk, not an already proved current-machine failure.

### S2 - Environment ownership and status conversion (D11-D12)

Recover `base/ntos/rtl/environ.c` creation, mutation and destruction as one
ownership unit, including reached query/current-environment dependencies.
Restore name validation, absent-variable deletion status, empty-block storage
and scanning, capacity/growth and failure ordering. Bind explicit private
environment blocks separately from the process-environment/PEB route; do not
silently overwrite the real host PEB or replace its lock with unrelated TLS.

The original uses `ZwAllocateVirtualMemory`, `ZwQueryVirtualMemory` and
`ZwFreeVirtualMemory`. Investigate a package-private same-shaped binding over
public `VirtualAlloc`, `VirtualQuery` and `VirtualFree`, retaining the original
environment algorithm. Prove page-rounded committed capacity, zero-fill,
in/out base and size semantics, allocation failure, release and returned
NTSTATUS. This is not a textual API substitution. For `MEM_RELEASE`, public
VirtualFree requires the allocation base and a zero size; account for every
reached original call. Never mix HeapFree, VirtualFree or environment-string
release ownership. A host environment snapshot is not automatically owned by
the imported virtual-memory allocator; clone/adopt only through a proved
boundary. No generic memory manager or kernel import is required by this plan.

Recover `base/ntos/rtl/error.c` and required tables, or a verified permissible
same-contract binding, instead of the current reduced status mapping. Cover
known/unknown statuses, special encodings and reached LastStatusValue behavior.
Test create/clone/set/replace/delete/destroy, empty environment, malformed
names, omitted environment arguments and allocation failures. No missing
operation may silently succeed. Regress COMMAND environment/child behavior.

### S3 - USER and Redirector encoding (D16 and D26)

Recover `windows/core/ntuser/rtl/chartran.c:MBToWCSEx` and the already mirrored
`netapi/netlib/copystr.c:NetpCopyWStrToStr` through audited allocation and
guest-copy bindings. Verify original paths/provenance at admission. Include
the reached RTL Unicode/OEM conversion provider: restoring only the outer
NetLib call does not establish original encoding semantics below it.

Preserve ACP versus non-ACP branching, partial-conversion results, allocation
and caller-pointer failure behavior. For OEM conversion prove byte capacity,
terminators, DBCS/unrepresentable input and original error handling rather
than assuming Unicode character count equals encoded byte length. Keep safe
guest writes distinct from the conversion algorithm; no durable guest pointer.
Test zero/short/exact buffers and allocated/caller-owned output, matching
release ownership, guest-copy failures and selected WOW/Redirector consumers.

## Acceptance and accounting

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
