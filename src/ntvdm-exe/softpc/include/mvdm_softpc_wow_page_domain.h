#ifndef MVDM_SOFTPC_WOW_PAGE_DOMAIN_H
#define MVDM_SOFTPC_WOW_PAGE_DOMAIN_H
#include "common/wow/page_domain.h"

/* Worker-private lifecycle and publication operations. */
/*
 * Finite worker-owned carrier for the original NT shared-user-data mapping
 * and PMODE32 client ABI. It owns no USER objects and does not handle page
 * faults: CPU40 still owns translation and DPMI still owns protected/real
 * mode transition semantics.
 */
int __cdecl mvdm_softpc_wow_page_domain_enter(void);

void __cdecl mvdm_softpc_wow_page_domain_leave_protected(void);

int __cdecl mvdm_softpc_wow_page_domain_reenter_protected(void);

/* Select the worker WOW descriptor view only after original W32Init has
 * completed. The caller remains responsible for original WOW dispatch. */
int __cdecl mvdm_softpc_wow_page_domain_activate_wow_context(void);

/* The remaining client-view addresses are exposed only to the worker's
 * original WOW32 provider and its narrow native object companion.  They are
 * guest-linear addresses, never host USER pointers. */
unsigned long __cdecl mvdm_softpc_wow_page_domain_guest_teb(void);

/* SoftPC's original host mouse boundary supplies the final guest virtual
 * coordinates before guest mouse processing. Publish only that already
 * transformed value into USER16's direct shared cursor field. */
int __cdecl mvdm_softpc_wow_page_domain_publish_cursor(long x, long y);

/* Publish the current original USER owner's already mapped client desktop.
 * desktop_info is client-linear; delta is the required nonzero original
 * server-to-client relocation amount. The caller serializes CCPU entry and retains backing
 * until withdrawal. This transport creates no desktop/window or USER policy.
 * Changing the view invalidates the original per-thread HWND/PWND cache. */
int __cdecl mvdm_softpc_wow_page_domain_set_client_desktop(
    unsigned long desktop_info, unsigned long delta);

/* Clear client references before the original owner releases its backing. */
int __cdecl mvdm_softpc_wow_page_domain_clear_client_desktop(void);

/* Valid release consumes its reference even if physical decommit fails.
 * The existing domain list then owns pending reclamation; caller must not
 * retry the consumed reference. Collection reports outstanding free failures. */
int __cdecl mvdm_softpc_wow_page_domain_collect_retired_windows(void);

/* Original three WW DWORDs persist in the projected WND. Reads require an
 * outstanding borrow; setters additionally require a still-published WND.
 * Native style/module sampling and original classification remain above. */
int __cdecl mvdm_softpc_wow_page_domain_read_window_words(unsigned long backing,
    unsigned long words[3]);

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

/* Reclaim only dead, unlinked query nodes. Live observations, managed nodes
 * and nodes still referenced by the published graph remain untouched. */
int __cdecl mvdm_softpc_wow_page_domain_collect_observed_windows(void);

/* Sample live native geometry/styles and numeric child control IDs before
 * returning to guest code. Top-level menu graph publication is separate.
 * Reject retired/reused identities; retain the existing guest allocation. */
int __cdecl mvdm_softpc_wow_page_domain_refresh_window(unsigned long window);

int __cdecl mvdm_softpc_wow_page_domain_publish_window_links(
    unsigned long count, const mvdm_softpc_wow_window_links *links);

/* Replace edges and withdraw confirmed-dead observed generations together.
 * Every surviving incoming edge must be replaced by this snapshot. Managed
 * task objects are never retired here. Failure leaves links/identities intact;
 * successful logical commit may leave domain-owned physical reclamation. */
int __cdecl mvdm_softpc_wow_page_domain_replace_window_links(
    unsigned long count, const mvdm_softpc_wow_window_links *links,
    unsigned long retired_count, const unsigned long *retired);

unsigned long __cdecl mvdm_softpc_wow_page_domain_publish_menu(
    unsigned long menu, unsigned long task, unsigned long count,
    const mvdm_softpc_wow_menu_query_item *items);
#endif
