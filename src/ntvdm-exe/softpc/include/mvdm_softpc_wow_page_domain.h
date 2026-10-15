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
/* An existing observed projection may become managed only for the same full
 * native identity and a registered non-observed class. Keep its WND backing,
 * links; supplied creator WOW words replace observation values. NULL retains
 * existing words (or zero for a new view). Duplicate management is an error. */
int __cdecl mvdm_softpc_wow_page_domain_publish_handle(unsigned short index,
    unsigned short uniqueness, unsigned char type, unsigned char flags,
    unsigned long window, unsigned long class_server, const unsigned long words[3]);
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
int __cdecl mvdm_softpc_wow_page_domain_release_window(unsigned long backing);
int __cdecl mvdm_softpc_wow_page_domain_release_retained_window(unsigned long backing);
/* Original three WW DWORDs persist in the projected WND. Reads require an
 * outstanding borrow; setters additionally require a still-published WND.
 * Native style/module sampling and original classification remain above. */
int __cdecl mvdm_softpc_wow_page_domain_read_window_words(unsigned long backing,
    unsigned long words[3]);
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
/* Caller supplies one quiesced native relationship snapshot, including every
 * affected record. Resolve all full identities before changing guest memory.
 * NULL links are explicit; an unknown identity is never silently omitted.
 * This copies relationships, not native tree policy. In particular parent
 * and child links need not be symmetric during original window creation. */
typedef struct mvdm_softpc_wow_window_links {
    unsigned long window, next, parent, child, owner;
} mvdm_softpc_wow_window_links;
int __cdecl mvdm_softpc_wow_page_domain_publish_window_links(
    unsigned long count, const mvdm_softpc_wow_window_links *links);
int __cdecl mvdm_softpc_wow_page_domain_callback_window(unsigned long window,
    unsigned long saved[2]);
int __cdecl mvdm_softpc_wow_page_domain_restore_callback(const unsigned long saved[2]);
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

/* Copied query fields consumed by USER16's unchanged local menu walkers.
 * This is not a native MENU/ITEM layout. Submenu values are full identities,
 * resolved to already published guest objects here; never host pointers.
 * The USER owner serializes graph updates with CCPU entry and must complete
 * every affected node/attachment/retirement before resuming guest execution.
 * Native menu policy, graph discovery and task cleanup remain with that owner. */
typedef struct mvdm_softpc_wow_menu_query_item {
    unsigned long type, state, id, submenu;
} mvdm_softpc_wow_menu_query_item;
unsigned long __cdecl mvdm_softpc_wow_page_domain_publish_menu(
    unsigned long menu, unsigned long task, unsigned long count,
    const mvdm_softpc_wow_menu_query_item *items);
/* Refuses with ERROR_BUSY while copied WND/ITEM edges still reference MENU;
 * the producer must publish the native unlink before retrying. */
int __cdecl mvdm_softpc_wow_page_domain_retire_menu(unsigned long menu);
int __cdecl mvdm_softpc_wow_page_domain_window_menu(unsigned long window,
    unsigned long menu);

#endif
