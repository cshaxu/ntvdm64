#include <windows.h>
#include <stdio.h>

/* Research only: can public class registration normalize native CPD without
 * reading private USER layouts? No production resolver is selected here. */
static unsigned failures;
#define CHECK(c) do { if (!(c)) { ++failures; printf("FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)
static LRESULT CALLBACK proc_a(HWND w, UINT m, WPARAM a, LPARAM b)
{ return DefWindowProcA(w,m,a,b); }
static LRESULT CALLBACK proc_w(HWND w, UINT m, WPARAM a, LPARAM b)
{ return DefWindowProcW(w,m,a,b); }
static void probe(BOOL source_ansi)
{
    const WCHAR *source = source_ansi ? L"T447CPDSourceA" : L"T447CPDSourceW";
    WNDCLASSEXW wc = {sizeof(wc)}, read_w = {sizeof(read_w)};
    WNDCLASSEXA ca = {sizeof(ca)}, read_a = {sizeof(read_a)};
    HINSTANCE module = GetModuleHandleW(NULL);
    HWND window = NULL;
    WNDPROC token;
    ATOM source_atom = 0, scratch_atom = 0;
    ca.hInstance = module; ca.lpfnWndProc = proc_a; ca.lpszClassName = "T447CPDSourceA";
    wc.hInstance = module; wc.lpfnWndProc = proc_w; wc.lpszClassName = source;
    source_atom = source_ansi ? RegisterClassExA(&ca) : RegisterClassExW(&wc);
    CHECK(source_atom != 0); if (!source_atom) return;
    window = CreateWindowExW(0, MAKEINTATOM(source_atom), L"", 0, 0,0,0,0,
        NULL,NULL,module,NULL);
    CHECK(window != NULL); if (!window) goto cleanup;
    token = (WNDPROC)(source_ansi ? GetWindowLongPtrW(window,GWLP_WNDPROC) :
        GetWindowLongPtrA(window,GWLP_WNDPROC));
    printf("source_ansi=%u source_proc=%p token=%p\n", source_ansi,
        (void *)(source_ansi ? proc_a : proc_w), (void *)token);
    CHECK(HIWORD((ULONG_PTR)token) == 0xffff);
    /* Register through the consumer encoding that obtained the token. */
    if (source_ansi) {
        wc.lpszClassName = L"T447CPDScratch"; wc.lpfnWndProc = token;
        scratch_atom = RegisterClassExW(&wc);
    } else {
        ca.lpszClassName = "T447CPDScratch"; ca.lpfnWndProc = token;
        scratch_atom = RegisterClassExA(&ca);
    }
    printf("scratch_atom=%04x error=%lu\n", scratch_atom, GetLastError());
    CHECK(scratch_atom != 0); if (!scratch_atom) goto cleanup;
    CHECK(GetClassInfoExA(module, (LPCSTR)(ULONG_PTR)scratch_atom, &read_a));
    CHECK(GetClassInfoExW(module, MAKEINTATOM(scratch_atom), &read_w));
    printf("queried_A=%p queried_W=%p\n", (void *)read_a.lpfnWndProc, (void *)read_w.lpfnWndProc);
    CHECK((source_ansi ? read_a.lpfnWndProc : read_w.lpfnWndProc) ==
        (source_ansi ? proc_a : proc_w));
    CHECK(HIWORD((ULONG_PTR)(source_ansi ? read_w.lpfnWndProc : read_a.lpfnWndProc)) == 0xffff);
cleanup:
    if (scratch_atom) CHECK(UnregisterClassW(MAKEINTATOM(scratch_atom), module));
    if (window) CHECK(DestroyWindow(window));
    CHECK(UnregisterClassW(MAKEINTATOM(source_atom), module));
}
int main(void)
{
    probe(TRUE); probe(FALSE);
    printf("cpd_class_normalization_probe failures=%u\n", failures);
    return failures ? 1 : 0;
}
