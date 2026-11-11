/* Native boundary observation only; no guest or Window presentation claim.
 * Use the original CONSOLE_GRAPHICS_BUFFER_INFO declaration and the exact
 * selected SoftPC top-down, BI_RGB, DIB_PAL_COLORS creation contract. */
#include <windows.h>
#include <stdio.h>
#include "conapi.h"
#undef CreateConsoleScreenBuffer
#undef CloseHandle

int main(void)
{
    struct {
        BITMAPINFOHEADER header;
        WORD indices[256];
    } bitmap;
    CONSOLE_GRAPHICS_BUFFER_INFO graphics;
    HANDLE control, output;
    unsigned int depth, index, supported = 0;
    DWORD error;

    control = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    if (control == INVALID_HANDLE_VALUE) {
        printf("INVALID TEST: text control error=%lu\n", GetLastError());
        return 1;
    }
    CloseHandle(control);
    for (depth = 1; depth <= 8; depth += 7) {
        ZeroMemory(&bitmap, sizeof(bitmap));
        bitmap.header.biSize = sizeof(bitmap.header);
        bitmap.header.biWidth = 640;
        bitmap.header.biHeight = -400;
        bitmap.header.biPlanes = 1;
        bitmap.header.biBitCount = (WORD)depth;
        bitmap.header.biCompression = BI_RGB;
        bitmap.header.biSizeImage = 640 * 400 / 8 * depth;
        for (index = 0; index < (1u << depth); ++index)
            bitmap.indices[index] = (WORD)index;
        ZeroMemory(&graphics, sizeof(graphics));
        graphics.dwBitMapInfoLength = sizeof(bitmap.header) + (1u << depth) * sizeof(WORD);
        graphics.lpBitMapInfo = (BITMAPINFO *)&bitmap;
        graphics.dwUsage = DIB_PAL_COLORS;
        SetLastError(0);
        output = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CONSOLE_GRAPHICS_BUFFER, &graphics);
        error = GetLastError();
        printf("depth=%u created=%u error=%lu bitmap=%u mutex=%u\n", depth,
            output != INVALID_HANDLE_VALUE, error,
            graphics.lpBitMap != NULL, graphics.hMutex != NULL);
        if (output != INVALID_HANDLE_VALUE) {
            if (graphics.lpBitMap && graphics.hMutex) ++supported;
            CloseHandle(output);
            if (graphics.hMutex) CloseHandle(graphics.hMutex);
        }
    }
    puts(supported == 2 ? "AVAILABLE: allocation only; no frame delivery tested" :
        "UNAVAILABLE: original graphics allocation contract not supplied");
    return supported == 2 ? 0 : 2;
}
