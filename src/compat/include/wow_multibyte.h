#ifndef WOW_MULTIBYTE_H
#define WOW_MULTIBYTE_H
/* Original USER chartran.c, defaulting to CP_ACP. */
int MBToWCSEx(WORD, LPCSTR, int, LPWSTR *, int, BOOL);
#define MBToWCS(text, count, output, capacity, allocate) \
    MBToWCSEx(0, text, count, output, capacity, allocate)
#endif
