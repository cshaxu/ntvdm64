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
 * addresses and fixed-width original fields.  The caller owns the matching
 * native association and must retire before native destruction. */
int __cdecl mvdm_softpc_wow_page_domain_publish_handle(unsigned short index,
    unsigned short uniqueness, unsigned char type, unsigned char flags,
    unsigned long window, unsigned long class_server);
/* Non-NULL retained_backing transfers the unpublished allocation to the
 * caller's existing borrow; that owner must VdmFreeVirtualMemory it after
 * the final borrow. NULL retains immediate physical release. */
int __cdecl mvdm_softpc_wow_page_domain_retire_handle(unsigned short index,
    unsigned long *retained_backing);
/* Sample the live native geometry/styles before returning to guest code.
 * Reject retired/reused identities; retain the existing guest allocation. */
int __cdecl mvdm_softpc_wow_page_domain_refresh_window(unsigned long window);
int __cdecl mvdm_softpc_wow_page_domain_callback_window(unsigned long window,
    unsigned long saved[2]);
int __cdecl mvdm_softpc_wow_page_domain_restore_callback(const unsigned long saved[2]);
/* A selected original CLS is represented in the same worker client/server
 * address space. The returned value is its server-form address; zero means
 * publication did not occur. All inputs are fixed-width original fields. */
unsigned long __cdecl mvdm_softpc_wow_page_domain_publish_class(
    unsigned short atom, unsigned short fnid, unsigned long flags, unsigned long wow0,
    unsigned long wow1, unsigned long task, unsigned long style,
    unsigned long procedure, unsigned long window_extra,
    unsigned long module);
int __cdecl mvdm_softpc_wow_page_domain_retire_class(unsigned long server);

#endif
