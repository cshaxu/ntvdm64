# T447 S2 audited development checkpoint

## Authority and delivery classification

Owner request: "批准 请你修改 并且审计工作区未提交的文件一起提交干净再开始继续执行".
Adopt the [cross-T plan](../operations/t447-wow-execution-plan.md#owner-requested-cross-t-planning-audit)
and save the reviewed implementation, fixtures and research together before
resuming runtime implementation. This is a source-development checkpoint,
not an accepted production P, S2 closure or release. The accepted thirteen
host images remain the T446 package ca0abb7c4; no release binary or installed
system32 file is changed by this checkpoint. Full production gates remain.

## Dirty-file audit and disposition

One implementer performed a separate review pass over the tracked diff,
untracked source inventory, build selections and existing evidence. All dirty
paths belong to T447; no unrelated user change or generated binary is selected.
The commit's exact file list is the inventory; the groups below account for
every source/build/test/document family rather than hiding them in a docs commit.

| Group | Files/surface | Disposition and proof limit |
| --- | --- | --- |
| Planning | Current, Queue, T447 plan, WOW program and affected W01/W02/W03/W06/W09/W17/error proposals | Adopt six-stage sequence, bounded transfers and residual ownership. No queued family is silently closed. |
| Research | Supporting index, message-boundary evidence and native-execution design | Preserve source provenance, rejected hypotheses and unresolved identity/early-reply requirements. Later explicit corrections supersede historical proposals. |
| Original callback/hook | mvdm README, wcall16.c, wowhooks.c; callback-frame source/header and sim32 comment | Restore original resolver, remove autonomous current-mode acquire_vp, retain bounded caller-field snapshot; hook wrapper pairs scopes. DIV-178 updated and DIV-332 registered. Guest unchanged. |
| Shared view | Worker page-domain source/header, original user.h declarations, layout header | Named CLS, high server-form address domain, child IDs and MENU/ITEM query storage. Real CCPU fixtures prove bounded layout/publication/release, not full guest execution. |
| Classes/private access | Original ntcftxt.h subset and provenance; class client/words/private access sources and headers | Original GetClassInfo composition, completed-registration projection, native CPD isolation and logical procedures beneath native subclass carriers. Mutation/lifetime completeness remains an S2 obligation. |
| Standard classes | New standard-class source/header and class-storage extensions | Reuse original walias table; nine native descriptors, borrowed metadata, cleanup restore targets and retirement fixtures. Prepare/cleanup setup not yet production-initialized; do not claim control integration. |
| Creation/dialog/task | Window/dialog creation, window words, registration bridge, lifecycle, cleanup sources and matching headers | Callback scopes, CBT rollback, native-dispatch split, class menu association, retry only launcher notification. Normal process-level lifecycle disposal remains to be connected. |
| Menus | New client/native menu sources/headers and cleanup binding | Query graph and destruction evidence retained. Native mutation redirects are not enabled in original parser/WOW boundaries. Post-mutation projection failure can leave stale guest state; no production-complete claim. Callback-time refresh is already connected and requires regression evidence. |
| Build | wow32-dll/sources and both Ninja generators | Select the added provider objects, worker imports and test closure; no guest build or root-level output. Preserve established incremental roots. |
| Tests | Changed/new adapter fixtures, real CCPU page-domain fixture, task-lifecycle/profile/spool observation scripts | Keep normal/negative/lifetime assertions and explicit mock distinctions. Static import cells reference real worker definitions, not replacement LDT/interrupt logic. Original WINSPOOL staging is restricted to build-owned package, hash checked, with original ExtTextOut limitation retained. |

Mirror change budget relative to pre-S2 HEAD: wcall16.c +18/-26;
wowhooks.c +2/-1; original ntcftxt.h retained subset +48; user.h declaration
subset +75 (MENU/ITEM +60, FNID +15). No guest binary diff. This is a diff
inventory, not a claim that imported declarations are a working USER server.

## Tests and remaining gates

Prior focused build/fixture records are retained in the
[boundary evidence](m0-t447-s2-message-boundary-audit.md). The newest successful
pre-checkpoint run is build/M0-T447/S2/native-class-projection: full worker and
provider builds, class-query/class-client/window-borrow checks, real CCPU
class publication and governance. These do not certify the three applications.

The checkpoint rerun uses build/M0-T447/S2/checkpoint-review-r2, hidden
PowerShell PID15568 started 2026-10-14 17:25:27 local, with driver stdout/stderr,
worker/provider logs and suite.exit. Its predecessor PID21588 at 17:24:34
failed before compilation because the new output directory was absent;
retain that harness failure separately, not as a source regression or pass.
The rerun's review deadline is 17:40 local; no foreground build/test is used.

Result: worker/provider build result.exit=0; class-query, class-client and
window-borrow all pass. Exact markers include WOW_CLASS_QUERY errors=0,
WOW_CLASS_COMMIT errors=0, WOW_CLASS_COMMIT_CCPU errors=0,
WOW_NATIVE_CLASS_CCPU errors=0 projected=9, WOW_WINDOW_BORROW_CCPU errors=0,
and WOW_PAGE_DOMAIN_LIFECYCLE_OK. The aggregate suite.exit is 1 because its
final documentation check rejected numeric task references in Queue, not
because compilation or a fixture failed. Corrected Queue names/links then
passed independent governance rerun PID25972 (17:28:34; governance-r2 logs);
both processes are exited. Keep the failed aggregate record unchanged.
The earlier parallel governance PID11828 has the same now-corrected finding.
Final diff whitespace check passes. No production inputs changed after the
build/fixture run; subsequent edits only record planning and these results.

Latest ordinary application evidence remains class-name-apps: WINMINE reaches
its main window; SOL reports OOM; WRITE reports insufficient memory with a
hidden MSWRITE_MENU window. No later playable-WINMINE or SOL/WRITE success,
DOS17, coherent publication or final S2 acceptance is claimed. Before a
production P, rebuild affected closure, run all required independent app and
DOS regressions, publish/hash-verify the coherent package, and review delivery.

## Next dependency-complete work

S2: pair production class/window foundations with ordinary retirement, not a
bootstrap-only toggle. Existing page-domain disposal already frees its class,
window and menu allocations; absence of lifecycle_dispose is not proof of a
post-worker guest allocation leak. Do not introduce a duplicate allocator sweep.
S3 retains prepared control/menu work and must fix mutation failure coherence
before enabling redirects, plus original EDIT/internal-child integration.
Full message contract remains S4 and task-targeted exit S5. No new runtime
repair is made as a side effect of this planning/checkpoint commit.
