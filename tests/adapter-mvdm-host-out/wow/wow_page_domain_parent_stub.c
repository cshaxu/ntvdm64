#include <windows.h>
#include "ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h"

/* Only the menu-cleanup case replaces storage. It tests the original task
 * selector and native destruction, not CCPU publication; the real combined
 * native-menu/page-domain fixture covers that separate boundary. */
LONG wow_menu_fixture_retire_fail;
unsigned long __cdecl mvdm_softpc_wow_page_domain_publish_menu(
    unsigned long menu, unsigned long task, unsigned long count,
    const mvdm_softpc_wow_menu_query_item *items)
{
    (void)task; (void)count; (void)items;
#ifdef WOW_MENU_CLEANUP_FIXTURE
    return menu ? 0x1000u + LOWORD(menu) : 0u;
#else
    (void)menu;
    SetLastError(ERROR_INVALID_STATE);
    return 0u;
#endif
}
int __cdecl mvdm_softpc_wow_page_domain_retire_menu(unsigned long menu)
{
    (void)menu;
#ifdef WOW_MENU_CLEANUP_FIXTURE
    return !InterlockedExchange(&wow_menu_fixture_retire_fail, 0);
#else
    SetLastError(ERROR_INVALID_STATE);
    return 0;
#endif
}

#ifdef WOW_CLASS_CLIENT_FIXTURE
DWORD wow_class_fixture_words[2], wow_class_fixture_task;
unsigned wow_class_fixture_publications;
BOOL wow_class_fixture_publication_fail;
/* Native class-policy/encoding test only. This explicit mock admits the
 * publication; real CCPU class layout is covered separately by page-domain
 * and immutable WINMINE tests, never by this fixture. */
unsigned long __cdecl mvdm_softpc_wow_page_domain_publish_named_class(
    unsigned short atom, unsigned short fnid, unsigned long flags,
    unsigned long wow0, unsigned long wow1, unsigned long task,
    unsigned long style, unsigned long procedure, unsigned long extra,
    unsigned long module, const char *class_name, unsigned long name_bytes)
{
    (void)fnid; (void)flags;
    (void)style; (void)procedure; (void)extra; (void)module;
    wow_class_fixture_words[0] = wow0;
    wow_class_fixture_words[1] = wow1;
    wow_class_fixture_task = task;
    ++wow_class_fixture_publications;
    if (wow_class_fixture_publication_fail) return 0u;
    return atom && class_name && name_bytes && !class_name[name_bytes-1u] ? 0x1000u : 0u;
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
