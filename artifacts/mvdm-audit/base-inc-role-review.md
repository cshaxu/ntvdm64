# T473 S10: selected common base headers

This checkpoint classifies five of 146 previously pending original base header
files. The [observer](base-inc-selection-observer.py) reads the real Ninja
commands and VALID dependencies for eight selected CPU40/CCPU production
translation units. Their compiler driver ran those same commands in
preprocess-only form; the resulting line markers and nonblank emitted
declarations are recorded in [observations](base-inc-selection-observations.json).
Production object, archive and worker hashes were checked unchanged before and
after preprocessing. This is not a link, ABI or runtime acceptance test.

| Original header | Observed selected contexts | Remaining proof |
| --- | --- | --- |
| `bios.h` | `c_main` emits 81 declaration lines. | BIOS state layout, providers, reset and guest-visible behavior across consumers. |
| `config.h` | `c_main` emits 70 declaration lines. | Config values, consumer ABI, mutation, errors and lifetime across subsystems. |
| `cpu4.h` | `c_tsksw` and `c_main` each emit five declaration lines. | Selected hardware interrupt and CPU entry contracts, ordering and failure behavior. |
| `timer.h` | `c_main` emits 36 declaration lines. | Timer/EOI state layout, ownership, reset, concurrency and guest-visible timing. |
| `xt.h` | All eight selected units emit 15 declaration lines apiece. | Effective-address and shared CPU type ABI, callers, fault ordering and lifetimes. |

All five match the frozen primary original byte for byte. They remain
**partial**: original text retention is 100%, but selected declarations do not
yield a defensible behavioral restoration percentage. The other 141 files stay
pending; absence of emitted declarations in these eight contexts does not
exclude macros, other translation units, profiles or product capabilities.
This checkpoint changes neither product nor guest images.
