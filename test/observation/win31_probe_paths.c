/* Bind fixed output paths in independently authored Win31 test witnesses. */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct image { unsigned char *bytes; size_t size; } image;

static int load(const char *path, image *value)
{
    FILE *file = fopen(path, "rb");
    long size;
    if (!file) return 0;
    if (fseek(file, 0, SEEK_END) || (size = ftell(file)) <= 0 ||
        size > 16 * 1024 * 1024 || fseek(file, 0, SEEK_SET)) {
        fclose(file); return 0;
    }
    value->size = (size_t)size;
    value->bytes = (unsigned char *)malloc(value->size);
    if (!value->bytes) { fclose(file); return 0; }
    if (fread(value->bytes, 1, value->size, file) != value->size) {
        fclose(file); return 0;
    }
    return fclose(file) == 0;
}

static int bind_field(image *value, const char *marker, const char *path)
{
    size_t i, found = 0, offset = 0, length = strlen(marker), written = strlen(path);
    if (!length || length >= 128 || written >= 128) return 0;
    for (i = 0; path[i]; ++i)
        if ((unsigned char)path[i] < 32 || (unsigned char)path[i] > 126) return 0;
    for (i = 0; i + length <= value->size; ++i) {
        if (!memcmp(value->bytes + i, marker, length)) { ++found; offset = i; }
    }
    if (found != 1 || value->size - offset < 128) return 0;
    memset(value->bytes + offset, 0, 128);
    memcpy(value->bytes + offset, path, written);
    return 1;
}

static int bind_path(image *value, const char *marker, const char *directory,
                     const char *name)
{
    char path[128];
    int written = snprintf(path, sizeof(path), "%s\\%s", directory, name);
    return written >= 0 && written < (int)sizeof(path) && bind_field(value, marker, path);
}
static int save(const char *path, const image *value)
{
    FILE *file = fopen(path, "wb");
    int ok;
    if (!file) return 0;
    ok = fwrite(value->bytes, 1, value->size, file) == value->size;
    if (fclose(file)) ok = 0;
    return ok;
}

int main(int argc, char **argv)
{
    image windows = {0}, dos = {0};
    int result = 1;
    if (argc >= 5 && !strcmp(argv[1], "--fields")) {
        int at;
        if ((argc - 3) % 2 || !load(argv[2], &windows)) goto done;
        for (at = 3; at < argc; at += 2) {
            /* Only named path slots in independently authored witnesses. */
            if ((strncmp(argv[at], "@VM_", 4) && strncmp(argv[at], "@W31_", 5)) ||
                argv[at][strlen(argv[at]) - 1] != '@' ||
                !bind_field(&windows, argv[at], argv[at + 1])) goto done;
        }
        if (save(argv[2], &windows)) result = 0;
        goto done;
    }
    if (argc != 4) return 2;
    if (!load(argv[1], &windows) || !load(argv[2], &dos)) goto done;
    /* Validate both complete inputs before writing either test copy. */
    if (!bind_path(&windows, "@W31_BOOT_PATH@", argv[3], "W31BOOT.BIN") ||
        !bind_path(&windows, "@W31_READY_PATH@", argv[3], "W31READY.BIN") ||
        !bind_path(&dos, "@W31_DOS_PATH@", argv[3], "W31DOS.BIN")) goto done;
    if (save(argv[1], &windows) && save(argv[2], &dos)) result = 0;
done:
    free(windows.bytes); free(dos.bytes);
    if (result) fputs("Win31 authored witness path binding failed\n", stderr);
    return result;
}
