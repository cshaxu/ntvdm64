# T480 mirror-diff review plan

Owner admission: queue head, latest main157c31855, dedicated worktree.
The [proposal](../../proposals/proposal-unconnected-mirror-diff-cleanup-001.md)
supplies the review method; [Current](../../states/CURRENT.md) alone admits S1.
Main T479 remains independently active. Historical proposal queue positions
are superseded by this admission and the current queue.

| Stage | Bounded result |
| --- | --- |
| S1 | Freeze provenance and product input identities; inventory host mirror C/H differences and prior owner decisions, distinguishing format and semantics. No product edits. |
| S2 | Restore only proven unnecessary unconnected differences after checking every relevant consumer and configuration; preserve approved #error/#line exclusion policy. |
| S3 | Review connected differences by complete original owner and minimize safely. Retain necessary compatibility and proven original-bug fixes. Split into sequential bounded owner stages if needed. |
| S4 | Reconcile corrected prior decisions, affected tests and divergence registrations; hand broader autonomous-implementation questions to their separate queue candidate. |
| S5 | Verify finite difference dispositions, quantify reduction, run applicable delivery checks and record remaining justified differences and limits. |

No guest audit or changes, ASM cleanup, unsupported-profile activation, new
capabilities or extra tolerance. Start with mvdm-host/non-mvdm C/H; explicitly
classify other roots rather than silently broadening scope. Connection means
actual build/include/conditional/link ownership, not runtime trace frequency.
An absent MAP or a header not directly compiled is not exclusion evidence.
Do not restore original code blindly or discard required bindings for cosmetic
diff reduction. Preserve historical snapshots and add successor conclusions.

S1 admission does not promise a new audit-derived count or completed review.
Future actual product edits require appropriate build/tests and worktree-only
publication under the owner's local-only rule; no O:/winnt write or push.
