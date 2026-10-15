/* Exercise the production creation-menu boundary with native USER ownership.
 * Resource lookup alone is replaced; this is not Win16 resource acceptance. */
#include "creation-menu.inc"

static unsigned errors, loads;
static BOOL fail_load, reject_create;
static BOOL creation_delivered;
static HMENU last_menu;
static HANDLE expected_module;
static LPCWSTR expected_name;
typedef struct menu_fixture_creation {
    wow_class_lookup_entry entry;
    wow_window_creation_menu_state menu;
    DWORD style;
    HMENU input_menu;
} menu_fixture_creation;
static menu_fixture_creation *pending_creation;
static unsigned sizing_callbacks;
#define CHECK(x) do { if (!(x)) { ++errors; fprintf(stderr,"FAIL %d error=%lu\n",__LINE__,GetLastError()); } } while (0)

HMENU WINAPI wow_user_load_menuW(HINSTANCE module, LPCWSTR name)
{
    ++loads;
    CHECK(module == expected_module && name == expected_name);
    if (fail_load) return NULL;
    last_menu = CreateMenu();
    CHECK(last_menu && AppendMenuW(last_menu, MF_STRING, 1, L"File"));
    return last_menu;
}

static LRESULT CALLBACK test_proc(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    if (message == WM_GETMINMAXINFO && pending_creation && !creation_delivered) {
        CREATESTRUCTA early = {0};
        early.style = pending_creation->style;
        early.hMenu = pending_creation->input_menu;
        CHECK(wow_window_creation_menu(window, &early,
            &pending_creation->entry, &pending_creation->menu));
        ++sizing_callbacks;
        if (pending_creation->menu.attached) CHECK(GetMenu(window) == last_menu);
    }
    if (!creation_delivered && (message == WM_SIZE || message == WM_MOVE ||
            message == WM_WINDOWPOSCHANGED))
        CHECK(FALSE); /* Original createw.c delivers these after WM_CREATE. */
    if (message == WM_CREATE) creation_delivered = TRUE;
    if (message == WM_NCCREATE) {
        menu_fixture_creation *creation =
            ((CREATESTRUCTA *)lp)->lpCreateParams;
        CHECK(wow_window_creation_menu(window, (CREATESTRUCTA *)lp,
            &creation->entry, &creation->menu));
        CHECK(((CREATESTRUCTA *)lp)->hMenu == GetMenu(window) ||
            (((CREATESTRUCTA *)lp)->style & WS_CHILD));
        if (reject_create) return FALSE;
    }
    if (message == WM_CREATE && !(GetWindowLongA(window,GWL_STYLE)&WS_CHILD)) {
        /* Public USER may use a different CREATESTRUCT for the two messages. */
        menu_fixture_creation *creation =
            ((CREATESTRUCTA *)lp)->lpCreateParams;
        CHECK(wow_window_creation_menu(window, (CREATESTRUCTA *)lp,
            &creation->entry, &creation->menu));
        CHECK(((CREATESTRUCTA *)lp)->hMenu == GetMenu(window));
    }
    return DefWindowProcA(window,message,wp,lp);
}

static void run_case(DWORD style, HMENU explicit_menu, HWND parent,
    LPCWSTR resource, BOOL failure, BOOL reject)
{
    menu_fixture_creation creation = {0};
    menu_fixture_creation *previous = pending_creation;
    unsigned before = loads;
    BOOL should_load = !(style & WS_CHILD) && !explicit_menu && resource;
    HWND window;
    creation.entry.hModule = expected_module = (HANDLE)0x13570001;
    expected_name = resource;
    creation.entry.lpszClientUnicodeMenuName = (LPWSTR)resource;
    creation.style = style;
    creation.input_menu = explicit_menu;
    pending_creation = &creation;
    fail_load = failure; reject_create = reject; last_menu = NULL;
    creation_delivered = FALSE;
    window = CreateWindowExA(0,"WOW_CLASS_MENU_FIXTURE","",style,
        0,0,100,100,parent,explicit_menu,GetModuleHandleA(NULL),&creation);
    CHECK(loads == before + (should_load ? 1u : 0u));
    CHECK(reject ? !window : !!window);
    if (window) {
        if (!(style&WS_CHILD)) CHECK(GetMenu(window) ==
            (explicit_menu ? explicit_menu : last_menu));
        CHECK(DestroyWindow(window));
    }
    if (last_menu) CHECK(!IsMenu(last_menu));
    pending_creation = previous;
}

int __cdecl main(void)
{
    WNDCLASSA wc = {0};
    HWND parent;
    HMENU explicit_menu;
    wc.lpfnWndProc=test_proc;wc.hInstance=GetModuleHandleA(NULL);
    wc.lpszClassName="WOW_CLASS_MENU_FIXTURE";
    CHECK(RegisterClassA(&wc));
    run_case(WS_OVERLAPPEDWINDOW,NULL,NULL,L"CLASS_MENU",FALSE,FALSE);
    run_case(WS_OVERLAPPEDWINDOW,NULL,NULL,MAKEINTRESOURCEW(17),FALSE,FALSE);
    run_case(WS_OVERLAPPEDWINDOW,NULL,NULL,L"MISSING",TRUE,FALSE);
    run_case(WS_OVERLAPPEDWINDOW,NULL,NULL,NULL,FALSE,FALSE);
    run_case(WS_OVERLAPPEDWINDOW,NULL,NULL,L"REJECT",FALSE,TRUE);
    explicit_menu=CreateMenu();
    run_case(WS_OVERLAPPEDWINDOW,explicit_menu,NULL,L"IGNORED",FALSE,FALSE);
    CHECK(!IsMenu(explicit_menu));
    parent=CreateWindowExA(0,"STATIC","",0,0,0,100,100,NULL,NULL,wc.hInstance,NULL);
    CHECK(parent);
    run_case(WS_CHILD,(HMENU)23,parent,L"IGNORED",FALSE,FALSE);
    CHECK(DestroyWindow(parent));
    CHECK(UnregisterClassA(wc.lpszClassName,wc.hInstance));
    CHECK(sizing_callbacks != 0);
    fprintf(stderr,"WOW_CREATION_MENU_SIZING errors=%u callbacks=%u\n",errors,sizing_callbacks);
    fprintf(stderr,"WOW_CREATION_MENU errors=%u loads=%u\n",errors,loads);
    return errors ? 1 : 0;
}
