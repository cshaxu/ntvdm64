#ifndef MVDM_SOFTPC_WOW_PAGE_DOMAIN_H
#define MVDM_SOFTPC_WOW_PAGE_DOMAIN_H

/*
 * Finite worker-owned carrier for the original NT shared-user-data mapping
 * and PMODE32 client ABI. It owns no USER objects and does not handle page
 * faults: CPU40 still owns translation and DPMI still owns protected/real
 * mode transition semantics.
 */
int __cdecl mvdm_softpc_wow_page_domain_enter(void);
void __cdecl mvdm_softpc_wow_page_domain_leave_protected(void);
int __cdecl mvdm_softpc_wow_page_domain_reenter_protected(void);
int __cdecl mvdm_softpc_wow_page_domain_active(void);
/* Select the worker WOW descriptor view only after original W32Init has
 * completed. The caller remains responsible for original WOW dispatch. */
int __cdecl mvdm_softpc_wow_page_domain_activate_wow_context(void);
/* PMODE32 KRNL386 consumes this original per-selector base table through a
 * flat guest pointer.  CPU40 cannot publish the host `FlatAddress` array, so
 * this returns descriptor-derived guest-linear bases. DPMI publication updates
 * this guest view before native conversion; WOW16 also writes it directly
 * before INT 2Ah updates the separate original native FlatAddress cache. */
unsigned long __cdecl mvdm_softpc_wow_page_domain_flat_address_array(
    const unsigned long *entries);
void __cdecl mvdm_softpc_wow_page_domain_update_flat_address(
    unsigned short selector, unsigned long base);
/* Guest-linear SHAREDINFO projection for the immutable USER16 client. This
 * is intentionally distinct from WOW32's native gpsi pointer. */
unsigned long __cdecl mvdm_softpc_wow_page_domain_guest_shared_info(void);
/* The remaining client-view addresses are exposed only to the worker's
 * original WOW32 provider and its narrow native object companion.  They are
 * guest-linear addresses, never host USER pointers. */
unsigned long __cdecl mvdm_softpc_wow_page_domain_guest_teb(void);
/* Original task initialization publishes the expected Windows version in the
 * current guest TEB before USER16 can take a version-sensitive branch. */
int __cdecl mvdm_softpc_wow_page_domain_set_expected_windows_version(
    unsigned long version);
/* Serialized original execution handoff. The inactive callback snapshot is
 * not an owning reference; its existing scoped borrow must still be live.
 * NULL next clears the pair. A failed validation leaves both view and output
 * unchanged; shared desktop/self/delta fields are never transferred. */
int __cdecl mvdm_softpc_wow_page_domain_exchange_client_context(
    unsigned long *previous_callback, unsigned long version,
    const unsigned long next_callback[2]);
/* Original monitor debug contexts follow the registered native task thread. */
int __cdecl mvdm_debug_task_register(unsigned long thread_id);
int __cdecl mvdm_debug_task_exchange(unsigned long previous, unsigned long next);
void __cdecl mvdm_debug_task_retire(unsigned long thread_id, int rollback);
/* SoftPC's original host mouse boundary supplies the final guest virtual
 * coordinates before guest mouse processing. Publish only that already
 * transformed value into USER16's direct shared cursor field. */
int __cdecl mvdm_softpc_wow_page_domain_publish_cursor(long x, long y);
/* Original USER's SetSysColor updates SERVERINFO.argbSystem after its public
 * SetSysColors call succeeds. This worker-local carrier only publishes that
 * accepted fixed-width value; it owns neither a host brush nor colour policy. */
int __cdecl mvdm_softpc_wow_page_domain_publish_system_color(
    unsigned long index, unsigned long rgb);
/* Original USER16 per-worker call-server flag, initially zero. */
unsigned long __cdecl mvdm_softpc_wow_page_domain_guest_csr_flag(void);
/* The worker's source-shaped desktop root is published as one client/server
 * relation before USER16 can observe SHAREDINFO.  It contains no host pointer
 * and does not own ordinary native window creation. */
int __cdecl mvdm_softpc_wow_page_domain_client_desktop_ready(void);
/* Publish the current original USER owner's already mapped client desktop.
 * desktop_info is client-linear; delta is the required nonzero original
 * server-to-client relocation amount. The caller serializes CCPU entry and retains backing
 * until withdrawal. This transport creates no desktop/window or USER policy.
 * Changing the view invalidates the original per-thread HWND/PWND cache. */
int __cdecl mvdm_softpc_wow_page_domain_set_client_desktop(
    unsigned long desktop_info, unsigned long delta);
/* Clear client references before the original owner releases its backing. */
int __cdecl mvdm_softpc_wow_page_domain_clear_client_desktop(void);
/* Publish/retire one worker-owned original-layout WND projection. `window`
 * is a host-only HWND identity used solely to sample public geometry before
 * the call returns; the guest record contains only client/server linear
 * addresses and fixed-width original fields. This does not associate, hook
 * or own the native HWND. The caller owns projection lifetime: managed
 * associations retire at their destruction boundary; sampled unowned views
 * may be withdrawn without destroying the native window. */
/* Managed publication is per WND, independent of class provenance. An
 * existing observation may be adopted only for the same full native identity;
 * zero class_server preserves its CLS, as native-window subclassing requires.
 * Keep its WND backing/links; supplied creator WOW words replace observation
 * values. NULL words retain existing words (or zero for a new view).
 * Duplicate management is an error. Observation retirement cannot remove a
 * managed WND even when its class remains an observed native-class view. */
int __cdecl mvdm_softpc_wow_page_domain_publish_handle(unsigned short index,
    unsigned short uniqueness, unsigned char type, unsigned char flags,
    unsigned long window, unsigned long class_server, const unsigned long words[3]);
/* Roll back only a managed adoption that retained its observed native CLS.
 * Caller supplies its pinned exact generation; observation/words remain live. */
int __cdecl mvdm_softpc_wow_page_domain_cancel_native_adoption(unsigned long,
    unsigned long);
/* Non-NULL retained_backing transfers the unpublished allocation to the
 * caller's existing borrow; that owner calls release_retained_window after
 * its final borrow. Shared-view borrows can independently defer physical
 * release. NULL drops publication ownership at this boundary. */
int __cdecl mvdm_softpc_wow_page_domain_retire_handle(unsigned short index,
    unsigned long *retained_backing);
/* Synchronous shared-view borrows retain allocation, not native HWND
 * ownership. The numeric backing token remains valid after handle retirement
 * until its final release; it is not a native pointer or another handle ID. */
unsigned long __cdecl mvdm_softpc_wow_page_domain_borrow_window(unsigned long window);
unsigned long __cdecl mvdm_softpc_wow_page_domain_window_handle(unsigned short index);
/* Copy HANDLEENTRY's type/flags/uniqueness DWORD, not phead/pOwner. This is
 * the shared query domain, including observations, not task-cleanup ownership. */
int __cdecl mvdm_softpc_wow_page_domain_handle_metadata(unsigned short index,
    unsigned long *metadata);
/* Valid release consumes its reference even if physical decommit fails.
 * The existing domain list then owns pending reclamation; caller must not
 * retry the consumed reference. Collection reports outstanding free failures. */
int __cdecl mvdm_softpc_wow_page_domain_collect_retired_windows(void);
int __cdecl mvdm_softpc_wow_page_domain_release_window(unsigned long backing);
int __cdecl mvdm_softpc_wow_page_domain_release_retained_window(unsigned long backing);
/* ADAPTER-WOW-069: one host-only storage owner per existing WND generation.
 * Success transfers storage to the generation; failure transfers nothing.
 * Bind/read require an outstanding borrow. Binding is single-assignment and
 * only allowed while live; reads remain valid during retained retirement.
 * dispose must only release host storage without throwing, never enter guest/USER or reenter
 * this domain. It runs once at physical reclamation or quiesced domain exit.
 * Storage must not hold a reference back to this backing (no owner cycle).
 * Neither pointer is written to guest memory or used as a handle identity. */
typedef void (__cdecl *mvdm_softpc_wow_host_window_dispose)(void *);
int __cdecl mvdm_softpc_wow_page_domain_bind_window_host_storage(unsigned long,
    void *, mvdm_softpc_wow_host_window_dispose);
void *__cdecl mvdm_softpc_wow_page_domain_window_host_storage(unsigned long);
/* Scope-only pin of an exact existing generation/storage pair. Unlike an
 * HWND lookup this may retain withdrawn backing still alive for a borrower.
 * Pair with release_window; storage itself must not own this reference. */
int __cdecl mvdm_softpc_wow_page_domain_retain_host_window(unsigned long, void *);
/* Original three WW DWORDs persist in the projected WND. Reads require an
 * outstanding borrow; setters additionally require a still-published WND.
 * Native style/module sampling and original classification remain above. */
int __cdecl mvdm_softpc_wow_page_domain_read_window_words(unsigned long backing,
    unsigned long words[3]);
/* Identity belongs to the retained backing generation, not a fresh HWND
 * query. Requires a live borrow; zero/unknown metadata is not synthesized. */
int __cdecl mvdm_softpc_wow_page_domain_read_window_identity(unsigned long backing,
    unsigned long *process_id, unsigned long *thread_id);
typedef struct mvdm_softpc_wow_window_words_view {
    unsigned long words[3], style, exstyle, module;
} mvdm_softpc_wow_window_words_view;
/* All fields come from the committed retained generation, without a USER
 * query. Module remains host metadata, never a new pointer in guest WND. */
int __cdecl mvdm_softpc_wow_page_domain_read_window_words_view(unsigned long backing,
    mvdm_softpc_wow_window_words_view *view);
int __cdecl mvdm_softpc_wow_page_domain_exchange_window_word(unsigned long backing,
    unsigned long index, unsigned long value, unsigned long *previous);
/* Sampled native view, separate from managed task/WW ownership. Only copied
 * class query fields enter the guest; native registration/procedures do not. */
int __cdecl mvdm_softpc_wow_page_domain_observe_window(unsigned long window,
    unsigned short atom, unsigned long class_style, unsigned long window_extra,
    const char *class_name, unsigned long name_bytes);
/* Withdrawal and class collection have separate completion results. A failed
 * class free must not report an already-consumed window withdrawal/borrow as
 * retryable. After withdrawal and final borrow release, the caller must sweep;
 * failed class frees stay in the existing allocation list for a later sweep. */
int __cdecl mvdm_softpc_wow_page_domain_forget_observed_window(unsigned long window);
int __cdecl mvdm_softpc_wow_page_domain_collect_observed_classes(void);
/* Reclaim only dead, unlinked query nodes. Live observations, managed nodes
 * and nodes still referenced by the published graph remain untouched. */
int __cdecl mvdm_softpc_wow_page_domain_collect_observed_windows(void);
/* Sample live native geometry/styles and numeric child control IDs before
 * returning to guest code. Top-level menu graph publication is separate.
 * Reject retired/reused identities; retain the existing guest allocation. */
int __cdecl mvdm_softpc_wow_page_domain_refresh_window(unsigned long window);
/* Caller supplies one structurally consistent observation view, including
 * every affected record, with guest entry excluded. This is not an atomic
 * native desktop snapshot. Resolve full identities before changing guest memory.
 * NULL links are explicit; an unknown identity is never silently omitted.
 * This copies relationships, not native tree policy. In particular parent
 * and child links need not be symmetric during original window creation. */
typedef struct mvdm_softpc_wow_window_links {
    unsigned long window, next, parent, child, owner;
} mvdm_softpc_wow_window_links;
/* Counted class query bytes are copied during this synchronous call. They
 * neither register native classes nor carry native pointers into guest state. */
typedef struct mvdm_softpc_wow_window_descriptor {
    unsigned short atom;
    unsigned long class_style, window_extra, name_bytes;
    char name[256];
} mvdm_softpc_wow_window_descriptor;
/* Call-local copy of the committed view. Numeric rectangles/styles retain
 * the original x86 meaning, not a modern USER pointer/layout. The caller
 * serializes two-pass sizing/copy with guest execution and owns the arrays.
 * An empty class descriptor denotes the not-yet-observed desktop root. */
typedef struct mvdm_softpc_wow_window_observation {
    mvdm_softpc_wow_window_links links;
    mvdm_softpc_wow_window_descriptor descriptor;
    long window_rect[4], client_rect[4];
    unsigned long style, exstyle, control_id;
    unsigned long process_id, thread_id, module;
    unsigned long context_help_id; /* Native scalar, retained with this generation. */
} mvdm_softpc_wow_window_observation;
/* Read an exact retained generation, including after logical retirement.
 * Requires an outstanding borrow and the existing execution owner. Returned
 * links are historical identities, not permission to resolve reused HWNDs.
 * Failure leaves output unchanged. No native observation is performed. */
int __cdecl mvdm_softpc_wow_page_domain_read_window_view(unsigned long backing,
    mvdm_softpc_wow_window_observation *row);
/* Pin a relationship's exact target allocation under the same execution
 * owner. relation: 0 next, 1 parent, 2 child, 3 owner. Success with target=0
 * means no edge; nonzero target owns one release_window reference. Failure
 * leaves output unchanged. The source must already have an active borrow. */
int __cdecl mvdm_softpc_wow_page_domain_borrow_window_relation(
    unsigned long backing, unsigned long relation, unsigned long *target);
int __cdecl mvdm_softpc_wow_page_domain_copy_window_view(
    unsigned long capacity, mvdm_softpc_wow_window_observation *rows,
    unsigned long *count);
/* Menu attachment refresh consumes only these committed numeric fields.
 * Same current-generation, sizing/order/LastError rules as the full view;
 * no class/relationship parsing, native sampling or new retained state. */
typedef struct mvdm_softpc_wow_window_menu_observation {
    unsigned long window, style, process_id;
} mvdm_softpc_wow_window_menu_observation;
int __cdecl mvdm_softpc_wow_page_domain_copy_window_menu_view(
    unsigned long capacity, mvdm_softpc_wow_window_menu_observation *rows,
    unsigned long *count);
int __cdecl mvdm_softpc_wow_page_domain_project_window_graph(
    unsigned long count, const mvdm_softpc_wow_window_observation *rows);
int __cdecl mvdm_softpc_wow_page_domain_publish_window_links(
    unsigned long count, const mvdm_softpc_wow_window_links *links);
/* Replace edges and withdraw confirmed-dead observed generations together.
 * Every surviving incoming edge must be replaced by this snapshot. Managed
 * task objects are never retired here. Failure leaves links/identities intact;
 * successful logical commit may leave domain-owned physical reclamation. */
int __cdecl mvdm_softpc_wow_page_domain_replace_window_links(
    unsigned long count, const mvdm_softpc_wow_window_links *links,
    unsigned long retired_count, const unsigned long *retired);
int __cdecl mvdm_softpc_wow_page_domain_callback_window(unsigned long window,
    unsigned long saved[2]);
int __cdecl mvdm_softpc_wow_page_domain_restore_callback(const unsigned long saved[2]);
/* Read the existing callback pair, not the current handle slot. Success with
 * zero means no exact HWND match. Nonzero transfers one backing borrow;
 * failure leaves output untouched. Caller owns the active execution domain. */
int __cdecl mvdm_softpc_wow_page_domain_borrow_callback_window(
    unsigned long window, unsigned long *backing);
/* A selected original CLS is represented in the same worker client/server
 * address space. The returned value is its server-form address; zero means
 * publication did not occur. Fields are fixed-width original values; the
 * counted ANSI name includes its NUL and is copied synchronously. Its host
 * pointer is never retained or published to guest memory. */
unsigned long __cdecl mvdm_softpc_wow_page_domain_publish_named_class(
    unsigned short atom, unsigned short fnid, unsigned long flags, unsigned long wow0,
    unsigned long wow1, unsigned long task, unsigned long style,
    unsigned long procedure, unsigned long window_extra,
    unsigned long module, const char *class_name, unsigned long name_bytes);
/* Reject a CLS still referenced by a published WND. Owners of unpublished
 * retained WND borrows must also retain the class until their final release;
 * this carrier does not replace the existing native class-reference owner. */
int __cdecl mvdm_softpc_wow_page_domain_retire_class(unsigned long server);
/* Native registration has ended: transfer copied storage to the existing
 * collector. Published WND associations still reject release; retired
 * borrowers keep the bytes until their final release. No new WND may use it. */
int __cdecl mvdm_softpc_wow_page_domain_release_class(unsigned long server);

/* Copied query fields consumed by USER16's unchanged local menu walkers.
 * This is not a native MENU/ITEM layout. Submenu values are full identities,
 * resolved to already published guest objects here; never host pointers.
 * The USER owner serializes graph updates with CCPU entry and must complete
 * every affected node/attachment/retirement before resuming guest execution.
 * Native menu policy, graph discovery and task cleanup remain with that owner. */
typedef struct mvdm_softpc_wow_menu_query_item {
    unsigned long type, state, id, submenu;
} mvdm_softpc_wow_menu_query_item;
typedef struct mvdm_softpc_wow_menu_observation {
    unsigned long menu, task, first_item, item_count;
    unsigned long local_state; /* Required local state, independent of creator ownership. */
} mvdm_softpc_wow_menu_observation;
/* Complete child-before-parent trees, copied in one non-reentrant commit. */
int __cdecl mvdm_softpc_wow_page_domain_publish_menus(unsigned long count,
    const mvdm_softpc_wow_menu_observation *menus, unsigned long item_count,
    const mvdm_softpc_wow_menu_query_item *items);
unsigned long __cdecl mvdm_softpc_wow_page_domain_publish_menu(
    unsigned long menu, unsigned long task, unsigned long count,
    const mvdm_softpc_wow_menu_query_item *items);
/* Refuses while required local WND/ITEM edges still reference MENU. External
 * observation edges may be cleared in the same retirement transaction. */
int __cdecl mvdm_softpc_wow_page_domain_retire_menu(unsigned long menu);
int __cdecl mvdm_softpc_wow_page_domain_window_menu(unsigned long window,
    unsigned long menu);

#endif
