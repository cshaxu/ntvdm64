# DPMI 0500: provider difference and bounded acceptance plan

This follows `dpmi-query-selected-object-review.md` and the existing provider reconciliation; it does not change source, guest or ledger. The only remaining pending `dpmi32/` C/H row in the current four-shard decisions is original `i386/dpmimem.c`. Its native-NT memory implementation and the selected CPU40 `dpmimemr.c` use different allocation and reporting owners. An unselected original source or a passing CPU40 test is not evidence that those owners are equivalent.

## Concrete source-derived example, not a runtime measurement

Take an illustrative 32-bit `GlobalMemoryStatus` snapshot with 128 MiB available page file, 64 MiB free physical, 256 MiB total physical, 1 GiB total virtual, 512 MiB available virtual and 512 MiB total page file. For CPU40 assume the independently managed XMS pool reports 8 MiB total free and a 4 MiB largest free span. Native i386 source computes 80% of available page-file space, rounds to pages, then caps `LargestFree` at 15 MiB; CPU40 publishes the allocator's largest span. The selected-object probe in the companion report actually executed the CPU40 side with those pool sizes, not the i386 side. Other fields are source calculations:

| 0500 field | Native i386 calculation in this example | CPU40 calculation in this example |
| --- | ---: | ---: |
| LargestFree bytes | 15 MiB | 4 MiB |
| MaxUnlocked pages | 3,840 | 1,024 |
| FreePages | 16,384 | 2,048 |
| AddressSpaceSize pages | 262,144 | 4,096 |
| PhysicalPages | 65,536 | 4,096 |
| FreeAddressSpace pages | 131,072 | 2,048 |
| PageFileSize pages | 131,072 | 131,072 |

Both implementations set MaxLocked `0xB61` and UnlockedPages `0xB68`. These values describe distinct native-address-space and shared-pool models; this synthetic difference does not establish a user-visible application failure, that NT4 i386 values are required in CCPU40, or that changing CPU40 reporting to the larger estimate would be safe. Native code uses DWORD arithmetic; real values and overflow must be tested in the actual target environment instead of extrapolated from this example.

## Existing tests and concrete limits

`tests/observation/dpmi_memory_info.asm` already exercises INT 31h 0500 and 0501/0502 through a separately built observation COM: it checks that a 64 KiB allocation subtracts 16 pool pages and that free restores the values; it also asserts the selected provider's specific 4,096-page compatibility fields. `m0-t420-s38-dpmi32-capability-review.md:2788` records a version-bound real direct/nested guest witness. Because the test hardcodes CPU40 values, it **cannot** prove equality to the original i386 provider. The same ledger's LIMIT_PROBE is an explicitly reproduced historical negative: 36 output bytes were written while the selector limit exposed 16. It is not a passing case or evidence of current-package failure; its approved boundary remains a separate task and must not be silently waived.

For finite next verification, extend or pair the existing observation (without modifying/rebuilding any original guest image) to record all nine raw 0500 fields and 0501/0502 results before allocation, after allocating varied sizes, after deliberately creating/freeing holes, after exhaustion and after task exit. Pin the tested worker, immutable guest and observation binary hashes and keep failures visible. Compare against a real original i386/native-monitor environment if one is available; a test reimplementing the arithmetic is only a source-derived prediction, not an original-runtime oracle. In parallel use controlled selected-object tests for actual VdmQuery/SAQuery failures and owner cleanup; the companion fixture replaces both calls and does not prove their production failure behavior. Determine a desired contract only from these results and existing approvals. Keep `i386/dpmimem.c` pending until the profile/equivalence decision is evidence-backed; do not introduce a second allocator.
