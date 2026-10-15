/* Exercise the production creation-menu boundary with native USER ownership.
 * Resource lookup alone is replaced; this is not Win16 resource acceptance. */
#include "creation-menu.inc"

static unsigned errors, loads;
static BOOL fail_load, reject_create;
static BOOL creation_delivered;
static HMENU last_menu;
static HANDLE expected_module;
static LPCWSTR expected_name;
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
    if (!creation_delivered && (message == WM_SIZE || message == WM_MOVE ||
            message == WM_WINDOWPOSCHANGED))
        CHECK(FALSE); /* Original createw.c delivers these after WM_CREATE. */
    if (message == WM_CREATE) creation_delivered = TRUE;
    if (message == WM_NCCREATE) {
        current_scope->window = window; /* Production CBT owns this binding. */
        CHECK(wow_window_creation_menu(window, (CREATESTRUCTA *)lp));
        CHECK(((CREATESTRUCTA *)lp)->hMenu == GetMenu(window) ||
            (((CREATESTRUCTA *)lp)->style & WS_CHILD));
        if (reject_create) return FALSE;
    }
    if (message == WM_CREATE && !(GetWindowLongA(window,GWL_STYLE)&WS_CHILD)) {
        /* Public USER may use a different CREATESTRUCT for the two messages. */
        CHECK(wow_window_creation_menu(window, (CREATESTRUCTA *)lp));
        CHECK(((CREATESTRUCTA *)lp)->hMenu == GetMenu(window));
    }
    return DefWindowProcA(window,message,wp,lp);
}

static void run_case(DWORD style, HMENU explicit_menu, HWND parent,
    LPCWSTR resource, BOOL failure, BOOL reject)
{
    creation_scope scope = {0};
    wow_class_lookup_entry entry = {0};
    unsigned before = loads;
    BOOL should_load = !(style & WS_CHILD) && !explicit_menu && resource;
    HWND window;
    entry.hModule = expected_module = (HANDLE)0x13570001;
    expected_name = resource;
    entry.lpszClientUnicodeMenuName = (LPWSTR)resource;
    scope.menu_class = &entry;
    current_scope = &scope;
    fail_load = failure; reject_create = reject; last_menu = NULL;
    creation_delivered = FALSE;
    window = CreateWindowExA(0,"WOW_CLASS_MENU_FIXTURE","",style,
        0,0,100,100,parent,explicit_menu,GetModuleHandleA(NULL),NULL);
    CHECK(loads == before + (should_load ? 1u : 0u));
    CHECK(reject ? !window : !!window);
    if (window) {
        if (!(style&WS_CHILD)) CHECK(GetMenu(window) ==
            (explicit_menu ? explicit_menu : last_menu));
        CHECK(DestroyWindow(window));
    }
    if (last_menu) CHECK(!IsMenu(last_menu));
    current_scope = NULL;
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
    fprintf(stderr,"WOW_CREATION_MENU errors=%u loads=%u\n",errors,loads);
    return errors ? 1 : 0;
}
