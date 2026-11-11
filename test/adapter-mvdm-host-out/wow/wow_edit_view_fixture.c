#include "edit-view/precomp.h"
#include <stdio.h>

/* Compile the three actual OPENNT-HOST-078 units with the isolated
 * edit-view precomp and /Gz. Synthetic WND relationships are supplemental;
 * this fixture does not prove production parent/view lifetime. */
int view_fixture_CXMINIMIZED = 90, view_fixture_CYMINIMIZED = 30;
static WND desktop, parent, leaf;
static unsigned failures;
#define CHECK(x) do { if (!(x)) { ++failures; printf("FAIL line %d\n", __LINE__); } } while (0)
PWND WINAPI view_fixture_validate(HWND hwnd)
{
    return hwnd == (HWND)(ULONG_PTR)1 ? &leaf : NULL;
}

int __cdecl main(void)
{
    RECT result;
    POINT point = {-23, 45};
    desktop.fnid = FNID_DESKTOP;
    parent.spwndParent = &desktop;
    leaf.spwndParent = &parent;
    SetWF(&desktop, WFVISIBLE);
    SetWF(&parent, WFVISIBLE);
    SetWF(&leaf, WFVISIBLE | WFCHILD);
    CHECK(_IsChild(&parent, &leaf));
    CHECK(!_IsChild(&leaf, &leaf));
    CHECK(!_IsChild(&parent, NULL));
    CHECK(GetTopLevelWindow(&leaf) == &parent);
    CHECK(GetTopLevelWindow(NULL) == NULL);
    CHECK(IsVisible(&leaf));
    SetWF(&parent, WFMINIMIZED);
    CHECK(!IsVisible(&leaf));
    CHECK(IsVisible(&parent)); /* Own minimized state is not ancestor hiding. */
    ClrWF(&parent, WFMINIMIZED);
    leaf.rcClient.left = -100; leaf.rcClient.top = 200;
    CHECK(_ClientToScreen(&leaf, &point) && point.x == -123 && point.y == 245);
    CHECK(_ScreenToClient(&leaf, &point) && point.x == -23 && point.y == 45);
    CHECK(_IsWindowVisible(NULL));
    CHECK(_IsWindowVisible(&leaf));
    CHECK(_FChildVisible(&leaf));
    ClrWF(&leaf, WFVISIBLE);
    CHECK(!_IsWindowVisible(&leaf));
    CHECK(_FChildVisible(&leaf)); /* Deliberately ignores own visibility. */
    ClrWF(&parent, WFVISIBLE);
    CHECK(!_FChildVisible(&leaf));
    SetWF(&leaf, WFPOPUP);
    CHECK(_FChildVisible(&leaf)); /* CHILD|POPUP is not TestwndChild. */
    ClrWF(&leaf, WFPOPUP);
    CHECK(!FChildVisible((HWND)(ULONG_PTR)1));
    CHECK(!FChildVisible(NULL));
    leaf.spwndParent = NULL;
    CHECK(_FChildVisible(&leaf)); /* Original tolerates absent parent. */
    leaf.rcClient.left = 13; leaf.rcClient.top = 17;
    leaf.rcClient.right = 70013; leaf.rcClient.bottom = 90017;
    CHECK(_GetClientRect(&leaf, &result));
    CHECK(result.left == 0 && result.top == 0 && result.right == 70000 && result.bottom == 90000);
    SetWF(&leaf, WFMINIMIZED);
    CHECK(_GetClientRect(&leaf, &result));
    CHECK(result.right == 90 && result.bottom == 30);
    SetWF(&leaf, WFWIN40COMPAT);
    CHECK(_GetClientRect(&leaf, &result));
    CHECK(result.right == 70000 && result.bottom == 90000);
    printf("original_edit_view_failures=%u\n", failures);
    return failures != 0;
}
