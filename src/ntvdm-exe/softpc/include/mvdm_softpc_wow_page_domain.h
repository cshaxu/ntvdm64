#ifndef MVDM_SOFTPC_WOW_PAGE_DOMAIN_H
#define MVDM_SOFTPC_WOW_PAGE_DOMAIN_H

/*
 * Finite worker-owned carrier for the original NT shared-user-data mapping
 * and PMODE32 client ABI. It owns no USER objects and does not handle page
 * faults: CPU40 still owns translation and DPMI still owns protected/real
 * mode transition semantics.
 */
int mvdm_softpc_wow_page_domain_enter(void);
void mvdm_softpc_wow_page_domain_leave_protected(void);
int mvdm_softpc_wow_page_domain_reenter_protected(void);
int mvdm_softpc_wow_page_domain_active(void);
/* Select the worker WOW descriptor view only after original W32Init has
 * completed. The caller remains responsible for original WOW dispatch. */
int mvdm_softpc_wow_page_domain_activate_wow_context(void);
/* Guest-linear SHAREDINFO projection for the immutable USER16 client. This
 * is intentionally distinct from WOW32's native gpsi pointer. */
unsigned long mvdm_softpc_wow_page_domain_guest_shared_info(void);

#endif
