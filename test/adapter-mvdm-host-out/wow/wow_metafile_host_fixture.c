/* Public-host capability witness for the selected original wgmeta.c provider.
 * It deliberately does not replace or extract the original guest bridge: the
 * fixture proves only that the legacy public GDI primitives it invokes retain
 * creation, bit-copy, replay, enumeration and failure behavior on this host. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

static int records;

static int CALLBACK count_record(HDC dc, HANDLETABLE *table,
    METARECORD *record, int object_count, LPARAM context)
{
    (void)dc;
    (void)table;
    (void)object_count;
    if (record == NULL || context != 0x4563) {
        return 0;
    }
    ++records;
    return 1;
}

int main(int argc, char **argv)
{
    HDC recording = NULL, playback = NULL;
    HBITMAP bitmap = NULL;
    HGDIOBJ old_bitmap = NULL, old_pen = NULL;
    HPEN pen = NULL;
    HMETAFILE original = NULL, copied = NULL, restored = NULL, reopened = NULL;
    BYTE *bits = NULL;
    UINT bit_count;
    COLORREF pixel;
    char copy_path[MAX_PATH];
    int result = 1;

    if (argc != 2) {
        return 2;
    }
    if (sprintf_s(copy_path, sizeof(copy_path), "%s.copy.wmf", argv[1]) < 0) {
        return 2;
    }
    recording = CreateMetaFileA(argv[1]);
    if (recording == NULL) {
        goto done;
    }
    pen = CreatePen(PS_SOLID, 1, RGB(0xff, 0, 0));
    if (pen == NULL || (old_pen = SelectObject(recording, pen)) == HGDI_ERROR ||
        !MoveToEx(recording, 1, 1, NULL) || !LineTo(recording, 20, 20)) {
        goto done;
    }
    SelectObject(recording, old_pen);
    old_pen = NULL;
    original = CloseMetaFile(recording);
    recording = NULL;
    if (original == NULL || (bit_count = GetMetaFileBitsEx(original, 0, NULL)) == 0) {
        goto done;
    }
    bits = (BYTE *)HeapAlloc(GetProcessHeap(), 0, bit_count);
    if (bits == NULL || GetMetaFileBitsEx(original, bit_count, bits) != bit_count ||
        (restored = SetMetaFileBitsEx(bit_count, bits)) == NULL ||
        (copied = CopyMetaFileA(original, copy_path)) == NULL ||
        (reopened = GetMetaFileA(copy_path)) == NULL) {
        goto done;
    }
    playback = CreateCompatibleDC(NULL);
    bitmap = CreateBitmap(32, 32, 1, 32, NULL);
    if (playback == NULL || bitmap == NULL ||
        (old_bitmap = SelectObject(playback, bitmap)) == HGDI_ERROR ||
        !PatBlt(playback, 0, 0, 32, 32, WHITENESS) ||
        !PlayMetaFile(playback, restored)) {
        goto done;
    }
    pixel = GetPixel(playback, 2, 2);
    if (pixel == CLR_INVALID || pixel == RGB(0xff, 0xff, 0xff) ||
        !EnumMetaFile(playback, reopened, count_record, 0x4563) || records == 0 ||
        SetMetaFileBitsEx(0, NULL) != NULL || PlayMetaFile(playback, NULL)) {
        goto done;
    }
    result = 0;
done:
    if (old_pen != NULL) {
        SelectObject(recording, old_pen);
    }
    if (old_bitmap != NULL) {
        SelectObject(playback, old_bitmap);
    }
    if (recording != NULL) {
        DeleteMetaFile(CloseMetaFile(recording));
    }
    if (pen != NULL) {
        DeleteObject(pen);
    }
    if (bitmap != NULL) {
        DeleteObject(bitmap);
    }
    if (playback != NULL) {
        DeleteDC(playback);
    }
    if (original != NULL) {
        DeleteMetaFile(original);
    }
    if (copied != NULL) {
        DeleteMetaFile(copied);
    }
    if (restored != NULL) {
        DeleteMetaFile(restored);
    }
    if (reopened != NULL) {
        DeleteMetaFile(reopened);
    }
    if (bits != NULL) {
        HeapFree(GetProcessHeap(), 0, bits);
    }
    DeleteFileA(argv[1]);
    DeleteFileA(copy_path);
    if (result == 0) {
        puts("WOW_METAFILE_PUBLIC_HOST_OK");
    }
    return result;
}
