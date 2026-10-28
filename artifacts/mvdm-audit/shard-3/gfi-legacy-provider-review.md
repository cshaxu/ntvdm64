# GFI legacy remote and diagnostic providers

Static review only; no guest or real floppy was run. All five files below are byte-identical to the corresponding `O:/repos.external/OpenNT/base/mvdm/softpc.new` files. The captured x86 worker compile commands in `../ccpu-remaining-header-source/commands.stdout.txt` define `PROD` and do not define `SLAVEPC`. `nm -g --defined-only` on the current cached `worker/obj/disks/gfi_ibm.obj`, `gfi_sflp.obj`, and `gfi_test.obj` yields no defined global symbols; `gfi.obj` exports the GFI dispatch functions. These object observations establish the current selected configuration only, not behavior in other build profiles or in a released executable whose identity is not checked here.

| File | Original role | Bounded disposition |
|---|---|---|
| `base/disks/gfi_ibm.c` | `SLAVEPC`-gated RS232 client RPC for the remote IBM PC floppy controller. | Exclude its implementation from current NTVDM profile coverage; retain as historical source. This does not establish equivalent support for the remote-PC protocol. |
| `base/disks/gfi_sflp.c` | `SLAVEPC`-gated FLA-to-remote-PC FDC server adapter, including DMA and command handling. | Same profile-specific exclusion; separately track any future remote-PC requirement, rather than equating it with real local floppy. |
| `base/disks/gfi_test.c` | `!PROD` diagnostic pseudo-server which traces GFI operations. | Exclude debug tracing implementation from current `PROD` runtime; no feature-recovery claim follows. |
| `base/inc/gfisflop.h` | Slave-PC RPC constants and protocol definitions, consumed by the two `SLAVEPC` files. | Retain with the excluded remote profile. The original fixed `/dev/ttya` value is not evidence of a selected NT device. |
| `base/inc/gfitest.h` | Diagnostic server prototypes; included by `gfi.c` and `gfi_test.c`, with calls in `gfi.c` gated by `!PROD`. | Classify as an included declaration header, but its diagnostic calls are excluded in the observed production profile; do not call the diagnostic behavior tested. |

The selected actual GFI dispatcher and empty/NT real-floppy providers remain separate **partial** runtime owners. See `../nine-way/part-4/gfi-real-floppy-provider-review.md`, `../host-ch-scope/virtual-real-floppy-owner-review.md`, and `../disk-native-role-review.md`: the existing GFI fixture uses actual dispatcher/empty objects with recording host seams and does not demonstrate actual media, while the prior guest witness is only an INT13 reset with no medium. Real-media read/write, media change, provider retirement and the independently identified FDC thread/session paths remain unverified. It would be incorrect to claim these legacy providers are active, or that excluding their profile completes the selected real-floppy capability.

No shared ledger or product code changed; classifications are recommendations for the central audit owner.
