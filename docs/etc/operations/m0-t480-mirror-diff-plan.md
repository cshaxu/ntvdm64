# T480 mirror-diff review plan

Owner admission: queue head, latest main157c31855, dedicated worktree.
The [proposal](../../proposals/proposal-unconnected-mirror-diff-cleanup-001.md)
supplies the review method; [Current](../../states/CURRENT.md) alone owns the active S.
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

## Initial audit checkpoint

[Brief, current findings and owner-group sequence](../evidence/m0-t480-s1-brief-audit-plan.md)
records the current1060-file baseline and six issue/disposition groups.
The [S1 successor](../evidence/m0-t480-s1-build-consumers-closure.md) completes
actual builds and file/include/link-input classification. All581 differing files
have product consumers; no whole-file unconnected restoration is proved.
S2 starts with the bounded formatting leads and independently evidenced inactive
sub-blocks, not stale removed-file candidates. S3 retains semantic review of
connected and mixed exclusion blocks. S1 closure does not close that review.

## S2 admitted restoration boundary

Owner admits S2 after S1 d08e0ffc7, requesting the problem/design explanation.
This admission changes documentation only, not mirror source or release assets.

S1 found no unconnected differing whole file. Accordingly, S2 does not promise
a functional restoration or deletion of an entire adapter. Its finite primary
input is the 146 whitespace-restoration-lead IDs in the S1 classification ledger.
They include four confirmed candidates, not 146 proved safe changes.

1. Restore F01-F03 in dos/command/cmdenv.c: original indentation of the
   cchRemain check, setBX publication and DWORD Length declaration. Keep all
   tokens, physical line counts, existing bindings and #line directives.
2. Restore F04 in softpc.new/base/system/qevnt.c: original spaces inside the
   lock-contract comment, retaining the #line and conditional structure.
3. Review the remaining 142 IDs once. Restore only non-semantic differences
   verified against original bytes and actual context. Blank-line, comment,
   directive and macro-continuation cases need special care: apparent whitespace
   equivalence does not prove unchanged preprocessing or diagnostic mapping.
   Record restore, retain or transfer-to-S3 explicitly rather than enlarging the
   task into an unbounded new audit. Candidate overlap does not create another
   change or require an already completed fix to be repeated.
4. An inactive substantive sub-block can be restored only if concrete compiler
   and all-consumer evidence emerges during this finite review. Otherwise record
   it for the existing semantic stage; lack of a runtime hit is never evidence.

Already handled vdmtib.h/wkglobal.c/wklocal.c/nt_aorc.c removals and approved
SafeLoadLibrary/i386, PIG, BIGEND and diagnostic #error exclusions remain outside
the edit set. Do not revert active keyboard sas_loadw expansion, DPMI stack-info
publication, floating-point exception reads, softpc.h calling conventions,
VCDEX WINAPI or recovered apisrv.c merely to shrink the diff. These are not
proved S2 restoration candidates. Guest source is neither read nor changed; its earlier
owner-requested submission was a separate completed operation.

Success is fewer unnecessary mirror differences with unchanged product behavior,
not new capability or generic fault tolerance. Reuse S1 evidence after identity
checks; build/test/publish actual product edits under the local-only rule and
verify affected preprocessing/line mapping. Do not rebuild or publish merely for
this documentation-only admission.

Owner subsequently authorizes S2 implementation through local build, test,
assets/release publication and commit. The [implementation evidence](../evidence/m0-t480-s2-whitespace-restoration.md)
records all146 candidate dispositions. Established tests may consume immutable
runtime media copies, without admitting guest source auditing or changes.

## S2 closure

All146 finite candidates are dispositioned:53 restored in30 files,70 retained
for existing diagnostic layout and23 retained for logical-line preservation.
All819 actual consumers are equivalent; both builds,16 Full gates and WINMINE
gameplay passed. All11 tested images were published to this worktree
assets/release; no guest changes, O:/winnt writes or push. The linked S2
evidence contains complete receipts. S3 remains unadmitted; S2 has no open item.
