# CCPU BSF execution owner

Frozen universe SHA256 F73FFD45DFF44FCA4ECEFC49EF769BEAB5C085542CD04FFE4D6A7B5F942E3196. `softpc.new/base/ccpu386/bsf.c` matches frozen OpenNT SHA256 229E244BDB8D904EE6E2E390C36A8F5C7BCCEF6AA8FB2BC832B7D91F723B1FD0; source equality alone is not coverage.

Current `c_main.c:1726` selects opcode 0F BC and decodes 16-bit or 32-bit operand, fetches, calls original `BSF`, then writes result. BSF sets ZF for zero and leaves its destination intact; nonzero scans low to high, clears ZF and writes index. The current worker graph and historical map attribute `_BSF` to bsf.obj as documented in file-audit; selection is not guest instruction behavior.

`python artifacts/mvdm-audit/shard-1/bsf-source-native-fixture.py` verifies frozen hash, extracts the actual function body and compiles with a minimal SET_ZF harness. On this x86-64 test host, for all 65536 zero-extended 16-bit values and each single-bit 32-bit value (65568 cases), nonzero destination and ZF agree with native BSF, and zero sets ZF and leaves the original source destination unchanged. Native BSF zero destination is architecturally undefined and deliberately not used as an oracle. This is source-body arithmetic only, not a linked production object, actual decoder, partial 16-bit register preservation, paging/fault behavior, processor mode, or full 32-bit domain.

Keep pending pending real selected CCPU decode/execute tests, 16/32 register/memory cases, zero, prefix, page fault/no writeback, concurrent or lifecycle boundaries and current executable identity. No runtime guest or product/guest changes.
