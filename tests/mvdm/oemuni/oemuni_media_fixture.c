/* Test-only extraction of the actual DEM owner and its packed output type. */
#include <windows.h>
#include <stdio.h>
#include <string.h>
#define DOS_VOLUME_NAME_SIZE 11
#define NT_VOLUME_NAME_SIZE 255
#define FILESYS_NAME_SIZE 8
#include "dem-media-type.inc"
#define STOREDWORD(target, value) ((target) = (value))
static const char *label, *filesystem;
static int fail_query, calls, bad;
static BOOL query_volume(const char *root, char *outLabel, DWORD labelSize,
    DWORD *serial, DWORD *maxComponent, DWORD *flags, char *outFs, DWORD fsSize)
{
    ++calls;
    if (strcmp(root, "O:\\") || maxComponent || flags ||
        labelSize != NT_VOLUME_NAME_SIZE || fsSize != MAX_PATH) bad = 1;
    if (fail_query) { SetLastError(ERROR_NOT_READY); return FALSE; }
    strcpy(outLabel, label);
    strcpy(outFs, filesystem);
    *serial = 0x12345678;
    return TRUE;
}
#define GetVolumeInformationOem query_volume
#include "dem-media-body.inc"
#define CHECK(x) do { if (!(x)) { printf("S37_MEDIA_FAIL line=%d\n", __LINE__); return 1; } } while (0)
int main(void)
{
    struct { DWORD before; VOLINFO info; DWORD after; } guarded, saved;
    const char *labels[] = { "ABCDEFGHIJK", "A", "", "LONGERTHAN11CHARS" };
    const char *types[] = { "NTFS", "FAT", "", "12345678" };
    unsigned int row, i;
    CHECK(sizeof(VOLINFO) == 25);
    for (row = 0; row < 4; ++row) {
        memset(&guarded, 0xA5, sizeof(guarded));
        label = labels[row]; filesystem = types[row]; calls = bad = 0;
        CHECK(GetMediaId(14, &guarded.info) && calls == 1 && !bad);
        CHECK(guarded.before == 0xA5A5A5A5 && guarded.after == 0xA5A5A5A5);
        CHECK(guarded.info.usInfoLevel == 0xA5A5);
        CHECK(guarded.info.ulSerialNumber == 0x12345678);
        for (i = 0; i < 11; ++i)
            CHECK(guarded.info.VolumeID[i] == (i < strlen(label) ? label[i] : ' '));
        for (i = 0; i < 8; ++i)
            CHECK(guarded.info.FileSystemType[i] == (i < strlen(filesystem) ? filesystem[i] : ' '));
    }
    memset(&guarded, 0xA5, sizeof(guarded)); saved = guarded;
    calls = bad = 0; fail_query = 1;
    CHECK(!GetMediaId(14, &guarded.info) && calls == 1 && !bad);
    CHECK(GetLastError() == ERROR_NOT_READY && !memcmp(&guarded, &saved, sizeof(guarded)));
    puts("S37_DEM_MEDIA_PADDING_LABEL_CANARY_FAILURE_OK");
    return 0;
}
