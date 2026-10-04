# T429 S3 frame-local code-page query

## Question and baseline

S2 r023 attributes1725656us of1828662us total text commit to39120 calls
of GetConsoleOutputCP inside prepare_text_frame's per-cell conversion loop.
That is94.36% in one actual EDIT200 run, not a universal speed claim. Source
and measured API aggregation agree on the cause. The production S2 package
is123c0ad3e/status651efdd7b, immutable build/M0-T429/S2/r025/runtime;
published r027/r028 hashes and runtime gates define the reference.

## Bounded implementation and ownership

The existing project-added NTCON frame importer owns Unicode Console cell
projection. It receives the same backend-neutral text frame from either worker;
guest/worker bitmap metadata and execution remain at their existing owners.
Move only the host output-code-page query outside the per-character loop.
One frame uses one current value; the next frame queries again. No permanent
cache, new protocol, glyph reflow, frame filtering or producer suppression.
The original clone/convert/commit/error rollback order must remain intact.

## Verification plan

Use the selected MSVC14.43/SDK22621 x86 /MT cache, actual frame-import code,
real Console buffers and the unchanged Window renderer. Assert one query per
frame, refreshed CP between frames, exact Unicode cells/attributes/underline,
cursor state, and failed conversion leaves committed logical state unchanged.
Mocked query counters supplement, not replace, the real Console boundary.
Compare alternating baseline/candidate actual EDIT200 runs with the same
instrumentation, original guest, queue/order and final-paint assertions;
retain all samples and original measurement limits. No physical/RDP mouse
claim follows from an injected200-event workload. Run retained frontend and
handoff tests plus complete Console17/Window17/WOW, then coherent publication,
smoke, review and sequential commit/push. Other-session proposal edits remain
preserved and excluded. No test result is claimed at admission.

## Implementation and focused results

The production diff adds one local `code_page`, queries it after the existing
clone/allocation and uses it for the unchanged per-cell conversion. No original
mirror, worker, wire, ownership or receiver filtering change is involved.
An output-CP change during a copied frame now takes effect on the next frame,
not between individual cells of the same frame; this is the intended bounded
snapshot, not a permanent cache or an atomic snapshot of Windows output itself.

Incremental x86 build:
`cmd /c build/M0-T427/S2/r001/run-ninja-parallel.cmd ntcon.exe
console-frame-failure-test.exe console-channel-lifetime-test.exe
frontend-text-handoff-test.exe console-video-test.exe` succeeds. Existing C4996
library warnings and previously present fixture warnings remain; no new warning
is attributed to the production change. The new fixture compiles with `/W4 /WX`
via `pwsh -File tests/observation/build-frame-codepage-test.ps1
-BuildRoot build/M0-T429/S3/r003`. r001 records its initial narrowing-constant
warning, fixed without disabling warnings; r002 records missing fixture link
dependencies, supplied from the actual cache. Neither failed attempt is a pass.

r005 runs the real importer over real private Console buffers: 13476 checks,
zero failures. CP437/850 are changed between frames, all2240 cells and optional
underline are exact, last-row cursor/shape match, and repeated identical input
still executes2240 conversions plus one query. A test-only conversion failure
returns the actual importer error, releases the candidate and preserves source
handle/geometry with exact handle-count equality. This internal importer fixture
does not claim a complete IPC transaction; the separately retained public
channel lifetime, allocation/projection failure and font-handoff tests pass
their actual production commit/abort, EOF, barrier, ownership and resource
assertions. These tests are hidden/private-desktop, not physical/RDP input.

Measured candidate r004 uses the same test-only wrappers and normal link graph
as S2 r022. r007's first baseline warmup exits correctly but injects zero mouse
events: the selected T427 r049 observer has hash2D9DB9A2 and lacks the S1 EDIT
hook contract. This is a rejected comparison, not a product regression or a
sample silently retried. r008 restarts the fixed A/B/B/A/A/B experiment with
the exact S2 observer hash68243BF4 from T429 S1 r012, same S2 r022 measured
worker, guest/hook, S2 r025 package and unchanged assertions. All results must
be retained before making a performance claim.

## Matched measurement conclusion

r008 completes all six predefined samples, each with a disabled warmup followed
by one measured EDIT200 run. The common worker/observer/hook/guest/workload
identities are asserted by `summarize-frame-codepage-comparison.ps1`; its JSON
retains every successful phase sample, per-run median/P95/max and elapsed time.
For successful imported frames, all candidate CP sample counts equal1; baseline
counts equal2000 or2240. Rejected NOT_READY publications remain in the original
logs and are not accepted-frame latency samples. Queue conservation, no overflow,
EDIT menu completion, MEM content and final barrier/close assertions pass.

| Fixed sample | Variant | Commit median us | Transfer median us | Commit P95 us | Workload ms |
| --- | --- | --- | --- | --- | --- |
| 0 | Baseline | 100350 | 100580 | 150129 | 8462 |
| 1 | Candidate | 5806 | 6122 | 6925 | 7163 |
| 2 | Candidate | 5512 | 5786 | 7548 | 7988 |
| 3 | Baseline | 103630 | 104119 | 151878 | 8924 |
| 4 | Baseline | 88996 | 89296 | 109421 | 9318 |
| 5 | Candidate | 5378 | 5690 | 6957 | 7958 |

Median of the three run-level commit medians falls100350→5512us (~94.5%);
whole scripted workload medians8924→7958ms (~10.8%). The latter includes
observer readiness/exit work and is not a general launch-speed guarantee.
Published frame counts differ18/18/20 versus16/16/17 as guest/input timing
changes; no frames are deduplicated or suppressed by the candidate, and counts
are not assumed identical. Only the number of host queries per explicit frame
is reduced. Frontend CP aggregate median falls~84–97ms to40–44us; the unchanged
conversion, cloning and projection still take several milliseconds. This is
causal evidence for the admitted cost, not justification for further speculative
transport, input or guest changes. There is no matched physical SoftPC or RDP
claim.

## Product delivery and review

`Invoke-ProductVerification.ps1 -RuntimeRoot build/M0-T429/S3/r006/runtime
-BuildCache build/M0-T427/S2/r001 -Observer
build/M0-T427/S4/r049/console-startup-observer.exe -WindowObserver
build/M0-T427/S4/r049/worker-window-snapshot.exe -LogRoot
build/M0-T429/S3/r009 -Suite Product -GuestFixture build/M0-T425/S9/r008/G7.COM
-WowBaselineRoots build/M0-T429/S2/r026` passes. Exact retained gates:
WOW67201ms, Console17 74088ms, Window17 73320ms, total219979ms. WINMINE enters
its retained window frontier; SOL/WRITE retain their known deeper/frontier
limits. Timeout-shaped observation reports are not GUI usability acceptance.
All34 exit/output/order cases and the eight-file input manifest agree.
The explicit console-video staging/dispatcher unit also passes.

r010 backs up all eight deployed files, publishes the coherent r006 package
to O:/winnt/system32, and checks all hashes; only NTCON differs from S2 r025.
NTCON SHA256 is B91528B002F05347636138421541D703CF8252EC1B7AAF8D2837D5BFD627AA9C.
Recovery is build/M0-T429/S3/r010/recovery. Its first publication invocation
incorrectly looked for DLLs at the cache root and stops before touching the
destination; the corrected check compares every artifact to the actual r009
tested manifest, and the changed EXE to its link product. r011's published
Console/Window empty/MEM/EDIT/native-zero smoke and all eight hashes pass.
The diagnostic/instrumented binaries were not published. Guest files and
SYSTEM.INI are unchanged.

Final diff review separately checks the production importer, real-buffer
fixture, cache-derived link script and comparison parser: no permanent cache,
dedup, original mirror change, new control/transport or resource owner; native
capture already obtains its CP at the capture boundary rather than per cell.
The negative conversion injection supplements public-channel rollback/EOF
tests, not a replacement for them. Report counters use explicit query versus
frame-byte units. Existing registered S2 normal/diagnostic CCPU objects and
native close/lifecycle implementation are unchanged inputs, not reimplemented
or claimed as newly repaired. Documentation governance, relative links and
diff checks must pass before the reviewed S3 P. Other-session proposal
chronology remains excluded. S3 delivers this bounded optimization; S4 still
owns the final integration/limits audit and T429 awaits owner acceptance.
