#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "wow_private_user_compat.h"

/* Standalone native boundary proof with independently generated DIB data.
 * No original guest input is edited, rebuilt, or loaded by this test. */

static int test_resource(BOOL icon, WORD bpp, BOOL core, BOOL public_route)
{
    DWORD storage[1024] = {0};
    BYTE *bits = (BYTE *)storage;
    DWORD prefix = icon ? 0 : 4;
    DWORD header_size = core ? sizeof(BITMAPCOREHEADER) : sizeof(BITMAPINFOHEADER);
    BITMAPINFOHEADER *header = (BITMAPINFOHEADER *)(bits + prefix);
    DWORD colors = bpp <= 8 ? 1u << bpp : 0;
    DWORD xor_size = ((16u * bpp + 31u) / 32u) * 4u * 16u;
    DWORD palette_size = colors * (core ? sizeof(RGBTRIPLE) : sizeof(RGBQUAD));
    DWORD size = prefix + header_size + palette_size + xor_size + 64;
    HCURSOR object, repeated;
    ICONINFO info = {0};
    BITMAP bitmap = {0};
    int width = GetSystemMetrics(icon ? SM_CXICON : SM_CXCURSOR);
    int height = GetSystemMetrics(icon ? SM_CYICON : SM_CYCURSOR);
    int failed = 0;
    int monochrome_pixels = 1;
    UINT flags = LR_DEFAULTSIZE | LR_SHARED | (icon ? 0 : LR_MONOCHROME);

    if (!icon) { ((WORD *)bits)[0] = 2; ((WORD *)bits)[1] = 3; }
    if (core) {
        BITMAPCOREHEADER *old_header = (BITMAPCOREHEADER *)header;
        old_header->bcSize = sizeof(*old_header);
        old_header->bcWidth = 16;
        old_header->bcHeight = 32;
        old_header->bcPlanes = 1;
        old_header->bcBitCount = bpp;
    } else {
        header->biSize = sizeof(*header);
        header->biWidth = 16;
        header->biHeight = 32;
        header->biPlanes = 1;
        header->biBitCount = bpp;
    }
    if (colors) {
        BYTE *white = bits + prefix + header_size +
            (core ? sizeof(RGBTRIPLE) : sizeof(RGBQUAD));
        white[0] = white[1] = white[2] = 255;
    }
    memset(bits + prefix + header_size + palette_size, 0xaa, xor_size);
    if (public_route) {
        object = (HCURSOR)CreateIconFromResourceEx(bits, size, icon, 0x30000, 0, 0, flags);
        repeated = (HCURSOR)CreateIconFromResourceEx(bits, size, icon, 0x30000, 0, 0, flags);
    } else {
        object = wow_private_user_load_create_cursor_icon(NULL, NULL, 0x30a,
            MAKEINTRESOURCE(1), size, bits, icon ? RT_ICON : RT_CURSOR, FALSE);
        repeated = wow_private_user_load_create_cursor_icon(NULL, NULL, 0x30a,
            MAKEINTRESOURCE(1), size, bits, icon ? RT_ICON : RT_CURSOR, FALSE);
    }
    printf("route=%s icon=%d bpp=%u core=%d created=%d repeated_same=%d\n",
        public_route ? "public" : "binding", icon, bpp, core,
        object != NULL, object == repeated);
    if (!object || !GetIconInfo(object, &info)) {
        failed = 1;
    } else {
        GetObject(info.hbmColor ? info.hbmColor : info.hbmMask, sizeof(bitmap), &bitmap);
        if (info.hbmColor) {
            BITMAPINFO dib = {0};
            DWORD pixels[128 * 128];
            HDC dc = CreateCompatibleDC(NULL);
            int i;
            dib.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            dib.bmiHeader.biWidth = bitmap.bmWidth;
            dib.bmiHeader.biHeight = bitmap.bmHeight;
            dib.bmiHeader.biPlanes = 1;
            dib.bmiHeader.biBitCount = 32;
            if (!dc || bitmap.bmWidth > 128 || bitmap.bmHeight > 128 ||
                GetDIBits(dc, info.hbmColor, 0, bitmap.bmHeight, pixels, &dib,
                    DIB_RGB_COLORS) != (UINT)bitmap.bmHeight) {
                monochrome_pixels = 0;
                failed = 1;
            }
            else for (i = 0; i < bitmap.bmWidth * bitmap.bmHeight; ++i) {
                DWORD rgb = pixels[i] & 0xffffff;
                if (rgb != 0 && rgb != 0xffffff) monochrome_pixels = 0;
            }
            if (dc) DeleteDC(dc);
            printf("pixels_black_white=%d\n", monochrome_pixels);
        }
        printf("size=%ldx%ld expected=%dx%d hotspot=%lu,%lu color=%d output_bpp=%u\n",
            bitmap.bmWidth, info.hbmColor ? bitmap.bmHeight : bitmap.bmHeight / 2,
            width, height, info.xHotspot, info.yHotspot, info.hbmColor != NULL,
            bitmap.bmPlanes * bitmap.bmBitsPixel);
        if (bitmap.bmWidth != width ||
            (info.hbmColor ? bitmap.bmHeight : bitmap.bmHeight / 2) != height ||
            info.fIcon != icon ||
            info.xHotspot != (DWORD)(icon ? width / 2 : MulDiv(2, width, 16)) ||
            info.yHotspot != (DWORD)(icon ? height / 2 : MulDiv(3, height, 16)) ||
            (!icon && !monochrome_pixels) ||
            (icon && bpp == 24 && monochrome_pixels)) failed = 1;
        if (info.hbmColor) DeleteObject(info.hbmColor);
        if (info.hbmMask) DeleteObject(info.hbmMask);
    }
    /* Both routes now preserve original LR_SHARED: objects are system-owned
     * until process exit. Only the GetIconInfo bitmap copies are deleted. */
    return failed;
}

int main(int argc, char **argv)
{
    BOOL public_route = argc == 2 && strcmp(argv[1], "public") == 0;
    HCURSOR stock, again;
    BYTE bad[64] = {0};
    int failed = test_resource(TRUE, 1, FALSE, public_route);
    failed += test_resource(TRUE, 24, FALSE, public_route);
    failed += test_resource(FALSE, 1, FALSE, public_route);
    failed += test_resource(FALSE, 24, FALSE, public_route);
    failed += test_resource(TRUE, 1, TRUE, public_route);
    failed += test_resource(FALSE, 1, TRUE, public_route);
    stock = wow_private_user_load_create_cursor_icon(NULL, NULL, 0x30a,
        IDC_ARROW, 0, NULL, RT_CURSOR, FALSE);
    again = wow_private_user_load_create_cursor_icon(NULL, NULL, 0x30a,
        IDC_ARROW, 0, NULL, RT_CURSOR, FALSE);
    printf("stock_shared=%d\n", stock != NULL && stock == again);
    if (!stock || stock != again) ++failed;
    if (wow_private_user_load_create_cursor_icon(NULL, NULL, 0x30a,
            IDC_ARROW, sizeof(bad), bad, RT_CURSOR, FALSE) != NULL ||
        wow_private_user_load_create_cursor_icon(NULL, NULL, 0x30a,
            IDC_ARROW, 0, bad, RT_CURSOR, FALSE) != NULL ||
        wow_private_user_load_create_cursor_icon(NULL, NULL, 0x30a,
            IDC_ARROW, 0, NULL, RT_MENU, FALSE) != NULL) {
        ++failed;
        puts("invalid_resource_refused=0");
    } else puts("invalid_resource_refused=1");
    if (!failed) puts("WOW_CURSOR_RESOURCE_BOUNDARY_OK");
    return failed ? 1 : 0;
}
