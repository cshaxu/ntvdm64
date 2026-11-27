/* Exercise the actual product RAP objects and XP validation contract. */
#include <windows.h>
#include <lmcons.h>
#include <rap.h>
#include <stdio.h>
#include <string.h>
static unsigned checks, failures;
#define CHECK(x) do { ++checks; if (!(x)) { ++failures; printf("FAIL line %u: %s\n", (unsigned)__LINE__, #x); } } while (0)
int main(void)
{
    DWORD input = 0x12345678, output = 0, required = 0;
    BYTE bytes[4] = {1, 2, 3, 4}, copied[4] = {0};
    WORD word = 0x4321;
    LPBYTE end = (LPBYTE)(&output + 1);
    CHECK(RapStructureSize("WD", Response, FALSE) == 6);
    CHECK(RapStructureSize("WD", Response, TRUE) == 8);
    CHECK(RapStructureSize("z", Response, FALSE) == sizeof(DWORD));
    CHECK(RapConvertSingleEntry((LPBYTE)&input, "D", FALSE, (LPBYTE)&output,
        (LPBYTE)&output, "D", FALSE, &end, &required, Response, RapToRap) == 0);
    CHECK(output == input && required == sizeof(input));
    required = 0; end = copied + sizeof(copied);
    CHECK(RapConvertSingleEntryEx(bytes, "B4", FALSE, copied, copied, "B4",
        FALSE, &end, &required, Response, RapToRap, 0) == 0);
    CHECK(required == 4 && memcmp(bytes, copied, 4) == 0);
    required = 0; output = 0; end = (LPBYTE)(&output + 1);
    CHECK(RapConvertSingleEntry((LPBYTE)&word, "W", FALSE, (LPBYTE)&output,
        (LPBYTE)&output, "D", FALSE, &end, &required, Response, RapToRap) == 0);
    CHECK(output == word && required == sizeof(output));
    CHECK(RapConvertSingleEntryEx(NULL, "D", FALSE, NULL, NULL, "D", FALSE,
        NULL, &required, Response, RapToRap, 0) == ERROR_INVALID_PARAMETER);
    CHECK(RapConvertSingleEntry((LPBYTE)&input, "D", FALSE, NULL, NULL, "D",
        FALSE, NULL, &required, Response, (RAP_CONVERSION_MODE)99) == ERROR_INVALID_PARAMETER);
    printf("RAP product objects: %u checks, %u failures\n", checks, failures);
    return failures != 0;
}
