# S14 WOW resource and callback ownership review

Research-only checkpoint against `732f4a23fcfa53aca579dbb965d1392d6c26ecc4`.
Question: which actual implementations behind the C-to-F edges can move to B,
and which contain original private state or product lifecycle decisions?
This record does not authorize a mirror increase or close dependency IDs.

## Inputs and method

Read the complete implementation bodies below in the current working tree;
follow named callers using `rg` over `src` and `test`. Inspect the associated
headers, `common/guest-memory/access.h`, `compat/include/guest_memory_access.h`,
`non-mvdm/windows/core/ntuser/inc/user.h`, and the abnormal-return portion of
`mvdm-host/wow32/wcall16.c`. Paths in the table are relative to
`src/wow32-dll/source`. This is source evidence, not a fresh compile, link or
runtime test. Earlier selected-graph inventory remains separately recorded.

## Implementation dispositions

| File | Observed responsibility and state | Proposed ownership and necessary follow-up |
| --- | --- | --- |
| `wow_bitmap_bindings.c` | TLS conversion context, serialized scratch/DC lifetime, screen capabilities; `wow_bitmap_load` calls original `WOWLoadBitmapA`. | Move independent scope/DC/lock mechanics to B. Keep original-call orchestration separate so B does not call C. Split C `newres.h`, original declarations and alias macros from the resource carrier. Preserve recursive calls and failure cleanup. |
| `wow_button_resources.c` | Caller-owned GDI atlas; create into a temporary resource object, publish only after rendering succeeds; restore selected object and release on failure. | GDI mechanics are a B candidate, but current metadata uses C `OEMBITMAPINFO` and `OBI_COUNT`. Resolve this representation boundary before relocation; moving its current header wholesale would introduce B-to-C dependency. Do not duplicate a button state engine. |
| `wow_button_lifetime.c` | TLS invocation stack, private `PBUTNWND`/`PLOOKASIDE`, deferred release at the oldest same-control invocation, native-window dispatch wrapper. | Private control-state lifetime belongs with C algorithms; native entry composition stays outer. Only independent resource leaves belong B. Exact original carrier and mirror cost remain to be established before editing C. |
| `wow_callback_frame_lease.c` | CPU continuation capture/restore; protected-address conversion; bounded copied-memory leases; worker-active check and fatal-selector boundary. | CPU and selector operations belong C, generic lease mechanics B, worker cancellation/terminal policy outer. Do not move the whole file to B or conceal active/fatal checks behind new callbacks. The D access header is only a forwarding include to B, but removing that include detour alone does not solve implementation ownership. |
| `wow_class_client_bindings.c` | Scoped class context; original registration; native class publication and procedure normalization; completed guest-class projection and rollback; optional CPU-position diagnostic. | Separate native resource publication from original class lists, task/procedure representation and guest projection. Original structures and algorithms remain C; product entry composition remains outer. Do not classify the whole implementation as independent B merely because it calls `RegisterClass`-related APIs. Trace class-word and window-creation providers before choosing the exact split. |
| `wow_cleanup_bindings.c` | Native window/procedure verification plus temporary reconstruction of private USER child/sibling lists; invokes `_WOWCleanup`, then restores root and clears links in `finally`. | Private cleanup projection and original invocation belong C; independent native operations may be B. Preserve no-application-callout procedure replacement and retained object identity. Native handle checks alone cannot replace original cleanup. Full object-retirement chain remains to audit. |
| `wow_user_message_bridge.c` | Native queue initialization, owner-thread checks, sent-message-only peeks, private queue wake-bit update and native-call release/reacquire hooks. | C owns original queue/task ordering; B can own independent native queue operations only if their reentrant call boundary is retained outside B. Preserve PM_NOREMOVE/PM_QS_SENDMESSAGE and the wait-specific PM_NOYIELD contract; do not introduce a second message loop. Native-call providers remain to audit. |

## Specific ordering evidence

- `wcall16.c` invokes `wow_callback_control_restore` only during abnormal
  unwinding. Its return value gates restoration of `ptd->vpStack`,
  `vpCBStack` and callback flags. The current provider checks whether the
  worker is still active before touching CPU state, reloads CS/SS against
  current descriptors, and does not restore CR3 or cached descriptors.
  A relocation must preserve both machine restoration and terminal-abandonment
  semantics; blindly copying CPU setters into B is not a solution.
- `wow_class_client_publish` calls `_RegisterClassEx` before
  `publish_completed_class`: the original algorithm supplies WOW words and
  task association after initial native publication. Projecting the initial
  allocation instead would publish incomplete state. Failure destroys the
  newly published native class through the existing class-word owner.
- `wow_user_message_bridge` wraps native peeks because they may invoke client
  code synchronously. Its ordinary return and scheduler-wait return use
  distinct existing end operations. These must be traced, not replaced by a
  plain public API call on the basis of matching function names.
- The button atlas header reaches `OEMBITMAPINFO` and OBI constants through
  `opennt_user_multipass.h`, which includes original C `user.h`. Defining
  `WOW_ORIGINAL_BUTTON_RESOURCES` in the wrapper is not proof of an independent
  B declaration closure.
- Source search finds `wow_bitmap_bindings_shutdown` calls in tests, but no
  production call in `src`; the production definition and declaration remain.
  Its current terminal-only INIT_ONCE/lock lifecycle must not be described as
  verified production teardown. Reconcile final link and unload ownership
  before deciding whether to wire, retain or remove it.

## Result and remaining evidence

Seven complete bodies classified; no implementation moved and no dependency
record closed. The historical direct ledger remains 150 eliminated / 418 open
of 618 total records. Mirror changes: zero points, zero lines.

This review gives concrete ownership constraints, not completed migration
designs for every chain. Next inspect class-word/window-creation, native-call
entry/return and USER retirement providers, then reconcile destination files,
source provenance, mirror cost and final-code validation. Full four-component
inventory and caller/lifetime verification remain unfinished. No guest source
or binary was changed; no tests or release package were rerun for this review.
