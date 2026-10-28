# S10 shard 1: CCPU profile and original build carriers

This group contains all six shard-1 pending `base/ccpu386` paths. All six
current files match both original editions byte-for-byte and each has one
DOS16 and two WOW32 frozen source-catalog rows. The [replayable observer](ccpu-profile-observer.py)
and [evidence](ccpu-profile-evidence.json) record the original identities,
current flags, object symbols, map, image binding and preprocessor hashes.
No guest, product, installed image or shared ledger was changed.

The original CPU recipe includes `ccpudefs.inc`, whose definitions include
both CPU_30_STYLE and CPU_40_STYLE. The current generator reads the `sources`
file and sets CPU_40_STYLE, CCPU and SPC386 in its own command flags; it does
not simply use the old include. CPU_30_STYLE selects a different V86 host
contract according to registered divergence DIV-044. Its omission requires a
source-owner and behavior review, not an automatic assertion of equivalence.

The original c-files list has 129 distinct entries; the recipe has 130. The
list alone contains `p.c_main.c` and `p.c_tsksw.c`; the recipe alone contains
`ntstubs.c`, `ntthread.c` and `sasCdef.c`. A project-wide search found no
direct `c-files` reference in the inspected SoftPC and generator trees, but
that does not establish its original generator or eliminate its role.

Selected 386 preprocessing retains the declarations in `bswap.h` and
`invd.h`, but their own objects have no corresponding defined implementation
symbols. The selected decoder routes opcodes 0F 08 and 0F 09 to `Int6`;
the source provides INVD/WBINVD only under SPC486. This is not proof of 486
guest support or a reason to discard it. The linked original `c_cpu_simulate`
body matches the actual release outside COFF relocation words; this does not
by itself prove execution of these opcode cases.

The [original preprocessed declaration and decoder excerpts](ccpu-profile-excerpts.json)
are retained with input and output hashes. Run
`python artifacts/mvdm-audit/shard-1/ccpu-profile-extract.py --check` while
the matching temporary compiler outputs exist, or `--verify-retained` to
verify the retained excerpts without them. The preprocessor's selected worker
objects and image were hash-checked unchanged.

| File | Recommendation | Evidence and remaining boundary |
| --- | --- | --- |
| `bswap.h` | Pending | Its own selected object sees the typed declaration; the 486 provider and decoder path remain conditional. S11 must reconcile both declaration and implementation gates and guest behavior. |
| `invd.h` | Pending | Its own selected object sees the declaration while the 486 provider is absent. Preserve original no-emulated-cache rationale and investigate selected-profile faults. |
| `invd.c` | Pending | Compiled object has no INVD definition; original 486 body intentionally does nothing in absence of an emulated cache. Do not equate this to modern host-cache handling. |
| `gdpvar.h` | Pending | The physical CCPU variant is included by `sasCdef.obj`, but no nonblank lines survive the sampled preprocessing; direct SAS source has no `GLOBAL_` macro access. Resolve transitive macro use and distinguish its layout from C-VID and selected x86 GDP. |
| `c-files` | Pending | Original list differs from the actually selected recipe; identify its producer/consumer and alternate-profile relevance before disposition. |
| `ccpudefs.inc` | Pending | Original recipe includes it; current generator independently projects a subset of its flags and does not select CPU_30_STYLE. Establish each omitted or replaced setting and ABI consequence before disposition. |

All retain 100% original text; executable restoration percentages are
unmeasurable from these observations. The earlier coherent 486 profile
recovery remains assigned to S11; no instruction or host capability is
removed from scope.
