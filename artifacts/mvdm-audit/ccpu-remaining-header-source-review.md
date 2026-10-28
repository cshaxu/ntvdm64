# Remaining twelve CCPU header roles

This follows the [header/macro checkpoint](ccpu-header-macro-review.md) at
`771ddfb0c`. It is a source/composition review, not another runtime repair or
an automatic ledger promotion. The previously sampled eight translation units
cannot prove global absence; the actual whole-worker Ninja commands and
dependency log were therefore captured read-only. Both Ninja processes exited
zero without rebuilding. The [observations](ccpu-remaining-header-observations.json)
bind current source/graph hashes and the complete dependency dump identity.

| Original header | Actual selected consumers and source contract | Conclusion and finite follow-up |
| --- | --- | --- |
| bswap.h | Its own bswap.obj includes the typed BSWAP declaration outside SPC486; the implementation body remains inside SPC486. | Declaration selection does not restore BSWAP dispatch. Retain pending with the complete S11 486 profile. |
| cmpxchg.h | Its own cmpxchg.obj includes the three width-specific declarations; guarded definitions are absent from the selected profile. | Reconcile both macro gates, dispatch and register/memory fault semantics with S11. |
| invd.h | Its own invd.obj includes INVD; original body selection is separately SPC486-guarded. | Preserve the original no-cache-model boundary; no modern host-cache equivalence is inferred. S11 profile obligation remains. |
| invlpg.h | Its own invlpg.obj includes INVLPG's address argument, while the selected decoder/provider is not enabled. | S11 must establish the actual paging/TLB owner and privilege/fault order. |
| wbinvd.h | Its own wbinvd.obj includes WBINVD; definition selection remains gated. | Same original cache-model boundary as INVD, not independent instruction enablement. |
| xadd.h | Its own xadd.obj includes two operand pointers and an operand-size argument. | S11 must preserve both results, aliasing, flags and atomic/fault commit within the coherent profile. |
| evid_c.h | No physical use of this CCPU-directory variant in the complete worker dependency log. It declares generated video structures; a different C-VID variant exists. | This is not proof that video is absent. Resolve the selected generated provider/schema family with S13 before excluding this physical variant. |
| evidfunc.h | No physical use of this CCPU variant in the complete worker dependency log. Its first SimpleByteWrite is S_2451; the C-VID header names S_2139. | These are distinct generated symbol families, not name-equivalent replacement files. Bind actual definitions/callers before deciding this variant's role. |
| evidgen.h | Ten VALID consumers: original SAS, mouse, trace and host presentation owners plus the text-video adapter. Its five-field VideoVector is selected through egacpu.h. | Unlike the preceding two headers this is actively selected. Verify actual expansions, field/provider correspondence and object/image consumers; do not exclude it as a duplicate C-VID header. |
| gdpvar.h | Only sasCdef.obj includes this physical variant. The generated CCPU layout places EDL_WORKSPACE_0 at 43 and marks HookNumber -1; C-VID uses 36 and 0, and the CPU register owner uses the separate selected x86 generated carrier. | An include is not a macro access. Trace actual SAS expansions before claiming any invalid-offset access or copying a different layout. Source sascdef.c defines the SasVector, not another GDP allocator. |
| pigger_c.h | c_reg.obj includes the physical CCPU header, which only defines Pig=1. The c_reg PIG-only state code is separately guarded by uppercase PIG. | The constant and build switch are distinct. Establish expanded consumers; neither an empty declaration emission nor the filename proves that runtime CPU diagnostics should be removed. |
| univer_c.h | sasCdef.obj includes the CCPU variant, which only defines ImpossibleConstraint=-1; the C-VID variant has additional universe structures. | Resolve actual expansion/definition needs; do not merge incompatible generated schemas merely because the basenames match. |

All twelve physical headers remain **pending** in this review: it has not yet
established a complete runtime/ABI contract or a justified whole-file exclusion.
The six 486 declarations have actual own-TU dependencies even though they were
absent from the earlier eight-TU sample. That observation refines the evidence,
not the already documented absence of their selected instruction bodies.

For the six schema variants, whole-file hashes differ from the adjacent C-VID
versions, including the deceptively small pigger/univer files. The build
generator explicitly orders the C-VID include directory first for video owners
while retaining the shorter CCPU variant for CPU-side callers. The original
`base/inc/egacpu.h` then maps video operations to C_Video under C_VID.
Source retention and include selection alone cannot establish layout or
indirect-call compatibility. No defect is inferred merely from an unused
macro's sentinel offset.

The [486 profile record](shard-3/cpu486-profile-boundary.md) and
[recovery worklist](recovery-worklist.md) own the finite S11/S13 integration
obligations. This review changes no ledger count, mirror, product or guest.
