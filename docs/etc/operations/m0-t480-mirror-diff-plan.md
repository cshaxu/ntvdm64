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

## S3 admitted connected-difference review

Owner admits S3 after b35effafd. This supersedes the preceding unadmitted
status without rewriting its historical closure. Admission is documentation-only.
There is currently no newly proved semantic restoration; S1 reachability and
S2 whitespace equivalence do not establish that connected edits can be reverted.

| Sequential owner group | Question and prospective reduction | Required retained boundary |
| --- | --- | --- |
| DOS/COMMAND/XMS/DPMI | Are edited control/data paths redundant now that original providers are connected? Restore original calls or narrow conditional changes only after proving the same registers, results and lifetime. | Keep CPU40 PmStackInfo publication and completed T478 repairs unless actual counterevidence appears. |
| SoftPC CPU/devices/video | Can changed CPU40 selectors, writer declarations/table carriers or host seams be narrowed without replacing original algorithms? | egawrtm0.c/egwrtm12.c active carriers are not dead code; preserve selected CPU40 and keyboard sas_loadw semantics. |
| WOW32/USER and shared headers | Which declaration/call adaptations are necessary for actual consumers, and can any unnecessary scope be removed? | softpc.h has148 object consumers and WOW32 uses /Gz; explicit __cdecl cannot be removed wholesale. Preserve task/callback/resource contracts. |
| Redirector/network | Separate necessary host I/O and async bindings from redundant changed code, including live content adjacent to exclusions. | Preserve completion ordering, ownership and error semantics; missing trace hits prove nothing. |
| non-mvdm services/declarations | Determine the minimum imported source-shaped binding for selected service owners and shared declarations. | Recovered apisrv.c is original functionality, not a redundant adapter; no recursive CSR/kernel/USER-server import. |
| VCDEX | Check whether differences exceed the finite VDD/host boundary. | WINAPI entry ABI is necessary; no renewed guest-driver or physical-device capability campaign. |

Each group uses S1 stable hunk IDs, complete functions/headers and actual
consumers, rebound to S2 hashes where changed. The review covers2132 substantive
blocks and connected content within3680 exclusion-bearing blocks; these are
lexical groups, not counts of defects or promised edits. Existing #error/#line
policy stays intact. Pure diagnostic blocks and all146 resolved S2 whitespace
IDs are excluded. Historical deleted-file candidates, SafeLoadLibrary/i386,
PIG and BIGEND exclusions are not resurrected.

Restore/minimize is allowed only with original-source identity and proof that
all supported consumers keep the required ABI, outputs, state and lifetime.
A smaller mirror diff obtained by copying original algorithms into adapters is
not minimization. No new behavior, additional tolerance or guest work is
admitted. Exact safe-edit counts will follow the owner-level evidence; do not
present these investigation questions as already proved corrections.


## S3 bounded closure

Owner subsequently admitted implementation, including unnecessary structure and
symbol-name differences. [S3 evidence](../evidence/m0-t480-s3-connected-restoration.md)
records the conservative 5,812-ID disposition and proved restorations in13 host
C/H files. Both builds, focused contracts, final Full16/16, gameplay and all11
worktree-only publication hashes passed. First non-passing attempts remain
recorded. Retention without safe-reversion proof is not a claim of minimality.
S4 and S5 remain planned and unadmitted; no completed recovery is repeated.


## S4 admission

Owner admits the next planned S after S3 commit511f23fc0. The active packet in
Current owns S4: reconcile S2/S3-affected divergence registrations and test/evidence
claims against the delivered source. Prior snapshots remain historical; add or
narrow successor descriptions rather than erase past conclusions. This is not
another full semantic audit or permission to revert the3,641 conservatively
retained blocks. S5 aggregate review remains unadmitted.

Start with the narrowed DIV-582/DIV-869 selector account, preserved
OPENNT-HOST-065 environment width boundary, recovered RTL1970 implementation,
and S2/S3 comment/layout claims. Then check the finite union of affected files
for other stale registrations, source hashes and test references. Product
behavior is not scheduled to change; compile/test/publication is required if
an actual product change is subsequently justified, not for this admission.


## S4 closure

Owner subsequently authorizes implementation through closure. The
[S4 reconciliation](../evidence/m0-t480-s4-registration-reconciliation.md)
checks39 affected host files, both sealed archives,819 S2 consumer records,
six final environment identities and11 unchanged release hashes. Successor
notes narrow DPMI claims, correct environment path/hash attribution and explain
historical verifier revision requirements. No product or guest change, rebuild,
runtime retest, publication, O:/winnt write or push was needed or performed.
S5 remains planned and unadmitted; conservative S3 retention is not minimality.


## S5 admission and implementation boundary

Owner admits S5 after465ba5dcb and authorizes implementation through closure.
Recompute the complete host C/H comparison, reconcile stage identities, and
look for remaining safe expression/layout/structure/naming reductions. Earlier
completed changes are not repeated. Conservative S3 dispositions alone do not
prove minimality. Validate any new edit against original ownership and all
actual consumers, then use the existing coherent build/test/local-publication
workflow. Guest and new capability remain excluded; no O:/winnt write or push.
