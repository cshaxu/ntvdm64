# NTVDM worker owner

ADAPTER-WOW-107 checks incoming window references by walking the existing
borrow records for currently published generations plus the separate desktop,
rather than scanning every handle-table slot. Window preparation inserts the
record before publication and retains it until withdrawal. Retired and
unpublished generations are excluded as they were by the slot scan. This is
call-local traversal of existing authority, not a second registry. Focused
graph and reclaim fixtures pass; product-wide acceptance and timing remain
open. It changes no original mirror or guest bytes.

ADAPTER-WOW-106 allocates the 64 KiB call-local withdrawal bitmap only when
the window projection has a nonempty retirement set. No identities can be
withdrawn for an empty set; a nonempty set keeps the same validation and
rollback checks. The graph-snapshot fixture covers lifecycle and failure
cases; end-to-end timing and full package acceptance remain open. This changes
the host adapter only, with no guest or original mirror change.

ADAPTER-WOW-105 bounds the temporary withdrawal array by the existing
window-record list (capped at the handle-table size), with a guard before
writing. Newly prepared windows have no published slot and newly pinned
existing windows belong to the validated view, so neither adds a withdrawn
entry. In a sampled SOL run the allocation was about 1.5 KiB rather than
256 KiB; this is temporary allocation size, not a working-set guarantee.
Existing owner, failure rollback and native-window semantics remain in place.
The candidate [evidence](../../artifacts/mvdm-audit/s11-foundation-evidence/atomic-task-retire/execution-check/retire-capacity/review.md)
does not establish an end-to-end latency improvement; full package acceptance
remains open. No mirror or guest bytes change.

ADAPTER-WOW-104 reuses the call-local, already validated slot-to-row index
between full window-graph validation and its synchronous commit. Standalone
link replacement still allocates and validates its own index. The index is
never retained across a call, and guest execution or native callbacks do not
intervene between validation and commit. This removes one 256 KiB temporary
allocation per full projection without adding state authority or changing
mirror/guest bytes. The integrated candidate's bounded fixture and package
evidence, including duplicate-generation rejection, is recorded in
[S11 projection-index review](../../artifacts/mvdm-audit/s11-foundation-evidence/atomic-task-retire/execution-check/projection-index/review.md).
Full product acceptance remains open; no SOL latency improvement is claimed.

T473 S11 unpublished lookup candidate supplements ADAPTER-WOW-088/092 with
exact-address indexes of the existing WND-generation and CLS allocation
records. These are non-owning intrusive indexes, not sampled window-state
caches: the original lists still own pins, retirement, failed physical frees
and teardown. Successful insertions index the same record; final disposal
unlinks it, including rollback. Collision resolution compares full addresses.
No native HWND reuse, graph-refresh boundary or observable state policy changes.
Each index uses 1024 pointers, plus one link per allocation. The prior entries'
"no persistent index" descriptions apply to those earlier changes, not this
candidate. See the measured comparisons, failure/reuse tests and limitations in
[S11 performance evidence](../../artifacts/mvdm-audit/s11-dialog-performance/investigation.md).
The repeated graph traversal and intermittent dialog non-pass remain open;
source lookup speedup does not establish complete functional recovery.

ADAPTER-WOW-096 completes the080 allocated-view scalar read path in graph
publication, window-slot identity, borrowed-generation relation and handle
metadata consumers. Only domain-table or retained-record-owned storage uses
the existing bounded synchronous RAM reader. Arbitrary guest pointers, link
targets and non-owned/noncanonical CLS still retain scalar SAS semantics.
Every generation check, capture/commit, pin, retirement and failure remains;
no sampled field is cached, no boundary is skipped and no guest/mirror changes.
Focused exact-body and actual CCPU lifetime checks establish correctness.
Owner freezes performance experiments; P13 retains this without a speedup claim.

ADAPTER-WOW-092 derives owned-CLS membership once per complete observation
copy in call-local sorted addresses. It replaces repeated list walks, not
class storage/identity/lifetime or descriptor authority. Single reads and
non-owned/noncanonical data keep their scalar path; allocation failure falls
back to the existing lookup. Menu projection has no added class dependency.
Scratch is released on normal, failed and exceptional exits without catching
or replacing the original exception.
No persistent index, original source, guest, private export or wire change.

ADAPTER-WOW-091 resolves complete captured-graph identities from their already
acquired prepared backing before considering the published-slot lookup. Full
generation/graph validation, pins, withdrawals, rollback and commit order stay;
unstaged/public link callers keep the original lookup. No cache, counter,
freshness, private-export, guest or mirror change. Real CCPU whole-view five-
field readback joins existing legacy links, stale generation/retained reuse,
32 partial rollbacks and terminal negatives before full delivery.

ADAPTER-WOW-090 binds original WOW Hook registration to the documented native
local-thread contract. Only a live same-process thread and callback/module
matching the permanently loaded provider may use NULL native module; original
state/stubs/conversion/ordering/retirement remain. Global, foreign, unknown,
dead and mismatched inputs forward unchanged. One native call, exact returned
error, no filename test, retry, helper window, guest/mirror change or hook
transport. Query-only/synchronize thread handles close after the native call.

ADAPTER-WOW-089 separates the three-field committed window projection needed
by menu attachment refresh from full graph/class descriptors. Original menu
behavior, native GetMenu, refresh boundaries and local/external error policy
stay unchanged. One private typed export reuses current-generation traversal,
two-pass sizing, ascending order, errors and LastError; no extra parent heap
allocation, cache/registry, guest/mirror edit or reduced freshness. Test full
subset equivalence, cold desktop, capacity/refusal and retired/reused identity,
then the entire composed native/guest/menu/lifetime and release closure.

ADAPTER-WOW-088 carries an existing window lifetime record through the private
captured-graph transaction instead of repeatedly finding it by backing. The
original USER HMUnlockObject/ThreadUnlock1 use their already-known object; a
direct import would require NT4 private PHEAD/HANDLEENTRY/TL/kernel destruction
state, not modern HWNDs. This existing copied-generation facade preserves pin,
retired-reader and physical-failure ownership. Public arbitrary tokens still
use safe lookup. A call-local row index replaces one duplicate scratch array;
record association is not ownership until a pin succeeds. Constructor, commit
and cleanup share that actual pin; no persistent index/cache, lower freshness,
new mirror/guest/wire change or automatic UI-speed claim. Validate the entire
success/failure/reuse/class/native/guest closure before delivery.

ADAPTER-WOW-087 carries the original thread-private client view at the
original USER execution handoffs, not at resource-lock entry. The original
task pClientInfo owns compatibility version; suspended callback snapshots
borrow only already-retained generation storage. The one guest TEB exchanges
version/callback fields atomically after validation; shared desktop/self/delta
remain shared. No graph refresh, native HWND sampling, new cache registry,
scheduler or guest patch is introduced. Failed restore leaves view/output
unchanged; terminal thread abandonment cannot read a withdrawn CPU view.
OPENNT-HOST-124 registers the two guarded helper-boundary calls. Validate
actual CCPU field exchange, alias/failure/retired-pin behavior, original task
handoffs and complete native/guest/retirement floors before delivery. This is
not proof that the historical common-dialog0102 witness is repaired.

ADAPTER-WOW-086 bounds physical copied-CLS reachability to existing WND
lifetime records plus the desktop. These cover published/prepared and retained
retired generations; do not confuse them with the original managed class
association count, which remains unchanged. Native UnregisterClass still owns
registration. Original class.c DestroyClass/DereferenceClass's no-associated-
WND rule is retained; its private shared-heap references cannot directly own
these copied generations. Keep the existing source-shaped physical retention
mechanics, remove duplicate table scans and excess temporary capacity instead
of adding a second counter or heap. No mirror intrusion or guest change.
Verify live-class refusal, retired pins, repeat collection, failed free/retry,
replacement identity and full runtime floors before delivery; no performance
benefit is implied merely by the source reduction.

ADAPTER-WOW-085 removes repeated metadata searches during owned-view copying.
The published slot table remains authoritative. The existing borrow lifetime
list supplies candidate storage; only the current backing for its full handle
is copied. A cold desktop is copied separately. Table-derived required count,
capacity refusal, ascending slot order and LastError remain. No new persistent
index/cache or native-state freshness policy is introduced. Original NT4 USER
reads its shared WND directly (ntuser/rtl/getset.c GetWindowData); it has no
standalone copied-view traversal to import. Reconstructing that private shared
heap is outside the approved host boundary; optimize the existing bounded
projection instead, retaining original CCPU reads and ownership. No mirror
intrusion or guest change is needed. Verify current/retired generations,
capacity/order, original CCPU graph lifetime and actual paired input plus
the full retained release floors before delivery. This is not independently
claimed as the principal stutter cause.

ADAPTER-WOW-080 bounds copied-view reads to the existing page-domain owner.
The original CCPU40 c_GetLinAdd service is reused for immediate copies of
known allocated WND/CLS RAM that fits one translated page. This removes
per-field/per-character SAS translation for the same immutable-at-this-edge
storage. It is not a new memory mapper or persistent observation cache.
The exact current domain, session and outstanding backing/class lifetime
remain prerequisites; aliases never escape the synchronous copy. Non-fitting
spans retain the existing scalar SAS path. Native observation frequency,
publication, original scheduling and guest bytes/layout remain unchanged.
The copied observation and WW-view read closure is one verification group:
fresh fields, stale-native/committed-view distinction, retirement/reuse,
wrong-owner/directory refusal, failure and actual ordinary-desktop input
latency must pass before publication. Source and quantified baseline are in
the T447 S4 message-source evidence. This entry registers staged source, not
a passing performance result or release.

ADAPTER-WOW-081 extends that owned-view closure to field publication and
reference scans. Reuse original CCPU xtrn2phy/c_GetPhyAdd with the exact
bios_write_double access flags, only for a same-page range in the already
validated allocated WND/CLS RAM. Write only the previously assigned fields;
never overwrite other original private fields. Trace-enabled, cross-page or
failed translations retain original scalar stores. No writable alias escapes
the synchronous operation, no mapping or dirty-state cache is introduced,
and no guest, original memory backend or mirror is changed. Reference scans
use the same immediate read copy; original retention/free checks stay intact.
Required checks include fresh geometry/style/links, untouched private fields,
trace fallback, wrong-directory refusal, rollback/retirement/reuse and the
same actual-input comparison. This registers a candidate, not delivery.

ADAPTER-WOW-083 short-circuits only an empty EMS-alias list in the existing
checked physical-translation service. The list itself remains the authority;
there is no cached enabled flag or new memory manager. Default output is still
the input address, and all nonempty-list owner/session/range validation,
map/remap/unmap/teardown, physical-page lookup and original CCPU accesses stay
unchanged. Original CCPU's existing097 seam and normal-RAM fallback are reused;
no additional mirror or guest change. Verify empty/unbound/invalid/null cases,
nonempty multi-owner identity/range/remap/teardown, retained physical mapping,
original EMS and every-P DOS/WOW floors, plus a same-input timing comparison.
The inactive-alias predicate applies identically to scalar, RMW/string, stack
and fetch/control accesses through unchanged c_GetPhyAdd; there is no new
instruction profile, device path or per-family compensation. Registration
alone is not a passing test or measured speedup.

The Console input adapter bounds each original five-record read by available
relative-mouse FIFO capacity under the original ICA lock. Only the event
thread produces these samples; the CPU consumer frees slots. At capacity it
leaves records with the frontend, yields briefly and returns to the original
alertable suspend/input wait. It does not discard motion/button edges, merge
direction changes, enlarge the queue or change original IRQ timing. See
tests/observation/verify-window-mouse-pressure.ps1 and the S11 evidence ledger.

Original mouse IRQ cancellation flushes pending samples and button/delta
state, but does not disconnect the frontend input source. The bridge retains
the last accepted ENTER/LEAVE ownership so queued upstream movement or source
retirement remains valid after guest reset or original input suspension.
Successful DOS frontend deactivation is a distinct boundary: the frontend
discards its DOS mouse records, and console_client clears the worker route
only after acknowledging that handoff. Failed deactivation preserves it.

This component owns worker-local bootstrap, session, guest-memory and thread
bindings around the original MVDM execution core. It does not own frontend
Console/Window presentation or broker record policy.

`package_layout.c` and its header configure worker-owned media roots and validate
the original COMMAND configuration limits. They were moved from product-package
without changing path or failure behavior; they are not shared product ABI.

The win32 directory keeps private declarations beside their implementation:
environment projection, thread alert, WOW hard-error presentation and the scoped
original-host CRT redirect. Their historical divergence records remain indexed
in opennt-abi/host-compat/README.md. Shared original declarations and genuinely
multi-owner bindings remain separate; this relocation does not introduce a
generic compatibility library or modify original guest media.
