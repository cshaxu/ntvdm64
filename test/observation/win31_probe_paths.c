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

static int bind_bytes(image *value, const char *marker, const void *bytes,
                      size_t written, size_t capacity)
{
    size_t i, found = 0, offset = 0, length = strlen(marker);
    if (!length || written > capacity) return 0;
    for (i = 0; i + length <= value->size; ++i) {
        if (!memcmp(value->bytes + i, marker, length)) { ++found; offset = i; }
    }
    if (found != 1 || value->size - offset < capacity) return 0;
    memset(value->bytes + offset, 0, capacity);
    memcpy(value->bytes + offset, bytes, written);
    return 1;
}

static int ascii_text(const char *text, int allow_cr)
{
    for (; *text; ++text)
        if (((unsigned char)*text < 32 || (unsigned char)*text > 126) &&
            !(allow_cr && *text == '\r' && !text[1])) return 0;
    return 1;
}

static int bind_field(image *value, const char *marker, const char *path)
{
    return strlen(marker) < 128 && strlen(path) < 128 && ascii_text(path, 0) &&
           bind_bytes(value, marker, path, strlen(path), 128);
}

/* Only the declared slots of dos-identity.asm and dos-retirement.asm.
 * Environment entries arrive separately: argv cannot carry embedded NULs. */
static int bind_identity(image *value, int argc, char **argv)
{
    static const struct { const char *marker; size_t capacity; } slots[] = {
        {"@VM_IDENTITY_RESULT@", 128}, {"@VM_PAIR_READY@", 128},
        {"@VM_A_RESULT@", 128}, {"@VM_NESTED_OUTPUT@", 128},
        {"@VM_NATIVE_READY@", 128}, {"@VM_DIRECTORY@", 128},
        {"@VM_NATIVE_COMMAND@", 256}, {"@VM_ORDER_COMMAND@", 256},
        {"@VM_ROLE@", 1}
    };
    char environment[512] = {0};
    size_t used = 0, i;
    int at;
    if (argc < 2 || argc % 2) return 0;
    for (at = 0; at < argc; at += 2) {
        const char *name = argv[at], *text = argv[at + 1];
        size_t length = strlen(text);
        if (!strcmp(name, "--env")) {
            const char *equals = strchr(text, '=');
            if (!equals || equals == text || !ascii_text(text, 0) ||
                length + 2 > sizeof(environment) - used) return 0;
            memcpy(environment + used, text, length + 1);
            used += length + 1;
            continue;
        }
        for (i = 0; i < sizeof(slots) / sizeof(slots[0]); ++i)
            if (!strcmp(name, slots[i].marker)) break;
        if (i == sizeof(slots) / sizeof(slots[0])) return 0;
        if (slots[i].capacity == 1) {
            if (length != 1 || (text[0] != 'A' && text[0] != 'B')) return 0;
        } else if (length >= slots[i].capacity ||
                   !ascii_text(text, !strcmp(name, "@VM_ORDER_COMMAND@"))) return 0;
        if (!bind_bytes(value, name, text, length, slots[i].capacity)) return 0;
    }
    return !used || bind_bytes(value, "@VM_ENVIRONMENT@", environment,
                               used + 1, sizeof(environment));
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
    if (argc >= 4 && !strcmp(argv[1], "--identity")) {
        if (!load(argv[2], &dos) || !bind_identity(&dos, argc - 4, argv + 4)) goto done;
        if (save(argv[3], &dos)) result = 0;
        goto done;
    }
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
