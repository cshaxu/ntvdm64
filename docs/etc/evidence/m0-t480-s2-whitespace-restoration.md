# T480 S2 — bounded original whitespace restoration

## Contract and baseline

Owner authorizes S2 implementation through local build, test, assets/release
publication and commit, without push or O:/winnt writes. Baseline is8bbba90bc,
following the [S1 closure](m0-t480-s1-build-consumers-closure.md). The finite
input is146 S1 whitespace candidates in62 host C/H files. This is not a renewed
functional recovery audit. Guest source, ASM, existing exclusions and already
removed historical files remain untouched. Established runtime tests use copies
of the immutable assets/winnt.zip media; no guest source/media archive changes.

## Complete disposition

| Disposition | Candidate count | Reason |
| --- | ---: | --- |
| Restored | 53 | Original indentation, comment spacing, trailing whitespace or blank placement can be recovered while preserving all code and logical source positions. |
| Retained diagnostic layout | 70 | The apparent whitespace lead consists of existing #line placement, with no independent spacing edit to restore. |
| Retained line-sensitive blank placement | 23 | Adding/removing the original blank would change subsequent logical locations before a resetting #line. |
| Total | 146 | Every admitted ID has a disposition; no open S2 candidate or new functional issue is invented. |

The53 restorations change30 mirror files. F01-F03 restore cmdenv.c indentation;
F04 restores qevnt.c comment spacing. The same finite review also restores
spacing in cmdexec/cmdredir, CCPU, serial/keymouse, event/reset and declaration
carriers, plus original blank lines before existing diagnostic resets. Every
#error and #line directive is retained verbatim; no function, ABI, conditional
selector, macro body or executable token is changed.

The retained23 blanks are not missing functionality. Restoring them would need
additional diagnostic changes, contrary to this S's preservation boundary and
without a demonstrated net minimization. Do not reopen them merely because
they are not byte-identical. There is no proved unused whole-file or functional
sub-block restoration in this finite set. Substantive owner review remains S3.

The [complete restoration ledger](../../../artifacts/mvdm-audit/t480-s2-restorations.json)
names every ID, path, physical location, before/after text and reason, plus exact
file hashes and consumer results. The pinned original SHA-256 was checked before
every proposed change. Source bytes outside each accepted hunk, including their
existing line-ending convention, are preserved.

## Reduction and independent verification

For the30 affected files, the same S1 SequenceMatcher comparison changes from
1397 to1383 lexical edit blocks, added lines5559 to5534, and removed original
lines22516 to22452. The combined changed-line count falls by89. These are
diff-accounting measures, not counts of corrected bugs. Relative to our own
baseline the patch adds63 and removes24 lines, mainly restoring original blanks.
It adds no behavior.

The restoration tool conservatively checks nonblank line text and logical #line
coordinates across each complete file before writing. It merges original
whitespace around existing directives instead of replacing their surrounding
block wholesale. All file identities and proposals are checked before mutation.
The31st source change is required product identity metadata: APP_VERSION advances
from0.0.479 to0.0.480 for the admitted T480; protocol remains67. This occurs only
after the whitespace-equivalence comparison, so it cannot obscure that result.

All819 affected actual compiler commands from S1 were run before and after:
720 x86 and99 x64. The comparison includes every affected header consumer and
included-source consumer, not just edited C translation units. All1638 compiler
preprocessing runs returned0. Every matching command emitted identical nonempty
preprocessor text at identical logical source locations. Original spelling of
macros is not compared to their expanded spelling; actual compiler output is.

[The independent acceptance entrypoint](../../../tests/build/verify-t480-s2-preprocessor-equivalence.py)
derives the expected consumer set from the frozen build evidence and changed
file set, rejects missing/duplicate/failed commands, compares command identities,
location-sensitive fingerprints and line counts, and verifies the30 final source
hashes. Run it with build/t480-s2 after both capture phases.
`git diff -w --ignore-blank-lines --exit-code -- src/mvdm-host src/non-mvdm`
also returns0. This supplemental Git check alone would not prove unchanged
preprocessor locations; the actual compiler comparison supplies that evidence.

Reproducible tooling:

- [Pinned restoration planner/applicator](../../../tools/audit/restore-t480-s2-whitespace.py): fresh output under build/t480-s2, optional --apply after review.
- [Actual consumer capture](../../../tools/audit/verify-t480-s2-preprocessing.py): matching VS environment, --arch x86/x64, four bounded compiler jobs per environment and180-second per-command deadline.

## Delivery verification

Both worktree-local MSVC architecture builds passed, producing all11 product
images. Full runtime verification passed16/16 selected gates in499255ms,
including WOW frontiers, both17-case DOS modes, RPC, native GUI, version
rejection, console transitions, relaunch, isolation, retirement and nested
handoff. The separate WINMINE gameplay check passed flag/counter, reset, open
cell, WM_CLOSE and run16 completion. DOS4GW/NESticle remains the previously
owner-excluded external-fixture case; no pass is claimed for it.

The tested package15b7d7df-c013-4b33-9162-e4fe892ed039 was published only to
this worktree assets/release. All11 published hashes match the staged package
and Full runtime manifest. Application version is0.0.480; protocol remains67.
No guest source or media archive was modified, no O:/winnt write occurred,
and no push is authorized. Local commit is the accepted delivery boundary.

The initial observer helper compilation lacked /std:c11; the temporary build
command was corrected and the retry passed. No product source workaround was
introduced. Restored upstream trailing whitespace and space-before-tab are
intentional: normal diff-check passes for docs/tools/tests/common, and the
mirror check disables only those two original-whitespace diagnostics.

[Delivery summary](../../../artifacts/mvdm-audit/t480-s2-delivery.json) records
build, test, publication and governance receipts, all11 hashes and the hash of
[the raw verification archive](../../../artifacts/mvdm-audit/t480-s2-verification.zip).
The archive retains commands, before/after consumer results and runtime logs,
without guest sources or duplicate product executables.

S2 is closed: all146 admitted candidates are dispositioned, all53 safe
restorations are delivered, and no S2 item is deferred. T480 remains open;
S3 substantive and mixed-hunk review is not admitted by this closure.


## S4 replay clarification

The S2 verifier intentionally requires the S2 post-restoration source hashes.
Replay it against commit b35effafd with the sealed S2 evidence, not the current
S3 source tree: cmdexec.c, dpmiint.c, nt_reset.c and ntexapi.h were subsequently
changed by S3. A hash mismatch there is correct version binding, not a failed
S2 compiler-equivalence result. The original receipts and verifier remain
unchanged. [S4 reconciliation](m0-t480-s4-registration-reconciliation.md)
checks the immutable S2 receipts against their Git revision and separately
binds the current S3 successor identities.
