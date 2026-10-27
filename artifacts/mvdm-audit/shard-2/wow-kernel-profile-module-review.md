# WOW KERNEL private profile and module-name boundary

Scope is shard-2 `wow32/wkernel.c`, frozen reconciliation SHA-256 F73FFD45DFF44FCA4ECEFC49EF769BEAB5C085542CD04FFE4D6A7B5F942E3196. Source-only review; no product, guest, or other shard changes. It remains partial.

## Private profile

`wktbl2.h:164-166` dispatches original private-profile integer/string/write entries. Each selected thunk calls `W32PrivateProfileFileName` for its filename then the original host profile API; path resolution uses `UpdateDosCurrentDirectory(DIR_DOS_TO_NT)`. The project adapter `mvdm_softpc_profile_shadow_system_ini` locates the selected root system.ini, copies it to a temporary file on first use, holds a delete-on-close handle, associates the copy with the current session and registers teardown. Reads and writes to the exact case-insensitive bare name use that same session copy. Other filenames pass through; failure to obtain the shadow also returns the bare name, which is subsequently passed to the native API. The one static owner and failure cases require lifecycle context rather than assuming arbitrary concurrent sessions.

**Registration needs reconciliation:** `src/mvdm/README.md` DIV-297 currently says the argument is replaced by the selected root file; the actual code uses a temporary copy and only falls back to the original bare name when the helper fails. This is an observable distinction for writes, session reset and helper failures. The source comment explicitly describes a session copy, and `m0-t427-s1-root-search-audit.md` notes a temp shadow. Review the intended contract and evidence before updating the registration; do not silently change implementation. Tests should verify selected source, repeated reads and writes within one worker, teardown cleanup, next-worker persistence semantics, failure of copy/teardown registration and unchanged arbitrary paths. In particular determine whether fallback reaches a host profile mapping and what behavior is authorized. These are verification/design questions, not a demonstrated runtime failure.

## Module filename

`wktbl2.h` k49 calls `WK32GetModuleFileName`: real module identity is host module; an HTASK alias uses original `GetHtaskAliasProcessName`. DIV-450 preserves that selection but returns zero for nonpositive capacity and bounds the guest allocation and flush to `cch`. The T452 S2 delivery receipt records production-command alias fixture normal/exact/truncated/one/unavailable and an integrated WinMine run. That does not establish every real-module, zero-capacity, invalid alias and guest memory failure case. `m0-t452-s1-modules-memory-alias-source-matrix.md` describes further intended cases.

No new behavior test was run. Existing file identity/line-count/matching heuristic and selected product evidence stay in the shard ledger; textual match is not a restored capability percentage.
