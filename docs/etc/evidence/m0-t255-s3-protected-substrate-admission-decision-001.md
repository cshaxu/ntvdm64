# M0 T255 S3 — Protected-Mode Substrate Admission Decision

## Decision

T255 closes with the generic context sublayer recovered, but **does not admit
any DPMI provider**.  The next prerequisite is a separate selector-blind
protected virtual-access and guest-linear mapping substrate.

## Evidence

| Requirement | Current evidence | Disposition |
| --- | --- | --- |
| Real/protected context transition, GPR32, selectors, FLAGS and resume | T255 S2 context fixture passes both native directions through `SetCR0`, data/stack segment load, protected CS branch and native flag setter. | Closed mechanical sublayer. |
| Protected selector:offset checked read/write | `access.cc:38..166` exposes boolean descriptor checks, but `access32.cc` immediately calls `exception()` after a failed check or translation. That path uses the CPU exception/longjmp contract; it cannot be safely called as a synchronous bx-vdm provider helper. `get_laddr` alone is only address arithmetic and would bypass access/page handling. | Not admitted. A new fixed-width, copied, fault-contained action must be designed and proven separately. |
| Original `Sim32GetVDMPointer` compatibility | Original `dpmi386.c`, `dpmiint.c`, `dpmimem.c` and `int21map.c` dereference returned host pointers. Returning Bochs backing pointers across the x64 boundary violates the project ABI; copying the routine would also assume NT4 LDT/process state. | Not reusable directly; future bx-vdm shim must retain source ordering/failure while replacing pointer representation. |
| LDT/process integration | Original `dpmi386.c:73` uses `NtSetLdtEntries`; fallback at `:96` uses `NtSetInformationProcess(ProcessLdtInformation)`. Those describe NT4 host-process LDT composition, not a modern Bochs guest descriptor owner. | Deferred to DPMI source-derived rehost after the generic virtual/mapping substrate exists. |
| Extended-memory identity | Original `xmem.c` stores `XMEM_BLOCK *` and host virtual addresses in guest-visible register pairs. This is invalid on x64 and cannot be repaired by a context result. | Deferred to the same future substrate plus a bx-vdm source-derived identity shim. |

## Boundary decision

No new core/mantle code is selected in this S.  A future package may only
propose a bounded, all-or-nothing copied protected virtual-range action after
it proves fault containment and ownership.  It must not expose a Bochs object,
backing pointer, raw linear address or historical service vocabulary.

This is not a claim that the original DPMI package is complete.  It is a
precise no-admission decision that prevents a premature `53:xx` leaf route.
