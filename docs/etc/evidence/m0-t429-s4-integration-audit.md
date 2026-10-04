# T429 S4 final integration audit

## Admission and exact input reuse

Sequential S4 follows pushed production0654f5e0b and the approved T429 sequence.
It adds no production source/ABI or policy. Pin the eight files to S3 r006,
r009 full-product and r010/r011 publication manifests. The formal x86 cache
is T427 S2 r001, APP0.0.427/RPC38/I/O25, MSVC14.43/SDK22621 `/MT` CCPU40.

Reuse S3 Console17/Window17/WOW, importer/CP/failure/channel/font tests and
six EDIT200 matched samples only while these exact inputs remain unchanged.
Reuse S2 normal/diagnostic CCPU proof and CPU startup comparison because the
worker/source/object identities have not changed. These are identified prior
passes, not fresh S4 tests. No runtime matrix is skipped merely for speed.

Fresh serial integration uses the production r006 package and existing real
private-desktop observers: Console/Window CMD↔DOS parent return/direct exit23,
modern EDIT return, cooked caller restoration, native frontend close, unexpected
worker loss and independent sessions. Reports stay under build/M0-T429/S4;
only Z: is mapped and removed finally. No physical desktop/RDP assertion follows.
The final result must report genuine failures, limitations and the owner-verification
boundary before T closure. No S4 result is claimed at admission.

## Requirement and input audit

| Requirement | Evidence and boundary |
| --- | --- |
| Remove project observer from normal CCPU DECODE | S2 registered minimal mirror diff and normal/diagnostic object proof; normal worker hash6A670193 is unchanged in S3/S4. Diagnostic capability remains selected separately, not a release per-instruction branch. |
| Measure actual DOS EDIT input/publication | S1 producer/transfer/frontend instrumentation, S2/S3 actual EDIT200 conservation and final acknowledgements; no physical/RDP latency inference. |
| Repair only a demonstrated material cost | S3 real host CP attribution, one-query/frame fixture and fixed A/B/B/A/A/B comparison. No speculative batching, transport rewrite or frame filtering. |
| Preserve exact cells/fonts/attributes/cursor and failures | S3 r005 actual importer, public channel/commit/abort/EOF/handle tests, next-frame CP437/850 refresh; conversion fault is explicitly test-injected. All bodies and linked dependencies remain unchanged. |
| Preserve complete product behavior | S3 r009 Console17/Window17 plus retained WOW frontiers and r011 deployed smoke; eight manifest hashes match S4. Fresh integration below supplements, rather than repeats/relabels, these passes. |
| Preserve authentic shutdown/completion | S2 priority fixture/shared shutdown37, real close4, unexpected failure and isolation evidence; unchanged worker/common/service implementations. S4 separately exercises the real wiring with the new frontend. |
| No production measurement/helper/ownership leak | Only S2 normal diagnostic-selection/close-ordering and S3 frame-local query changes are production diffs; wrappers stay under tests/build, not deployed. APP/RPC/I/O unchanged. |

Incremental formal x86 graph check after S4 admission reports `ninja: no work
to do` for all six EXEs. S3 r009's eight-file manifest, r010 publication,
r011 smoke and S4 per-case identity checks are the reuse boundary. The
native CP capture already queries at its existing capture boundary; no additional
per-character query was found there. No unrelated original mirror implementation
or prior failure was silently replaced. Other-session proposal edits remain
outside this delivery.

## Claims and retained limitations

S2's matched48 cases support ~9.9% Console EDIT startup aggregate median
reduction; Window startup was essentially unchanged and candidate worst tails
were sometimes worse. This is not universal CCPU acceleration. S3 supports
~94.5% reduction in the median of run-level successful text-commit medians and
~10.8% reduction in its scripted EDIT200 workload median, not an additive
whole-product improvement or a latency SLA. Frame counts may differ with real
guest timing; every explicit publication is still delivered.

The proposal's matched SoftPC runtime comparison and physical/RDP/raw-input/focus
measurement remain unperformed, not passed. The delivered optimization has a
causal same-worker/same-guest/same-observer comparison independent of those
limits. No physical mouse smoothness guarantee is inferred. WOW acceptance is
WINMINE's retained UI entry and SOL/WRITE execution-frontier nonregression,
not general usability or all Win16 apps. Host scrollback is outside the agreed
projection contract. NTVWM's30ms capture policy remains unchanged. No1000-event
extreme mouse test is added; the accepted200-event workload is retained.

## Fresh integration and the retained failed attempt

r001's native, nested-Console and nested-Window cases pass actual parent
return/direct exit23. Its modern EDIT case times out and lacks line02; the
screen shows EDIT, but the observer log reports Window input and failed delivery.
This is retained as an aggregate failed run, not erased or called a product pass.
r002 explicitly isolates each test's Console/paced policy and uses the S1 r012
observer; all five remaining cases pass their original actual screen, Ctrl+Q,
echo, MEM, receipt, failure and isolation assertions. That result alone does not
repair or reclassify the original combined-run failure.

The actual source cause is the broker-I/O test's finally restoration:
`[Environment]::SetEnvironmentVariable(name,$null,'Process')` through this
PowerShell/.NET environment leaves an empty variable. Native
`GetEnvironmentVariableA(name,NULL,0)` then returns1, while PowerShell's empty
string condition is false. Both Window and generic milestone-input flags can
therefore leak into a later Console-only Ctrl+Q fixture; observed prompt input
also cannot stand in for its legacy fullscreen/paced input contract.
r003 diagnoses the actual native API, not an assumed environment string.
The reproducible `/MT /W4 /WX` x86 source/runner is now
`tests/observation/observer_empty_environment_probe.c` and
`verify-observer-empty-environment.ps1 -BuildRoot build/M0-T429/S4/r005`:
empty gives needed1/error0/read0, explicit removal gives needed0/error203/read0.
Its sole temporary variable is owned/checked and deleted finally; this diagnostic
is not a product/helper component and is not deployed.

The minimal test-only fix restores an originally absent flag using `Remove-Item
Env:` and preserves actual existing values unchanged. This matches the already
correct measurement runner's restoration pattern. No product I/O, event, wait,
deadline, exit/output assertion, or production binary changes. r004 then reruns
the original fixed eight-case script with the original T427 r049 observer, same
production package and original policies—not the isolated r002 substitution.
All eight pass; the formerly failing EDIT case now reaches Ctrl+Q and its real
CMD/DOS completion. No sleep, retry-until-success or production repaint fix is
involved.

| r004 case | ms | Actual result |
| --- | --- | --- |
| Native | 8977 | Direct Windows exit23 |
| Nested Console | 37245 | DOS VER/MEM and actual parent marker; exit23 |
| Nested Window | 11409 | Actual CAF/Window route, DOS output and parent marker; exit23 |
| Modern EDIT | 25971 | Real EDIT UI, Ctrl+Q, CMD echo, DOS MEM and launcher completion |
| Cooked caller | 23214 | DOS MEM/EXIT → native VER → DOS MEM/EXIT → outer CMD exit19 |
| Frontend close | 3685 | Accepted close completes native Console-session shutdown |
| Unexpected worker loss | 2854 | Receipt1067 while direct native target remains alive |
| Two sessions | 12465 | Selected close leaves independent input/echo/exit23 intact |

The script checks all eight copied and published hashes around each case;
r004/verified-package.json agrees with S3 publication. Z: is absent afterwards.
Durations include observation/owned cleanup; the37s Console case is not a new
performance guarantee and its original40s observation deadline is unchanged.
The final test-only source review preserves other-session work and confirms
only this identified restoration changes, not a broad test-suite rewrite.

## Delivery boundary

No S4 production change requires redeployment: O:/winnt/system32 remains the
verified S3 eight-file set. The original failed r001 and the isolated r002 remain
alongside the passing original-sequence r004 and diagnostic r005. Documentation,
links and staged diff review must pass before S4's verification/test-only P.
S1–S4 reach bounded implementation/automatic-verification delivery. T429 stays
open for owner manual acceptance; no next T or scope is admitted.
