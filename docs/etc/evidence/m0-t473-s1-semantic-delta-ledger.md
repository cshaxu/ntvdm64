# M0 T473 S1 — semantic delta owner ledger

## Scope

This ledger resolves the seven source-like semantic deltas left by the
whole-closure baseline without treating a build as behavioral proof. The rows
are the current `r12` inventory's files that have neither an inline
`DIVERGENCE` marker nor a direct `src/mvdm/README.md` leaf reference. It does
not close the remaining 188 semantic deltas or establish whole-package
reachability.

| Current original path | Compile selection | Actual change and authority | Owner / disposition | Evidence and remaining boundary |
| --- | --- | --- | --- | --- |
| `dos/command/cmd.c` | worker | Original `GetSystemDirectory` becomes selected-product `NtvdmGetSystemDirectoryA`; COMMAND initialization remains original. | Retain project firmware/root adapter as sole path authority. | T427 S4 root-layout evidence and `d0c2fc1d0`; startup remains a runtime regression. |
| `dpmi32/dpmiint.h` | worker | Kernel-VDM `i386` carrier is unavailable; CCPU40 selects the original non-monitor carrier storage. | Retain narrow `CPU_40_STYLE` profile; do not import NT4 kernel-VDM state. | T404/T430 CCPU evidence; package reachability remains S1 work. |
| `dpmi32/dpmimscr.c` | worker | Original portable FastBOP failure conditionally publishes an adapter endpoint and reports the result. | Retain provisionally as T422 bridge, owned by `mvdm_softpc_fast_bop`, not a DPMI engine. | `806598b9b` and T422 FastBOP fixture; carrier/teardown reconciliation remains open. |
| `softpc.new/base/ccpu386/iret.c` | worker | Privilege-return stack write follows loaded SS width via `set_current_SP`, not operand width. | Retain CCPU40 stack-width correction. | `065ba3b3f` and T430 stack-transition fixture. |
| `softpc.new/base/ccpu386/ret.c` | worker | Same lower-privilege stack-width correction as IRET. | Retain atomically with IRET. | `065ba3b3f` and shared T430 fixture. |
| `wow32/winsockp.h` | wow32 | Async Winsock contexts gain task-thread serial identity and cancellation declarations. | Retain original WOW32 provider plus one task-lifecycle identity bridge. | `96eb20b87`; full socket teardown remains package evidence. |
| `wow32/wow32.def` | wow32 | Exports `W32TerminateTaskTicket` for selected parent BOP task termination. | Retain narrow parent-to-WOW ABI; implementation is `wow_task_termination.c`. | T449 S3 and `7bdc5169f`; export alone is not lifecycle acceptance. |

## Removed non-functional deltas

S1 removed an unused `mvdm_shadow_registry` include from `dpmi32/vxd.c` and
restored exact macro whitespace in `dpmi32/xlathlp.h`. It also classifies
trailing whitespace and line endings separately from semantic modifications.
No guest, provider ABI, generated source, or package byte changes result from
these audit cleanups.

## Consequence for recovery order

The first five rows are parent `ntvdm.exe` owner-package concerns; the final
two are `wow32.dll` provider concerns. They do not justify importing an
unselected kernel VDM monitor, a private USER subsystem, or a duplicate
project implementation. The next ledger phase must reconcile complete
original package selection and link/extraction before deciding whether any
retained bridge can be removed.
