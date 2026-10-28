# Part 8: original Win16 SHELL interface and Registry ownership

Owned file identities and original/ZIP comparisons are in `guest-family-inventory-review.md` and its companion observer. This report examines the five owned SHELL entries as one call chain. It does not claim a freshly executed test or binary provenance from an original build.

## Source and provider chain

`src/mvdm/wow16/shell/library/shell.def` exports Registry ordinals 1–7 and ShellExecute / FindExecutable 20–21. `shell.asm` declares original `SHELLThunk` entries and initializes its guest local heap; WEP reports success. `src/mvdm/wow32/wshtbl2.h:15-21,38-39` registers corresponding original WS32 thunk bodies, not a new shell engine. `wshell.c:792` maps the guest arguments, and original callback enters the guest WinExec path; `wshell.c:853` maps and flushes the FindExecutable result. The modern Shell provider supplies native association behavior. See `docs/etc/evidence/m0-t461-s1-shell-registry-owner-matrix.md` for detailed ownership and `m0-t447-s2-scope-freeze.md` for the callback adapter boundary.

Registry ordinals call original selected thunk bodies `wshell.c:123-700`, but the project's formal build binds their native-facing calls to the project's NTVDM Registry facade. The parent owns the layered Registry state; WOW32 owns only mapped worker-local tokens and teardown. Historical `m0-t461-s2-binding-implementation.md` describes token lifecycle and initial memory overlay; `m0-t461-s3-registry-persistence-candidate.md` records a subsequent durable overlay, serialization and restart test. These statements concern those versioned candidates, not automatic proof of the current image's code identity or an untested API's behavior.

## Per-file classification

| Owned path | Role and action |
| --- | --- |
| `wow16/shell/library/shell.def` | Preserve original guest ordinal contract. Modern host association/Registry calls are behind the existing original thunks; do not replace ordinals with a new dispatcher. |
| `wow16/shell/shell.asm` | Preserve original guest initialization, WEP and thunk entrypoints. `SHELLThunk` is an entry, not proof a specific operation reached a current provider. |
| `wow16/shell/SHELL.DLL` | Immutable guest input; the prior observer found byte equality with the ZIP `WINNT/SYSTEM32/SHELL.DLL`, which proves identity only, not runtime loading or source-to-image reproducibility. |
| `wow16/shell/shell.rc` | Original guest resource source; retain for historical source completeness; independent active resource use not established. |
| `wow16/shell/SHELL.RES` | Original resource build input/artifact; identity does not prove the packaged DLL contains this exact resource result. Preserve; do not rebuild guest. |

## Lifecycle, failure and evidence limits

The original ShellExecute callback uses guest allocation/free and a synchronous native callback scope; nested callbacks, exception unwind and guest return have historical focused evidence in the T461 matrix. This is not an assertion that all executable associations behave identically on all hosts. FindExecutable's missing/error output and guest buffer flush require a caller-level witness. Registry key close, deleted key and reuse require token invalidation; S3's focused persistence fixture covered restart/tombstone and two-process commit for its recorded package, not every Registry API or callback interleaving. Native Shell provider's Windows.Storage exception branch was explicitly a non-pass observation in T461 S2; do not reinterpret that as success.

S14 should preserve original SHELL ordinals and thunk mappings, run immutable guest success/error/cleanup witnesses for FindExecutable and applicable Registry operations, and retest ShellExecute nesting and unwind against the actual provider. Registry persistence/failure and token reuse require verification against the current package, while host association semantics remain host owned. S19 must record package hashes and separate historical evidence from current results. Do not import a competing shell database or reinterpret modern private structures from NT4.

Reproduce source entrypoints via `rg -n "SHELLThunk|LocalInit|WEP" src/mvdm/wow16/shell/shell.asm`, `rg -n "WS32ShellExecute|WS32FindExecutable|WS32Reg" src/mvdm/wow32/wshtbl2.h src/mvdm/wow32/wshell.c`, and rerun the part-8 guest inventory observer. Read the T461 S1/S2/S3 and T447 S2 evidence cited above. No product, guest or shared ledger changed.
