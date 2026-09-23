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
/* Guest-linear SHAREDINFO projection for the immutable USER16 client. This
 * is intentionally distinct from WOW32's native gpsi pointer. */
unsigned long __cdecl mvdm_softpc_wow_page_domain_guest_shared_info(void);
/* The remaining client-view addresses are exposed only to the worker's
 * original WOW32 provider and its narrow native object companion.  They are
 * guest-linear addresses, never host USER pointers. */
unsigned long __cdecl mvdm_softpc_wow_page_domain_guest_teb(void);
/* Original USER16 per-worker call-server flag, initially zero. */
unsigned long __cdecl mvdm_softpc_wow_page_domain_guest_csr_flag(void);
/* Publish the current original USER owner's already mapped client desktop.
 * desktop_info is client-linear; delta is the required nonzero original
 * server-to-client relocation amount. The caller serializes CCPU entry and retains backing
 * until withdrawal. This transport creates no desktop/window or USER policy.
 * Changing the view invalidates the original per-thread HWND/PWND cache. */
int __cdecl mvdm_softpc_wow_page_domain_set_client_desktop(
    unsigned long desktop_info, unsigned long delta);
/* Clear client references before the original owner releases its backing. */
int __cdecl mvdm_softpc_wow_page_domain_clear_client_desktop(void);
/* Identity-only legacy boundary: native-only fixtures remain a no-op, but
 * an active guest view rejects this incomplete publication. The original
 * object producer must replace this seam with full phead/lifetime binding
 * before S42 closure; metadata-only success would publish a free-list link
 * as a WND pointer. Failure leaves the guest entry unchanged. */
int __cdecl mvdm_softpc_wow_page_domain_publish_handle(unsigned short index,
    unsigned short uniqueness, unsigned char type, unsigned char flags);
int __cdecl mvdm_softpc_wow_page_domain_retire_handle(unsigned short index);

#endif
