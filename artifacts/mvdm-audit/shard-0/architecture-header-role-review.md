# Nonselected architecture declaration variants

The companion CSV records 33 unchanged shard-0 header paths: 29 MIPS and four
PPC variants beneath `softpc.new/host/inc`. Each current hash matches the frozen
inventory. These paths are excluded only as nonselected platform declaration
variants, not as instruction, video or guest functionality.

The original `softpc.new/obj.vdm/CDEFINE.INC` chooses the architecture directory
through `PROCESSOR_ARCHITECTURE` in its non-386 branch, then selects `prod` or
`pig`. The current T310 worker generator instead explicitly selects CPU40/CCPU,
`PROD`, adapter `generated/x86/prod`, original `base/ccpu386` and `base/cvidc`
include owners. Its three required adapter headers are `PigReg_c.h`,
`sas4gen.h` and `gdpvar.h`. This is a profile choice, not a finding that the
historical declarations or their algorithms are defective.

The freshly read worker `ninja -t deps` snapshot contains 632 VALID blocks,
zero STALE blocks and 11,531 dependency path occurrences. No exact path in
this batch is an observed consumer dependency. Same-basename selected paths
are recorded per row; basename equality does not imply equal layout or
equivalent semantics. Configuration-time consumers are considered separately:
the examined T310/CMake code does not select these architecture include roots
or directly copy these headers into its x86 carrier directory. This does not
establish the historical origin of every generated adapter declaration.

Snapshot SHA-256:
`B0E1045F59CA9D067EE5D240C7FDE7F4372EC663D45061A51D9DD576CE396FDB`.
Command: `ninja -C build/.ninja/msvc-x86-release/worker -t deps`.
The snapshot reflects cached observed dependencies, not every possible future
conditional inclusion. Recheck classification if the selected profile or
generator changes. The independent CPU40, C-VID and adapter ABI/behavior
reviews remain open. No product, firmware or guest changed in this batch.
