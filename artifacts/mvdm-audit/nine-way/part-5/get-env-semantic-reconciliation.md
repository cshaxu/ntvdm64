# Part 5: SoftPC environment helper and conditional diagnostic consumer

Owned `softpc.new/base/support/get_env.c` matches both frozen originals (`host-input-family-observations.json`). Captured x86 Ninja `build/.ninja/msvc-x86-release/worker/build.ninja:266,1241` compiles it into `original-softpc-support.lib`; this is object/archive selection, not a verified link-map reference or proof the functions execute in a released image. Current `src/mvdm/softpc.new/host/inc/host_def.h:77` maps `host_getenv` to the C runtime `getenv`.

The original NTVDM helper has three behaviors: IBOOL defaults when missing, otherwise is true only for case-insensitive TRUE; ISM32 parses base-0 integer and falls back if no conversion starts; STRING returns the environment-owned pointer or caller default. Those are not interchangeable with a new parsed configuration cache. STRING ownership is borrowed: callers must not free or retain it across environment mutation without a separate valid lifetime guarantee. Integer partial parsing and range handling follow `strtol`; do not silently claim complete validation.

A full repository source-text search of `IBOOLgetenv|ISM32getenv|STRINGgetenv` finds one nondefinition caller: `ccpupig.c:193` reads CCPU_HISTORY_SIZE for an instruction history buffer. The file is wrapped by `#ifdef PIG` at lines 15–320, while the captured x86 Ninja's CCPU flags do not explicitly define PIG and also build `ccpupig.obj`. This is evidence of a likely inactive diagnostic caller in this exact explicit-flag profile, not proof no forced include defines PIG, no other dynamic caller exists, or that linked code was stripped. No existing runtime observation was found in this review.

| Owned path | Decision |
| --- | --- |
| `softpc.new/base/support/get_env.c` | Preserve original selected support helper pending actual link-map and caller proof; do not mark as executed from archive inclusion. No product code change recommended. |

S11/S17 should establish whether the selected object is retained in the current linked image and whether any real caller can reach it before proposing cleanup. If selected, exercise absent, valid, invalid and partial numeric input plus borrowed string lifetime in an isolated host fixture; do not introduce a new environment state owner. Retain PIG diagnostics for separately admitted profiles. S19 binds any claimed result to the exact image. No build, guest, product, process environment or shared ledger was modified.

Replay: `rg -n "IBOOLgetenv|ISM32getenv|STRINGgetenv" src/mvdm`, inspect `get_env.c`, `ccpupig.c` and the quoted Ninja edges and full compile flags; rerun part-5 host-input observer with bytecode disabled.
