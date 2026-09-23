# T422 S1 normalized WOW32 mirror-delta audit

## Method

The 77 active files from `src/mvdm/wow32/sources` were compared with their
same-path OpenNT source under `O:/repos.external/OpenNT/base/mvdm/wow32`.
The comparison normalizes CRLF/LF before hashing. It therefore measures source
content, not Git's checkout line-ending policy. No mirror file was rewritten.

Result: 71 files are byte-equivalent after newline normalization; only six
have a semantic content delta. A raw `git diff --no-index` reports 76 whole
files because the two trees use different line endings, which is not a
functional delta and must not be "fixed" through whole-file formatting churn.

## Six real deltas

| Mirror file | Disposition | Reason and receiving owner |
| --- | --- | --- |
| `wow32.c` | retain pending S2/S7 acceptance | DIV-296 scopes native private USER borrows through original thunk reentry; DIV-291 preserves the original absent-value retail fallback when the NT4 machine-global key is unavailable. S2 owns reentry/lifetime; S7 owns the shared layered-registry reader. |
| `wcall16.c` | retain pending S2 acceptance | DIV-178 replaces only invalid durable guest aliases with bounded frame leases, releasing them before recursive `host_simulate`. S2 must prove real callback/reentry/cancellation. |
| `wkman.c` | retain two compile seams | The temporary `_X86_` definition scopes the original `vdmdbg.h` inclusion and is required by the current compiler profile. Restoring the unused original `ntseapi.h` directly conflicts with modern SDK `winnt.h` security declarations; its omission is therefore also required. The previous three explicit `WINAPI` spellings and unused private include were restored to original text and are no longer semantic deltas. |
| `wuman.c` | retain pending S2 acceptance | DIV-300 redirects immutable USER16's `gpsi` and CallCsrFlag references to guest-linear worker storage. A native pointer or static host byte cannot be published into the guest. S2 must produce and retire the full view. |
| `wuser.c` | retain pending S3/S4 acceptance | DIV-295 provides a bounded read lease for original bitmap resource use; the original resource conversion and cleanup remain intact. S3/S4 own real drawing/resource lifetime. |
| `wdos.c` | retain | `demexp.h` supplies the worker's actual `__cdecl demClientErrorEx` declaration. The original local prototype would inherit `/Gz` and silently declare the wrong ABI in this standalone x86 build. |

## Measured reduction

The fresh x86 rebuild permitted and verified this S1 cleanup: six physical
locally authored lines were removed (one unused `wkman.c` private include and
five `wkfileio.c` cache/declaration lines), and six redundant source-level
`WINAPI` tokens were restored to the original `/Gz` form in `wgdi31.c`,
`wkman.c` and `wucomm.c`. Standardized semantic mirror differences fell from
nine files to six. The retained rows are not permission to treat their
semantics as accepted: their owning S packets must prove the stated contracts.

## Reproduction

Run normalized SHA-256 over each selected pair, replacing both CRLF and lone
CR with LF before hashing. Inspect only the listed nine with
`git diff --no-index --ignore-space-at-eol`; do not stage a newline-only
rewrite.
