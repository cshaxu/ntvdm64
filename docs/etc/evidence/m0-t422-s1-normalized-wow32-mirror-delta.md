# T422 S1 normalized WOW32 mirror-delta audit

## Method

The 77 active files from `src/mvdm/wow32/sources` were compared with their
same-path OpenNT source under `O:/repos.external/OpenNT/base/mvdm/wow32`.
The comparison normalizes CRLF/LF before hashing. It therefore measures source
content, not Git's checkout line-ending policy. No mirror file was rewritten.

Result: 68 files are byte-equivalent after newline normalization; only nine
have a semantic content delta. A raw `git diff --no-index` reports 76 whole
files because the two trees use different line endings, which is not a
functional delta and must not be "fixed" through whole-file formatting churn.

## Nine real deltas

| Mirror file | Disposition | Reason and receiving owner |
| --- | --- | --- |
| `wow32.c` | retain pending S2/S7 acceptance | DIV-296 scopes native private USER borrows through original thunk reentry; DIV-291 preserves the original absent-value retail fallback when the NT4 machine-global key is unavailable. S2 owns reentry/lifetime; S7 owns the shared layered-registry reader. |
| `wcall16.c` | retain pending S2 acceptance | DIV-178 replaces only invalid durable guest aliases with bounded frame leases, releasing them before recursive `host_simulate`. S2 must prove real callback/reentry/cancellation. |
| `wgdi31.c` | removal candidate | The added `WINAPI` on the local `GetRandomRgn` declaration is redundant because the formal provider deliberately compiles the whole original package with its original `/Gz` default. Restore original spelling after the fresh formal x86 rebuild. |
| `wkman.c` | split: retain one compile seam, remove four redundant deltas | The temporary `_X86_` define only scopes the original `vdmdbg.h` inclusion and is required by the current compiler profile. `ntseapi.h` has no remaining caller in this unit. The three added `WINAPI` spellings are redundant under `/Gz`; the added `wow_private_user_compat.h` include has no use in this unit. Restore the latter four deltas after formal x86 verification; do not conflate them with the required `vdmdbg.h` compile seam. |
| `wkfileio.c` | removal candidate | `fLastReference` merely caches `!pSFT->SFT_Ref_Count` immediately after the original decrement, and the initialized declaration adds no reachable failure behavior. Restore the original two direct conditions and declaration; rerun named-pipe close coverage. |
| `wuman.c` | retain pending S2 acceptance | DIV-300 redirects immutable USER16's `gpsi` and CallCsrFlag references to guest-linear worker storage. A native pointer or static host byte cannot be published into the guest. S2 must produce and retire the full view. |
| `wucomm.c` | removal candidate | The two `WINAPI` spellings on the writer-thread declaration/definition are redundant under the selected original `/Gz` package default. Restore original text while retaining actual COMM acceptance under S7. |
| `wuser.c` | retain pending S3/S4 acceptance | DIV-295 provides a bounded read lease for original bitmap resource use; the original resource conversion and cleanup remain intact. S3/S4 own real drawing/resource lifetime. |
| `wdos.c` | retain | `demexp.h` supplies the worker's actual `__cdecl demClientErrorEx` declaration. The original local prototype would inherit `/Gz` and silently declare the wrong ABI in this standalone x86 build. |

## Measured reduction opportunity

Six physical locally authored lines can be removed immediately after the
fresh formal x86 provider rebuild: the unused `wkman.c` include plus five
`wkfileio.c` cache/declaration lines. Six further source-level tokens
(`WINAPI`) are redundant in `wgdi31.c`, `wkman.c` and `wucomm.c`. These are
not new behavior and are assigned to S1's minimum-diff cleanup before S1
closes. The four retain rows are not permission to treat their semantics as
accepted: their owning S packets must prove the stated contracts.

## Reproduction

Run normalized SHA-256 over each selected pair, replacing both CRLF and lone
CR with LF before hashing. Inspect only the listed nine with
`git diff --no-index --ignore-space-at-eol`; do not stage a newline-only
rewrite.
