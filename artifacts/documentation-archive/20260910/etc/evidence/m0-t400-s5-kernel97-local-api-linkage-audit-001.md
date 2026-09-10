# M0 T400 S5 — KERNEL.97 local-API provider/linkage audit

## Corrected disposition

KERNEL ordinal 97 is **not** a missing WOW32 C provider. The initially
suspicious declaration `WK32GetTempFileName` is not the callable ownership of
the selected original Write path.

| Layer | Verified record | Meaning |
| --- | --- | --- |
| Original Write | `FCreateFile` imports KERNEL.97 `GetTempFileName` | The caller has the original 16-bit KERNEL ABI. |
| Original KERNEL31 | `ikernel.inc` aliases it to `IGetTempFileName`; `ldopen.asm` supplies the local implementation and performs its original DOS-create sequence | This is the intended provider for a correctly local call. |
| WOW table | `wktbl2.h` ordinal 97 uses `W32FUN(LOCALAPI, "GETTEMPFILENAME", MOD_KERNEL, sizeof(GETTEMPFILENAME16))` | It is a guard entry, not a request to dispatch this API to a `WK32...` body. |
| Retail x86 WOW32 | `wow32.h` maps `LOCALAPI` to `WOW32UnimplementedAPI` when `DEBUG` is absent; the selected formal build has `/DPROD` and no `/DDEBUG`/`DEBUG_OR_WOWPROFILE` | Any KERNEL.97 arrival at the 32-bit WOW table is itself an invalid cross-boundary route. |
| Table materialization | `wowtbl.c` includes `wktbl2.h`; release `W32FUN` stores the function pointer directly | No string/export lookup can silently bind the unused `WK32GetTempFileName` declaration. |

The active `wkernel.h` declaration without an active C definition is therefore
historical/dead at this product boundary, not an x86 link or provider
completeness defect. Adding a host implementation would change the original
ownership direction and would be a wrong repair.

## Relation to S4

S4's no-scratch-transition result stays non-causal. It neither proves nor
disproves that the 16-bit KERNEL.97 routine ran; it only rules out a physical
root transition visible to that watcher. This S5 result removes the proposed
WOW32 provider body from the candidate list entirely.

## Outcome

No code, ABI, table, build, guest, media, runtime, memory, or x64 change is
selected. The remaining ordinary WRITE low-memory boundary is still the
original `FInitMemory` branch set from S3, not a KERNEL.97 host-provider
problem.
