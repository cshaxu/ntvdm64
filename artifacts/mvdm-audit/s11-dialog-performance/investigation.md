# S11 WOW dialog performance investigation

The user reports severe multi-window slowdown across SOL, Notepad, Program Manager and Minesweeper, while their primary windows remain responsive. The cross-program cause is not yet established.

The exact final candidate package bound by `no-trace-package-manifest.json`, with application tracing disabled, took 6234, 4844, 4907 and 4718 ms to show SOL Options and 5219, 5047, 5484 and 5344 ms to close it and recover the parent. See `no-trace-probe.txt` and `no-trace-result.json`.

A separate, nonpublishable instrumented package (`borrow-profile-package-manifest.json`) measured both rounds in `borrow-profile-probe.txt`. Its thread-local counters in `borrow-profile-counters.txt` show up to 1440 graph calls, about 400 graph rows per call, and 12838 ms cumulative graph traversal compared with 328 ms pin transfer and 62 ms publish. The counters are cumulative for the process, include startup and both dialogs, and may include reentrant work; they are not a wall-clock breakdown of a single dialog. The diagnostic modifies timing and is not a release candidate.

`wow_user_borrow_client_graph` expands relations from borrowed client state, and each edge calls `retain_window_generation`, which scans scope borrow records. `graph_row_index` separately linearly searches captured rows. Repeated graph traversal and search are evidenced performance leads, not proof that either particular search dominates. Do not delete external windows, bypass lifetime or weaken the approved snapshot contract to make the probe fast. Establish operation-level cost and compare identical noninstrumented builds before accepting any fix.

An earlier profiling run failed parent recovery; see `profile-result.json` and `profile-probe.txt`. This is not superseded by the later two-round pass, nor attributed solely to performance. None of this evidence demonstrates a fix or covers all user-reported applications.

Further controlled diagnostic runs split relation lookup from graph indexing. In one two-round SOL run, at 1440 graph calls, cumulative graph processing was 15761 ms; relation calls consumed 10895 ms and row-index searches 203 ms. These are diagnostic cumulative times, may include nesting, and are not independent wall-clock totals. Another diagnostic collected 884736 scope lookups with 353428200 visited records, taking 1133 ms cumulative scan time; it failed close/parent recovery. These counters identify excessive repeated work but show the scope-list scan alone does not explain all measured relation cost.

The host page-domain `window_borrow_link` also walks a singly linked list. A separately instrumented NTVDM recorded 262144 lookups and 53114406 visited records in 156 ms; this run failed to observe the dialog within the probe deadline. Its instrumentation and failure mean that result cannot establish a complete timing budget. Both diagnostic failures are retained, not counted as application passes. Preserve exact generation pinning and all failure handling when evaluating reductions in redundant work.

A relationship-level diagnostic (`relation-parts-counters.txt`) at 622592 non-null relations reported cumulative pin 1136 ms, relationship borrow 1848 ms, source release 811 ms, full-view reads 1884 ms, and retained-generation lookup 1861 ms. These buckets include all preceding calls in the same instrumented thread, cannot be added as non-overlapping end-to-end timings, and the run failed parent recovery. This confirms that no single measured piece explains the entire delay.

A separate ephemeral, transaction-local exact-generation cache experiment (`cache-candidate.diff`) reused already validated nodes within one graph pass and fell back to the original route on misses. It remains an isolated experiment: its first SOL Options took 5531 ms and close/parent recovery failed (`cache-candidate-result.json`). The result neither demonstrates a meaningful speedup nor distinguishes a candidate regression from the previously observed intermittent baseline failure. Do not merge or publish it without stronger evidence.

The independent message-only PID-bound matrix probe (`tests/observation/wow_multi_dialog_probe.c`) runs copied guest executables in an isolated package and measures menu-to-dialog and close/parent recovery without trace logging. On the exact baseline identified by `no-trace-package-manifest.json`, Notepad Find appeared in 1812 ms and recovered in 750 ms; Winmine Custom appeared in 2875 ms and recovered in 3266 ms. Each application passed that single interaction, not a complete functional regression. Program Manager did not yield a matching owned application window/menu in the isolated environment; its probe is a non-pass, not evidence about New Group or a reason to modify installed guest files. Inspect the retained per-app result, observer and probe records. Original O:\w31 files were only read and copied.

A second isolated experiment skipped `wow_window_words_prepare_resource_state` only when the same exact generation had just been pinned and its full view read successfully in the same graph transaction and a client state already existed. Its first SOL Options still took 6109 ms and close/parent recovery failed (`redundant-candidate-result.json`). This neither establishes a meaningful improvement nor establishes that the candidate caused the already-observed intermittent failure. It was not applied to product source. The next candidate must address redundant whole-graph work while preserving callbacks, native returns, full identities, nested scopes and the approved external snapshot semantics.

A node-ownership diagnostic (`nodes-counters.txt`) recorded graphs with 6–16 local and 250 external nodes, followed later by graphs with 5–16 local and 29 external nodes. This supports the hypothesis that the current reachable desktop graph can dominate the workload; the different counts are observations from different calls/time, not a controlled desktop-size experiment. The diagnostic passed two SOL rounds (`nodes-result.json`), with visible/close times of 4938/2453 and 1938/2203 ms. It is an instrumented package and does not prove an optimization. The final message-only matrix probe was independently rebuilt and rerun for Notepad: 1922 ms show, 797 ms close/recovery (`notepad-recheck-result.json`).

The original client consumes direct retained WND relations, so simply omitting external nodes or leaving dangling pointers would break the accepted semantics. The relevant execution boundaries are: original thunk entry owns a scoped borrow (`wow_user_thunk_scope.c`); native hooks enter nested borrows and refresh payloads (`wow_hook_callback.c`); native returns refresh the page-domain projection, suspended client graph, and menus (`wow_user_task_lifecycle.c`); graph validation itself can start from a particular retained WND (`wow_window_client_state_bindings.c`). `page_domain_replace_window_links` performs transactional graph publication and retirement. Any cross-call reuse must demonstrate correct invalidation for every one of those boundaries, exact pinned-generation retention across nested scopes, and handle reuse, failure rollback and deferred cleanup. A cache based only on a previously seen HWND, or on a single callback flag, is therefore not an acceptable fix.

Program Manager startup attribution: the PID-bound probe recorded a real WOWExec error, `Cannot find file ...PROGMAN.EXE (or one of its components)`, with the worker alive. Supplying isolated copies of the original INI/group files did not change the failure. Moving the executable copy from the isolated Windows root into a subdirectory also did not change the failure; therefore the root-path redirect is not established as its cause. Read-only NE inspection finds imports KERNEL, GDI, USER, KEYBOARD and SHELL. Original `wkman.c:3780` documents an intentional redirection for exact Windows-root PROGMAN and states that the Windows 3.1 program manager has shell/group compatibility conflicts under WOW; that comment does not prove why this copied executable failed outside that exact path. The real New Group dialog and its timing remain unverified. Source guest files were not modified.

Correction and resolution of the Program Manager probe: placing guest executables in `guest-fixture` produced the same file/component error for Notepad, which had passed at the short root path. The long directory is not a valid DOS 8.3 path in this setup. With the guest copy in a short `GUEST` directory, isolated Program Manager reached `Program Group Properties`: show 5265 ms, close/recover 4844 ms (`progman-short-result.json`). Its copied INI/group files were isolated and did not point to original group paths. The same final probe and short layout passed Notepad at 1390/531 ms and Winmine at 891/688 ms (`notepad-short-result.json`, `winmine-short-result.json`). The differing earlier times and changing desktop node counts mean these single runs are not stable benchmark comparisons. Original WOW code does contain exact Windows-root PROGMAN redirection, but the long-path failure is explained by the probe layout and not proven to be that redirection. Hashes of the probe source and read-only guest inputs are in `multi-dialog-input-hashes.json`.

An isolated relation-target fast-path experiment used an already held exact generation's client identity only inside a graph transaction, with the original complete-view path retained for cold targets and the public relation API. Its first SOL Options was not observed within the existing ten-second dialog probe deadline (`target-candidate-result.json`). This is a non-pass and does not prove a crash, semantic equivalence or improvement. The experimental code remains outside production and its pointer/lifetime and failure semantics have not been accepted.

The SOL observer now supports an opt-in, diagnostic-only `DialogWaitMs`, keeping the existing 10000 ms default. With a 30000 ms observation, the previously non-passing fast-path candidate completed one SOL interaction in 3453 ms show / 6032 ms close, while a subsequent unmodified baseline completed in 4454 / 6734 ms. A single sequential pair on a changing desktop is not a controlled performance comparison and does not prove either improvement or semantic equivalence. The candidate remains outside product source; retained-window and failure-path behavior still requires proof. The earlier ten-second non-pass is not reclassified as a crash.

A single-process SOL desktop-scaling experiment (`external-scaling-result.json`) completed its first interaction at 3484 ms show / 5516 ms close. The probe then created 150 hidden top-level windows in its own process. The next dialog appeared after 7750 ms, but close/parent recovery failed the then-existing 2000 ms responsiveness wait, so the post-removal round was not reached. The probe removed the windows in its cleanup. This is suggestive, not yet a complete controlled A/B/A result. Diagnostic runs with explicitly longer `DialogWaitMs` now use that same bound for the parent WM_NULL check; the default 10000 ms test retains its original 2000 ms parent bound.

A same-process, single-session A/B/A diagnostic completed with exact baseline package, no application trace logging, and three SOL Options interactions. In A1, no probe windows: 3844 ms show / 6609 ms close and parent response. The probe then created 150 hidden top-level windows in its own process: B measured 8657 / 7453 ms. The probe destroyed exactly those owned windows before A2: 5563 / 6156 ms. The receipt and timing log are `external-scaling-r2-result.json` and `external-scaling-r2-probe.txt`; probe hashes are `external-scaling-input-hashes.json`. Every interaction completed within diagnostic bounds, but this is not a product performance pass: diagnostic 30 s bounds deliberately exceed the existing default. It supports external desktop population as an important contributor; a single ordered A/B/A trial does not isolate warmup, scheduling or other desktop changes and does not establish it as the only contributor. All added windows were probe-owned and removed before the last round.

Source-boundary audit for a future graph reuse implementation: `graph-boundary-index.txt`, `graph-direct-link-writes.txt` and `page-domain-mutation-index.txt` retain the exact current call sites. A borrow scope lives around an original thunk or nested callback; native return invokes both projection and client-graph publication, while `wow_window_client_validate_window` can independently request a fresh graph. `wow_window_words_publish_client_graph` validates all rows before writing relationships, and existing source also writes links during cleanup and state attachment. Therefore a scope-local `already refreshed` bit keyed only by HWND is insufficient: the same scope can contain nested calls and native return publication, and cleanup may alter live host relations. The checked source searches are a starting index, not a proof of complete invalidation coverage. Safe reuse must either derive validity from the existing committed projection owner and cover every changing field and retirement/reuse path, or avoid cross-boundary reuse. Existing failure-atomicity, native-mutation, nested-scope and retained-reader fixtures must pass unchanged. No production cache or additional state authority was introduced in this audit.

An explicitly unsafe measurement-only build (`ceiling-unsafe.diff`) skipped a repeat client-graph traversal when the root client WND existed, without maintaining graph lifetime or proving freshness. It completed one isolated SOL interaction at 2938 ms show / 2484 ms close (`ceiling-result.json`). This is neither a correct candidate nor a controlled comparison; the remaining seconds indicate that skipping this particular entry alone is insufficient. Never deploy or integrate this bypass.

A second, deliberately unsafe upper-bound experiment also omitted the client-graph publication on native return. SOL Options was not observed within the 30 s diagnostic bound (`ceiling2-result.json`); it is a non-pass and the precise failure is not established. This bypass discards the required suspended-reader update and is rejected regardless of the timing. It is not proof that the native-return publication is intrinsically slow or that any production crash was caused by it. The experimental build is not a candidate.

A SOL diagnostic counted graph calls and reset its comparison marker on nested borrow entry, native-call begin, and native projection refresh. At 768 calls, 265 followed another call in the same unbroken marked scope; 167 also used the same non-null binding pointer. See `scope-same-counters.txt` and `scope-same-result.json`. This measures an upper bound on a narrow possible reuse opportunity, not proven safe cache hits: it does not cover every page-domain mutation, cleanup, error, or reference-lifetime transition, and pointer equality alone is not generation validation. The matching result cannot justify skipping the graph without a complete invalidation and pinning contract.

The current source-object lifecycle baseline passed four focused fixtures (`graph-lifecycle-baseline/result.json`): `graph-snapshot`, `window-borrow`, `native-nested`, and `native-creation`. The graph and window fixtures include mutation, retained readers, failure atomicity and handle retirement/reuse; native creation also exercises nested callbacks, rejection and veto. These verify the fixture assertions with current built objects, not real application performance or a future optimization. The package and product source are unchanged by this run.

An isolated candidate adds a call-local direct-mapped index for existing exact `(window, backing)` scoped samples, retaining the scope list as owner and falling back to the existing scan on collisions. It is not production code (`index-candidate.diff`). A SOL A/B/A interaction failed before the first dialog appeared within the 30 s diagnostic bound (`index-candidate-result.json`); this is not proved to be a crash or candidate-specific, but it is a real non-pass. The current production lifecycle test script was copied into a build-owned diagnostic wrapper; after selecting and refreshing normal objects it substitutes exactly one `wow_user_borrow_scope.obj` with the candidate object before assembling the test library. `index-lifecycle/result.json`, the four full case logs, and `input-hashes.json` bind the substitution: graph-snapshot, window-borrow, native-nested and native-creation fixtures passed. This validates only their assertions and does not repair or waive the failing real SOL observation. No candidate was merged or published.

The isolated indexed-candidate lifecycle fixtures all passed (`index-lifecycle/result.json`), but a traced real SOL run (`index-trace/result.json`) failed to show Options; its trace recorded dialog creation returning 5023 and a prior `WindowRejected` from a different thread. Another run with a diagnostic NTVDM that records the first page-domain borrow-context rejection passed in 3781 ms show / 6016 ms close; it also recorded a `WindowRejected` 5023 on a different thread but no `BorrowContextReject` marker (`index-context/result.json`). Thus the rejected message alone is not sufficient to explain the dialog failure, the first failing predicate remains unproven, and the diagnostic worker is different from the original candidate worker. These observations do not establish the index candidate's equivalence or performance. Retain the real non-pass and do not publish the candidate.

The isolated creation-enrollment diagnostic logs each of the five assignments to `creation_scope.failure` at the original failure site, preserving the captured error, and uses a diagnostic NTVDM that logs first borrow-context rejection. Four SOL Options rounds completed; no `CreationClaimFailure` or `BorrowContextReject` was recorded (`index-failure-diagnostic/result.json`). This is additional successful evidence for that diagnostic package, not a root-cause explanation for the earlier 5023 or a performance pass. The helper and worker differ from the non-passing package, so their results are not interchangeable.

The non-instrumented indexed candidate completed a second A/B/A run (`index-scaling-r2-result.json`): no probe windows 3640/6250 ms, 150 hidden external windows 8937/8344 ms, then removed 5547/6531 ms (show/close). The corresponding earlier baseline A/B/A was 3844/6609, 8657/7453, 5563/6156 ms. These are not interleaved randomized runs and cannot establish small differences; they do show no convincing improvement and remain several seconds. Together with the earlier candidate non-pass, the extra temporary index does not justify integration. It stays outside product source; pursuing whole-graph work remains necessary.

A measurement-only same-scope/same-binding reuse experiment invalidated on borrow entry, native-call begin and projection refresh, but did not cover all possible graph mutations and was never a production candidate. In the controlled A/B/A probe (`scope-reuse-experiment/result.json`), its first SOL interaction completed at 3141/6172 ms; after 150 probe-owned windows were added, the worker exited before the second dialog appeared. The diagnostic cleanup removed its own windows. This is a real non-pass; the exact failing instruction and whether the earlier known intermittent baseline issue contributed remain unproven. The source diffs and exact package manifest are retained for audit. Reuse must not be merged without a complete state owner/invalidation model and lifecycle evidence.

A separate traced A/B/A run of the same incomplete-invalidation scope-reuse experiment completed all three rounds (`scope-reuse-trace/result.json`), with probe timings 3281/5719, 6594/6578 and 3453/4391 ms. Its unrelated thread recorded `WindowRejected` 5023 while normal task retirement completed; it did not reproduce the previous worker exit and supplies no cause for it. Tracing changes timing, baseline comparisons are not randomized, and the cache still lacks required invalidation and retirement proof. The experiment remains unsafe and is not merged or published.

A non-production combined graph-transaction experiment indexes existing pinned sampled generations, bypasses duplicate target view reads only on exact backing hits, and avoids reconstructing client state when it already exists after the node view was validated. Its four selected-object lifecycle fixtures passed (`combined-candidate/lifecycle-result.json`). In one diagnostic A/B/A run (`combined-candidate/scaling-result.json`), show/close times were 2687/4016 ms, 4860/4812 ms with 150 probe-owned external windows, and 2922/3344 ms after removal. The earlier baseline was 3844/6609, 8657/7453 and 5563/6156 ms respectively; these ordered runs were not interleaved or randomized and do not prove a stable magnitude. The candidate still takes seconds and does not resolve the complete performance problem. It also defers full validation of an indexed edge target to that node's subsequent processing. Graph commit remains after traversal, but the relative error precedence under simultaneous malformed target and allocation failure, exact alias lifetime under reentry, and earlier intermittent SOL failure require further proof. It is not merged or published.

A batched-source-pin variant of the previous combined candidate pins each exact source generation once for its four relation reads, releases in `__finally`, and preserves the public single-relation route. The four selected-object fixtures passed (`batched-candidate/lifecycle-result.json`). The isolated real SOL scaling probe did not observe the first Options dialog within 30 s and did not reach the injected-window phase (`batched-candidate/scaling-result.json`). This is a non-pass, with no valid timing comparison; it does not establish whether the candidate or the previously observed intermittent baseline failure caused it. Further exact failure attribution and independent failure-injection checks are required. The candidate is neither integrated nor published.

A separately sealed batched-candidate diagnostic captured a specific creation non-pass (`batched-failure-diagnostic/windows.txt`, result in same directory). During Options child creation, `mvdm_softpc_wow_page_domain_borrow_window` rejected its captured context at stage 4: active domain and session owner matched, while the contemporaneous sampled CR0 had paging disabled (`0x10`). `CreationClaimFailure stage=4` then recorded `wow_window_client_create_state` failure 5023, and the child `WindowCreate` failed. The CR3 and directory values printed as zero are unevaluated fields after the stage-4 short circuit, not measurements. This establishes the proximate failure for this diagnostic run, not why the CPU was in that mode or whether all other historical failures share the cause. `modesw.c` uses `leave_protected` on DOSX real-mode transitions and `reenter_protected` on protected-mode transitions; no evidence yet identifies which transition led to this callback. The diagnostic worker and DLL are not production and the result does not prove a performance fix.

A separately sealed mode-sequence diagnostic logged page-domain enter/leave/reentry into the same window trace as creation failures (`context-sequence/windows.txt`). Four SOL Options rounds completed and no `BorrowContextReject` or `CreationClaimFailure` marker was observed (`context-sequence/result.json`), so this run does not explain the previous failure. The trace records repeated successful page-domain transitions and distinct host thread IDs. Source trace shows `session_thread_current()` returns a thread-local session pointer, not the OS thread ID; equal session pointers alone do not prove exclusive CCPU execution. `modesw.c` explicitly leaves the WOW protected domain for real-mode transitions and reenters on protected-mode transitions, while `nt_bop.c` enters the domain for the WOW BOP. Determining whether the failed callback overlapped a legitimate real-mode excursion or an incorrect ownership handoff still requires a failure with the transition trace. No change to guest state or production runtime was made.

Creation-boundary source audit: `wow_window_creation_forward_internal` calls `wow_window_creation_enroll_internal`, which enters the USER runtime SRW lock and can call `wow_window_client_create_state`. The latter can reach page-domain APIs that require the CCPU protected paging context. The explicit `creation_hook` forwards directly; the separate `wow_window_creation_observer` deliberately describes itself as metadata-only and does not call `wow_task_callback_enter`. By contrast, guest hook delivery (`wow_hook_callback.c`) enters the task callback scope before guest-facing work. The native call boundary may release the USER lock and schedule the original task before native USER callbacks. These facts expose a plausible missing execution-ownership boundary, but the recorded stage-4 failure alone does not establish that another task was executing, identify which hook path claimed the window, or prove that simply acquiring task execution is safe: `CallNextHookEx` may synchronously reenter another task and must retain original hook ordering without holding a lock or execution token across an unsafe callout. A correct fix must identify the actual callback route and prove its entry, callout, nested reentry, failure unwind and exact prior CPU state; force-setting CR0/CR3 would corrupt legitimate DOSX mode and is not acceptable.

A temporary explicit-creation experiment acquired `wow_task_callback_enter` only around `claim_explicit` enrollment and released it in `__finally` before the next native hook. It did not change guest or production source and also contained the earlier unaccepted batched graph candidate. Selected-object lifecycle fixtures failed `graph-snapshot`, `window-borrow`, and `native-creation` at `implicit-creation-failure-menu-retirement` with 5023, while `native-nested` passed (`creation-owner-candidate/lifecycle-result.json` and fixture logs). Baseline and the batched-only candidate passed these four fixtures. The result contradicts the adequacy of simply adding that task scope; it does not pinpoint whether scope restoration, callback ordering, or candidate interaction caused the failure. This candidate is rejected and must not be published.

The previously recorded page-domain failure also shows a key temporal distinction: the initial stage-4 sample recorded CR0 `00000010`, while the subsequent `BorrowFailure` record in the same diagnostic call reported `80000011` and CR3 `00900000`. CCPU's `CCPU_CR` is a process-global array (`c_main.c`), while `session_thread_current()` returns a thread-local session pointer that can refer to the same session from multiple threads. This supports investigating execution ownership around the callback; the two non-atomic samples alone do not prove which thread changed CPU state or license writing CR0.

The first creation-task candidate unconditionally invoked the scheduler. The failing `verify_native_menu_entry` fixture explicitly creates a resource-only thread with `ptdb` set but no `pwpi` and no `TIF_16BIT`; `wow_task_callback_enter` rejects that incomplete scheduling state. Original `queue.c::xxxInitTask` sets `TIF_16BIT` on real WOW tasks, and the existing native-call path already distinguishes real tasks from resource-only carriers with this flag. An isolated second candidate invokes the task callback scope only for `TIF_16BIT` during explicit `claim_explicit` enrollment, leaving it before `CallNextHookEx`; resource-only carriers keep their existing path. Four selected-object lifecycle fixtures passed, including prior menu rollback, and four real SOL Options interactions completed under a separately instrumented NTVDM (`creation-owner2-candidate/lifecycle-result.json`, `sol-result.json`). These do not prove that the real TIF branch was actually reached, that execution was relinquished correctly at downstream hook reentry, or that the previous paging-off failure is resolved. The candidate also includes the unaccepted batched graph experiment. It remains isolated and nonpublishable.

A diagnostic-only branch witness (`creation-owner-proof/result.json`, `windows.txt`) records stage 7 before task callback entry and stage 8 after its leave for the `TIF_16BIT` creation path. In one SOL interaction, 18 stage-7 and 18 stage-8 records occurred, with no stage 1–6 failure or `BorrowContextReject`. This establishes that the candidate branch was actually exercised and its instrumented entries returned in that observation; it does not prove all nested/interleaved pairings, all native callout order, absence of the earlier race or equivalent behavior under failures. The same run used the batched candidate and instrumented worker, took 2828/3453 ms, and is not a product performance acceptance. The underlying task-ownership candidate remains isolated.

The isolated ownership-plus-batched candidate also completed an A/B/A SOL run with 150 probe-owned hidden windows: show/close 2938/3625 ms, 4813/4312 ms, and 3015/3454 ms after removal. The trace contains 46 recorded task-scope entries and 46 exits and no recorded stage 1–6 failure or paging-context rejection. See creation-owner2-stress for the receipt, traces and hashes. This instrumentation and candidate combination do not isolate the ownership change, establish nested ordering, prove the intermittent failure fixed, or meet a product performance acceptance. The four selected-object fixtures initialize no TIF_16BIT flag in their native-creation main path, so their success alone does not exercise the added real-task branch; a deterministic branch and rollback test remains necessary.

A selected-object native-creation fixture was rerun with the unmodified production borrow-scope object and a diagnostic creation-owner object only. A temporary copy of the fixture sets TIF_16BIT around create, nested create, rejected create and CBT veto, then checks original task ownership and USER lock restoration. It reports WOW_NATIVE_CREATION_BINDING errors=0 and fixture-exit 0; the trace records five task-scope entry markers and five exit markers. Inputs and output are in creation-owner-isolated-fixture. This covers the already-owned task path and demonstrates the real-task branch independently of the batched graph optimization. It does not yet cover a suspended task, cross-task hook reentry, exception-unwind injection or the reported paging-off interleaving, and is not a production fix or performance acceptance.

A fault-injected creation-unwind variant used the same isolated creation candidate with production borrow object, set TIF_16BIT during the exception case, and reported WOW_CREATION_UNWIND errors=0 with restored task owner, hooks, scope, lock and successful retry. Evidence is in creation-owner-isolated-unwind. This covers the injected terminal exception for an already-owned task; suspended-owner and cross-task callout cases remain unproven.

Further lock audit invalidates acceptance of the isolated creation-owner candidate: claim_explicit is normally called under wow_user_runtime exclusive ownership, but wow_task_callback_enter unconditionally releases that lock before returning. The candidate immediately performs enrollment while the enclosing enroll_internal_locked still assumes exclusivity, and restores the lock only in wow_task_callback_leave. Therefore the earlier passing fixture did not assert the required lock during the body; its success is insufficient. The candidate must not be integrated. Any revised candidate must preserve the entry lock across enrollment, release it appropriately for task-scope exit, and test both lock states, failures, nested callouts and exact restoration.

A revised isolated candidate restores the caller's USER exclusive lock after task-scope entry, releases it before task-scope leave, and lets the existing callback leave restore the original lock. With production graph traversal, both the TIF_16BIT already-owned native-creation fixture and the injected terminal-unwind variant passed (creation-owner-lock-candidate). Diagnostic trace had five entry and five exit markers and no lost-lock marker in the ordinary fixture. This is still a candidate: it does not establish suspended-task scheduling, cross-task nested hook ordering, or whether acquiring a task at this point matches the original behavior under all producer routes. No product source, guest, deployment or release changed.

A separate graph-row-only experiment uses a transaction-local direct-mapped index with exact binding comparison and the original linear lookup on collisions; it does not skip graph nodes, view validation, lifetime operations or publication. Four selected-object lifecycle fixtures passed (graph-snapshot, window-borrow, native-nested, native-creation). A separately sealed SOL A/B/A probe showed 4016/6563 ms in A1, 8516/7547 ms with 150 hidden external windows, then worker exit before the third dialog; receipt, trace and exact manifest are in graph-row-cache. This is a non-pass and cannot establish its cause or equivalence; the first two timings do not demonstrate useful improvement compared with prior baseline and are not randomized. The experiment is not integrated or released.

The graph-row-cache SOL non-pass ends with TaskRetirement stage=publish-native-return status=1400 (invalid window handle), after the second successful dialog and creation of the third dialog's child windows. The observer reports launcher exit 0x42b. These are observed terminal markers only: without the failing publication's exact handle, its ownership and earlier state, they do not show whether this is the previously observed paging-context failure, a native-window lifetime race, or an effect of the row-cache candidate.

A separate diagnostic linked the unmodified graph path with failure-only logging around the three native-return publication steps, without the graph-row candidate. Six SOL Options rounds completed; no PublicationFailure was observed. A1 was 5312/8282 ms, 150 external windows 11547/12688 ms, A2 7140/8016 ms; remaining rounds also took several seconds. The normal task retirement logged enter/complete status 0. The result, probe, trace, source and package manifest are in publish-failure-diagnostic. This does not reproduce or explain the preceding status-1400 exit and cannot establish that any candidate caused or fixed it.

The failure-only publication diagnostic was also linked with the isolated graph-row cache and run for six SOL rounds. All six completed, task retirement was normal, and no PublicationFailure was recorded (publish-failure-row-diagnostic). Timings varied from 2.25 to 7.05 seconds, so this pass neither explains the prior status-1400 non-pass nor proves a stable performance improvement against the separate baseline run. Diagnostic package and source are preserved; no candidate is integrated.

A further baseline diagnostic logs the failed exact native root handle, its observed liveness, process and thread, and the native-return publication stage only when those operations fail. Six SOL rounds completed normally; no root or publication failure was captured. Exact diagnostic sources, timings, trace, receipt and package identity are retained in publish-failure-root-diagnostic. This result cannot explain the previous intermittent 1400 or establish a fix. Root liveness is sampled after a failure and remains an observation, not proof of atomic native history.

A same-scope graph mutation regression creates and removes a native child while holding the original window-state alias, explicitly republishes each native change, and checks that both child relations update without replacing the alias. It is now part of the existing window-borrow fixture and its marker gate. A selected-object baseline direct link passed the full fixture and new check; a separately linked deliberately unsafe graph-skip variant failed exactly at the created-child relation. The first version placed the probe inside a different ownership test and disturbed its later cleanup assertions; that invalid test arrangement and output are retained as direct.log, while independent-fixture direct2.log and unsafe.log are the valid counterexample. Inputs and hashes are in same-scope-mutation. It verifies this local create/destroy contract, not comprehensive invalidation, concurrency, guest behavior or performance improvement.

A failure-neutral instrumentation experiment counted page_domain_class_allocation linear scans in the isolated NTVDM worker during three SOL A/B/A rounds. The probe completed all rounds; one thread reached 2,752,512 calls and 1,253,810,463 visited class records, with 4,381 ms cumulative QPC-measured lookup time. These counters include startup and all three rounds, QPC/log instrumentation overhead, and are not a single-dialog wall-time attribution. The exact diagnostic source, probe, trace and package manifest are in class-profile. They identify a substantial hot path, not proof that replacing its owner or adding an independent cache is safe.

An isolated candidate adds an intrusive 1024-bucket lookup index to the existing class allocation owner; the original class list remains ownership and enumeration authority. Insertion, successful retirement and domain teardown maintain the index, with no allocation on lookup. The standard window-borrow fixture including same-scope mutation passed when linked against the candidate worker object. A second candidate with 16 buckets also passed the existing 32-class live/retired collection fixture, ensuring at least one collision by pigeonhole, though it is not exhaustive collision-order, failure-injection or reuse coverage. A sealed instrumented SOL A/B/A run passed, timing 2375/2735, 3015/3563 with 150 external windows, 2203/2532 ms. Its diagnostic recorded 983,040 class lookups, 1,283,131 index nodes and 63 ms cumulative time; a separately timed baseline recorded 2,752,512 lookups, 1,253,810,463 nodes and 4381 ms. Counts and desktop state differed and QPC instrumentation adds overhead; this is not a randomized end-to-end speed claim. Exact candidate source, hashes, maps, logs and package identity are in class-index-candidate. It remains isolated and unpublished pending explicit index-integrity, allocation/free failure, reuse, cross-task and wider product checks.

The class-index candidate now checks index/list consistency before freeing a class; a missing index entry fails while preserving both the allocation and list, and a failed physical free leaves both intact. The new class failure fixture is added to the existing window-reclaim gate: it creates 48 classes, finds three colliding modulo 16, injects a physical-free failure for a middle entry, verifies its exact object and neighbors remain, then verifies successful retirement, an actually reused backing address and final cleanup. The unmodified owner, 1024-bucket candidate and forced-16-bucket candidate all passed the same real-object reclaim and same-scope graph tests; all observed actual address reuse. Exact logs, maps and hashes are in class-index-failure. This does not by itself prove cross-thread synchronization, full domain restart, allocation-failure rollback or whole-product performance; the candidate remains isolated and unpublished.

The checked-index worker in a separately sealed package completed PID-bound immutable-guest Notepad Find (1312/516 ms show/close), Winmine Custom (2219/2078 ms) and isolated Win3.1 Program Manager Group Properties (5875/4985 ms). Matrix receipt and exact inputs are in class-index-checked-matrix. These are single observations on a changing desktop, not stable comparisons or performance acceptance; in particular Progman remains visibly slow. Guest images and original group data were not modified. The index does not resolve all overhead and remains a candidate.

The checked-index object additionally passed selected-object cross-task native-nested callbacks, external-process snapshot/retirement, and native-creation tests; exact maps, logs and hashes are in class-index-cross. The formal failure fixture now also verifies class allocation ownership and any present derived index are empty after session disposal. Baseline, checked 1024-bucket and forced-16-bucket variants all passed the actual-free failure, retry, observed backing reuse and teardown assertions. This is evidence for those scopes only: it does not prove all calls are correctly serialized with guest CPU mode switches or eliminate the previously observed intermittent SOL error.

The checked index has now been applied to the project-owned page-domain source without diagnostic instrumentation. Rebuilt selected-object tests compiled the actual worktree source through the existing free-failure wrapper, the committed baseline source in isolation, and an otherwise identical forced-16-bucket worktree variant; all three passed collision, failed-free retention, successful free, actual address reuse and session-teardown empty assertions, plus the existing window/menu failure tests. Evidence and hashes are in class-index-production-source. This is not full product build, proof of every cross-task synchronization condition or a release. Original CCPU/guest sources are unchanged by this adapter optimization.

The uninstrumented actual-source class-index worker passed Notepad, Winmine and Progman one-round dialog probes, but its first SOL Options run failed: the worker exited before the first modal, after WindowCreate reported error=5023 and TaskRetirement reported publish-native-return status=5023. The failure receipt, full window trace, observer and exact source/object/package hashes are in class-index-clean-failure. This resembles earlier baseline/candidate errors but does not establish causality. The worktree index is unaccepted and must not be released based on prior instrumented passes.

The clean actual-source worker passed the Notepad (1407/562 ms), Winmine (2078/1625 ms), and Progman (5859/4859 ms) single-run dialog probes; receipts and exact package manifest are in class-index-clean-matrix. These elapsed samples are not stable comparisons, nor do they erase the separate SOL non-pass.

Independent source inspection identifies another potential scaling cost, distinct from the measured class lookup: `wow_user_borrow_client_graph` traverses each discovered window and up to four relationships; `graph_row_index` linearly searches all rows for each target, and the subsequent pin-transfer loop searches active borrow lists for each row. Thus larger graphs can require nested scans. This is a source-level complexity finding, not a measurement of its contribution to dialog latency. A transaction-local row-cache experiment did not demonstrate a reliable speedup and had an unattributed status-1400 non-pass, so it remains rejected as a release candidate. The same-scope mutation regression demonstrates why omitting graph refresh is not an acceptable shortcut.

The separately constructed scheduled-task creation fixture is not a passing ownership test: the direct process terminated with `0xC000041D` after the last trace entered creation claim stage 7. Its manually assembled scheduler state may be incomplete; this outcome does not establish a product failure or explain the intermittent 5023. A diagnostic-only provider with decision-point scheduler traces has been compiled and linked to the same fixture for follow-up after the active product gate; it has not yet run. The separate fixture runner is still waiting on its Ninja child and must not be counted as a pass.

The diagnostic fixture was subsequently started after the product gate finished. Its trace records the candidate's callback reaching `xxxDirectedYield` with no scheduled task, no execution owner and the callback marked ready; no return from that call was recorded. Earlier callbacks in the same fixture retained their owner and returned. The fixture terminated with `0xC000041D`, so this is a non-pass, not a continuing wait. This identifies the last recorded call, not why the original scheduler failed to return or whether a real guest can reach the same state. Preserve its trace and resolve the synthetic task-list/event setup before using it as product evidence.

The clean class-index ten-image package completed the `Full` selected product gates with exit zero (`class-index-clean/full-run3/result.json`; sealed `ntvdm.exe` SHA-256 `C818455D975D5034EFD751F4696810AE8F5EE9510C77E64EA2696442A75A5531`). This establishes those gate results for that exact package; it does not cover or erase the separately captured clean SOL Options error 5023. The candidate remains unpublished pending that failure's attribution and the outstanding ownership review.

The first synthetic scheduler setup left both `hEventWowExecClient` and the ordinary `xxxDirectedYield` wait argument null. The original scheduler compares them directly and can then take the WOWEXEC branch, which dereferences the fixture's uninitialized `pcti`; this is a concrete fixture defect, not proof of the precise exception address. A revised test creates a distinct WOWEXEC event and keeps the normal task event; it reaches `xxxDirectedYield` and is still running at the time recorded. Its outcome must be assessed independently of the invalid first run. Inputs, trace and link log are under `build/t473-s11/scheduler-diagnostic`.

The revised but still incomplete synthetic fixture had no trace progress for over ten minutes. Its exact process tree and executable were checked, then the owned parent was stopped. Windows denied a direct stop of its child, which disappeared when the parent exited; both PIDs were subsequently absent. This remains a non-pass and no product process was stopped.

Further inspection found that the same synthetic fixture also has no `thread.host` callbacks. `xxxSleepTask` reaches host wait, lock and death-check operations through that pointer, whereas production `wow_user_task_lifecycle_init` installs `host_ops` and its task context. Therefore even the event-corrected synthetic fixture cannot prove suspended-task scheduling or validate the ownership candidate. A useful follow-up must use the real initialized task lifecycle, or a complete, explicitly verified test host, rather than filling isolated fields in the resource-only owner fixture.

The actual lifecycle fixture now exercises a callback after `wow_user_native_call_begin` has parked a fully initialized task. It checks the callback acquires the task, releases the runtime lock to its caller, then restores the parked execution frame, owner state and LastError. The selected-object native-nested fixture exited zero and emitted `WOW_SUSPENDED_CALLBACK_SCOPE errors=0`; the normal native-nested verifier now requires that marker. Source, output and hashes are retained in `real-suspended-callback`. This proves that controlled generic task-scope path, not that CBT creation uses it correctly, that a concurrent CCPU mode switch caused SOL's failure, or that all hook ordering is preserved.

A separate actual-lifecycle creation experiment linked the isolated lock-preserving CBT candidate and registered a test class under the initialized lifecycle. Its creation trace recorded candidate claim entry and exit, but the fixture returned 1: guest class publication failed with 5023 and enrollment failed with 87. This lifecycle fixture explicitly has no active guest page domain, so its class-publication failure is expected at this boundary; the individual enrollment cause has not been established. It is not a passing creation test or evidence that the candidate repairs SOL. Exact temporary source, trace, output, object identity and hashes are in `real-task-creation-nonpass`. A conclusive test must combine real initialized lifecycle with a valid active page domain and the proper native class/client-state setup.

The existing original-modal fixture supplies that missing combination: real CCPU page domain, registered lifecycle and original modal dialog entry. Relinked with the isolated lock-preserving CBT candidate and checked class-index object, it exited zero after six modal create/end/retire cycles (ANSI and Unicode, each with full, zero-extended and sign-extended handle forms). Trace records six candidate entries, six exits and no claim failure. The exact fixture output, trace and binary/input hashes are in `real-domain-modal-candidate`. This is a selected-object test using controlled task callbacks, not a full guest interaction or proof of the prior SOL paging-context failure. The stage markers do not show whether a parked task was reacquired; nested cross-task hook ordering and failure unwind still require direct evidence before adopting this candidate.

A follow-up linked a failure-neutral scheduler trace into that same real-domain modal fixture and exited zero. All six creation claims recorded a parked state (no scheduled task or owner, execution not ready), then `xxxDirectedYield` returned with the same task scheduled and owning execution before enrollment; all six claim scopes returned. The whole trace includes 36 directed-yield entries and 36 returns across creation and other modal callbacks. Inputs, exact object hashes and ordered trace are in `real-domain-modal-scheduler`. The trace establishes the suspended-task transition in this controlled single-task modal fixture; it does not prove cross-task nesting, every exception path, or that the intermittent live SOL 5023 has this cause.

A real shared-worker integration reused the pre-existing 16-round CDLPROBE guest from `build/t459-s2/common-dialog-guest-r2` after verifying its recorded source and binary SHA-256; the temporary runner copies that unchanged binary and does not invoke NASM. With the separately sealed `class-index-context` diagnostic package, WINMINE and the sibling ran in worker PID 19196; the sibling reported 16 closed dialogs and one additional dialog retired at task exit, zero guest errors, and task 1737h retired once. WINMINE remained live, passed flag/reset/open observations, closed and retired as task 17B7h. The trace contains no recorded borrow, claim or publication rejection. The exact runner, guest identity, package manifest, event trace and reports are in `two-task-prebuilt`. This tests the indexed class owner and diagnostic provider with two guest tasks, not the isolated creation-lock candidate, nor a synchronously nested cross-task hook, and does not erase the earlier SOL failure.

The same prebuilt guest and two-task probe were then run with a newly sealed diagnostic package adding the isolated lock-preserving creation object. This run did not pass: the sibling completed seven reported create/close rounds, then its launcher returned 1067; the worker failed native-return publication at stage 1 with error 1400 while the recorded payload contained WINMINE's HWND, and the WINMINE launcher also returned 1067. Four candidate creation claims entered and exited before the failure; no stage 1–6 claim rejection was recorded. Full partial guest output, trace, identities and manifest are in `two-task-owner-nonpass`. The package actually linked `publish-failure-diagnostic/wow_user_task_lifecycle.obj`, not the separately prepared scheduler trace object; the input hashes were corrected after checking the link command. The unmodified-creation diagnostic run passed immediately before this one, but these are separate executions with potentially timing-sensitive behavior, and earlier non-candidate experiments also saw status 1400. The failure establishes a concrete non-pass, not that the candidate caused it or that the recorded HWND was invalid at the decision point. Do not publish the candidate pending attribution.

A follow-up diagnostic augmented the same candidate with failure-only identification of each native window-capture step and mandatory-root failure, preserving the native error. The prebuilt two-task probe passed all 16 sibling rounds, both task exits and surviving WINMINE interaction. Four candidate creation scopes entered and exited. Thirteen individual capture failures reported stage 1 / 1400 with the HWND no longer live; the graph transaction tolerated those observations and logged no mandatory-root or native-return publication failure. These samples are not the prior 1400 failure and cannot explain it. The trace, diagnostic source, hashes and sealed package identity are in `two-task-graph-diagnostic`; added logging changes timing and this package is not publishable.

One further run of the exact graph diagnostic package returned 1067 after a native-return publication failure at stage 1, now with 5023 and WINMINE's HWND in the payload. No graph capture or mandatory-root failure was logged before that terminal result. This is a separate observed failure, not evidence that the prior 1400 had the same cause. The partial guest report, trace and package identity are retained in `two-task-graph-nonpass`; failure can occur before the instrumented native capture or in an uninstrumented graph step. A failure-only page-domain context diagnostic was prepared to distinguish those paths without changing guest.

The page-domain-context diagnostic also produced a 5023 native-return non-pass in the shared-worker run, this time with no returned payload. Neither a context rejection nor a native capture failure was logged. Evidence, binary hashes and diagnostic source are in `two-task-domain-nonpass`. The context diagnostic covers `page_domain_menu_context` but not all page-domain checks, and does not identify the failed graph stage. A further diagnostic adds failure-only graph-phase markers; it is not a behavior fix.

That graph-phase diagnostic reproduced 5023 and identified phase 2: the count-only `mvdm_softpc_wow_page_domain_copy_window_view` succeeded, while the subsequent populated copy failed before native capture. No page-domain context rejection was logged. The exact trace and package are in `two-task-phase-nonpass`. Source inspection identifies explicit invalid-state checks when the number of current WND borrow records differs from the occupied slot count; other error paths are not excluded solely by an error number. A new failure-only view diagnostic records the counts and mismatched slots to establish the specific invariant before selecting a fix.

The view-diagnostic run instead reproduced the 1400 branch. Mandatory root index 1 was full HWND `02E9048A`, and its first `GetWindowThreadProcessId` failed with 1400; subsequent `IsWindow` and owner queries also found it absent. It was distinct from the surviving WINMINE payload HWND `01270AB2`. The graph aborted while recapturing mandatory roots and native-return publication then terminated the worker. Root index 1 is from the project window table rather than the desktop or optional payload; the trace does not establish its original owner, whether it died before collection, or why retirement had not removed it. No view-copy mismatch was captured in this 1400 run. Evidence is in `two-task-view-nonpass`.

A second run of that identical view-diagnostic package independently hit the same kind of 1400 failure at mandatory root index 5, with a different dead HWND and no view-count failure (`two-task-view-nonpass2`). These are two separate 1400 witnesses, not explanations for the distinct 5023 view-copy failure. Further failure-only instrumentation must capture the stale entry's owner and lifecycle before changing the graph's mandatory-root contract.

An additional diagnostic logs dead mandatory-root table entries, their owner and backing identity only after a graph failure. Two shared-worker runs using that exact sealed package both passed all 16 sibling dialog rounds and normal cleanup, so no failure-point owner metadata or view mismatch was captured. Both outcomes, source and hashes are in `two-task-root-diagnostic`. These passes do not erase the two preceding failures; the producer of the stale mandatory root remains unproved. No runtime fallback or guest change was introduced.

The next diagnostic recorded actual window publication and terminal-observer retirement events. Its first run passed; its second hit the distinct view-copy 5023: the occupied-slot scan counted 23 while the current-borrow scan copied 22. The failure trace shows task thread 22236 completing two native window retirements immediately before task thread 32088 reports that mismatch, and a third retirement begins immediately afterwards. A later diagnostic slot scan found no persistent missing record. This is direct evidence of interleaved retirement and view failure, but not yet an atomic proof that one of those exact retirements occurred between the count and borrow scans. The exact trace, diagnostic source, partial guest report and hashes are in `two-task-lifecycle-nonpass`. Source trace identifies a potential shared-state boundary: `wow_window_words_terminal` retires page-domain/table state under its publication lock, while graph refresh uses the USER runtime lock; the terminal observer explicitly does not acquire guest execution. Determine the complete lock/callback ordering and verify the implicated interleaving before changing either owner.

The ownership diagnostic captured a direct interleaving for 1400 (`two-task-ownership-nonpass`). Thread 19904 owned the USER runtime lock and the scheduled execution while projecting; thread 11012 had `userHeld=0`, observed thread 19904 as USER/execution owner, and successfully retired three table/window records. One retired full HWND had already been copied into the first thread's mandatory root array. After retirement its native capture failed; the failure-time table and page-domain slot were both empty for that exact generation. Thus the root was valid in the copied root set but had been retired concurrently, rather than a permanently orphaned table entry. This establishes a synchronization gap between the two owners for this witness; it does not by itself prove the earlier 5023 has the same exact interleaving or justify silently ignoring a required root. The callback and lock-order requirements still govern the repair.

The original-modal fixture now also installs a separate CBT veto after actual registration and before the normal six-cycle matrix. The vetoed creation returned -1 without an application initialization callback, the native HWND and associated WND binding were retired, the page-domain handle no longer matched it, and the task execution owner and USER lock were restored. The same task then completed all six normal modal cycles and cleanup. The exact worktree fixture, isolated CBT candidate and checked class-index object passed this selected-object test; `OriginalModalOnly` now requires its marker. See `real-modal-veto`. This covers a downstream CBT veto, not arbitrary exceptions or cross-task reentry.

The same worktree veto fixture also passed with the unmodified creation provider and the checked class-index object (`real-modal-veto/baseline-stderr.txt`, with distinct binary and provider hashes). This is a compatibility control, not a reproduction of the intermittent execution-ownership failure; both variants passing does not establish that the candidate fixes it.

The same real-page-domain original-modal gate now creates a nested modal dialog synchronously from a downstream CBT hook while the outer creation is still active. It checks the hook callout runs with the USER lock released, inner and outer dialog results, callback count, task execution owner and lock restoration, and then repeats the veto and six ordinary dialog cycles. Both isolated candidate and unmodified creation provider passed with exit zero; exact worktree inputs, logs and candidate trace are in `real-modal-nested-hook`. This covers same-thread nested hook reentry, not callbacks crossing different WOW task threads or causality of the live 5023.

The existing registered-owner real-dialog failure matrix was rebuilt from current fixture and page-domain source with the isolated CBT candidate. It exited zero: ten recorded claim entries/exits, two injected publication failures, two downstream vetoes, callback admission rejection, a captured exception and original finally restoration, nested dialog callback, resource ownership and physical cleanup all passed. Exact output, trace and hashes are in `real-dialog-fault-candidate`. The fault-injection wrappers deliberately alter the corresponding boundaries; these are controlled selected-object results, not a live SOL fix or cross-task scheduling proof.

The same rebuilt failure-matrix objects passed against the unmodified creation provider as a control (`real-dialog-fault-candidate/baseline-stderr.txt` and `baseline-hashes.json`). Both variants preserve the tested paths; neither result identifies the source of the intermittent live failure.

The separate scheduled-fixture runner's Ninja child produced no output since 01:50:17; at 02:00:54 its exact process tree was inspected and had no compiler children. The owned stale Ninja and runner were stopped and waited for after exceeding the active packet's ten-minute no-progress budget. This is an interrupted test, not a build or test pass. The event-corrected direct diagnostic is a separate process and was not stopped by that cleanup.

The immediate unmodified baseline A/B/A comparison following the clean-index SOL failure completed all three rounds (class-index-clean-baseline). The earlier baseline also had independent 5023 non-passes, so this single subsequent pass does not establish that the new index caused the failure; conversely it cannot exonerate it. Candidate still cannot be released without attribution and full gates.

A separately sealed diagnostic package retained the actual worktree class index, added decision-point page-context logging to NTVDM and failure-only creation/publication logging to WOW32, then ran six SOL rounds. All six completed with no BorrowContextReject, BorrowFailure, CreationClaimFailure or PublicationFailure; the normal retirement was recorded. Source, trace, probe, package manifest and hashes are in class-index-context. The diagnostic changes image layout/timing and cannot erase or explain the clean candidate's earlier 5023 non-pass. No retry is counted as proof of repair.

A separate diagnostic-only terminal-serialization candidate acquires the task execution scope and USER lock for shared window retirement after the original terminal callbacks, retaining the publication lock for the mutation. Its first two-task prebuilt-guest run passed 16 common-dialog rounds, sibling retirement, and surviving WINMINE interaction (`build/t473-s11/terminal-serialized-probe`). The ownership trace recorded 70 retirement entries, all with the USER lock held. This is one timing-sensitive pass with additional creation and tracing instrumentation, not proof of causality, complete performance recovery, or safe lock ordering. The native terminal observer currently discards the retirement result; therefore failure and exit handling require further design and deterministic cross-thread validation before any production change or release.

A selected-object real registered-owner dialog lifecycle and failure-injection fixture was rebuilt with the isolated terminal-serialization object substituted into its provider archive. The fixture exited zero with four rollback cases, callback rejection, captured exception, nested dialog, owner/cache restoration and physical cleanup; the link map resolves `wow_window_words_terminal` from that archive (`build/t473-s11/terminal-serialized-fixture`). Candidate object SHA-256 AF9FFD9185DE246C2CAE9994F5CFB04081489BB40C681393E2A29D6C00FF7FF9; provider archive SHA-256 BCBD1B579C2FE3A03173AAE21B9F44CED32EE7E0705685C4DEA1FA96531BFE79. This single-thread and controlled-failure fixture does not establish cross-thread lock safety or remedy the observer's ignored failure result.

A subsequent source-boundary review shows why the passing terminal-only diagnostic cannot be promoted as a complete fix. `wow_window_words_terminal` first writes `WFDESTROYED` into client WND state before its original finalizer and before the diagnostic's newly acquired task scope; this write is not covered by that scope. `detach_matching` is a separate caller of the same `wow_user_window_retire` operation, used by failed creation; publication and native-adoption cancellation also mutate the same table via distinct paths. Their actual caller ownership must be verified rather than assumed. `wow_user_window_refresh_graph` invokes `wow_window_words_collect_retired` before collecting roots, and collection reenters terminal processing; acquiring/yielding a task scope from that route needs explicit analysis of the current runtime owner and lock order. In addition, the observer ignores a FALSE terminal result, while deferred collection only sees bindings already marked terminal_pending. A failure before that state is established cannot be described as safely deferred. The existing positive fixture does not cover these edges. These are source-level uncovered paths and a concrete candidate limitation, not additional reproduced failures.

Call-chain refinement: the sole production `wow_window_words_detach` call occurs if acquiring the just-attached binding fails, before `wow_window_words_publish_owner`; in that route there is no table owner yet, so this call does not itself demonstrate a second production table-retirement race. Native subclass adoption enters the USER runtime lock before publication and holds it through `cancel_native_adoption` in the failure `finally`; its table withdrawal is already serialized against graph traversal on that path. Those facts narrow the unresolved issue, but do not establish that every creation enrollment has execution ownership: explicit CBT enrollment calls `enroll_created_window` through `claim_explicit`, and that boundary is the subject of the separate unmerged creation-scope experiment. Deferred collection occurs from borrow-scope leave as well as graph refresh and task cleanup, so its caller may already own runtime state; the chosen terminal protocol must preserve that prior state rather than blindly acquiring a nonrecursive lock.

An isolated first-write diagnostic now also enters the task cleanup scope and USER lock before setting `WFDESTROYED` and reading FNID, then releases/restores them before calling the unchanged original finalizers; the existing later retirement scope remains. Its selected-object registered-owner dialog and injected lifecycle fixture exited zero, including nested finalization, four rollback modes and callback exception handling (`build/t473-s11/terminal-first-write-fixture`). Link map identifies the substituted provider object; object SHA-256 0EB1C6936A2E583AD90302B1B43208626EE76C550C9DC4A5FFAFD53A1396E8AE, provider archive SHA-256 45BC8CC2CCE4BB2143D949EC7847C83917B501CED272927C38F3514B40827A3A. This is a diagnostic single-thread control only. Its new initial critical section does not yet protect task/USER unlock against an exception within that section; scope-admission rejection, deferred collection from a different current task, cross-thread scheduling and actual product performance remain unverified. It is not suitable for production or release.

A further isolated variant wraps the first `WFDESTROYED` critical section in SEH `finally`, releasing the publication lock, USER lock and task scope while preserving the entry error. It compiles and the selected-object registered-owner dialog fixture again exits zero (`build/t473-s11/terminal-finally-fixture`; object SHA-256 8DA23793580BFA2E3BEF48DF820892E93DB2FF6D0899E86B69EA5794E0FA6ACF). The existing fixture's captured exception exercises a different callback boundary: it does not inject a fault inside the new critical section, prove scope-admission failure recovery, or demonstrate cross-thread progress. This remains an isolated diagnostic, not a production modification.

A diagnostic-only build injected exception 0xe0473001 immediately after acquiring the first terminal publication lock, inside the new SEH `finally`. A real registered-owner/page-domain fixture caught that fault twice: once with the USER lock initially released and once with it already held. In each case the fixture checked restoration of the prior USER lock state and task execution owner, then reentered the relevant locks and completed terminal cleanup; the remaining lifecycle matrix also passed. Output and exact artifact hashes are recorded in `terminal-critical-fault`, while diagnostic sources and linked fixture remain in the task build directory. This tests the injected boundary on one registered task; it does not establish cross-task lock ordering, admission failure handling, or live-application recovery. The injection and candidate are not product code.

The SEH-protected initial-write diagnostic was linked into a ten-image diagnostic package with the same CBT creation and object-ownership instrumentation used in the earlier terminal experiment. Two independent same-worker immutable-guest runs each passed 16 COMMDLG rounds, sibling task exit, WINMINE interaction and launcher completion (`terminal-first-write-dual-task`). Each trace records 70 window retirements with the USER lock held. Package and object hashes are retained in that evidence directory; the complete traces and logs remain in the corresponding build directories. These are real cross-task runs but not a deterministic forced overlap: diagnostic timing and the additional creation candidate are confounders, and they do not prove failure handling, no deadlock, or an end-to-end performance repair.

The task-exit call chain narrows the cleanup admission contract: `wow_user_task_prepare_exit` sets `resource_cleanup_live=TRUE` around `destroy_terminal_windows`, which sets `TIF_INCLEANUP` and calls the shared pending-window collector before and after native destruction. Therefore the newly attempted cleanup scope is admitted during that explicit window. This does not make all collector calls equivalent: `wow_user_borrow_leave` and graph refresh also collect the process-wide object table from the currently bound task, and pending entries can belong to another task. The candidate uses the current binding's cleanup scope, not the pending object's recorded owner; whether the collector reaches finalization or only already-finalized bookkeeping for such an entry must be established before integration. Scope denial outside `resource_cleanup_live` still returns FALSE before setting pending, and the terminal observer currently ignores it. No untested retry or ownership transfer is assumed.

A focused controlled-owner experiment adapted the existing real-page-domain deferred-terminal fixture: the deferred table entry names a distinct synthetic `wow_task_order_thread` with the same process while the current runtime binding remains the original thread. It forces the first withdrawal to fail by leaving protected mode, verifies that the native property is removed while the table remains, destroys the HWND, restores the page domain, and retries through the process-wide collector under the distinct current binding. The candidate completed the deferred withdrawal and physical release with zero fixture errors (`terminal-deferred-other-owner`). This supports the source finding that an already-pending, no-longer-enrolled binding skips original control/dialog finalizers; it does not demonstrate a second registered task, actual concurrent execution, or guest callbacks, and the synthetic owner must not be misrepresented as such.

A real-page-domain registered-owner fixture now demonstrates the isolated terminal-scope candidate's denied-admission defect. With a published STATIC HWND, the test sets the current task's cleanup flag while `resource_cleanup_live` is false, then invokes terminal directly: the candidate returns FALSE with ERROR_INVALID_STATE before clearing the native property or setting terminal_pending. The process-wide collector returns success while the table entry remains, because it only selects terminal_pending bindings. The fixture restores the task flag and explicitly retires/destroys the test window, and the rest of its lifecycle matrix passes (`terminal-admission-reject`). This is an intentionally controlled direct-entry test; it proves the candidate cannot represent a rejected scope as pending, not that this state has been reproduced on a live NCDESTROY. The production observer discards the terminal return, so any solution must decide this boundary without falsely reporting successful retirement or bypassing original finalization.

An isolated observer-boundary candidate distinguishes the two FALSE results: if the binding is still enrolled after terminal returns, it invokes the existing worker fail-fast ownership contract; if terminal has already removed the property and marked the entry pending, it retains ordinary deferred collection. A registered-owner/page-domain fixture with a synthetic terminal observer notification and deliberately denied cleanup admission terminated with STATUS_ASSERTION_FAILURE (0xc0000420), without returning through the observer; the same executable without injected denial completed its lifecycle fixture. A separate real-page-domain fixture forced withdrawal failure, sent a synthetic terminal observer notification, and confirmed the observer returned, pending storage remained and later retry physically retired it. Exact stderr and hashes are in `terminal-failfast-boundary`; tests use controlled conditions, not a naturally occurring task exit. This variant deliberately fails the whole worker on an impossible-to-safely-continue ownership break, consistent with the existing native-call fail-fast contract, but is still an unmerged experiment: two-task lock order, natural rejection reachability, original finalizer behavior and the user-visible failure consequences need review.

The failure-boundary diagnostic package passed an independent immutable-guest two-task run: 16 common-dialog rounds, sibling retirement and surviving WINMINE interaction all completed; 70 traced table retirements held the USER lock and no terminal-incomplete or native-return publication failure was logged. The same diagnostic package completed three no-trace SOL Options rounds: show 4047/3203/2468 ms and close/recovery 3516/3016/2782 ms. Output and hashes are in `terminal-failfast-live`. This is neither a randomized comparison nor a performance acceptance: interactions remain seconds long, creation and object tracing code are still linked, and a prior uninstrumented candidate failed intermittently. No natural scope-denial was witnessed. The isolated fail-fast boundary is not a replacement for proving correct task ownership and callback lock order.

A new sequential no-trace SOL control ran three rounds of the existing final-stage baseline immediately followed by three rounds of the clean class-index package on the same desktop. Baseline show times were 3672/3219/3062 ms and close/recovery 3281/3656/3407 ms; indexed show times were 3391/2610/2625 ms and close/recovery 3171/3015/3141 ms. Every round completed. The roughly 13%/10% difference in these small-sample means is suggestive, not a randomized or same-process causal estimate; later baseline and candidate runs can differ from desktop state or warmup, and the earlier clean candidate 5023 remains an unreconciled failure. Both still take seconds. Outputs and both package image hashes are under `paired-sol-current`.

A relationship-level diagnostic was run with the clean indexed NTVDM and the existing instrumented WOW32 provider, using unchanged guests. Both SOL rounds completed: show 3610/2906 ms, close 3406/2969 ms. At its last sampled 950272 non-null relations the thread-local cumulative buckets were pin 890 ms, relation 1451 ms, release 610 ms, full target-view read 1781 ms, and retain 1173 ms (`relation-parts-indexed`). GetTickCount64 granularity, reentrancy and the cumulative intervals prevent adding these figures or treating them as one dialog's wall time. The count and relative bucket magnitudes indicate the full-view work remains a material target even with the class index, but do not justify bypassing descriptor validation. The provider is instrumented and the package is diagnostic only.

A one-line isolated graph experiment skips `wow_window_words_prepare_resource_state` only when the immediately preceding full exact-generation node read succeeded and the binding already has client state; missing state still takes the original preparation route. It was linked together with the fail-fast/creation diagnostics and indexed worker, not applied to product source. Three no-trace SOL Options rounds completed with show 3359/2719/2657 ms and close/recovery 3032/2984/3000 ms (`validated-state-reuse`). This small, nonrandom comparison to the earlier fail-fast run (4047/3203/2468 and 3516/3016/2782) does not establish a meaningful performance improvement. A selected-object real-page-domain window-borrow/terminal-deferred fixture passed, but this is not a complete mutation/reentry/failure-equivalence proof: graph execution ownership across every caller and the possibility of mutation between node validation and state use remain to be checked. The experiment is not merged or published.

A further isolated graph experiment tightens the previous index: an edge may reuse a scoped sample only after that exact backing/binding completed a full node-view validation in the current graph traversal; preloaded or colliding samples default to unvalidated and retain the ordinary full-read path. It does not include the separate skip of resource-state preparation. The selected-object graph-snapshot and lifecycle fixtures passed, including existing injected failure and retained-generation checks. Three no-trace SOL rounds completed at show 3750/2328/2281 ms, close/recovery 2625/2579/2579 ms (`validated-edge-index`). The diagnostic package also contains the unreleased creation and terminal candidates; this is not a randomized causal comparison, does not yet prove the fast branch was exercised, and leaves reentry/invalidation and all product gates open. It remains nonproduction.

A separate counted build confirmed the validated-edge fast path is actually exercised: at the last 32768-edge sample, one thread recorded 983040 non-null edge attempts and 375902 exact-backing, previously-validated hits (38.2%). Both instrumented SOL rounds passed (show 3828/3140 ms, close 3422/3344 ms). The counters include startup and both rounds and exclude null relations; periodic file writes and other diagnostic objects perturb timing. Counts and input hashes are in `validated-edge-counted`. This demonstrates avoided full target reads on those hits, not a reliable end-to-end speedup, nor equivalence under mutation, collision, exceptional unwind, or cross-thread callback. The counted and uncounted variants remain isolated and unpublished.

Forced-collision control: the isolated graph index was rebuilt with exactly one bucket, so all distinct backing generations collide. Both the uncounted and separately counted variants passed the selected-object graph-snapshot fixture, including native mutation, retained reader, withdrawal/reuse and controlled terminal failure cases. The counted run recorded 1792 non-null edge attempts and zero fast-path hits at its last sample (`validated-edge-collision`), demonstrating that this fixture exercised the fallback under extreme collisions; the earlier separate real SOL counted run establishes that the normal-sized index does take its hit branch. This is not a proof for every nested mutation or for a naturally interleaved two-task callback. Neither variant is production code.

A diagnostic-only build sampled each real-TIF_16BIT graph invocation for current runtime lock and CSOwningThread, logging violations immediately and cumulative samples every 1024 checks. Two SOL rounds completed; at the last sample 1024 checks had zero violations (`graph-owner`). The first shared-worker probe was interrupted without a result receipt; its partial binary contains 16 created/closed common dialogs with zero reported guest errors but no final exit record, and its observer reports exit 6; it is a non-pass, with termination cause not established. A fresh same-package run completed all 16 rounds, sibling exit and surviving WINMINE checks. It emitted no graph-owner sample for the two-task run because the sampling threshold was not reached and no violation was logged; do not represent this as a positive per-task owner witness. Trace instrumentation changes timing and neither run proves all callers or cross-thread lock exclusion.

With sampling reduced to every 64 real-task graph invocations, a fresh same-worker two-task run completed all 16 common-dialog rounds and sibling retirement, and surviving WINMINE gameplay and launcher exit. Two distinct thread/owner pairs were witnessed in the same object table: one logged 128 and the other 64 graph checks, both with USER lock and current scheduled execution owner, and no violation log (`graph-owner-frequent`). These are cumulative lower-bound samples; they do not force concurrent graph traversal and retirement or prove other callers. The earlier interrupted run remains recorded as a non-pass. Package identity and complete guest report are in that evidence directory; the candidate is still not publishable.

The final withdrawal critical section in the isolated terminal candidate lacked
publication-lock unwind, independently of the previously tested initial WND
marking section. A follow-up adds a local acquired-lock flag and releases it
in the existing finally before restoring runtime/task ownership. An exception
is injected immediately after final withdrawal lock acquisition, before any
withdrawal mutation. The registered-task/real-page-domain fixture passes with
this repair (exit 0), for both initially held and initially released USER lock.
A paired negative control omits only the finally release: it exits 1 at the
two explicit TryAcquireSRWLockExclusive recovery assertions (fixture lines
1681 and 1701). Test-only code then releases the witnessed same-thread leaked
lock so the negative run can finish without a timeout. Both controls restore
task ownership; that alone was insufficient to detect the publication leak.
Input hashes, exits and assertion excerpts are retained in
[terminal-withdrawal-fault-results.json](terminal-withdrawal-fault-results.json).
Raw build/test inputs remain in build/t473-s11/terminal-withdrawal-fixed and
terminal-withdrawal-negative. These are isolated host fixtures, not product or
guest changes. They prove this pre-mutation exception boundary only; they do
not establish recovery after partially committed retirement or explain the
multi-window slowdown.

The source entry audit identifies three graph entry routes:
`wow_window_client_validate_window` enters/reuses USER runtime ownership before
`wow_user_borrow_window_state`; `wow_window_client_prepare_graph` does likewise
before refresh and graph projection; `publish_guest_return` reaches the graph
from initial, scheduled and native-return publication. Native return reacquires
the USER runtime lock and execution before publication. Actual lock operations
are the same runtime SRW lock in ntvdm-exe/wow/wow_user_runtime.c, not the
window binding's separate publication_lock. Resource-state preparation reads
and constructs a retained WND and does not fetch native window text; the text
capture belongs to the preceding native-observation publication. The graph
itself traverses all four relations, performs linear row and scoped-borrow
lookup, and transfers pins to the outermost borrow before final publication.
This explains repeated work amplification; it does not authorize persistent
cached graph validity across native callouts. The missing retirement lock
coordination remains relevant even if every graph caller holds its own lock.

An event-coordinated two-thread host fixture now distinguishes the shared-lock
boundary without probabilistic repetition. The main registered-task thread
holds the actual USER runtime SRW lock; a second resource-only thread binds to
that same runtime and enters window retirement. Test-only hooks rendezvous
immediately before runtime acquisition or before the first WND mutation. The
main thread checks its ownership, then releases the lock and the rendezvous.
Both mutation sections record the executing thread's runtime-lock ownership.
The current production terminal body fails two assertions: it reaches mutation
first (phase 2) and records both sections unheld (held=0, unheld=2; exit 1).
The exception-safe candidate reaches runtime acquisition first (phase 1) and
records both sections held (held=2, unheld=0; exit 0). Native property cleanup
completes in both runs. The test does not use scheduling delay to select the
interleaving; its finite wait is only a test watchdog. Exact inputs, receipts
and assertion excerpts are retained in
[terminal-schedule-results.json](terminal-schedule-results.json).
The second thread intentionally has no registered Win16 task and this row
uses property-only storage: it proves actual shared-lock exclusion, not
cross-task scheduler progress, copied-window withdrawal, or the full captured
stale-root race. No guest source or installed product changed.

The tempting runtime-lock-only simplification is not yet valid for copied
window retirement. The call chain is wow_user_window_retire ->
mvdm_softpc_wow_page_domain_retire_handle -> page_domain_withdraw_window.
The latter checks session ownership, paging enabled and CR3 before writing
shared copied WND/handle fields through c_sas_storedw; retained backing release
also uses the page-domain context and VdmFreeVirtualMemory. Unlike the
property-only scheduling row, this path requires a coherent CCPU execution
context. Finalizers already have their original task scopes, but that alone
does not cover the subsequent retirement. Do not replace the candidate's task
ownership with only a runtime lock based on this narrower passing fixture.
All current terminal callers also need result disposition: the terminal
observer, creation rollback and post-create failure currently ignore FALSE;
the pending collector propagates it. A new scope rejection before pending
registration cannot silently use the existing deferred-retirement path.

Natural-caller review exposed an idempotence regression in the isolated
serialization candidate: creation rollback and nested destruction can call
terminal again using a retained, already-completed binding. Applying task
scope admission before checking that completed state rejects the no-op with
5023 when INCLEANUP is set and resource_cleanup_live has closed. The original
terminal body accepts it. A monotonic no-longer-enrolled/non-pending check under
the existing publication lock now precedes task admission in the candidate;
it touches no copied guest state and preserves LastError. A paired registered
owner/page-domain test passes with the check (exit 0) and fails without it
(exit 1, rejected with 5023). This is a candidate regression, not evidence that
the original product's intermittent 5023 had that cause.

The idempotence contract is now in tests/mvdm-host/wow_page_domain_fixture.c,
with an explicit required marker in verify-wow-task-lifecycle.ps1 for the
original-modal/real-dialog modes. The formal dialog-lifecycle case linked the
selected current production objects and passed with exit 0 and
WOW_TERMINAL_RETIRED_NOOP errors=0 accepted=1 cleanup_scope_closed=1.
[terminal-noop-results.json](terminal-noop-results.json) records positive,
negative and formal input hashes and assertions. The first verifier launch
under Windows PowerShell could not load Get-FileHash and never ran the test;
the same formal command under the available PowerShell 7 host completed.
This adds a regression guard for existing behavior; it does not promote any
terminal serialization candidate or establish full task-exit acceptance.

The terminal candidate has been reduced from two task cleanup scopes to one
outer task scope with two short data-lock regions. The original control/dialog
finalizers still use their existing nested task/borrow scopes, with acquired
runtime/publication locks released across callouts. One finally releases any
held publication/runtime lock and then restores the caller's task scope, so
exceptions from either data section or the finalizers cannot bypass that outer
restoration. Completed retirement remains an early no-op. This is isolated
under build/t473-s11/terminal-single-scope; production source is unchanged.

The registered-owner real-dialog matrix, including the new no-op regression,
passes. Separate injection builds raise immediately inside the initial and
final data sections; both pass lock reacquisition, execution-owner restoration
and subsequent cleanup for initially held/unheld callers. A coherent diagnostic
ten-image package also passes the immutable same-worker two-task test: all 16
COMMDLG rounds, sibling exit with an outstanding final dialog, surviving
WINMINE gameplay and launcher completion. Retire-entry trace ownership and
exact fixture/package hashes are summarized in
[terminal-single-scope-results.json](terminal-single-scope-results.json).
This integration still includes the earlier CBT creation candidate and worker
ownership tracing; it is not a clean package performance comparison or a forced
two-real-task stale-root interleaving. Failed admission before pending
registration remains to be disposed consistently across all terminal callers;
the single-scope refactoring has not silently adopted the observer-only
fail-fast experiment or claimed the whole S11 closure.

The reviewed terminal group is now integrated in the worktree's project-owned
wow_window_words_binding.c, with no new mirror or guest changes. It retains one
execution scope and two runtime/publication-lock regions. A source-derived
needs_task predicate uses existing object/generation/resource fields; pure
property metadata does not require guest execution. A completed binding is an
early no-op. Failed admission while the binding remains enrolled invokes the
existing terminal native-return ownership contract in the common entry, so
observer and creation rollback cannot silently abandon it. Once property
removal has registered terminal_pending, FALSE still belongs to the existing
collector, with no added retry, queue, timer or success substitution.

The normal registered-owner test passes; deliberately rejected live-owner
admission through a direct terminal call exits with 0xc0000420 and never
returns through the bad-return marker. The pending page-context failure test
returns from the observer, retains table ownership, and later physically
reclaims the allocation after context recovery. Completed/no-task metadata
controls pass. The actual integrated source was then rebuilt using cached
Ninja/ccache inputs by the formal dialog-lifecycle verifier and passed all
required markers, including both newly formalized cleanup contracts.
[terminal-central-results.json](terminal-central-results.json) retains the
receipts and exact inputs. Earlier single-scope live guest acceptance predates
the centralized failure/metadata refinement; the integrated source still needs
its own full-package and cross-task acceptance. Nothing was published.

Adjacent retirement sweep: detach_matching has another call to
wow_user_window_retire. Its sole production caller is enroll_created_window's
failure to acquire newly attached metadata, before publish_owner is attempted;
that path has no newly published table owner to withdraw. detach_if has no
production caller. The retained APIs and their independent tests are not
silently redefined as finalizer entry points by this fix. The published-owner
failure path instead reaches rollback_enrollment -> terminal, now covered by
the common failure disposition.

The integrated worktree was rebuilt on 2026-10-29 using the normal cached
Ninja/ccache graphs: x86 and x64 product targets exited 0, and the worker graph
also rebuilt VDMREDIR/nthook32 successfully. No serial fallback was required.
The newly sealed ten-image package contains the production terminal repair and
class index, without the isolated creation-lock candidate or diagnostic worker.
The immutable same-worker sibling probe passes 16 common-dialog rounds, sibling
retirement with a final outstanding dialog, and surviving WINMINE flag, reset,
open-cell and close/launcher completion. Exact package hashes, source hashes and
behavior receipts are retained in terminal-current-product-results.json.
The selected Full suite was then started as owned PID 8888; it is not yet a
pass. This is not performance acceptance, a forced race schedule, or publication.
The first current-package Full process and its waiting performance runner disappeared before a final receipt, after modern EDIT passed and during cooked-console return. Neither process nor owned product peers remained at inspection. stderr was empty; cause is not established. This is a non-pass, not a product failure attribution. Console-17/Window-17 partial summaries remain evidence only for those cases. A same-package repeat uses a tracked tool session and outer exit receipt.
The tracked same-package Full repeat completed at 2026-10-29T15:37:10Z with outer exit 0 and PASS Full selected gates; identical 10-file package. It passed cooked return, lifecycle/front-end loss and nested-window handoff as well as the earlier product/control groups. The first interrupted run remains a non-pass of unknown cause. Full timings and exit are retained in terminal-current-product-results.json. Performance is being measured separately; this does not prove the multi-window latency resolved.
The clean current package failed the separate SOL Options acceptance despite
Full passing. The no-trace run exited before the first modal window (probe 7).
A same-package fault-capture run did not report a caught exception, but timed
out without a modal window (probe 8). Its window trace records successful dialog
and group creation followed by BUTTON child creation returning NULL with
error=5023, bindingError=5023, claimed=1, menu/id=0x12C, style=0x50030004.
The parent resumes WM_COMMAND and paint afterwards. This establishes a concrete
explicit-creation binding failure in that traced run; it does not attribute the
untraced worker exit to the same cause. The existing isolated creation-owner
candidate remains unmerged. Performance comparison is deferred until the clean
creation route is understood. The current edge-index candidate was relinked
against current production objects only, but has not been run or merged.

A failure-only current-source creation diagnostic added stage markers at the
five existing claim failure returns, without adding execution ownership. One
round and a subsequent three-round run completed; no failure marker was caught.
This does not invalidate the clean package's 5023 or worker-exit failures.
The previous lock-preserving creation-scope candidate was stripped of all
trace code and relinked with current production objects only (no edge index or
worker diagnostic). Its first standard three-round no-trace test showed the
first dialog in 5516 ms but failed close/parent recovery (probe 10); later rounds
were not reached. A separate one-round 30-second diagnostic observation showed
4406/7328 ms and completed; this changes the probe's response bound and is not
a substitute for the failed standard acceptance. No creation production change
was made. Source hashes and outputs are in creation-current-review-results.json.
The normal SOL recovery probe now reports separate dialog-presence, worker
wait/exit, parent WM_NULL result/error and elapsed time on failure; the original
10-second dialog and 2-second parent response bounds are unchanged. Recompiled
with this failure-only reporting, the isolated creation-owner package completed
three standard no-trace rounds (show 4438/6016/6047 ms; recover 7500/6953/6875 ms).
The same current objects plus only the transaction-local validated-edge index
completed three rounds (show 3609/5203/7406 ms; recover 5969/5906/5750 ms).
Recovery was shorter in this sequence, but show timing was mixed and latency
remains seconds. This nonrandom sequential comparison is not causal performance
acceptance and neither candidate is integrated. Exact observations and the
candidate manifest are in creation-edge-current-comparison.json. The earlier
standard recovery failure remains open; reporting improvements cannot erase it.
The clean explicit-creation task-scope repair is now in the project-owned
production source. The formal native-creation fixture exercises TIF_16BIT during
nested creation, rejection/veto and exception recovery and checks restored owner
and lock state; resource-only cases remain later in the same fixture. Cached
formal native-creation, CreationUnwind and registered dialog-lifecycle runs all
exit 0 against the actual integrated source (creation-integrated-results.json).
The earlier isolated unwind attempt failed before link because its temporary
runner replaced the creation object twice; the corrected candidate runner and
the unmodified formal runner both pass. This is not a product regression failure.
The prior ten-image Full result predates creation integration. Fresh coherent
package/live sibling acceptance and SOL correctness/performance remain required;
the edge-index optimization remains isolated and nothing new is published.
The actual integrated creation source was rebuilt through the ordinary cached x86/x64 graphs and sealed as a new ten-image package. Its immutable 16-round sibling retirement and surviving WINMINE flag/reset/open/close probe passes (creation-integrated-product-results.json). The new package Full suite is running in tracked session 82251; earlier Full receipts are not reused for this change. The SOL probe also now records actual worker exit status when failure precedes a modal window, without changing wait or acceptance semantics.

The creation-integrated package Full run completed with exit 0 at 2026-10-29T16:07:19.6928083Z and the identical-ten-file marker. This covers the integrated creation change, unlike prior receipts. Four-app no-trace modal probes now run sequentially on this same package; performance and the historical intermittent failures remain separate acceptance questions.

Same-package no-trace application matrix: Notepad Find show/recovery 1188/562 ms, WINMINE Custom 2328/2688 ms and Program Manager New 3218/2985 ms pass their functional probe. SOL round 1 shows/recovers in 5079/4593 ms; round 2 fails to show within the unchanged probe bound (exit 8). These are not performance passes. Complete results are retained in creation-integrated-app-results.json. A same-package traced three-round follow-up is collecting attribution; no product change or publication accompanies this run.

The same-package CaptureFault follow-up completed three SOL modal rounds without a recorded AV or NULL WindowCreate. It does not erase the no-debug second-round failure. Source review of capture_fault confirms a fixed ten-second debug-event collection before window enumeration; its 10000/10015-ms show readings include probe overhead and must not be used as application latency. No-trace timings remain the performance evidence. The failure attribution and graph hot-path work are still open; retained receipt: creation-integrated-sol-fault-result.json.

Added a nested-publication regression to the same-scope real-page-domain fixture: retain parent/child aliases, enter a nested scope, change the child ID and publish, verify both directions and the updated ID, leave nested scope, verify no stale rollback, then destroy/publish as before. Current graph-snapshot and the isolated validated-edge single-object replacement both pass. The formal window-borrow marker now also requires nested_publication=1. Exact results/hashes are in graph-nested-publication-results.json. This closes one nested invalidation case, not every concurrent interleaving or performance acceptance.
A same-package non-debug traced SOL run passes three rounds (show 5547/4343/4406 ms; recovery 4922/4719/4828 ms). It retains ordinary task scheduling but trace writes perturb timing, and it does not erase the failed untraced second round. Old creation-edge-current package input hashes do not match the latest creation-integrated product, so that old package must not be used as its causal performance control.

Fresh edge-integrated-product was linked from the current production graph with only wow_user_borrow_scope.obj replaced by the existing isolated validated-edge candidate. Staging input hashes now match the creation-integrated baseline for all nine other components; only WOW32 differs. A bounded edge/baseline/baseline/edge no-trace comparison (two SOL rounds each) runs sequentially in session 87984. Results must include all failures; the candidate remains isolated. Source review confirms the index is function-local, full node validation precedes a usable hit, exact backing must match, and replacing a colliding slot clears its validated flag. The added nested-publication fixture covers a subsequent traversal seeing a committed ID change and retaining it after nested unwind; it does not by itself prove all concurrent callers or end-to-end performance.

The edge/baseline/baseline/edge comparison completed all eight SOL modal rounds, all four process runs exit 0. Baseline show mean is 4929.75 ms and recovery mean 4707.25 ms; candidate show mean 3847.5 ms and recovery mean 3664 ms. This small same-input-nine-component, no-trace sequence supports a roughly 22% reduction in these operations, not a broad performance guarantee. Both remain unacceptably seconds-long; the earlier baseline intermittent failure remains recorded. The candidate has not been merged or published. Source identity, all per-round timings and raw probe reports are retained in edge-integrated-comparison.json.

Current-source native-return attribution: a diagnostic-only replacement of
wow_user_task_lifecycle.obj counts elapsed native_call_end time by actual caller
return address and maps it to the matching DLL link map. Two SOL rounds pass.
At the last 448-completion sample, leading sites include DefWindowProcA
(127 calls/1474 ms), previous-window-proc forwarding (75/1058), create_bound
(30/437), SendMessageW (32/419), BeginPaint (31/409), EndPaint (31/389) and
control-color return (28/357). This does not support geometry getters as the
main cost in this sample. Counts include startup; elapsed includes execution
reacquisition and may include nested work, so totals are not additive wall-time
components or pure graph timings. Source/map/package hashes and the complete
retained counters are in native-return-current-result.json.

The reviewed transaction-local validated-edge candidate is now in the worktree.
Its scope remains limited to one graph traversal, not cross-native-return
freshness reuse. The comment now states that the exact full view has already
been validated, rather than suggesting deferred validation. The formal
current-source four-case suite is running in session 35527; old Full receipts
must not be reused to certify this new integration.

The actual production-source indexed traversal passes all four formal cases: graph-snapshot, window-borrow, native-nested and native-creation (edge-formal-integrated-results.json). A fresh ordinary cached x86/x64/aux build, ten-image seal and immutable 16-round sibling acceptance now run in session 12041 under edge-production-product. This is not default publication; Full and application verification remain required for that freshly built package.

Fresh production edge integration rebuilt and sealed successfully; the immutable 16-round sibling retirement and surviving-task checks complete with exit 0 at 2026-10-29T16:34:36.6496923Z. Full is running against this exact package in tracked session 59773, with its own copied sealed-image test cache and fresh result directory. The receipt/manifest are edge-production-product-results.json; default release is unchanged.

Follow-up source review rejects an unproven geometry-getter-only shortcut:
those calls do use the common publication path, but the current callsite
profile does not make them dominant. Projection name capture also uses
InternalGetWindowText, not GetWindowText/WM_GETTEXT dispatch; a supposed
name-query recursion is therefore not established. No getter shortcut or
publication skip has been applied.

An isolated current-source diagnostic now separates native-return execution
restoration from native observation projection, copied client-graph publication
and menu publication. It preserves the original short-circuit failure order
and restores LastError after counters/logging. Its compiled ten-image package
is build/t473-s11/return-phase-profile/package; no application has run on it
while the production Full gate remains active. This diagnostic is excluded
from source integration and release.

The new production edge package Full completed with exit 0 at 2026-10-29T16:40:28.4167970Z and identical-ten-file verification. It covers the integrated transaction-local reuse, unlike the earlier creation-only receipt. Default publication remains unchanged because the broader multi-window performance and intermittent SOL observation remain open. Only after Full completed was the phase-separated diagnostic started, in tracked session recorded by the active tool call.

The phase-separated diagnostic completes two SOL rounds (4328/3906 and
3110/3312 ms show/recovery). Last emitted samples on its application thread:
restore 1920 calls/172 ms; projection 448/1265; client_graph 448/2716;
menus 448/701. Counts differ because restoration includes nonpublication
completions. Instrumentation, startup, nested timing and unsampled tails
prevent additive per-dialog wall-time claims. Nevertheless this sample directs
further work toward copied-state synchronization rather than changing original
WOW scheduling. Exact package/source hashes and raw counters are retained in
return-phase-current-result.json.

Source review confirms projection already commits live observed scalars before
the borrowed-client graph rereads committed WND/CLS storage and rebuilds links.
Reusing input observation rows would require exact committed backing matching,
retired-generation fallback, field-equivalence proof and all existing
failure/rollback rules; the native HWND alone is insufficient. No such bypass
or cross-call cache has been implemented. The current transaction-local index
remains the only newly integrated optimization.

The later current-source experiment skipped resource-state preparation only
when the same fully validated pinned node already had state. Graph-snapshot,
window-borrow and class-lifetime fixtures passed; an added controlled missing
class-name test proves existing state still rejects invalid data, then returns
the same alias after restoration. The four-app package nevertheless showed no
convincing end-to-end benefit: SOL round 2 closed its dialog but its live worker
failed the parent WM_NULL response (1460); WINMINE dialog was not observed;
Notepad and Program Manager passed, with Program Manager still seconds-long.
This is not enough to attribute either failure to the one-line change, given
prior baseline failures and uncontrolled desktop population. The preparation
skip was reverted, preserving the already Full-tested indexed graph code.
All candidate results are in validated-state-current-results.json; it was not
published. The new existing-state validation regression is retained.

The SOL probe now takes a one-shot stack/PC snapshot of threads belonging only
to its selected worker after an assertion has already failed. It suspends each
thread individually and resumes it in finally; no debugger is attached during
normal interaction. This is intended to distinguish a hot loop from a blocked
return at the actual failing boundary, without changing success timing or
acceptance deadlines. A three-round probe on the unchanged Full-tested edge
package is running in session 77492.

The pending unchanged-package SOL snapshot probe completed three rounds, all
passing (4625/3984, 3859/4312, 3750/4016 ms show/recovery). Therefore its new
failure-only stack branch was compiled but not exercised. The retained
existing-state guard passes after the speculative preparation skip was
reverted; use current-guard-and-sol-results.json. The first restricted-token
fixture returned CreateDesktop error 5; the established desktop-capable runner
then passed. That launch failure is not a product failure or waived gate.

A finer isolated graph diagnostic preserves control flow and records QPC
intervals around eight operations. Two SOL rounds pass (4625/3797 and
3625/3922 ms). The application's last sample contains 550587 nodes and
2202348 relation operations: pin 341 ms, full-view read 941 ms, retain 304 ms,
resource preparation 1463 ms, relations 7126 ms, row lookup 333 ms,
pin transfer 280 ms, publication 47 ms. These are cumulative startup plus
interaction samples, include instrumentation overhead, and are not a sum of
per-dialog latency. Exact input comparisons, counters and package identities
are retained in graph-phase-current-results.json.

Source follow-up finds that a relation's full successful target validation is
not marked in the transaction-local validation index; only subsequent node
processing marks it. A build-only candidate marks an exact retained target
after that successful full read. It leaves standalone relations, collisions,
node revalidation, failure cleanup and cross-call freshness unchanged. It is
not integrated or published; same-source application comparison and formal
lifecycle checks are required before accepting it.

The target-validation candidate's two rounds pass at 4157/6984 and
5937/7110 ms; the immediately following unchanged-package control passes at
5156/4828 and 3937/4454 ms. The other nine component inputs match. This small
ordered comparison demonstrates no useful improvement and does not establish
candidate-specific regression. The candidate remains build-only and is not
integrated. See edge-target-validation-comparison.json. Production source and
the default released package are unchanged by this experiment.

The current source review distinguishes external snapshot freshness from
traversal cost: project_graph_attempt already retains validated external
observations (ADAPTER-WOW-082), while refreshing local roots and explicit
payloads. Thus merely proposing external snapshots again does not fix the
current implementation. The downstream borrowed-client graph still expands
and validates their relationships on each publication and direct borrowed
state exposure. Any structural reduction must preserve exact backing identity,
retired generations and nested publication; a scope-level HWND-only cache is
still inadmissible. The measurements narrow the cost to that repeated closure,
not to an established original OpenNT scheduling defect.

The relation-only diagnostic passes one SOL round (5343/4110 ms). Its last
983039-call sample attributes 589 ms to source pinning, 1172 ms to relation
borrowing, 522 ms to source release, 145 ms to indexed-target release, 836 ms
to target full-view reads and 330 ms to target retention. These measurements
include instrumentation and startup, and do not establish independent
per-dialog totals. See relation-phase-current-results.json.

A current-source candidate extends the project-owned observation record with
output-only exact relation backing identities read from the same retained WND.
Projection does not consume these fields; no native/guest pointer is invented,
no independent state authority is added, and these values own no references.
The borrowed graph may reuse an already pinned, fully validated target only
inside that synchronous traversal, before any native/guest callout. Null edges
need no second source pin. Collisions/cold targets retain the old path; all
nodes still undergo full validation. This reduces source/target pin churn
without changing graph reachability, retirement, handle reuse or cross-call
freshness. Ordinary standalone relation borrowing is unchanged.

This alters the internal observation layout, so worker and WOW32 must be
rebuilt together; old-package Full evidence cannot certify it. There is no
mirror or guest diff for this candidate. It remains unpublished while the
four formal lifecycle cases and coherent-package comparison are pending.

The captured-relation candidate passes the four formal cases and the added
retired/reused-slot relation assertions. Coherent-package staging uncovered a
build-runner gap: outer Ninja could report no work without relinking WOW32
after inner fixture object updates. An initial package therefore had a stale
WOW32 image and is invalid evidence for this layout change. The second build
explicitly linked inner worker/ntvdm.exe and wow32/wow32.dll before staging.
Runtime launched under inherited VsDevCmd still exited early (-1). The same
second package passes the 16-round sibling lifecycle and surviving game probe
from the established ordinary modern-pwsh parent without compiler environment.
This comparison does not identify the responsible environment variable or
prove the initial package failure had only one cause. Preserve both failures.

Current-package SOL round 1 completes at 4156/6860 ms; round 2 fails to observe
the modal dialog within its unchanged bound. The failure-only snapshot now
executes. Thread 29312 is in wait_for_task_or_message -> xxxSleepTask ->
wow_user_task_lifecycle_wait; another thread (29516) has its PC at
NTVDM+1670e9, copy_window_observation+0xa9, with a reported phyR frame. A one-shot
sample does not prove a sustained hot loop or establish which application owns
every thread; optimized/FPO unwinding may be partial. It nevertheless provides
a concrete copied-window/SAS path to examine instead of assuming all threads
are blocked in scheduling. Results, exact manifest and source hashes are in
captured-relation-current-results.json; raw output is captured-relation-sol-stall.txt.
The candidate is not accepted for publication; previous Full receipts cover
only their exact older packages, not this new ABI/layout candidate.

Owner optimization criterion (2026-10-29): code reduction is valuable even
without speedup; an optimization adding code without demonstrated time
reduction must be reverted. Correctness repairs retain their separate defect
contract. The captured-relation layout and owned-target read variants add code
without demonstrated timing improvement. The latter passes SOL/Notepad/
WINMINE/PROGMAN interaction, but SOL remains 3.9–5.9 s show and 6.3–6.6 s
recovery, Progman 5.9/5.2 s. Both variants have now been narrowly reverted;
the extra internal observation field is gone. Regression tests for exact
relation identities and noncanonical target scalar reads remain, expressed
through the original API. See owned-relation-read-rejected.json.

The previous 30-second diagnostic on the captured-relation package completed
three rounds at 4312/3157, 2797/3281 and 3219/3515 ms. This is not acceptance
under the ordinary deadline and does not explain the earlier intermittent
failure. Game-thread identification has been added to future failure records;
the earlier sampled waiting thread was not proven to be WOWEXEC.

The retained foundation repairs were explicitly relinked in both inner Ninja
trees and sealed into reviewed-foundation-product. Its fresh Full run uses
the established clean runtime parent, separate from the compiler environment.
Prior binary receipts remain historical evidence only; this new receipt must
complete before any delivery claim. No default publication has occurred.

The retained-source Full run completed with exit 0 at 2026-10-29T17:38:18Z,
including the identical ten-file package assertion. Afterward the final-source
graph-snapshot and window-borrow cases both passed (17:39:05Z and 17:39:21Z).
These rerun the retained relation-identity, noncanonical scalar-target and
nested-publication assertions after removing both rejected candidates. The
receipts are recorded in reviewed-foundation-product-results.json. This is
regression evidence for the retained foundation package, not proof of a
multi-window performance repair, completed S11, or publication.

## Caller attribution after candidate rollback

An isolated replacement of the borrow-scope object records caller RVA, graph
nodes and elapsed QPC ticks; no production source changed. The prepare script,
exact inputs, manifest and raw counters are retained in graph-origin-results.json
and graph-origin-prepare.ps1. Link-map identities establish three callers on SOL
thread 13472 at the last diagnostic flush:

| Caller | Calls | Nodes | Cumulative graph time |
| --- | ---: | ---: | ---: |
| wow_user_borrow_window_state | 336 | 119431 | 4.542 s |
| publish_guest_return | 336 | 89359 | 3.508 s |
| wow_window_client_prepare_graph | 252 | 89523 | 3.251 s |

This confirms substantial repeated graph work in the project-owned adapter,
including handle validation; it is not evidence that the unchanged original
control algorithm spends this time. Counts cover startup and one modal round,
not only the click. Instrumentation and desktop state affect elapsed time.
SOL displayed the dialog in 7125 ms, then the probe returned code 10: dialog
gone, parent response timed out, worker still alive. The identified game
thread was sampled in wait_for_task_or_message/xxxSleepTask, with
sleep_message_task and lifecycle_message frames (nearest linker symbols,
not native_call_end). Sequential failure snapshots do not prove a stable deadlock, nor
that graph cost alone caused this non-pass. Keep it distinct from Full success.

The next bounded source question is why a validated WND acquisition repeats
full reachable-graph projection between publication boundaries. Any proposed
elimination must preserve exact retired generations, nested changes, local
publication and failure semantics; an existing client-state pointer alone is
not proof that the copied graph is current. No new cache or skipped boundary
is admitted by these measurements.

### Publication-boundary source review

The `control-client/precomp.h` macros route ValidateHwnd, ValidateHwndNoRip
and RevalidateHwnd through wow_window_client_validate_window. That path calls
wow_user_borrow_window_state and rebuilds the reachable graph. In contrast,
wow_window_client_validate_hwnd (lowercase suffix) only checks an enrolled
binding and its owned state. These are different contracts: the latter cannot
replace the former for external sampled windows, short handles or retired
callback identities. In particular, dialog_finalization_binding's immediate
validate_hwnd after prepare_graph is **not** a second whole-graph traversal.
Removing it would discard an ownership check without removing the measured
graph cost. This candidate is rejected by source inspection.

There are also two projections: project_graph_attempt captures native state
into the page domain, while borrow_client_graph retains exact generations and
publishes host WND pointers used by the recovered original code. Eliminating
either whole pass unconditionally would remove a required representation and
lifetime operation, not just duplicate computation. The latter also moves
new nested graph pins into the outermost scope; an existing host WND pointer
does not by itself prove those references are held.

Any bounded reuse investigation must cover these actual mutation owners:
page-domain refresh_window, publish_window_links, project_window_graph and
exchange_window_word; client-state attachment and original control writes;
native-return publication; creation/retirement; callback pair restoration;
and nested scope pin transfer. A new scope-local boolean based only on
callback_published cannot certify all of them. These findings rule out the
simple deletion and existence-only fast paths without adding production code.

Owner priority correction: stop the unselected 486/XADD experiment; its added
sources, candidate and records were withdrawn. The active repair is the
measured repeated graph projection on selected WOW paths. Do not divert this
repair into enabling unselected CPU functionality.

The proposed reuse boundary must be owned by the existing copied-page domain,
not inferred solely from callback entry or HWND existence. Source review finds
that callback_window itself calls refresh_window after control-entry graph
preparation. Other mutation routes include publish/replace_window_links,
project_window_graph, handle adoption/cancellation/retirement, class descriptor
publication and release, window_menu and exchange_window_word. Therefore a
native-return-only dirty flag would miss actual writes. A correctness fixture
must change a relation within one borrow scope, then validate again, and must
exercise nested publication and same-handle replacement before accepting reuse.
The measurable target is fewer graph traversals for repeated validation of an
unchanged exact generation; elapsed-time comparisons must retain the ordinary
dialog deadline and the same coherent package inputs.

Further write-path inspection rules out instrumenting only c_sas_storedw:
page_domain_store_owned_words can write mapped physical RAM directly with
CopyMemory. It is used for geometry/styles and relationship publication.
The fallback uses scalar stores, so tracking only that fallback would make
correctness depend on which memory-access path happened to run. Reuse must
also be invalidated across original execution, which can access this shared
representation outside the adapter's exported mutation functions. Until both
paths are covered, a publication revision getter alone is not a safe fix.
No such incomplete cache has been added to production.

### 2026-10-29 exact window-record index candidate

The selected page-domain relation access repeatedly resolved exact backing addresses
by walking the entire owning window-record list. Add a non-owning 1024-bucket
index of those same records; retain the list, pins, retirement, guest identities
and all graph refresh/publication behavior. All four publication sites insert;
central disposal removes the exact record, including rollback and domain teardown.
Prepared but unpublished records are not indexed. No native HWND-only cache,
independent state, new lock, guest modification or mirrored-source change.

Cached build/staging passed. graph-snapshot and window-borrow formal tests passed.
PID-bound visible SOL Options measurement used A/B then B/A, three rounds per
process: all 12 dialog/show/close/parent recovery checks passed. Average show time
A=3068 ms, B=1945 ms (36.6% lower); average close/recovery A=3011 ms, B=1836 ms
(39.0% lower). Both execution orders favor B. Evidence: window-index-results.json
and the four window-index-*.txt probe receipts. This is a short sequential desktop
sample, not a randomized benchmark or complete compatibility proof.

Eight other image input hashes match. NTVDM changed; VDMREDIR was relinked against
its regenerated import library (build-aux.log). No global publication performed.
Index overhead is 4096 bytes plus one pointer per record on x86. This candidate
has measured benefit and remains pending broader review/acceptance. It does not
remove repeated graph traversal and must not be described as the complete fix.

Window-index follow-up: the reclaim fixture now verifies three records in one
actual production hash bucket, removal of the middle entry without losing peers,
reinsertion at the same exact address, and empty index after disposal. These are
explicit metadata-only tests, not guest-memory tests. The existing real CCPU
reclaim fixture also passes failed physical release and complete domain teardown;
its teardown now checks both the window ownership list and all window buckets.
Receipt: window-index-reclaim.txt. No additional production changes were needed.

Full completed 2026-10-29T18:18:15Z with exit 0 and the identical ten-file
package marker; 16-round retiring-sibling check also passed. Receipts are
window-index-full-result.json and window-index-sibling-result.txt.

Review found the multi-dialog runner accepted Rounds but only lengthened the
observation deadline. Corrected it to actually repeat on the same owned parent,
with one final parent close and an exact PASS-count assertion. Historical
three-app invocations with Rounds=3 therefore proved only ONE dialog each.
The corrected three-round Notepad and WINMINE tests pass. PROGMAN initially
failed to observe its FIRST dialog within the existing 30-second bound; retain
window-index-repeated-progman.txt as a non-pass. A subsequent same-probe A/B
control passes all three rounds in both baseline and candidate. This does not
explain or erase the failure and does not prove that the index caused it.
The CaptureFault switch only adjusts an environment flag in this generic probe;
it does not itself collect a thread snapshot. No hang-root-cause conclusion is
supported by these logs. Full passing is not multi-window acceptance or S11 closure.

### Shared failure-only diagnostics and entry-boundary review

The generic dialog probe now reuses SOL's PID-bound thread/stack snapshot helper
rather than copying a second implementation. Successful-path timing is unchanged.
Both probe executables compile and run: SOL one round and PROGMAN six real rounds
pass on the same window-index package. No new failure was captured in that sample;
the earlier first-dialog failure remains unresolved. A noninteractive owned-child
fixture invokes the same snapshot, then releases the child's event and proves
normal child exit, confirming that diagnostic suspension is resumed. Module-list
iteration is bounded by its actual local buffer, including a larger reported count.
No runtime image changes are introduced by this test-tool refactoring.

Source trace: only dialog_message and the recovered EDIT entry call
wow_user_borrow_set_callback, after prepare_graph. General ValidateHwnd callers
also include standalone button/list/combo/dialog queries, so that entry invariant
cannot be applied universally. Native-return publication refreshes retained roots,
but explicit observation publications within a scope can change the graph; the
same-scope fixture demonstrates creation, destruction and nested ID changes.
Removing the graph projection merely because a binding already has host storage
would violate that demonstrated contract. No such shortcut has been implemented.

### Final lookup sweep

The common borrow_window_backing path now queries window_borrow_find before
allocation. Only insertion and physical unlink still walk the owning list;
insertion order is unchanged. Compared with the first index candidate this
removes three lines and avoids one remaining lookup scan without changing
publication, graph refresh or pin semantics. Cached build completed at
2026-10-29T18:35:55Z. graph-snapshot, window-borrow, window-reclaim and three
SOL Options rounds pass against the new package; see window-index-complete-*
receipts. The previous package's Full pass is not transferred to this package;
a new Full run is in progress. No new global deployment or runtime commit.

Original-source confirmation: OpenNT windows/core/ntuser/client/wow.c:154-188
implements ValidateHwnd/NoRip as CallbackWnd identity fast path followed by
HMValidateHandle[NoRip]. It does not traverse a window graph. The current graph
work is an adapter responsibility, not an original USER algorithm. Relocating
that responsibility must account for first acquisition of arbitrary windows,
callback preparation, same-scope publication and native/guest execution returns;
this source observation alone does not prove that any one current refresh is
redundant. The selected original copy in this repository is only _GetDlgItem,
so its absence of ValidateHwnd is intentional subset selection, not evidence
that the upstream function had no behavior.

Final lookup-sweep package Full completed with exit 0 at
2026-10-29T18:43:52.9963471Z and `PASS Full selected gates; identical 10-file
package`. Its manifest and focused results are already retained above. This
supersedes the prior package only for the verified gates, not the unresolved
multi-window non-pass. Current-source sibling and repeated application checks
are being rebound to this final package before any delivery decision.

The final lookup-sweep package also passes the 16-round retiring-sibling test
and three actual same-parent open/close rounds each for Notepad Find, WINMINE
Custom and PROGMAN properties. Exact receipts: window-index-complete-extended.json
and window-index-complete-{notepad,winmine,progman,sibling}.txt. The earlier
PROGMAN failure remains retained and unlocated; repeated passes do not prove
its removal. No test process remains active from this validation batch.

### PROGMAN startup-state attribution control

The earlier repeated-progman observer ended with Run16 0x42b (1067), but the
runner throws on the probe failure and its finally invokes owned-process
cleanup. The receipt is therefore not sufficient to establish that worker
exit preceded the missing-dialog assertion. The generic probe now records a
pinned process handle's wait/exit status before cleanup on failure, alongside
owned windows and the failure-only thread snapshot. Before each command it
records window enabled state, menu state and owning thread without changing
message delivery or readiness waits.

Three independent final-package starts, two dialogs each, all pass. All six
commands observed enabled=1/menu-state=0. This does not reproduce or explain
the historical failure and cannot exclude a timing issue on that failed run.
Stop this bounded repeat series; preserve the diagnostic for the next real
failure rather than treating repeated success as proof of repair. Source
inspection confirms Stop-IsolatedPackageScope kills only identity-checked
owned processes, which can itself end an outstanding launcher receipt.

### Rejected source-pin reuse experiment

An isolated source change moved source-node pin acquisition out of each internal
edge read, reusing the traversal's existing scope pin; public relation reads
kept a finally-protected pin. Only WOW32 input changed; the other nine inputs
matched. graph-snapshot, window-borrow and window-reclaim passed.

A/B three-round averages (show/recovery ms): baseline 2057/2240, candidate
2000/1979. Reverse B/A: candidate 2015/2000, baseline 1984/1948. All 12 modal
checks pass, but the added code has no repeatable speed advantage. Reverted
this experiment byte-for-byte to its saved pre-change file, preserving the
retained transactional edge index and all previous repairs. Receipts are
source-pin-*.txt and source-pin-focused-results.json. Not published or accepted.
The earlier final window-index package remains the candidate baseline; no
runtime provenance is transferred from the rejected package.

### Indexed graph diagnostic: reproduced native-return assertion

The isolated graph-phase-indexed package failed before its SOL modal dialog
with worker exit C0000420. A debugger-assisted repeat passed two rounds but
inspection showed the probe only captured AV and acknowledged unrelated
first-chance exceptions. That repeat is not evidence that assertion handling
was correct. The probe now records STATUS_ASSERTION_FAILURE plus its error
parameter and passes other exceptions through (except debugger breakpoints).

The corrected probe compiled with /W4 /WX and reproduced C0000420,
first-chance=0, error=5023. The existing trace identifies
publish-native-return on thread 4504. Retained probe and window tail are
indexed-assertion-probe.txt and indexed-assertion-window-tail.txt.
The final cleanup reported access denied removing the isolated ntvdm.exe;
this does not replace the prior captured runtime failure. The test is failed.

Source narrows publication to native window payload, the borrowed client graph,
or menu refresh. It does not yet distinguish these three stages. No finding
here establishes that original SOL is defective or that graph traversal is the
assertion cause. This instrumented package is not published. Production source
is unchanged by the phase counters; the diagnostic probe is the only new code
change in this investigation. The retained candidate's earlier Full passes do
not discharge this failure or the outstanding multiwindow acceptance.

A failure-only isolated build split native-return publication into payload,
client graph and menu stages with error-preserving trace labels. Three SOL
rounds passed (return-failure-detail); a second build also linked the prior
graph profiler and passed three rounds (return-failure-combined). Both used
the corrected debugger probe and tracing, whose timing may affect reproduction.
Neither success identifies the earlier 5023 stage or proves its absence. The
failure-only instrumentation remains in build/ and is not production code.

A further eight-round SOL run used the combined diagnostic package without a
debugger; all eight completed, with tracing enabled. No failed-stage marker
was produced. Inspection identifies explicit ERROR_INVALID_STATE checks in
all three publication paths, including graph transaction identity/retirement,
borrowed generation/ownership, and menu attachment. The earlier error code
alone cannot select among them. This remains a failed isolated run with an
unresolved cause; repeat success is not a repair.

### Scope of the available graph-phase sample

The last counter flush before the failed graph-phase-indexed run recorded
4,234 node visits and 16,936 edge reads over 12 pin-transfer/publish phases.
At the recorded 10 MHz counter frequency, accumulated relation time was
13.3403 ms, node-view reads 4.6555 ms, resource preparation 4.9589 ms,
row lookup 2.2943 ms, and transfer 1.7830 ms. Counters are inclusive and
cover only that thread up to the last flush; the process failed later and
this sample is not a complete interaction or end-to-end benchmark. In
particular it cannot attribute the multi-second delay to any one phase.
The native projection separately performs desktop-wide sampling and is not
included in these borrow-graph counters. Its separate measurement is needed
before selecting another optimization.

### Native projection cumulative measurement

The isolated source-only timing wrapper preserved return errors and measured
project_graph and capture_window on the SOL game thread. Three modal rounds
passed. At the final 64-call flush: 1,472 projections consumed 53,264,791
QPC ticks (5.326 s at 10 MHz); 29,837 capture_window calls consumed
1,735,918 ticks (0.174 s), included in the projection total. These cumulative
figures include startup and portions of three rounds, profiler overhead, and
exclude activity after the last flush; they do not prove a particular modal
latency or attribute the remaining time to a single function. They do show
that simply optimizing capture_window cannot account for most measured
projection cost in this run. Profile package, commands and counters are in
build/t473-s11/native-projection-cumulative and native-projection-phases;
no diagnostic instrumentation is in product source.

### Native projection breakdown

A second isolated instrumentation run passed three SOL modal rounds. At its
last flush, 1,472 projections took 5.837 s cumulatively; capture_window took
0.217 s, normalize_observation_chains 0.048 s, and
wow_window_words_project_observed 2.869 s (all included in the projection
figure). These samples include setup and instrumentation overhead; remaining
projection work accounts for the unpartitioned time. This identifies two
areas for further attribution, not a safe optimization by itself. Source and
receipts remain in build/t473-s11/projection-breakdown; no production code was
changed by this instrumentation.

### Project-observed breakdown

A separate isolated SOL run passed three dialog rounds. At 1,472 calls, the
project-observed routine accumulated 2.588 s, of which page-domain graph
projection consumed 2.379 s and title/name capture 0.037 s (10 MHz QPC).
These are inclusive cumulative game-thread timings through the last flush,
not per-dialog timings; instrumentation and desktop state differ from prior
runs. The evidence points to page-domain graph projection as the next hotspot,
not name capture. The diagnostic code and receipts are in
build/t473-s11/commit-breakdown and were not linked into a release.

### Page-domain scan review

The measured page-domain routine scans guest_windows across all 65,534 usable
slots once per projection to find withdrawn observations. It also iterates the
existing window_borrows list immediately beforehand. A live borrow record is
retained even at zero references (release_window_record), and prepared window
backings are inserted in that list before publication. This suggests a possible
single-list retirement discovery instead of the full sparse-array scan, while
keeping the full-generation/current-slot checks and excluding retired records.
However, page_domain_publish_handle supports existing/cold backings and
borrow_window_backing explicitly creates a missing record, so list completeness
for every selected publication path is not yet established. No scan has been
removed: a missing record would otherwise silently retain a withdrawn object.
The scan's own time has also not yet been measured independently of graph
validation, per-row page writes and class collection. Any implementation must
first prove these invariants and demonstrate a measured improvement.

### Retirement discovery list experiment

An isolated worker replaced only the 65,534-slot withdrawal scan with an
iteration of existing live borrow records, checking exact backing against the
published slot and excluding the separately held desktop. The source remains
in build/t473-s11/retirement-list-experiment, not the production tree. The
selected publish paths create a record before writing guest_windows; zero-pin
live records remain until withdrawal. Nevertheless this prototype has not
proved equivalence for all rollback, list order, reuse and failure cases.

Three sequential SOL rounds passed for each package: baseline show
2141/1781/1828 ms and recovery 2109/1875/1984 ms; candidate show
2172/1750/1766 ms and recovery 2000/1891/1843 ms. This small ordered sample
shows no convincing end-to-end speed gain and cannot justify adding code or
claim behavioral equivalence. The experiment is not integrated or released;
the original complete scan remains. Further optimization should target
measured cost, not assume iteration count predicts wall-clock savings.

### Owner-approved complexity optimization

The owner clarified that for compat/project-owned code a meaningful time or
space complexity improvement with no large net code increase should be accepted;
a short end-to-end benchmark need not show a conspicuous gain. Accordingly the
retirement-list experiment is now a worktree candidate, not rejected for that
benchmark. The fixed 65,534-slot scan becomes a traversal of existing window
records, with the published backing and full handle checked before withdrawal;
no new persistent index or second authority is introduced. Non-retired records
remain at zero pins; preparation creates the record before slot publication and
the desktop is separate. Retired or uncommitted records are skipped.

Focused graph-snapshot, window-borrow and window-reclaim fixtures passed with
the established desktop token. A restricted-token attempt failed with desktop
access error 5 and is not a product failure. The candidate's three-round SOL
comparison is recorded above. The prior domain timing probe recorded 1,408
projections: retirement scan 0.200 s, graph replacement 0.963 s, validation
0.138 s, record association 0.112 s and preparation 0.203 s at 10 MHz;
counts are cumulative and include instrumentation. A dedicated complete
retirement/reuse regression and coherent product gates remain required before
release or S11 closure.

The coherent ten-image candidate was rebuilt and staged at
build/t473-s11/retirement-list-final/package. The desktop-token focused fixture
reports managed slot reuse in two modes, retained-generation release, dead
object graph atomicity/live guards and preparation failure rollback. The Full
product gate is running as exec session 28206; do not restart it while live.
The finished result and exact package checks are still required.

The rebuilt retirement-list-final package passed Full with identical ten-file
package. Its separate 16-round sibling and three-round Notepad and Program
Manager probes passed, but the first Winmine modal round exited the worker
with C0000420 before the dialog. extended.ps1 continued to later cases and
returned zero for the last case; the per-case result records Winmine exit 1.
Therefore the extended matrix is a non-pass and must not be reported as a
whole success. This failure has not been attributed to the list candidate or
to the earlier SOL 5023. Preserve package and failure; no release.

After the Winmine non-pass, the same candidate worker with a diagnostic WOW32
that records native-return publication stages passed eight traced Winmine modal
rounds. Because instrumentation changes the provider/timing, and no assertion
recurred, it does not establish the prior failure stage or fix. The temporary
extended.ps1 aggregator has been corrected to return nonzero if any application
case fails; prior recorded per-case output and prior false zero remain intact.

### Winmine assertion located at native-return payload

A stage-instrumented provider with the retained worker reproduced Winmine's
C0000420 on independent startup 3. The existing trace reports
return-payload-failed status=5023 followed by publish-native-return status=5023;
this narrows the failure to publish_call_windows, before subsequent borrowed
graph and refresh-all-menu publication. Retained trace:
build/t473-s11/winmine-stage-diagnostic/start-3/windows.txt. The specific
window projection versus returned-menu path is not yet distinguished, and this
does not prove the retirement-list change caused it. A second isolated provider
adds separate labels and its startup series is in exec session 59049; do not
restart while live. No diagnostic package is eligible for release.

A second payload-instrumented worker/provider combination reproduced the
Winmine failure on independent startup 8. Trace labels payload-window-failed
status=5023, then return-payload-failed and publish-native-return. Therefore
this failure occurred in wow_user_window_refresh_payload, not the optional
returned-menu publication. Its specific internal check is still unproven;
trace retained in build/t473-s11/winmine-payload-detail/start-8/windows.txt.

The projection-stage diagnostic reproduced Winmine on retest 5 with
initial-view error=5023: the first size query to
mvdm_softpc_wow_page_domain_copy_window_view fails before any window graph
capture/retirement commit. That query returns ERROR_INVALID_STATE when
page_domain_menu_context fails: active domain, client delta, owning session
thread, CR0 paging, and CR3 directory are all required. The captured label
does not distinguish which predicate failed; do not claim the exact root cause.
Evidence is build/t473-s11/window-transaction-failure/retest-5/windows.txt.
The isolated provider is diagnostic only and not release eligible.

A further worker diagnostic snapshots the page-domain check operands at the
check itself, before any trace I/O. Winmine independent retest 2 reproduced:
active=1, delta=80000000, owner=current, CR0=00000010 (paging disabled),
CR3=00900000 equal to expected 00900000. In this run the transaction label
was allocation, so the failed check was the second copy_window_view call,
after the initial size query. This proves paging was absent at that check; it
does not yet establish which execution edge changed CR0 or whether another
thread concurrently owns the emulated CPU. The earlier post-failure sample
with CR0=80000011 was taken after trace I/O and is not evidence of the state
at the failed check. Source/package/trace are retained in
build/t473-s11/window-context-failure/retest-2. No product fix or release.

The return-execution-detail diagnostic reproduced Winmine at startup 7. At
failure CR0 paging was disabled and the native-return trace flags were zero:
execution_held=0, resource-held-on-entry=0, scheduler_wait=0, and sender was
not the scheduler owner. This demonstrates publication was attempted without
original task execution ownership; the page-domain state can then reflect a
different CPU context. The existing native_call_end defers publication only
when scheduler_wait && !execution_held, while the ordinary end path publishes
when !execution_held. The observed ownership defect is now concrete, but the
necessary deferred-publication semantics, execution reacquisition and tests
must be audited before changing it; no fix or release is claimed. Evidence:
build/t473-s11/return-execution-detail/start-7/windows.txt.

The return-payload-identity diagnostic reproduced the same non-owner, paging-
off failure on independent startup 10. At that failure the native return had
zero target window, zero result window, zero result menu and zero typed payloads.
This explains why the existing no-argument scheduled graph refresh could cover
this particular instance. It does not prove all non-owner returns are empty;
other native operations may carry identities that must be retained until
execution ownership is regained. The current wait-only pending flag therefore
cannot simply be extended to all calls without auditing those payloads and
lifetime/reentrancy. Evidence: build/t473-s11/return-payload-identity/start-10.

### Non-owner native-return contract review

The existing waiting-path pending flag records only an obligation, not HWND or
HMENU identity. It cannot represent every ordinary non-owner return:
peek_message_published copies queued MSG window operands into returned_payload,
and other wrappers collect returned windows/menus. A later generic desktop
refresh cannot prove preservation of an unlinked or external explicit operand.
Also, the GetMessage loop invokes sleep_message_task(owner, FALSE) before its
next peek, so a fix cannot assume the wait path has already performed the
pending publication. Callback entry independently reacquires original taskman
execution; scheduled publication currently occurs at only explicit directed
or successful wait/sleep paths. Any solution must cover nested native scopes,
callback entry, exact typed operands and failure/teardown before guest resumes,
while preserving original queue ordering and avoiding an invented scheduler.
The current successful empty-payload repro is not sufficient to justify a
blanket change. Relevant source: wow_user_task_lifecycle.c:729-803, 875-910,
1054-1079 and 1121-1176; wow_user_native_call.h.

### Execution-owner prototype non-pass

An isolated WOW32 prototype reused callback_enter/callback_leave around
ordinary non-owner return publication, carrying the original call operands
without a new queue. It compiled and the first two-round Winmine startup
passed, but the second independent startup exited C0000420 before its first
dialog. No trace was enabled in that run, so the failing stage and relation
to the prototype are unknown; this does not establish the approach safe or
fixing the issue. The prototype was never applied to production source or
released. Evidence: build/t473-s11/return-owned-prototype/start-2/probe.txt.

Eight independently launched traced Winmine runs using the execution-owner
prototype passed, but none reproduced the untraced second-start C0000420.
They do not identify that exception or establish the prototype's correctness.
Source audit confirms callback_enter uses original xxxDirectedYield, which adds
an event and reorders the TDB; using it solely to publish a native return may
therefore alter task ordering. The prototype also uses the callback cleanup
entry contract, so a general replacement needs explicit scheduler semantics
and nested, task-lock, payload and unwind tests. It remains outside product.

### Original taskman reacquisition audit

Original xxxDirectedYield(DY_OLDYIELD) increments both the task and global
pending event counts, reinserts the TDB and calls xxxSleepTask. That is a real
scheduler operation, not a lock-only acquisition. xxxSleepTask removes the
current owner when applicable and later consumes an event when the task is
selected. The prototype therefore cannot be treated as a transparent mutex
around publication. In particular, task-lock and nested native waits require
proof that the artificial event is balanced and native continuation is parked
exactly as before. Existing WOW_NATIVE_WAIT_UNWIND tests check the wait-only
path, not an ordinary non-owner native return with explicit MSG operands.
The relevant real message loop obtains typed payloads from PeekMessage and can
return the result directly, including PM_NOYIELD; asynchronous refresh of the
desktop alone cannot substitute for that typed return contract.

### Message-wait native return correction

Source trace found wow_user_msg_wait is an original scheduler wait with no
returned HWND/HMENU payload, yet its finally path called ordinary native end.
Changed that one call to the existing end_wait boundary: owned returns remain
unchanged; a non-owner wait records the existing scheduled-publication
obligation rather than touching the guest page domain. No new queue, timeout,
CPU state mutation or guest change was introduced. Added a native-nested test
that invokes the actual wait on a signaled event while an outer native scope
is suspended and checks result, error, ownership and LIFO restoration. The
native-nested fixture passed (msg-wait-nonowner-r1). This is a local candidate;
the fixture does not force the observed paging-off condition, so this alone is
not a negative control or proof that the earlier Winmine crash is fixed. The
ordinary PeekMessage and other payload-bearing non-owner paths remain open.

The coherent msg-wait-product package, containing the message-wait return
correction and retained window candidates, passed ten independent traced
Winmine starts and one modal interaction each. The previously failing package
and earlier diagnostics remain retained. This limited sample is encouraging,
but does not prove the intermittent crash fixed or establish that the failure
came from MsgWait; ordinary payload-bearing non-owner return cases and Full
package verification for these new bytes remain outstanding. No release.

The pending publication audit also identified wow_user_task_lifecycle_yield:
it called the original xxxUserYield but did not run publish_scheduled_return
when taskman gave the thread execution, unlike directed yield and the successful
wait path. A local candidate now invokes the same owner-checked helper after a
successful UserYield; the helper only publishes when an obligation exists and
the calling thread owns execution. This change was made after the sealed
msg-wait-product package was created: its running Full gate cannot validate
the new bytes. A focused regression and a new coherent rebuild are required.
The general non-owner return with message/window payload remains unresolved.

The sealed msg-wait-product ten-image package passed the Full selected gates
and ten independent Winmine starts. The later UserYield source change is not
in that package. The current-source native-nested lifecycle fixture passes,
including ordinary UserYield and the nested message-wait case, but it does not
isolate a pending publication consumed specifically by UserYield. Therefore
that branch still needs a direct witness and coherent package verification.
The two candidate identities must not be conflated.

The updated UserYield and message-wait candidate was explicitly relinked and
staged in build/t473-s11/yield-product/package. Full selected verification is
running against that exact package as exec session 89003; it must not be
restarted while live. The existing native-nested test passes but does not yet
prove pending publication is discharged specifically through UserYield.

The yield-product Full gate exited 1 at the nested-window broker handoff.
The observer timed out and reported scripted-console-input=failed; prior
visible output reached the outer CMD's echo command. An immediate repeat of
the exact candidate also timed out, this time before completing the nested
command input. The previously Full-passing msg-wait-product package was then
run under the same observer conditions and also failed. These are three
preserved non-passes, not a passed gate; the control makes attribution to the
UserYield change unsupported but does not establish the underlying cause.
See build/t473-s11/yield-product/full-results/handoff.*, handoff-repeat.* and
handoff-prior-control.*. No release or success claim.

### Handoff non-pass: observer comparison

The previous same-package control and new candidate both verified that the
visible LibKvmWindow belonged to the isolated NTCON. They both used identical
80x300 buffer / 80x5 viewport setup. In both immediate repeat failures the
observer sent 78 key messages, completed one Enter, then failed its input
milestone; its final console showed the outer CMD had received the nested
run16 command, while the isolated NTVDM and NTVWM remained alive. The earlier
successful Full run used the same viewport and reported input-delivered=1 in
3.953 s; the failed control reported input-delivered=0 after 21.656 s.
These observations rule out a simple initial geometry difference or an
unverified front-end owner. They do not establish whether the guest, host I/O,
or probe's milestone observation failed. Observer source has a further
unfiltered FindWindow in wait_input_milestone even after the CAF owner check;
no evidence yet shows a second LibKvmWindow caused these failures. Do not
change product or weaken the gate on that hypothesis.

The owned-console observer was changed to retain its existing stderr output in
<prefix>.observer-stderr.txt; no readiness assertion or timeout was changed.
A fresh control on the previously passing package again failed. Its preserved
milestone reports 1,219 readable frame samples and expected an empty prompt;
the final sampled frame still showed the outer CMD's nested run16 COMMAND.COM
line, not a new DOS prompt. At timeout the isolated NTCON, NTVDM, NTVWM and
nested Run16 were alive; the captured NTVDM main thread was in the emulator's
execution image. This narrows the failure beyond a missing observer window or
unreadable frame, but a sampled host stack alone cannot prove whether the guest
was looping, waiting for input, or delayed by host execution. The stderr
redirection did not turn this run into a pass and may affect test process handle
configuration; its effect should be controlled before relying on it generally.
Evidence: yield-product/handoff-stderr-control.observer*, timeout-live.txt and
native-errors.txt. Original Full and repeats remain non-passes.

The failed control's retained NTVDM main-thread stack was mapped against the
candidate worker map after accounting for module base: its sampled PCs fall
in original CCPU _d_mem, _vir_write_byte and _ccpu. The previously passing
run's first-line frame contains the DOS banner and prompt; the failing run's
final readable frame still contains only the outer CMD command line. This
shows the emulator thread was executing at the instant of sampling, not that
it was blocked in a host wait. One stack sample cannot distinguish progress
from an emulated busy loop or establish whether a later frame would arrive.
The host frames and these module-relative mappings are evidence only; no
timeout or completion assertion has been relaxed.

### Handoff clean-environment control

The owned-console timeout dump was taken by the existing read-only CPU
snapshot: three 100 ms samples had CS:IP F000:FF33, identical general
registers, with the fault stack CS:IP 03F4:21E9. BIOS dispatch maps BOP 06
to illegal_op_int; the captured guest address 06129 contains bytes from a
host PATH string. The samples are explicitly non-atomic and the guest-memory
dump was taken later, so this is strong evidence consistent with the known
COMMAND inherited-environment defect, not an exact fault-frame proof.

The immediate prior-package controls had inherited the caller's large
PowerShell environment, unlike the Full gate. Re-running the prior package
using the exact clean environment of Full passed nested-window handoff with
exit 23 and delivered input. Therefore those uncontrolled repeats must not be
used as equivalent Full environment controls. A clean-environment latest
candidate repeat still failed differently: DOS banner, VER and MEM input
appeared, then observer reported readable=0 while NTCON remained alive and no
LibKvmWindow appeared in its timeout window snapshot. The original Full
failure reached outer CMD echo. These are distinct observations, not proof of
one cause or attribution to UserYield; no gate was waived. Evidence prefixes
handoff-clean-env-control and handoff-clean-env-candidate.

The clean-environment candidate and prior control manifest inputs match for
nine of ten host images; only WOW32.DLL differs. No WOW32.DLL or WOWEXEC is
listed in the candidate timeout's descendant module snapshot, consistent with
this native-CMD/DOS scenario not loading the changed provider. This makes a
direct causal connection from the UserYield code to the observed frontend
window disappearance unlikely, but does not eliminate indirect or timing
influences. In the candidate failure, NTCON remained alive, the owned Window
was initially verified, and the timeout snapshot contained no LibKvmWindow;
the captured Console buffer reached the DOS MEM command. Source inspection
shows frontend_window_clear destroys the Window on a committed video frame
with no pixels; it can also close on a Console selection, park, or return.
The evidence does not identify which transition occurred. Do not attribute it
to a specific branch without event trace.

### Handoff observer PID lookup defect

The retained failed transition diagnostic timeout snapshot showed two visible
LibKvmWindow windows: the isolated NTCON and an unrelated process. Although
CAF admission verified the expected NTCON PID, scripted_input_window used
FindWindowW once and rejected a different PID rather than searching further;
wait_input_milestone and write_console_snapshot also selected an unqualified
first window. This is a concrete test isolation defect (not proof of which
window FindWindow returned in any particular prior failure).

Updated the observation tool to enumerate visible class windows and select the
previously authenticated frontend PID for input, milestones and snapshots.
No product code, input semantics, retry policy or timeout changed. Rebuilt the
x64 observer with /W4 /WX. The clean-environment exact yield-product candidate
handoff then passed all six submitted lines, delivered input and returned 23.
The earlier failed outcomes remain failures; a full re-run of the identical
candidate using the fixed observer is live in exec session 61336, with reports
under build/t473-s11/yield-product-full-r2. Additional unrelated diagnostic
window transitions were observed only on an instrumented package and are not
product acceptance.

A subsequent source audit found the CAF-return counterfactual also searched
only the first LibKvmWindow, then treated a different owner as target absence.
The test observer now uses the same visible-window enumeration keyed to the
explicit frontend PID for that check. This follow-up change was made while
Full session 61336 was already running; its process uses the previously built
observer, so that Full run cannot validate the follow-up. No product code or
external window was acted upon by this edit.
The CAF-return PID helper also distinguishes visible-window selection for
input from existence checks for close: an invisible but still-live target
must not be reported destroyed. Lookup checks GetWindowThreadProcessId success
before comparing the owner. This source-only refinement postdates the running
Full observer binary and still requires compilation and focused verification.

Full session 61336 subsequently finished with exit 0: its terminal log reports
`PASS Full selected gates; identical 10-file package`. This establishes the
Full result for the sealed yield-product package and the earlier observer
binary only. The later PID-lookup refinement was rebuilt with /W4 /WX and the
same candidate passed the clean-environment nested-window handoff, including
actual CMD/DOS input and outer exit 23 (`handoff-final-pid`). That focused
check does not exercise the hidden-window CAF-return branch and does not
retroactively change the Full observer binary. Earlier failures remain in the
record and the general native-return payload ownership question remains open.

### UserYield publication ordering limit

The retained UserYield candidate calls `publish_scheduled_return` only after
`xxxUserYield` returns. Source inspection of taskman.c shows that the original
algorithm may call `xxxReceiveMessages` both before scheduling (when the
caller is already selected) and after `xxxSleepTask` returns. The latter
normal sleep path enters the WOW critical section before returning, while the
special HEVENT_REMOVEME path explicitly returns without acquiring it. The
project's receive binding invokes native PeekMessage, so the post-UserYield
helper cannot by itself prove that every native return inside UserYield was
published under an acquired execution owner. In particular, selection and
ownership must not be treated as interchangeable without evidence. The
existing nested-wait fixture verifies restoration but does not directly prove
the pending obligation is discharged on this edge. This is a limitation of
the current candidate, not evidence that a particular receive actually failed.

The current-source `native-indirect` fixture was rebuilt and passed in
`build/t473-s11/native-indirect-current`: sent-only receive retained posted
messages and quit, indirect wait returned with its owner, and two cross-task
modeless common-dialog callbacks completed and retired. This exercises real
adapter code and native USER, but uses the fixture's parent boundary; it is
not a paging-disabled non-owner payload negative control and does not settle
the general return-path question above.

The current-source `native-nested` fixture was independently rebuilt and
passed under `build/t473-s11/native-nested-current`. It covers suspended
callback entry/exit, nested wait unwind and the signaled message-wait result,
plus target destruction, receiver retirement, native exception and timeout
returns. Its parent stub explicitly reports no active CCPU page domain, so
these passes cannot establish that an ordinary non-owner return carrying HWND
or HMENU safely projects into the actual guest view. The previously reproduced
page-domain failure remains the stronger evidence for that unverified edge.

### CallNextHookEx native-return witness

An isolated diagnostic provider was built from the prior return-payload
instrumented source, adding only a return address and the loaded WOW32 module
base at the already-failing native-return boundary. A fresh ten-image test
identity was assigned within the isolated package; no installed image or
guest was changed. On Winmine start 11, the existing C0000420/5023 failure
reproduced with empty returned HWND/menu/payload, no execution owner and
CR0=00000010. `return-address=1772105385` and
`return-module=1771700224` give RVA 0x62EA9. The matching diagnostic link map
places that address within `wow_native_CallNextHookEx` (entry RVA 0x62DE0;
the next function starts at RVA 0x62ED0), so this witness
identifies the native hook-chain return, not MsgWait. See
`build/t473-s11/return-stack-diagnostic/start-11/windows.txt` and the matching
`wow32.dll.map`. Starts 3 through 10 passed; starts 1 and 2 failed before
worker creation with 0x51a version mismatch, and after assigning a fresh
package identity later starts reached the worker. That correlation does not
by itself identify the peer that caused the mismatch. This instrumented
package is diagnostic, not product acceptance. The hook-chain argument may
carry in/out data, so no general assumption that hook return has no observable
payload follows merely from the empty window-root fields in this witness.

The next diagnostic recorded the direct caller at the hook wrapper itself.
Start 2 reproduced the same C0000420/5023, non-owner, CR0=0x10 failure;
`hook-return-caller=1756036236` and module base 1755578368 give RVA 0x6FC8C,
within `WK32ForegroundIdleHook` (0x6FC60..0x6FC9F) in that exact map. The
earlier stack capture provided only one frame and was not used to infer this.
Source trace shows the idle hook finishes `dispatch_idle` and leaves its
guest callback scope before forwarding to the next native hook. Its precomp
macro accidentally routed that forwarding through `wow_native_CallNextHookEx`,
which publishes guest state even when that native continuation owns no WOW
execution. This is distinct from guest DefHookProc's actual hook conversion.

The local candidate undefines the macro only in the idle binding, so its
post-callback forwarding calls host USER directly; actual guest callback
entry and original hook chain ordering remain unchanged. The idle fixture no
longer supplies a fake `wow_native_CallNextHookEx`, proving this object does
not depend on that shim. Real isolated desktop native-chain and guard fixture
passed (`idle-direct-chain`, errors=0, next=13). The coherent candidate under
`idle-direct-product/package` is undergoing independent Winmine repetitions;
this focused result is not Full or release acceptance, and other generic
non-owner return paths are not proven safe by this correction.

The `idle-direct-product` Winmine run passed five independent starts and
dialogs. Start 6 displayed and closed the dialog but later failed with probe
exit 9 and `publish-native-return status=1400`; this is a distinct invalid
window publication failure, not the captured 5023 hook-forwarding failure.
No causal attribution or release acceptance is inferred from this small run.
Evidence: `idle-direct-product/start-6/probe.txt`, `windows.txt` and log.

An isolated `invalid-return-diagnostic` provider was then built from current
source with failure-only return address, HWND/menu/payload and owner tracing,
alongside the idle forwarding correction. Its 16 independent Winmine starts
and modal interactions passed, so it did not capture the `1400` recurrence.
Instrumentation and repeated successes cannot erase the uninstrumented
start-6 failure. The graph source requires each selected root to be captured
successfully, while expired non-root observations may be withdrawn, but the
available failure does not identify which operation returned 1400. Package,
map, per-run logs and exact identity are retained in that diagnostic folder.

The real-CCPU graph-snapshot fixture now also submits a previously captured,
destroyed HWND as a required root. It verifies `ERROR_INVALID_WINDOW_HANDLE`,
that the failed transaction did not withdraw the old full identity, and that
a subsequent valid desktop-root transaction withdraws it while a retained
reader remains safe. The updated `graph-dead-root-current` fixture passed,
including `dead_root_rejected=1` and `dead_withdrawal=1`. This proves the
required-root contract in that controlled case, not that the Winmine `1400`
was caused by such a root; its actual failed operation remains unidentified.

The same real-CCPU fixture already distinguishes a destroyed secondary HWND
from a required root: `verify_returned_windows` destroys a window, refreshes
with that dead value as payload, verifies no backing is revived, and combines
it with a live full HWND and a narrow child identity. This is why the dead
root negative above must not be generalized into dropping every invalid
returned value or treating every 1400 as a harmless payload. The current
graph-snapshot receipt includes this existing message-return coverage; it
still does not identify the actual source of the intermittent failure.

After the idle-hook candidate was compiled, the current native-creation
fixture was independently rebuilt and passed with `-CreationUnwind`
(`build/t473-s11/native-creation-current-r2`). It exercised real CCPU page
storage with 111 fixture callbacks, nested creation, rejection, CBT veto and
15 destroyed windows; the unwind marker reports TLS, hook and borrowed-scope
restoration and a successful subsequent retry. Its host-controlled guest
callback double does not establish immutable guest application behavior or
identify the later Winmine 1400 failure.

### Other native hook forwarding owners

The creation observer and terminal observer also call `CallNextHookEx`, but
their selected current objects both import native `__imp__CallNextHookEx@16`;
they do not call the guest-publication wrapper. The macro in
`wow_user_private_access.h` applies only under its explicit redirect defines,
so merely including the header was insufficient to establish a problem.
The rebuilt idle object likewise imports native USER after the candidate
change. A binary-symbol scan of the selected WOW32 object tree found only
`wowhooks.obj` and the lifecycle wrapper object containing the wrapper symbol;
`wowhooks.c` has eight original 16-to-32 DefHookProc conversion call sites,
where preserving the guest call boundary is a separate contract. Ninja
reported the four inspected objects up to date. This rules out the same
direct macro mistake in the inspected creation/terminal observers, but does
not establish all hook-chain semantics or the general native-return fix.

### Unattributed 1400: publication failure boundaries

The uninstrumented failure reports only `publish-native-return`. Source review
shows that this stage covers distinct operations: retirement collection and
root/payload capture, returned-menu refresh, borrowed client-graph reconstruction,
and refresh of menu client views. A required HWND failing capture aborts before
graph commit; a vanished secondary payload is instead kept as a numeric guest
value. Local-state name capture and property validation are also fallible before
commit. The single 1400 code cannot identify which operation failed, its HWND,
or whether an external observation changed. The retained Winmine trace shows
modal creation, close recovery and main-window messages before failure, but does
not prove a particular window was destroyed at the failed operation. Ignoring
1400 or treating every invalid HWND as an external race would change the
required-root contract without supporting evidence. Dead-root and dead-payload
fixture results are negative controls, not a diagnosis of this occurrence.
In particular, `wow_menu_client_refresh_all` independently calls
`GetWindowInfo` for every registered local window and immediately returns its
failure; an invalid HWND there can produce the same 1400 even if the earlier
graph publication succeeded. The retained trace has no stage discriminator,
so interpreting the error as proof of a failed graph root would be incorrect.

### Independent immutable common-dialog guest

An unchanged, previously built 16-bit CDLPROBE guest (SHA-256 `406D84F9E4EA04CD269045B354E95C47A342C986E0400A2C4F6A58DC818D3A02`) ran without Winmine in a short, freshly identified isolated package staged from `idle-direct-product/package`. Its report records 17 FindText dialogs created, 16 explicitly destroyed, 17 initialization hooks, zero guest errors and one live dialog deliberately left to task exit. The trace records `TaskRetirement` enter and complete for task 17BF, both status 0. Run16 observer exit was 0; it returns at Win16 startup and is not by itself a task-completion assertion. The retained [evidence](cdl-standalone-current/) includes the staged ten-image manifest, guest report, observer, trace and test runner. This validates one standalone guest and selected lifecycle on the candidate package; it does not validate Winmine, concurrent sibling tasks or the unresolved earlier 1400 failure.

The first attempt incorrectly treated launcher return as guest completion and saw no checkpoint; it is a non-pass. The second waited for the checkpoint and task retirement but searched `tests` while the guest wrote its file to the isolated runtime root, so its runner also failed despite separately captured guest and retirement evidence. The corrected third and fourth attempts passed; only the fourth additionally retained the staged package manifest. No guest was rebuilt or modified.

### Same-worker idle notification regression and repair

The unchanged prebuilt shared-idle guest and host probe exposed a retained regression in both `yield-product` and `idle-direct-product`: two tasks overlapped and retired normally, but the registered task recorded zero idle callbacks executing under the peer task. The full cross-task gate failed; lifecycle-only success is not a substitute. The approved T457 S3 acceptance records that the original worker-shared INT 2F:1689 intent must dispatch on the currently selected task, with no cross-worker/global priority guarantee.

Source and history isolate the mismatch: commit `0a5129d77` selected the original worker-wide TD registration query for taskman, while `aa331499f` replaced it with a query restricted to the selected thread. The worker-shared dispatch code still queries all live TDs, but taskman never invokes it for the unregistered peer. The S11 worktree restores the zero-thread query at that existing seam; it does not add a hook registry, alter guest, or change the native per-thread observer. This is project-owned adapter code and adds no MVDM mirror diff.

With the rebuilt candidate, the exact same prebuilt pair passed the strict shared-worker test: worker 31072, registration task 17BF, peer task 177F, four callbacks observed on the peer, both requests and task retirements completed, and registrar continued after the peer exit. The independent two-worker prebuilt test also passed, checking both process exits and the surviving worker. A separate immutable common-dialog guest completed its 16 close cycles and final task retirement in the same candidate source package. Fresh package identities were used for isolation; manifests, binary hashes, reports and trace are retained in [regression evidence](idle-shared-regression/). The first lifecycle-only pass did not assert cross-task callbacks. These focused tests do not erase the unrelated Winmine 1400 result, establish all hook ordering, or constitute the required Full gate.

The current lock-order source sweep found `gcsWOW` acquisitions at TD link/unlink, hung-task list lookup, cleanup identity lookup, and the existing idle-registration scan. The inspected list sections release `gcsWOW` before native or guest callback execution; the shared query adds no new lock or callback within the lock. This supports the local lock-order rationale, but is not a proof of every interleaving or native-hook priority.

The `idle-shared-fix` coherent ten-image candidate subsequently passed the complete `Control` suite (RPC, native GUI routing, version rejection, terminal/EDIT, relaunch, session isolation, lifecycle and nested handoff). [Control timings and runtime identity](idle-shared-regression/control/) are retained. The first Control attempt failed while compiling the version-negative fixture because the coordinator omitted the previously required `OPENNT_BROKER_PRODUCT_BUILD` source-graph path; the corrected rerun supplied the historical value and exited 0. This is not the Full suite: that suite also launches Winmine, and the prior automatic review explicitly rejected a Winmine diagnostic path that could reach the user profile. No released image or guest was changed.

The same `idle-shared-fix` ten-image candidate separately passed the standard Console-17 and Window-17 command matrices under a short owned runtime, using the existing unmodified G7 guest test input. The test supervisor exited 0 and retained the staged image manifest and 34 primary case reports in [product-matrix evidence](idle-shared-regression/product-matrix/). This supervisor invokes the same two case runners as the product gate, but is not itself the Full suite: the independent WOW frontier, game interactions, comprehensive S11 review and final release identity remain outstanding.

After the worker-shared idle query correction, three current-source selected-object lifecycle gates were rebuilt and passed: native creation with nested callback and injected unwind, graph snapshot with required dead-root rejection and safe withdrawal, and window/class reclaim with failure, collision, retained generations and address reuse. The creation fixture recorded 111 controlled callbacks and 15 destroyed windows. Link maps bind the tested lifecycle functions to the selected provider object; [results, hashes and markers](idle-shared-regression/selected-lifecycles/) are retained. These tests use controlled guest callbacks and do not identify the separate intermittent Winmine 1400.

The current selected native-nested fixture also passed callback scope, suspended wait, exception unwind, result/error preservation and receiver-exit cases after the idle-query change. Its parent page-domain seam is inactive; this is not proof of publishing payloads while another task owns real CCPU state. The exact fixture/map/object hashes and markers are in `idle-shared-regression/selected-lifecycles`.

### Shared idle and dialog task combination

A separate isolated ten-image stage of the idle-shared-fix candidate ran two previously built, hash-verified Win16 test inputs in one actual worker: IDLPROBE registered idle and remained alive while CDLPROBE created 17 FindText dialogs, explicitly destroyed 16 and left one for exit. The native host used `run16 --wait` for both, observed the dialog task finish while the registrar remained running, then observed the registrar finish. Guest checkpoints report zero errors; trace records exactly one successful retirement for each distinct task in the same worker and registrar idle dispatch after the dialog task retired. [Receipt, staged manifest, guest binary reports and trace](idle-shared-regression/idle-dialog-shared/) are retained. This witness does not include Winmine, deliberately forced concurrent retirement, or all message and callback semantics, and therefore does not identify the separate intermittent 1400. No guest was rebuilt.

The reusable [idle/dialog shared-worker test](../../../tests/observation/verify-wow-idle-dialog-shared.ps1) was then run three times with fresh package identities and the same hash-checked prebuilt host and two guest fixtures. All three passed distinct task identity, 17 dialog creations, 16 explicit closes, each task retirement and registrar idle after dialog-task exit. [Per-run manifests, traces, guest reports and hashes](idle-shared-regression/idle-dialog-repeat/) are retained. This checks repeatability of this combination, not a deterministically forced root-capture/retirement race or the user Winmine failure.

An additional run after strengthening the formal test to assert nonzero idle callbacks, original vector restoration and all 17 dialog initialization hooks also passed; its exact final script and output hashes are in [final combination receipt](idle-shared-regression/idle-dialog-final/receipt.json). The previous three-run receipt records the earlier test-script hash separately.

The terminal-ownership call-chain review distinguishes three entrances to
`wow_window_words_terminal`: native `WM_NCDESTROY` observation on the task's
hooked thread, creation rollback, and pending-object collection. Collection is
called from borrow-scope release, graph/object preparation, cleanup, and task
retirement; it is therefore not equivalent to a single native-hook edge.
The candidate takes the shared runtime's exclusive execution lock when the
current thread has a runtime binding, and the publication lock protects the
binding/table transitions. It deliberately releases the runtime lock before
calling original control/dialog finalizers and reacquires it before table
retirement. Runtime unbind calls `retire_current_thread` before clearing its
thread-local binding, and that function rejects surviving windows owned by the
retiring task. These observations narrow the lifetime analysis but do not prove
that every stateful collection or callback has a bound runtime, nor that a
nested finalizer cannot interleave with graph publication. The retained 1400
contains no failing stage or window identity, and cannot establish this gap as
its cause. Existing fixture passes do not force this specific interleaving.

A current-source `menu-resource` selected-object run passed with a desktop
capable token, binding `wow_menu_client_refresh_all` to the selected provider
object (link map) and reporting the original menu resource/parser, native
return, tracking/reentry, detach and cleanup markers without fixture errors.
[Fixture output and hashes](idle-shared-regression/menu-resource/) record the
scope. The initial restricted-token run failed with desktop error 5 and is not
counted as a product failure. These controlled tests do not force a destroyed
window between graph publication and `GetWindowInfo`, do not identify the real
1400 source, and are not a product Full gate.

Two additional current-source task-lifetime fixtures passed: `task-lifecycle
-InitRollback` exercised two injected allocation failures, name release,
execution scopes, unbind refusal and idle-observer installation failures;
`process-retirement` exercised task/object/class refusal and retry, cleanup,
events and native ownership. The former uses an `init-release.obj` compiled
with the fixture's injection seams; the latter's link map identifies the
selected provider lifecycle object. [Outputs and input hashes](idle-shared-regression/task-lifetime/)
record both. These are controlled host-boundary tests, not actual guest failure
injection, a proof of all concurrent teardown, or a product Full gate.

The focused current-source idle hook fixture is a non-pass: its guard section
reported zero errors and its actual USER chain delivered 16 next-hook callbacks,
but teardown failed at fixture line 141 (`SetThreadDesktop(original)`). The
preceding `SwitchDesktop(previous)` check passed; the output does not record
GetLastError and therefore does not establish why thread-desktop reassignment
failed. This is an owned test-desktop teardown failure, not evidence of a
product idle-hook failure and not a passing test. [Output and hashes](idle-shared-regression/idle-boundary/)
are retained. The already-passing prebuilt shared-worker witness is independent;
it does not turn this non-pass into a pass.

A diagnostic rerun recorded Windows error 170 (`ERROR_BUSY`) on the fixture's
attempt to reassign its window-owning thread to its original desktop. The
fixture now runs its isolated desktop chain on a dedicated thread; it restores
the input desktop, exits the thread, and only then closes the test desktop
handle from the joining thread. Failed desktop assignment aborts the chain.
The final fixture passed both the guard and actual native chain, recording 13
next-hook calls and successful teardown; [diagnostic and final evidence](idle-shared-regression/idle-boundary/)
retain both outcomes. This repairs test lifecycle only; it does not prove a
product-wide hook ordering contract or the separate 1400 failure.

A final fixture review also made an unsuccessful thread wait terminate the test
process rather than let a still-running worker retain a pointer to its caller's
stack. The final source was rebuilt and both idle guard and native chain passed
again (`final-wait.txt` and `final-wait-hashes.json`); the exceptional wait path
was not injected.

The boundary-only ten-image candidate was independently retested with the
hash-verified prebuilt idle registrar and common-dialog guest in the same real
WOW worker. The guest created 17 dialogs, closed 16, let the last be retired
at task exit, and the registrar continued idle dispatch after the dialog task
ended; both distinct tasks recorded one successful retirement and their host
waits returned zero. [Inputs, manifest and receipts](idle-shared-regression/boundary-idle-dialog/)
are retained. This covers this shared-worker interaction, not a forced
1400 race, arbitrary application behavior or the Full product gate. Guest
images were not rebuilt or modified.

A read-only recheck of direct `build/t473-s11` child packages found no staged `system32/WOW32.DLL` with the historical `no-trace-package-manifest.json` sealed SHA-256 `DA278F7ADB404703A106876D33D968395E6B0CCB27BFA702B61A373EF88AF804`. This search does not cover nested directories or reconstruct an original ten-image package. Accordingly, earlier no-trace timings must not be presented as a fresh paired comparison with the current candidate solely because both probes use SOL. Reproducing an equivalent baseline would require the matching full package and identical test conditions; existing timing observations remain observational evidence only.

The current candidate's three-round 0/150/0 test-owned external-window probe passed and did not show an increase in this ordered run (open 2641/2031/2078 ms; close 2297/2219/2157 ms). [Receipt](../s11-foundation-evidence/atomic-task-retire/execution-check/sol-scaling/review.md) records limitations: this does not establish graph inclusion for each synthetic window or rule out scaling in other conditions. The seconds-long latency persists.

A new current-package SOL probe samples `GetProcessTimes` around each dialog. Three runs measured worker CPU deltas close to wall time (open 2234/1593/1766 ms vs 2406/1672/1828 ms; close 2016/1782/1812 ms vs 2078/1891/1968 ms). [Receipt](../s11-foundation-evidence/atomic-task-retire/execution-check/sol-cpu-sample/review.md) binds the staged manifest. This is process-wide accounting, not a function-level CPU profile, and does not prove all latency causes; it points toward CPU work rather than a simple timer wait.

A test-only PC sampler on the current candidate observed 115 game-thread locations during one successful SOL Options open/close. [Receipt](../s11-foundation-evidence/atomic-task-retire/execution-check/sol-pc-sample/review.md) includes native module bases, map hashes and nearest-symbol attribution. Repeated samples include session_thread_current, phyR, xtrn2phy, copy_window_observation and qsort plus win32u/ntdll. This is one thread and one interaction, not a stack or process CPU profile. It narrows where to investigate; it does not establish that an original CPU implementation is defective.

A subsequent bounded stack-word sample found possible return addresses in the
project-owned retired-window collector while execution was in ntdll. The
collector scans handle slots under per-slot shared locking on scope exit and
graph refresh. [Evidence and limits](../s11-foundation-evidence/atomic-task-retire/execution-check/sol-stack-sample/review.md)
describe why these words are only candidate return addresses, not proven
stacks or a quantified bottleneck. In contrast, the separately tested empty
physical-alias lookup [candidate](empty-alias-session-lookup.md) was not
adopted; its standalone microbenchmark improved, but its purported SOL
candidate comparison was invalid because the original archive object won.

An isolated lock-batching candidate for the retired-window scan passed a
focused reclaim fixture and 27 SOL interactions across A/B/A and B/A/B runs.
The initial apparent speedup did not repeat reliably in a later B/A/B run
without compiler activity. The table also has task/domain-owned writers, so
longer publication-lock coverage needs more than a lock-local proof. The
candidate was not adopted; [measurements and concurrency limits](../s11-foundation-evidence/atomic-task-retire/execution-check/retire-batch-ab/review.md)
are retained.

An isolated sampled-QPC diagnostic subsequently measured the actual
retired-window collector during three SOL interactions. It repeatedly scanned
about 3,329 slots, but estimated inclusive cost during later opens and closes
was approximately 49–56 ms per phase against 2.0–2.3 seconds wall time.
[Evidence](../s11-foundation-evidence/atomic-task-retire/execution-check/retire-profile/review.md)
includes sampling and nested-call limitations. This is a real cost, not
sufficient evidence that this collector is the dominant delay.

The native-return publication entrance was sampled independently during
three SOL interactions. Representative mapped callers include default window
processing, SendMessage, original control color and painting boundaries;
sampling, nesting and callback limitations are recorded in the
[caller receipt](../s11-foundation-evidence/atomic-task-retire/execution-check/native-return-profile/review.md).
The result does not justify treating those native calls as callback-free or
skipping the required guest return publication.
