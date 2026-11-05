#include <stdio.h>
#include <string.h>
#include <wchar.h>

int __cdecl _swprintf(wchar_t *buffer, const wchar_t *format, ...);

int main(void)
{
    char text[64];
    wchar_t wide[64];
    int number = 0;
    char word[16];
    if (sprintf(text, "%s:%04d", "value", 37) != 10 ||
        strcmp(text, "value:0037") != 0) return 1;
    if (sscanf("42 token", "%d %15s", &number, word) != 2 ||
        number != 42 || strcmp(word, "token") != 0) return 2;
    if (sscanf("invalid", "%d", &number) != 0) return 3;
    if (_swprintf(wide, L"%ls:%d", L"wide", 37) != 7 ||
        wcscmp(wide, L"wide:37") != 0) return 4;
    puts("PASS CRT compatibility formatting, scanning and failure");
    return 0;
}
