# C-VID CPU/memory constant headers — shard 3

Six current pending files `softpc.new/base/cvidc/{adjust_c,ea_c,muldiv_c,ports_c,stack_c,xfer_c}.h` are reviewed as a generated CPU constant group. `cvidc-constants-observer.py` and captured JSON compare each to both original editions, historical MIPS/prod and current project x86/prod directories; normalize retained worker Ninja C-dependency paths; and record representative original CCPU map symbols. All six base/cvidc files match both reference editions. No build, guest, product or shared ledger was modified.

| file | role and proposed status |
|---|---|
| `adjust_c.h` | pending: flags/mask constants for adjusted arithmetic; no full actual consumer or behavior established. |
| `ea_c.h` | pending: scale constants for effective-address related generation; no selected consumer or addressing semantics established. |
| `muldiv_c.h` | pending: flag constants for multiply/divide generated context; the historical MIPS/prod file has different bytes, so do not assume all generated variants are identical. |
| `ports_c.h` | pending: EGA_GC_ADAP_DATA constant; selected port dispatch and meaning require owner-specific evidence. |
| `stack_c.h` | pending: stack slots, margin and enum; historical MIPS/prod bytes differ. No runtime stack-width/exception or lifecycle equivalence follows. |
| `xfer_c.h` | pending: privilege-level and far control-transfer type constants; unchanged header is not proof of actual protected-mode transitions. |

The normalized retained C-dependency snapshot lists none of the six exact base/cvidc or compared MIPS/x86-producer paths; x86/prod copies at those exact names are absent. This is limited to the captured worker snapshot, not proof of alternate builds, generated/indirect code or all relevant runtime functionality. Existing worker map lists selected original CCPU AAD, AAM, CALLF, DIV16, MUL16, POP and PUSH providers: these are representative related instruction bodies, not evidence they consume these specific header constants. The map and snapshot are not bound here to latest deployed EXE. Every item remains pending until actual generated consumer/profile and instruction/fault evidence are determined; no unsupported behavioral percentage is inferred.
