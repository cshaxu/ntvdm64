#include "wow_class_query_bindings.h"
#include <stdio.h>

static unsigned errors;
#define CHECK(x) do { if (!(x)) { ++errors; printf("FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)

int __cdecl main(void)
{
    wow_class_lookup_context context = {0}, other = {0};
    wow_user_borrow_scope outer, inner;
    WNDCLASSEXA wc = {sizeof(wc)};
    UNICODE_STRING captured;
    DWORD words[2] = {0x12345678, 0x9abcdef0};
    DWORD *first, *nested;
    char name[161];
    WCHAR wide_name[161];
    ATOM atom;
    unsigned i;
    for (i = 0; i < 160; ++i) { name[i] = 'Q'; wide_name[i] = L'Q'; }
    name[160] = 0; wide_name[160] = 0;
    RtlInitUnicodeString(&captured, wide_name);
    wc.lpfnWndProc = DefWindowProcA;
    wc.hInstance = GetModuleHandleA(NULL); wc.lpszClassName = name;
    atom = wow_class_words_register(&context, &wc, &captured, words, NULL);
    CHECK(atom != 0);
    if (!atom) return 1;
    CHECK(!GetClassWOWWords(wc.hInstance, name));
    CHECK(GetLastError() == ERROR_CLASS_DOES_NOT_EXIST);
    CHECK(wow_user_borrow_enter(&outer)); outer.classes = &context;
    CHECK(!GetClassWOWWords(wc.hInstance, NULL));
    CHECK(GetLastError() == ERROR_CLASS_DOES_NOT_EXIST);
    CHECK(!GetClassWOWWords(wc.hInstance, ""));
    CHECK(GetLastError() == ERROR_CLASS_DOES_NOT_EXIST);
    CHECK(!GetClassWOWWords(wc.hInstance, "missing"));
    CHECK(GetLastError() == ERROR_CLASS_DOES_NOT_EXIST);
    first = (DWORD *)GetClassWOWWords(wc.hInstance, name);
    CHECK(first != NULL);
    if (first) CHECK(first[0] == words[0] && first[1] == words[1]);
    name[0] = 'q';
    CHECK((DWORD *)GetClassWOWWords(wc.hInstance, name) == first);
    CHECK(wow_user_borrow_enter(&inner));
    nested = (DWORD *)GetClassWOWWords(wc.hInstance, name);
    CHECK(nested == first);
    inner.classes = &other;
    CHECK(!GetClassWOWWords(wc.hInstance, name));
    CHECK(GetLastError() == ERROR_CLASS_DOES_NOT_EXIST);
    CHECK(wow_user_borrow_leave(&inner));
    CHECK((DWORD *)GetClassWOWWords(wc.hInstance, name) == first);
    CHECK(wow_class_words_unregister(&context, atom, wc.hInstance, NULL));
    CHECK(!GetClassWOWWords(wc.hInstance, name));
    CHECK(GetLastError() == ERROR_CLASS_DOES_NOT_EXIST);
    if (first) CHECK(first[0] == words[0] && first[1] == words[1]);
    CHECK(wow_user_borrow_leave(&outer));
    CHECK(outer.classes == NULL);
    printf("WOW_ORIGINAL_CLASS_QUERY errors=%u real_capture=1 real_tls=1\n", errors);
    return errors != 0;
}
