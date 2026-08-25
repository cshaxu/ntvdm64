#include "byob_launch_declaration.h"

#include <stdio.h>
#include <string.h>

int main(void)
{
    wchar_t *arguments[] = { L"/c", L"hello" };
    wchar_t kind[4], tail[BYOB_LAUNCH_DECLARATION_TAIL_BYTES + 1u];
    byob_launch_declaration value, restored;
    if (!byob_launch_declaration_from_arguments(&value, L"TARGET.COM", 2, arguments) ||
        value.target_kind != BYOB_LAUNCH_TARGET_KIND_COM || value.tail_bytes != 8u ||
        memcmp(value.tail, "/c hello", 8u) != 0 ||
        !byob_launch_declaration_to_environment(&value, kind, 4u, tail, sizeof(tail) / sizeof(tail[0])) ||
        wcscmp(kind, L"com") != 0 || wcscmp(tail, L"/c hello") != 0 ||
        !byob_launch_declaration_from_environment(&restored, kind, tail) ||
        memcmp(&value, &restored, sizeof(value)) != 0 ||
        byob_launch_declaration_from_arguments(&value, L"OTHER.COM", 0, 0) ||
        byob_launch_declaration_from_environment(&value, L"com", L"bad=value") ||
        byob_launch_declaration_from_environment(&value, L"bat", L"")) return 1;
    puts("byob-launch-declaration-test: path-free target kind and tail contract verified");
    return 0;
}
