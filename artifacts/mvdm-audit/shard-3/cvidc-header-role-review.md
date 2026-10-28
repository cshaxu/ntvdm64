# Shard 3: selected C-VID declarations

Propose **partial** for the three pending file roles below. The exact current
compiler commands for the named objects were rerun in preprocessing mode by
the [runner](cvidc-header-preprocess.ps1). It checks the production object
and worker hashes before and after. The [observer](cvidc-header-observer.py)
and [results](cvidc-header-observations.json) verify primary and secondary
original file hashes, both audit catalogs and nonblank compiler-selected text.

| Header | Actual compiler context | Observed first contiguous declaration portion | Remaining proof |
| --- | --- | --- | --- |
| `cpuint_c.h` | original `ccpu386/localfm.c` | 35 lines | Interrupt state identities, callback ordering, reentry, reset and worker teardown. |
| `fm_c.h` | original `host/src/nt_cpu.c` | 52 lines | `CleanedREC` layout versus actual host callers and CPU execution semantics; mirror inclusion is registered DIV-048. |
| `fpu_c.h` | original `cvidc/accessfn.c`, compiled using the actual non-PROD access flags | 21 lines | Floating-point constants/layout do not prove FPU provider reachability, numeric behavior or exceptions. |

The partial count is not a behavior-restoration percentage. Each file is
byte-identical to both original editions (100% source text retained), but a
preprocessor line marker and declarations are not proof of ABI compatibility,
all conditional branches, linked runtime behavior or guest capability. Other
shard-3 C-VID headers remain pending; absence from these three contexts is no
reason to exclude them. No product or guest image was modified.
