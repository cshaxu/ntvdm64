# T147 S1 CPU archive reachability-divergence audit

## Question

Can the earlier T146 stop after `50:36` be explained from retained CPU
archive/object/toolchain/link provenance, without a second execution or a
guest/adapter/Bochs semantic change?

## Inputs and procedure

The audit compared retained T130/r5/T146 archives, maps, generated makefiles,
manifests, extracted `cpu.o` members and PE headers.  The extracted object
evidence is retained at
`artifacts/analysis/t147-s1-cpu-archive-provenance-001`.

## Direct facts

| Surface | T130 | T146 | Disposition |
| --- | --- | --- | --- |
| CPU archive member names/count | 93 | 93 | Identical membership/order. |
| CPU source/config baseline | r5 | r5 except registered `cpu.cc` | No guest/adapter/device source delta. |
| CPU Makefile base flags | `/MT /W3 /EHs-c- /Ox` | Same plus one enabled and four zero diagnostic definitions | Intended diagnostic-only delta. |
| Final linker major/minor | 14.43 | 14.43 | Same reported linker family. |
| Direct machine exception object | `DD3765...D5A` | `DD3765...D5A` | Identical explicit input. |
| `cpu.o` size | 15,624 bytes | 16,193 bytes | T146 is 569 bytes larger. |
| `prefetch` entry | section 5 +0x2c0 | section 5 +0x2c0 | Same entry; T146 `prefetch` grows from 0x260 to 0x2b0 and shifts following functions. |
| Normalized map extraction | 4,713 CPU lines | 4,714 CPU lines | Only seven compiler-generated `cpu.o` static symbols differ after address normalization. |

The retained T130 CPU archive was built on 2026-08-11 and T146 was compiled
on 2026-08-12.  T130 preserves a generated Makefile and a 14.43 final linker
record but no exact CPU compiler version, command log, or per-member object
hashes.  The complete archive hashes differ, as expected after a full archive
rebuild, and no retained evidence establishes whether all unchanged CPU
members are code-identical.

## Interpretation

There is no evidence for a BOP, adapter, guest, firmware, or device cause.
Conversely, provenance is insufficient to attribute the changed reachability
to the BX-TRACE-056 source block: the block executes only at the unreached
prefetch-fault branch.  T146's full archive rebuild adds an avoidable
uncontrolled variable because the retained T130 CPU member binaries are not
otherwise reproduced.

The justified next build is therefore narrower than T146: copy T130's
`cpu/libcpu.a`, replace only its `cpu.o` archive member with an explicitly
compiled BX-TRACE-056 object, preserve the original 92 remaining members and
archive order, and link without a direct object override.  This does not
repair runtime behavior; it only restores the strongest feasible comparison
baseline before any separately admitted observation.
