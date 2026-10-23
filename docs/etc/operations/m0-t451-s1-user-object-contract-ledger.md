# M0 T451 S1 — USER object and shared-view contract ledger

## Purpose and method

This is the bounded, source-first inventory required before T451 changes
production behavior.  It reconciles the residual part of C04--C07/object-C10:
the parts that T447 deliberately did not close as a blanket claim.  It does
not reopen T447/T449's accepted local execution foundation, and it does not
alter guest source, guest binary, guest configuration, or host product code.

The reviewed baseline is `61a1698f6` (which contains the accepted ten-image
`6c661b579` package).  Primary inputs are the immutable original USER16
client, the selected WOW32/page-domain implementation, and the T447 transfer
audit.  A source hit is not treated as runtime proof: each row below names its
existing executable evidence and its missing acceptance evidence separately.

The governing ownership rule is:

```
modern USER (HWND lifetime, geometry, messages)
        -> bounded public sample
        -> worker-owned copied original-layout page-domain view
        -> unchanged USER16 PMODE32 reader / original WOW32 algorithm
```

The page domain is a projection, not a second USER server.  It must never
store a native pointer in guest memory, own a foreign HWND, or turn a sampled
external object into a task cleanup/callback object.

## Direct-reader denominator

`src/mvdm/wow16/user/usercli.asm` selects these immutable USER16 client
thunks: `GETDESKTOPHWND`, `GETDESKTOPWINDOW`, `GETDLGITEM`, `GETMENU`,
`GETMENUITEMCOUNT`, `GETMENUITEMID`, `GETMENUSTATE`, `GETNEXTWINDOW`,
`GETPARENT`, `GETSUBMENU`, `GETTOPWINDOW`, `GETWINDOW`, `ISCHILD`,
`ISWINDOW`, `ISWINDOWENABLED`, `ISWINDOWVISIBLE`, `ISICONIC`, and `ISZOOMED`.
For the graph-sensitive set, `usercli.asm` additionally selects local
PMODE32-client branches; the corresponding WOW32 fallbacks are guarded out
for PMODE32.  Thus a new host thunk cannot repair a stale WND/MENU/ITEM
projection after USER16 resumes.

| Consumer family | Required copied facts | Current provider and result | Residual owner |
| --- | --- | --- | --- |
| Desktop / WND identity and graph | Full native identity only at the sampling boundary; original-layout WND, client/server relocation, parent/child/next/owner edges, typed handle metadata | `mvdm_softpc_wow_page_domain_{set_client_desktop,project_window_graph,replace_window_links}`; `wow_user_object_bindings.c::project_graph` collects a coherent call-local view | S2 publication and S3 mutation/reuse proof |
| WND scalar reads | Window/client rectangles, style/exstyle, child ID, module, copied class/name fields and original WOW words | `mvdm_softpc_wow_page_domain_refresh_window` and `wow_window_words_project_observed` update prepared local records around graph commit | S3 host-originated geometry/mutation boundary |
| CLS reads | Atom, class style, extra bytes, copied ANSI name and selected original fields; never native procedure/private class storage | `mvdm_softpc_wow_page_domain_publish_named_class`, called from class/window bindings | S2 class scope, registration, failed cleanup |
| MENU / ITEM local walkers | Window-to-menu association; local menu/item/type/state/id/submenu graph | `publish_menus`, `window_menu`, `retire_menu`; `wow_menu_client_bindings.c` publishes and retires it | S2 publication audit, S3 mutation/retirement evidence |
| Local task-owned callback/cleanup object | Existing task owner, first-callback pair, original WND/CLS and WOW words | Existing T447 project bindings and page-domain `publish_handle`/`retire_handle` path | Closed foundation; S2 only verifies it is not broadened to foreign objects |
| System / desktop object | One copied desktop root and system-class facts required by original readers, without a task owner | `client_desktop_ready`, `set_client_desktop`, observed named-class path | S2 must prove bootstrap/order and withdrawal semantics |
| External/native/other-task object | Read-only full-HWND generation snapshot; copied graph edges and fields only | `ADAPTER-WOW-082` path retains a committed external snapshot; required roots are freshly sampled | S2 defines explicit discovery/publication entry; S3 proves stale/reuse/mutation behavior |

`GETWINDOWRECT`, `GETCLIENTRECT`, text, DC, placement, system metrics/colour,
and normal host-thunk conversion are not silently added to this denominator:
they do not by themselves establish an immutable local WND/MENU/ITEM read.
Their returned handles and object operands remain consumers of the same
identity rules where a given original caller requires a projection.

## Finite contract matrix

| ID | Object / actual entry | Authority and allowed representation | Update, withdrawal and failure rule | Evidence now / exact remaining S |
| --- | --- | --- | --- | --- |
| R01 | Desktop root: USER16 desktop reads; page-domain desktop functions | Modern desktop/HWND existence remains host authority.  The one guest desktop WND/client-server relation is the copied view authority. | Establish before USER16 can see `SHAREDINFO`; clear client references before backing release.  No native desktop creation or task ownership. | Layout fixture and profile check establish layout/selected bytes; bootstrap/withdraw runtime chain is S2. |
| R02 | Managed local WND/CLS: `publish_handle`, `publish_named_class`, callback path | Existing original task cleanup/callback owner retains authority; page domain owns only its copied allocation. | Full HWND uniqueness/generation is validated at publication.  Failure rolls back typed slot; retirement consumes only the managed generation while retained borrowers remain valid. | Existing creation/ownership/word-lifetime fixtures and T447 evidence; S2 confirms no regression. |
| R03 | Read-only observed WND/CLS: `project_graph`, `observe_window` | Host HWND and live geometry remain host authority; copied descriptor/links are a read-only worker-local snapshot. | Required roots and local rows sample/validate before commit.  External retained rows may be reused under approved snapshot semantics; dead identities withdraw, never become a new low-word identity. | Page-domain fixture exercises project/reclaim cases; explicit external discovery and system-object acceptance are S2. |
| R04 | Relationship readers: parent/child/next/owner/top/window/dlg-item | Original USER16 traverses copied relationships; native hierarchy remains host authority. | Preserve separate parent and owner facts; do not synthesize `GetParent` as every relationship.  Resolve all full identities before changing guest memory; unknown external edge becomes unavailable/zero only under the graph contract, never a reused handle. | `verify-wow-user-profile.ps1` and graph fixture; S2 needs end-to-end external/system graph assertion. |
| R05 | WND scalar/geometry fields | Native `WINDOWINFO`/public numeric fields are sampled input; the copied original-layout WND is direct-reader input. | A successful prepared graph commit swaps all local scalar/name values; native mutation must refresh at its designated producer boundary.  Failed capture/commit leaves old view untouched, rather than partially refreshing it. | `wow_window_words_*` fixtures; host-originated mutation without a thunk is S3. |
| R06 | MENU/ITEM and WND-menu attachment | Original local walkers own menu algorithm; page domain owns copied query layout.  Modern USER owns native menu resources/policy. | Publish complete child-before-parent trees in one non-reentrant commit; retire only after required WND/ITEM edges release.  No native pointer or host menu policy enters the guest view. | Page-domain menu cases; S2 audits all publish/retire callers, S3 covers mutation/reentry. |
| R07 | System classes and native standard classes | Native registration/procedure remains host-owned.  Guest sees only copied original CLS fields. | No invented registration, procedure gateway, or host pointer.  A failed class release leaves collector-owned bytes for later sweep; a referenced CLS cannot retire. | Class-copy unwind fixture / existing class binding review; S2 proves public/private/system scope and failed registration. |
| R08 | First callback and native returned HWND operands | Original callback/dispatch decides execution.  Adapter maps only a live exact generation to its copied WND. | No callback/property/cleanup attachment to a foreign object.  A native result that cannot be bound/projected fails the original call path; it is not coerced to a short HWND16. | T447/T449 callback evidence; S2 validates foreign result/discovery boundary, S3 covers nested destroy. |
| R09 | Destruction, rollback and reuse | Host owns actual window destruction; page domain owns its allocation/borrow lifetime; task owner owns managed cleanup only. | Two-phase observe/validate/commit.  Remove incoming observation edges before retirement.  Managed objects are not retired by observation sweeping.  Generation mismatch or dead target rejects/withdraws without retrying a consumed release. | reclaim, ownership, creation-order and word-lifetime fixtures; S3 proves lifecycle under actual host mutation/reentry. |
| R10 | `SERVERINFO` / `CLIENTINFO` / per-thread state | Original task/USER source owns behavior-private fields.  Page domain may publish only documented fixed-width copied values (cursor, accepted system colour, expected version, selected client context). | Producer updates accepted value before its direct consumer; inactive callback snapshots do not transfer desktop/self state.  Missing/withdrawn state is a failure, not a fabricated default. | Header/provider review; exact host-originated field timing is assigned to S3 after each direct reader is enumerated. |

## Ownership and prohibited shortcuts

1. **No two window managers.**  Native HWND validity, dimensions and message
   facilities are read through modern USER.  Original user algorithms and
   direct local walks remain in original USER16/WOW32.  The adapter never
   maintains independent window policy.
2. **No native identity leakage.**  Native pointers/procedures/properties and
   short HWNDs are not guest identities.  The only durable projection is an
   original-layout allocation keyed by a verified full generation.
3. **No foreign promotion.**  An external or system node may be copied for
   reading but gets neither cleanup ownership nor a callback/property gateway.
   Its sampled state cannot block a local mutation from being freshly captured.
4. **One transactional publisher.**  `wow_user_object_bindings.c` captures,
   validates and delegates to `wow_window_words_project_observed`; the latter
   commits page-domain graph then prepared local scalar swaps under the
   existing publication lock.  A failed prepare or validation has no partial
   guest-view publication.
5. **No claimed NT4-private reconstruction.**  Missing private flags,
   procedures and server memory are not inferred from approximate public
   values.  A later S either identifies an exact original field producer or
   records a bounded modern-host semantic boundary.

## Existing executable evidence and its limit

| Entrypoint | What it proves | What it does not prove |
| --- | --- | --- |
| `tests/adapter-mvdm-host-out/wow/wow_user_client_view_layout_fixture.c` | Selected original-layout sizes for WND, desktop, CLS, MENU and ITEM compile together. | Runtime publication or lifecycle. |
| `tests/mvdm-host/wow_page_domain_fixture.c` | Page-domain WND graph, menu attachment/query, retirement and selected local/external capture transactions. | A complete immutable-guest USER16 run for every object family. |
| `tests/mvdm-host/wow_page_domain_reclaim_fixture.c` | Failed page decommit remains a retained reclamation condition, not a retryable consumed borrow. | Real native destruction. |
| `tests/adapter-mvdm-host-out/wow/wow_window_{ownership,words_lifetime,creation_order}_fixture.c` | Managed ownership, retained words and creation/rollback ordering. | Foreign/system class scope or host-originated geometry. |
| `tests/observation/verify-wow-user-profile.ps1` | The selected USER.EXE contains the pinned direct-reader/layout profile. | Correct current producer invocation at every runtime boundary. |

These are executable evidence for their stated slice only.  S1 ran no
product build or runtime test because it changes no implementation and has no
new executable artifact; later implementation S must run the row-specific
fixtures plus selected immutable-guest acceptance against one coherent
package.

## Successor assignment and closure boundary

| Successor | Bounded work accepted from this ledger | Not absorbed |
| --- | --- | --- |
| S2 — identity, classes and publication | R01, R03, R04, R06, R07 and R08: enumerate all producers/callers; establish desktop/system/external read-only publication, CLS scope and complete menu graph attachment. | General control/dialog behavior, message scheduling, graphics, host policy. |
| S3 — mutation and destruction | R05, R09 and R10 plus S2-created identities: host-originated updates, stale reuse, nested destruction, creation rollback and copied fixed-field timing. | Rebuilding native USER or changing immutable guest. |
| S4 — acceptance and handoff | Exact direct-data/callback/lifecycle matrix and package regression; prove that later consumers reuse this one view owner. | New feature families not reached by selected original consumers. |

S1 is complete when this finite matrix is indexed and reviewed: there are no
unassigned C04--C07/object-C10 residuals, only explicitly bounded S2/S3/S4
work.  This ledger makes no implementation-complete claim for those rows.
