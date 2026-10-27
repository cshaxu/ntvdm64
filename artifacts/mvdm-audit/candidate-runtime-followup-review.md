# S10 diagnostic comparisons after the first candidate failures

This supplements the [candidate review](candidate-runtime-review.md). It
records diagnostic observations, not a selected runtime correction or release.
The published package remains unchanged. No guest or production source was
modified by these comparisons.

The [follow-up inputs and outcomes](candidate-runtime-followup/observations.json)
bind the additional reports, scripts and owner records:

| Comparison | Result | What it does and does not establish |
| --- | --- | --- |
| Same error-location WOW32 diagnostic, two further fresh isolated runs | Both pass | Three diagnostic-provider passes in total. The ordinary candidate's two failures are retained; instrumentation/layout changes are not a repair. |
| Candidate with only the two new real-mode CPL assignments removed | Fail: modal dialog absent; launcher later needed diagnostic cleanup | The generated complete modesw.c equals pre-354 source and its frozen hash. Removing those assignments is not a demonstrated remedy. This run did not record the same 5023 creation failure, so it does not prove an identical mechanism or exonerate every changed path. |
| Candidate worker with error-only observation in page_domain_publish_handle, normal candidate WOW32 | Pass | No rejection branch was witnessed on that run. This identifies no offending branch and does not establish that all publication guards are correct. |

Both comparison worker builds reused the actual production compile, library
and final-link commands, including the VdmTib storage check. Output objects,
libraries and EXEs went only to fresh diagnostic directories. Before/after
checks verified that the normal cached object, archive, worker and import
library hashes remained unchanged. The candidate and accepted packages were
not overwritten. The three unsuccessful early CPL-builder invocations were
runner construction failures before a comparison runtime was tested; their
retained logs are not counted as product failures or passing tests.

The rejection observer records only host-owned metadata and current CPU
register values on a return-zero branch, preserving its return and last-error
value; it does not dereference extra guest addresses or change publication.
Its additional function code can still affect a diagnostic execution, and
the passing observation is therefore deliberately inconclusive.

The actual remaining question is why ordinary production creation sometimes
rejects a valid native window before completing its original dialog/control
state. The current source separates property enrollment, class/version facts,
native handle generation publication, observed-menu/window identities and
borrow retirement. A useful next causal witness must identify the failed
invariant and its exact generation/state; simply rerunning until a pass,
deleting a guard, or reverting a source change because a window failed would
not establish that contract. These failures remain open within this T while
the whole-source reconciliation continues.
