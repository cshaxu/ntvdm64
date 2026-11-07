# Project Status

## Current Work

**Active: M0 T481 S3 — Win3.1 VM I/O and nested-chain completion.**
Owner expands T481 to own the complete queue-first guest multitask completion
package. S1 integration delivery is committed and pushed at ceea7f1f8; its
package remains the comparison baseline, not proof of complete VM I/O.
The [working plan](../etc/operations/m0-t481-multitask-completion-plan.md)
retains every T479 S6 obligation and the remaining sequential stages.

## Active Packet

| Field | Record |
| --- | --- |
| Candidate Proposal | Queue-first guest multitask I/O completion, now admitted into T481. |
| Identifier Mode | M0 T481 S3, Ordinary Mode. |
| Admission And Approval | Owner directs this T to finish the queue-first task and sets active goal: complete multitask payload functionality. |
| Objective | Complete remaining reached Win3.1 input, text, geometry, fonts, codepages, display/metadata and nested chains without suspending unrelated VMs. |
| Non-goals | No original guest modification/rebuild; no Win95-first work, hardware V86 monitor, speculative protocol completion or new lifecycle authority. |
| Reference Baseline | f25220d98; S2 published package8b216ec7-75d1-4979-a21e-c588ca4d02d6. |
| Files And ABI Surface | Authored ntvdm64-386, NTVDM VM text/I/O/frontend, NTVWM presentation and existing RPC/I/O callers; exact changes follow caller/provider audit. |
| Applicable Rules | Execution, architecture, coding, source policy and the approved BIOS/INT16 input-return boundary. |
| Verification | Reached caller/provider matrix; actual Win3.1/native I/O acknowledgements, nested chains in both completion orders, input/output/redirection and cancellation; cached builds and applicable Full gates. |
| Expected Markers | No lost returned keys, no cross-VM delivery, no stranded worker or blocked retirement; actual guest receipts rather than dispatch-only coverage. |
| Asset Needs | Existing immutable Win3.1 media and owned probe runners; independently authored addon build. |
| Reporting Requirements | Selected code versus candidate; caller/provider evidence; exact open failures and coherent package hashes. |
| Stop Conditions | New semantic concession or original guest patch required; retain failing evidence and continue independent work. |
| Exit Criteria | Required Win3.1 operation and nested-chain matrix proved; coherent host/addon publication, review, commit and push. No known foundation defect transferred to force closure. |
| Original Owner Request | This T must also own complete queue-first multitask payload completion. |
| Similar-Issue Sweep | VM generation reuse, all four cancellation owners, input ownership, nested handback and NTSRV coordination. |
| Temporary Evidence | build/t481-s3, task-owned runs; 24-hour/4-GiB review budget, inspect no-progress runs at their contract deadlines; archive required evidence before disposal. |

## Current Technical Baseline

S2 P1 package8b216ec7-75d1-4979-a21e-c588ca4d02d6 is published at both
release destinations: all11 host images, Full04116/16, final production VM
retirement/reuse, diagnostic unread-input handback and normal Win3.1 exit.
Win3.1 ntvdm64.386 is published only in assets/release. This is a bounded
verified baseline, not complete VM I/O. S2 is closed with owner-approved historical failure disposition in the working plan.


## Recent M0 Closures

- [T480](../history/m0-t480-closure.md): adjacent audit closure and qualification limits.
- [T479](../history/m0-t479-closure.md): owner-directed closure; unfinished guest multitasking is preserved in the queued successor.

## Recent Governance

S2 P1-P5 are committed/pushed through f25220d98. Cached builds, Full04116/16,
real input/lifecycle probes and input0573126 assertions per width qualify the
bounded S2 closure. Owner directs that unreproduced fill002/119 remain history,
not active defects or TODO; investigate only on a current reproducer. S3 is
admitted under the existing sequential plan and continuous-execution approval.
No whole-T completion or Win95 qualification is claimed.

## S3 Progress

Previous committed P2 dc1adea38 is complete11 package
b3ea681a-18f4-490b-960c-b1acd249eeba, matching both destinations. Full04716/16
and real editor049 keyboard menus/normal exit qualified that bounded baseline.
It does not contain the current mouse changes.

Current candidate retains original NIDDB/VMD mouse ownership, allocates child
instances at Create_VM before BIOS work, scales relative input on the CPU owner
using the selected original mouse instance, and transports exact-VM/token mouse
records through the existing I/O queue. Mouse modifier decoding reuses the
keyboard helper. No original guest or MVDM mirror source is changed.

- Cached139 complete x86/x64 builds and shared tests14/14 per width pass.
- Mouse142 x86/x64 unit tests cover queue return, identity, lock toggles and
  right Ctrl/Alt. Candidate143 builds the current host and authored .386.
- Actual editor144/145 pass both orders: separate native EDIT.EXE and DOS
  EDIT.COM VMs, mouse File/Exit, both completions and normal Windows exit.
- Editor146 DOSONLY1 passes guest EDIT mouse interaction, native EDIT rejection,
  zero whole-run host command admissions and normal Windows exit. Its actual
  NTSRV snapshot has only root NTCON and DOS worker: no VM payload frontend,
  native task or nested worker. Ordinary guest DOS VM concurrency is permitted.
- Full140 passed16/16 for the pre142 frozen package. Full147 passed16/16 against
  the current candidate; old Full results cannot qualify the new bytes.
- Governance149 passed. Publication150 completed: complete11 package
  42a32c73-c220-4be6-98b6-de443efdc6cc matches both destinations; two
  addons are in assets/release only. P3 review is in review151.json; committed and pushed as 4895e93c4.

Evidence is in artifacts/t481-s3/editor144.json through editor146.json,
mouse-modifiers142.json, full140.json, full147.json and publication150.json; the working plan retains earlier
controls and failed observations. Editor132's once-observed wrong callback
column remains unattributed; later repeated reverse-order tests pass. Do not
label that old failure fixed or use it as proof of a product cause.

S3 remains active: complete the reached operation/geometry/font/codepage,
display/metadata and nested-chain obligations from the working plan. The mouse
candidate is a bounded P delivery, not S3 or whole-T closure. S4 Win95 and S5
combined qualification remain unchanged. Addons publish only to assets/release,
not host system32; the paired candidate requires original device=*vmd loading.

Operation witness153 passes both editors with read-only dispatch observations.
BUFFER_SIZE, GET/SET_TITLE_A, SET_POINTER_CLIP and KEYBOARD_LAYOUT reach the VM provider and
return ERROR_NOT_SUPPORTED. They remain S3 gaps; overall editor success does
not qualify these operations. No font-query hit was observed in this case.
Evidence: artifacts/t481-s3/io-operations153.json. Diagnostic-only changes are
unpublished; the complete P3 package remains the default.

Parameters155 confirms BUFFER_SIZE is the existing80x25 and clipping is NULL
release. Candidate156 reuses exact-extent validation for the identity request;
different sizes remain unsupported. Dual-width text156 passed169 assertions each; real editor158 passes. Not yet
qualified or published. Other S3 gaps remain unchanged.

Title candidate161 separates complete API title from existing bounded caption,
with native160 small-buffer/empty-title controls and double-width unit passes.
Final cleanup review, dual-width tests and actual editor163 pass. Title GET/SET
and identity BUFFER_SIZE now return0; evidence: title163.json.
It is unpublished; it does not implement guest USER title changes. Layout and
clip ownership remain open.

Nested164 failed: B returned MEM output, A and final completion did not finish
within the90s test contract. Cleanup also reported nthook64.dll occupation.
Evidence: artifacts/t481-s3/nested164.json. Basic nested control165 passed both MEM output returns and normal Windows exit;
no root-cause or passing claim, and no new default publication.

Deep-chain repeat166 passed both output returns,320 callback checks and normal
Windows exit; it does not erase unexplained164. Evidence: nested166.json.
Cached167 (PID26488) and dependent Full168 (PID25276) qualify the current
complete package; Full waits for successful cached completion. Cached167 passed (14/14 shared tests per width); Full168 passed all16 gates; evidence: artifacts/t481-s3/full168.json.

Deep chain171 reproduced the timeout; B completion marker was absent, so the
optional task-tree trigger did not fire. Evidence: nested171.json. Test-only
failure capture now queries NTSRV before cleanup; repeat172 PID19760 is running.
Full168 remains passing, but the nested failure is unresolved and unpublished.

172/174/175 passed;173 timed out. Task-tree evidence is nested172-174.json.
Diagnostic176 built successfully, adding read-only READ_INPUT/PREPEND_KEYS
record tracing. Run177 PID3656 is active to test the unused-input hypothesis;
no root cause or new product fix is claimed. Full168 bytes remain frozen.

Run177 reproduced the failure with live registry capture: native CMD remains,
nested DOS worker is EMPTY. Input tracing observes exit later read as texit;
see nested177.json. Attribution to hardware/BIOS return or VM adaptation is
still open. No product changes after Full168 and no new publication.

Origin178 extracts the production keyboard paths and fails the added
read-before-EOI unused-input assertion. output_full remains true after data
consumption, and PendingKeyboardHistory still selects its origin. Evidence:
artifacts/t481-s3/origin178.json. This is a boundary defect witness, not yet
causal proof for nested177; product source remains unchanged.

Diagnostic179 built; nested180 reproduced texit and EMPTY nested DOS/CMD-active
state. The keyboard wrapper linked but emitted no boundary trace; child trace
availability is unproved. Evidence nested180.json retains this observation
limit. No live run remains and no new publication occurred.

Nested182 reproduces texit in both VMs with working child keyboard tracing.
Every observed handoff has unread full output or retired output: no read/EOI
gap. Origin178 is not the demonstrated cause. Next boundary is native input
admission/return. See nested182.json. Diagnostic181 build and run are terminal.

Reset188 proves the production PIC cancellation callback schedules the next
keyboard byte before whole-device reset clears the queue (89 assertions,1
failure). Candidate189 captures IRQ ownership, empties the device first and
then cancels:89/89 pass. DIV-321 updated; original guest unchanged. Diagnostic190
PID4284 and dependent nested191 PID18692 verify the real chain. Evidence
reset189.json; publication pending. Origin178 remains a separate open defect.

Diagnostic190 completed; nested191 passes both MEM outputs, all320 callback
checks and normal Win3.1 exit. Both traced queues retain exit with no texit.
Evidence nested191.json pairs with failing reset188 and passing reset189.
Repeat/full affected regressions and delivery remain pending; no S3 closure.

Nested192 repeats the passing deep chain. Origin193 now passes the separate
read-before-EOI origin exclusion and all previous origin cases; DIV-317 updated.
Evidence repeat193.json. Combined candidate cached194 PID10708 and dependent
Full196 PID12384 are running. Review must restore the original diagnostic
line after the two-line DIV-321 insertion before final delivery; this is not
a behavioral change. No new publication or S closure claimed.

Cached194 completes successfully: full cached components, shared14/14 per
architecture and keyboard/PIC89/89. Full196 remains active (child5384); live
frontend fixture observed. Evidence cached194.json. No restart or gate waiver.

Full196 remains active. Final197 PID26912 waits for its successful receipt,
then restores original #line1163 after DIV-321, rebuilds diagnostic197 with
the production NTVWM, and serially runs editor198, DOSONLY1 editor199 and
nested200. Each step requires exit0; no source change occurs before Full ends.

Full196 passed16/16. Final197 completed: original #line1163 restored, editor198,
DOSONLY1 editor199 and nested200 passed on the final candidate. Package201 is
ready:10 input hashes identical to Full196; NTVDM differs only in COFF/debug
link timestamps after package-ID normalization (4 bytes). Evidence final201.json.
P4 governance/publication/review/commit remain pending; all broader S3/S4/S5
obligations are retained.

Publication203 completed: package c7f7cdca-4214-4452-a01d-d2f1c3858d25,
all11 host images and manifest hash-verified at assets/release and
O:/winnt/system32; unchanged addons remain assets-only. Evidence:
artifacts/t481-s3/publication203.json. Governance202 passed. Production diff
review confirms reset callback ordering and consumed-input exclusion; same-size
BUFFER_SIZE acceptance is not arbitrary resizing. Commit/push remain pending;
S3 remains active with its full remaining matrix.

P4 committed/pushed at6f0c60bfa; working tree was clean after delivery.
Remaining-owner audit205 is recorded in the plan: original layout-query
fallback is distinguished from actual layout support; geometry requires guest
application, and guest codepage is authoritative. Worker-close205 PID8696
runs the proven isolated lifecycle recipe on final package197; result pending.

Worker-close205 terminal exit0: actual ordered VM native chains retire through
NTSRV managed worker close; original installation unchanged. Evidence
artifacts/t481-s3/worker-close205.json. This is not a normal-exit or full nested
cancellation-matrix claim. Source remains P4; further S3 work remains active.

Lifecycle206 completed exit0: service-loss206 and console-close207 both pass
real ordered VM task retirement, with original installation bytes unchanged.
Evidence artifacts/t481-s3/lifecycle206.json. New test-only nested redirection
witness adds actual file/append/pipe assertions in separate VM directories.
Diagnostic208 and dependent redirect209 run under owned PID25100; results
pending, no production change or new publication.

Diagnostic208 and redirect209 terminal exit0. Both real nested DOS/native
chains produce MEM files, append their own A/B marker and pipe filtered output
back to the correct guest screen;320 callback checks and normal Windows exit
pass. Evidence artifacts/t481-s3/redirect209.json. Synthetic provider input
is explicitly not physical-keyboard proof. Changes are test/evidence only;
P4 remains the published production baseline.

P5 test-only qualification: diagnostic208, redirect209, original nested210
and governance211 all exit0. Test input mapper changes preserve original
nested behavior. No production source/build input changed; existing published
package203 is reused subject to final both-destination hash check. Evidence
artifacts/t481-s3/qualification210.json. P5 does not close S3.
