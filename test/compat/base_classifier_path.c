/* The caller uses the fixed original layout; the provider is the real product object. */
#include <nt.h>
#include <nturtl.h>
#include "compat/include/base_classifier_path.h"
#include <stdio.h>
#include <string.h>

int main(void)
{
    const WCHAR *names[] = {L"C:\\Windows\\not-required.exe", L"relative.exe", L""};
    unsigned i;
    unsigned failures = 0, successes = 0, rejected = 0;
    C_ASSERT(sizeof(RTL_RELATIVE_NAME) == (sizeof(void *) == 4 ? 12 : 24));
    for (i = 0; i < sizeof(names) / sizeof(names[0]); ++i) {
        struct { ULONG before; RTL_RELATIVE_NAME relative; ULONG after; } guard;
        RTL_RELATIVE_NAME expected;
        UNICODE_STRING actual = {0}, native = {0};
        PWSTR actual_part = NULL, native_part = NULL;
        BOOLEAN a, n;
        memset(&guard, 0xa5, sizeof(guard));
        memset(&expected, 0xa5, sizeof(expected));
        n = RtlDosPathNameToNtPathName_U(names[i], &native, &native_part, NULL);
        a = OpenNtBaseDosPathNameToNtPathName_U(names[i], &actual, &actual_part, &guard.relative);
        if (a) { memset(&expected, 0, sizeof(expected)); ++successes; }
        else ++rejected;
        if (a != n || guard.before != 0xa5a5a5a5UL || guard.after != 0xa5a5a5a5UL ||
            memcmp(&guard.relative, &expected, sizeof(expected))) ++failures;
        if (a && (actual.Length != native.Length ||
            memcmp(actual.Buffer, native.Buffer, actual.Length) ||
            (!!actual_part != !!native_part) ||
            (actual_part && (actual_part - actual.Buffer != native_part - native.Buffer)))) ++failures;
        if (a) RtlFreeUnicodeString(&actual);
        if (n) RtlFreeUnicodeString(&native);
        actual.Buffer = NULL;
        a = OpenNtBaseDosPathNameToNtPathName_U(names[i], &actual, NULL, NULL);
        if (a != n) ++failures;
        if (a) RtlFreeUnicodeString(&actual);
    }
    if (!successes || !rejected) ++failures;
    printf("CLASSIFIER_PATH failures=%u layout=%u cases=%u successes=%u rejected=%u\n", failures,
        (unsigned)sizeof(RTL_RELATIVE_NAME), i, successes, rejected);
    return failures ? 1 : 0;
}
