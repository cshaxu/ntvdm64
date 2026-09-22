#include "wow_class_words_binding.h"
#include <stdio.h>

static unsigned errors;
#define CHECK(x) do { if (!(x)) { ++errors; printf("FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)
int __cdecl main(void)
{
    static const char name[] = "S40_WC_LIFETIME";
    WNDCLASSEXA wc = {sizeof(wc)};
    wow_class_lookup_context context = {0};
    wow_class_words_binding *public_record, *private_record, *first, *second;
    DWORD public_words[2] = {0x12345678, 0x87654321};
    DWORD private_words[2] = {0x23456789, 0x98765432};
    HANDLE module = (HANDLE)0x23451234, outside = (HANDLE)0x45671234;
    ATOM atom, private_atom;
    HWND window;
    UNICODE_STRING captured_name, query;
    CLSMENUNAME resource_menu = {MAKEINTRESOURCEA(12), MAKEINTRESOURCEW(12), NULL};
    CLSMENUNAME returned_menu;
    WCHAR captured_buffer[] = L"S40_WC_LIFETIME";
    RtlInitUnicodeString(&captured_name, captured_buffer);
    wc.lpfnWndProc = DefWindowProcA; wc.hInstance = (HINSTANCE)module;
    wc.lpszClassName = name; wc.style = CS_GLOBALCLASS;
    CHECK(wow_class_words_register(&context, &wc, &captured_name, NULL, NULL) == 0);
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
    resource_menu.pusMenuName = &captured_name;
    atom = wow_class_words_register(&context, &wc, &captured_name, public_words, &resource_menu);
    CHECK(atom != 0);
    if (!atom) return 3;
    public_record = wow_class_words_acquire(&context, atom, outside);
    wc.style = 0;
    private_atom = wow_class_words_register(&context, &wc, &captured_name, private_words, NULL);
    CHECK(private_atom == atom);
    if (!private_atom) {
        wow_class_words_unregister(&context, atom, (HINSTANCE)module, &returned_menu);
        wow_class_words_release(public_record);
        return 3;
    }
    private_record = wow_class_words_acquire(&context, atom, module);
    /* Original client capture expires after registration. Registry must own
     * the name, use case-insensitive Unicode lookup, and retain native IDs. */
    captured_buffer[0] = L'X';
    RtlInitUnicodeString(&query, L"s40_wc_lifetime");
    CHECK(wow_class_words_find_atom(&context, &query) == atom);
    query.Buffer = MAKEINTRESOURCEW(atom); query.Length = query.MaximumLength = 0;
    CHECK(wow_class_words_find_atom(&context, &query) == atom);
    RtlInitUnicodeString(&query, L"S40_NOT_REGISTERED");
    CHECK(wow_class_words_find_atom(&context, &query) == 0);
    first = wow_class_words_acquire(&context, atom, module);
    second = wow_class_words_acquire(&context, atom, (HANDLE)0x23455678);
    CHECK(first == private_record && second == first);
    CHECK(wow_class_words_value(first)[0] == private_words[0]);
    wow_class_words_value(first)[1] = 0x11223344;
    CHECK(wow_class_words_value(second)[1] == 0x11223344);
    wow_class_words_release(second);
    second = wow_class_words_acquire(&context, atom, outside);
    CHECK(second == public_record);
    CHECK(wow_class_words_value(second)[1] == public_words[1]);
    wow_class_words_release(second);
    window = CreateWindowExA(0, name, "", 0, 0, 0, 32, 32,
        NULL, NULL, (HINSTANCE)module, NULL);
    CHECK(window != NULL);
    CHECK(!wow_class_words_unregister(&context, atom, (HINSTANCE)module, NULL));
    CHECK(GetLastError() == ERROR_CLASS_HAS_WINDOWS);
    CHECK(context.pclsPrivateList != NULL);
    CHECK(wow_class_words_value(first)[1] == 0x11223344);
    if (window) CHECK(DestroyWindow(window));
    if (wow_class_words_unregister(&context, atom, (HINSTANCE)module, NULL)) {
        /* A retained callback/reader survives unlink. New lookups now see
         * the public class, not a freed or per-window copy of this WC. */
        CHECK(wow_class_words_value(first)[1] == 0x11223344);
        second = wow_class_words_acquire(&context, atom, module);
        CHECK(second == public_record);
        wow_class_words_release(second);
    } else CHECK(FALSE);
    wow_class_words_release(first);
    wow_class_words_release(private_record);
    CHECK(wow_class_words_unregister(&context, atom, (HINSTANCE)module, &returned_menu));
    CHECK(returned_menu.pszClientAnsiMenuName == MAKEINTRESOURCEA(12));
    CHECK(returned_menu.pwszClientUnicodeMenuName == MAKEINTRESOURCEW(12));
    CHECK(returned_menu.pusMenuName == NULL);
    wow_class_words_release(public_record);
    CHECK(wow_class_words_acquire(&context, atom, module) == NULL);
    CHECK(GetLastError() == ERROR_CLASS_DOES_NOT_EXIST);
    CHECK(context.pclsPrivateList == NULL && context.pclsPublicList == NULL);
    RtlInitUnicodeString(&query, L"S40_WC_LIFETIME");
    CHECK(wow_class_words_find_atom(&context, &query) == 0);
    printf("WOW_CLASS_WORDS_NATIVE_LIFETIME errors=%u\n", errors);
    return errors ? 3 : 0;
}
