#include <windows.h>

#ifdef WOW_CLASS_CLIENT_FIXTURE
/* Native class-policy/encoding test only. This explicit mock admits the
 * publication; real CCPU class layout is covered separately by page-domain
 * and immutable WINMINE tests, never by this fixture. */
unsigned long __cdecl mvdm_softpc_wow_page_domain_publish_class(
    unsigned short atom, unsigned short fnid, unsigned long flags,
    unsigned long wow0, unsigned long wow1, unsigned long task,
    unsigned long style, unsigned long procedure, unsigned long extra,
    unsigned long module)
{
    (void)fnid; (void)flags; (void)wow0; (void)wow1; (void)task;
    (void)style; (void)procedure; (void)extra; (void)module;
    return atom ? 0x1000u : 0u;
}
#endif

/* The task-lifecycle fixture does not enter the CCPU page domain or publish a
 * class.  It nevertheless pulls the original class-cleanup body, which has a
 * parent ABI reference for the production class record retirement.  Keep the
 * parent boundary explicit here so this fixture neither links a second
 * session/CCPU image nor pretends to test page-domain publication.  The real
 * parent export is checked by the formal WOW32 DLL import audit and exercised
 * by wow-page-domain-test.exe. */
int __cdecl mvdm_softpc_wow_page_domain_retire_class(unsigned long server)
{
    UNREFERENCED_PARAMETER(server);
    return 1;
}

/* Task-lifecycle fixtures intentionally have no CCPU PMODE32 page domain.
 * They verify only that the original cleanup order retains and releases the
 * native task carrier.  The dedicated page-domain fixture verifies the
 * actual TEB +50h store and withdrawal. */
int __cdecl mvdm_softpc_wow_page_domain_set_expected_windows_version(
    unsigned long version)
{
    UNREFERENCED_PARAMETER(version);
    return 1;
}
