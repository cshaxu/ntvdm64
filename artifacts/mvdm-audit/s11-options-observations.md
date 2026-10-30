# S11 SOL Options observations

The provisional ten-component package's visible probe passed WINMINE and SOL
gameplay, but Options failed in its first round. The retained
[window trace](s11-options-witness/windows.txt) records a dialog child creation
returning `ERROR_INVALID_STATE` (5023). This is an actual creation failure,
not merely a screenshot omission; the exact failed internal predicate was not
recorded by that build. The [package manifest](s11-options-witness/package-manifest.json)
and [process receipt](s11-options-witness/probe-processes.json) bind the run.

In a separately sealed, nonpublishable diagnostic package, a later failure
reached `wow_window_words_attach_client_state`: its first
`page_domain_menu_context()` evaluation returned false. The subsequent
diagnostic read showed active state, matching session owner, paging and CR3;
those later reads do not establish which earlier predicate failed. See the
[trace](s11-options-combined-witness/windows.txt) and
[package identity](s11-options-combined-witness/package-manifest.json).
Another run created its dialog controls without a recorded 5023 but the probe
did not observe a visible dialog; that observation needs separate attribution.

The next diagnostic version captured the individual short-circuit decision
values at the first evaluation. All four Options rounds passed, so it did not
capture a reject and does not prove a repair. Its [receipt](s11-options-context/probe-result.json),
[trace](s11-options-context/windows.txt), exact package manifest, source
hashes and diagnostic-only patches are retained in the same directory. None
of these diagnostic binaries is a release candidate. Continue to distinguish
the proven 5023 creation failure, first-context-rejection evidence, and the
separate unobserved-dialog outcome; do not assume they have one cause.

A further 16-round run of the same decision-point diagnostic completed with
exit zero; its [receipt](s11-options-context-r3/result.json) and observation
files are retained. No rejection was captured. Repetition does not disprove
the earlier failure or identify which initial context predicate failed.

A second 16-round diagnostic run also completed with exit zero. Its
[trace](s11-options-context-r4/windows.txt) contains two `ERROR_INVALID_STATE`
reports from the borrow-scope retirement collector while the Options rounds
continued successfully. These reports are a separate source-level lead; no
evidence yet establishes which collector failed or that either report caused
the earlier window-creation failure. The first-evaluation context diagnostic
did not report a rejection in that run.

The final ten-image candidate passed WINMINE and SOL gameplay but its Options
probe failed in round one because the worker exited. The [trace](s11-options-final-failure/windows.txt)
shows the dialog and child creation calls succeeding, followed by
`publish-native-return` reporting status 1400; the launcher observed exit
1067. This is not the earlier recorded 5023 child-creation failure, and the
specific failed publication step remains unproven. The exact candidate
manifest and process observations are retained with that trace.

A further nonpublishable diagnostic instrumented the three publication steps
and the page-domain context decision. Its first Options dialog opened, but the
close/recovery probe failed; no publication-step or context-rejection marker
was emitted. The [probe result](s11-options-publish-diagnostic/result.json),
window trace and package manifest are preserved. This is another observed
non-pass, not an identification of the earlier status-1400 failure.

A control package built from hash-checked current published images also
opened Options and then failed the close/parent-recovery probe in its first
round. The [control result](s11-options-published-control/result.json),
[trace](s11-options-published-control/windows.txt), resealed package manifest
and original input hashes are preserved. This confirms that this particular
observable non-pass is not unique to the new candidate; it does not establish
that its underlying cause is the same as the candidate's worker exit.

The two diagnostic `wow_user_borrow_scope.c:146` reports are emitted by
`wow_user_borrow_leave` after a scope has already been consumed. Its production
`wow_user_borrow_leave_required` intentionally treats this return as deferred
collector work rather than a fatal unmatched-scope error. The logs do not
identify which collector returned false, nor prove it was a leak or the
Options failure; inspect retirement and later collection separately.
