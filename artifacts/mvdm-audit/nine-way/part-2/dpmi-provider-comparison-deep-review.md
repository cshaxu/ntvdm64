# Part 2: DPMI memory-provider semantic comparison and executable-witness availability

This supersedes no accepted source disposition; it sharpens `dpmi-i386-allocator-semantic-reconciliation.md` without changing it. The question is whether different provider values establish an actual missing supported operation, not whether two architectures produce identical numbers.

## Proven source boundary

Original `dpmi32/sources:56-59` selects `i386/dpmimem.c` for original native monitor i386 and `dpmimemr.c` for MIPS/PPC/Alpha. This standalone product's captured x86 worker graph instead selects `dpmimemr.c` under CPU_40_STYLE, not i386/dpmimem.c. `src/mvdm/README.md` registers DIV-224: CPU40 XMS already owns the extended address range, and the bounded adaptation reuses original shared XMS suballocator for DPMI allocate/free/reallocate/query. `xmem.c` retains per-task allocation tracking and its DIV-272 BOOL-status correction. A second NtAllocateVirtualMemory owner would overlap/reserve different state; it is not a drop-in implementation of the current pool.

## Precisely observable differences

Both implementations write nine DWORDs and compatibility constants MaxLocked=0xb61 and UnlockedPages=0xb68. Their 0500 reports deliberately differ:

| Field | Original native i386 (`i386/dpmimem.c:187-260`) | CPU40 selected (`dpmimemr.c:217-306`) |
| --- | --- | --- |
| LargestFree | Four-fifths of available page-file estimate, rounded down and capped at 15 MiB. | Largest free extent of shared XMS/DPMI pool. |
| MaxUnlocked | Reported LargestFree/4096. | Reported LargestFree/4096. |
| FreePages / FreeAddressSpace | Host available physical / host available virtual pages, respectively. | Both use shared-pool total free bytes / 4096. |
| AddressSpaceSize / PhysicalPages | Host reported total virtual / physical pages. | Both fixed 4096 pages (16 MiB) compatibility values. |
| PageFileSize | Host total page file / 4096. | Same host total page file / 4096. |

`config.c:160,454-487` specifies defaults of 15 MiB XMS for DOS and 8 MiB for WOW (PIF can configure DOS); `xms.c:35-64` subtracts 64 KiB from that XMS size when creating ExtMemSA. Therefore immediately initialized pool capacity cannot exceed 14.9375 MiB (DOS default) or 7.9375 MiB (WOW), before any allocations; this is a source-derived bound, not a captured 0500 runtime value. A host's native i386 estimate can approach 15 MiB independently of actual pool fragmentation. The value difference is real at the source-contract level, but does **not** by itself prove a missing allocate/free capability or a user-visible compatibility failure: host-i386 information is an estimate in a different native address-owner topology, while CPU40's pool-derived fields change with the actual allocation domain.

A potential compatibility question remains for a real program that branches on reported numbers or requires an allocation above the standalone pool. There is currently no reproduced such failure, no proof exact host statistics were a contractual guarantee across original architectures, and no evidence justifying changing DIV-224 or inventing capacity. Track real configuration/fragmentation and consumer needs before treating it as a defect. Separate issue: both original i386 and selected dpmimemr construct the output pointer from selector base plus DI/EDI after validating one byte. T420 S38 documents a reproduced reduced-limit invalid output case and its explicit historical owner acceptance as original guest/compatibility limitation; this is not a normal 0500 pass or proof of current general memory safety.

## Existing verification and exact limits

`tests/observation/dpmi_memory_info.asm` and `tools/audit/Verify-T420S38DpmiInterrupt.ps1 -MemoryInfo` exercise INT31/0500 via DOSX → BOP 53:0B, then 0501/0502 64 KiB allocation/free with 16/32-bit and direct/nested routes. It checks pool field relationships, free-page delta and restoration, compatibility constants and guarded output, **not** i386-vs-CPU40 A/B, >15 MiB, fragmentation, valid host physical/virtual field equality or absent backing. Historical `m0-t420-s38-dpmi32-capability-review.md:2670-2684,2788-2816` records those observations for worker `56ba3839fac527b8b452df1fd5cc1f54f4f11aec5e6144040a9085f806f51554`; they are not a test of the current release.

Read-only inventory on 2026-10-28 found no prebuilt D38M.COM under the repository build/assets/tests paths or `O:/winnt/tests`; other D38 files do not match the published r2 probe identity. The preserved S38 runner copies the probe into a package and launches its run16/NTSRV. `package-isolation-review.md` establishes ordinary package copies retain UUID and can cross-talk. No unique verified package plus prebuilt matching probe was established in this read-only review, so it was not launched, and guest was not rebuilt. This is a finite test-availability limit, not a claim the desktop or product is broken. The parent coordinates whether an independently identity-stamped test package and previously built probe are available.

## Disposition

Current i386 alternative remains **pending**, not globally excluded. CPU40 shared-pool operation is supported by historical end-to-end 0500 evidence, but current release behavior and alternate host allocation semantics are not newly accepted. S11/S12 should re-run existing positive guest probe when a hash-identified prebuilt artifact and unique isolated package exist, then test configured memory sizes, fragmentation, task-exit free and actual program-sensitive cases. S19 binds observations to release. No product, guest, build, process or shared-ledger change was made.
