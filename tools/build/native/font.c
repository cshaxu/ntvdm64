/* Fixed V7 bitmap extraction. No build commands or guest execution. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <bcrypt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int emit(FILE *out, const unsigned char *rom, unsigned offset,
                unsigned height, const char *name)
{
    unsigned glyph, row;
    fprintf(out, "static const unsigned char %s[256][%u] = {\n", name, height);
    for (glyph = 0; glyph != 256; ++glyph) {
        fputs("    {", out);
        for (row = 0; row != height; ++row)
            fprintf(out, "%s0x%02x", row ? "," : "", rom[offset + glyph * height + row]);
        fputs("},\n", out);
    }
    fputs("};\n", out);
    return !ferror(out);
}

static int same_file(const wchar_t *left, const wchar_t *right)
{
    FILE *a = _wfopen(left, L"rb"), *b = _wfopen(right, L"rb");
    int same = 0;
    if (a && b) {
        int x, y;
        do {
            x = fgetc(a);
            y = fgetc(b);
        } while (x == y && x != EOF);
        same = x == y && !ferror(a) && !ferror(b);
    }
    if (a) fclose(a);
    if (b) fclose(b);
    return same;
}

int wmain(int argc, wchar_t **argv)
{
    static const unsigned char expected[32] = {
        0x97,0x0f,0x10,0x5c,0xd9,0xe4,0x2e,0xe5,0x6f,0x07,0xaa,0xe6,0x95,0xba,0xc8,0x97,
        0x86,0xd3,0x45,0x5a,0xb9,0xd4,0xc1,0xea,0x9a,0x1d,0x16,0x43,0xb1,0xe8,0xf6,0xf0
    };
    BCRYPT_ALG_HANDLE algorithm = NULL;
    unsigned char *rom = NULL, digest[32];
    wchar_t *temporary = NULL;
    FILE *input = NULL, *output = NULL;
    long length;
    size_t count;
    int result = 1;
    if (argc != 3) {
        fputs("Usage: ntvdm-font V7VGA.ROM output-header\n", stderr);
        return 64;
    }
    input = _wfopen(argv[1], L"rb");
    if (!input || fseek(input, 0, SEEK_END) ||
        (length = ftell(input)) < 0x4990 || length > 1024 * 1024 ||
        fseek(input, 0, SEEK_SET)) goto done;
    rom = malloc((size_t)length);
    if (!rom || fread(rom, 1, (size_t)length, input) != (size_t)length || ferror(input)) goto done;
    if (!BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM, NULL, 0)) ||
        !BCRYPT_SUCCESS(BCryptHash(algorithm, NULL, 0, rom, (ULONG)length, digest, sizeof(digest))) ||
        memcmp(digest, expected, sizeof(expected))) {
        fputs("Selected V7VGA font source changed\n", stderr);
        goto done;
    }
    count = wcslen(argv[2]) + 5;
    temporary = malloc(count * sizeof(*temporary));
    if (!temporary || swprintf_s(temporary, count, L"%ls.tmp", argv[2]) < 0) goto done;
    output = _wfopen(temporary, L"wb");
    if (!output) goto done;
    fputs("/* Generated from pinned V7VGA EGA_CGMN bitmap data, not executable firmware. */\n", output);
    if (!emit(output, rom, 0x2230, 14, "frontend_native_font") ||
        !emit(output, rom, 0x3990, 16, "frontend_native_vga_font")) goto done;
    result = fclose(output);
    output = NULL;
    if (result) goto done;
    result = 1;
    if (same_file(temporary, argv[2])) {
        if (!DeleteFileW(temporary)) goto done;
    } else if (!MoveFileExW(temporary, argv[2], MOVEFILE_REPLACE_EXISTING)) goto done;
    result = 0;
done:
    if (output) fclose(output);
    if (input) fclose(input);
    if (algorithm) BCryptCloseAlgorithmProvider(algorithm, 0);
    if (result) {
        fputs("Font generation failed; existing output was not replaced\n", stderr);
        if (temporary) DeleteFileW(temporary);
    }
    free(temporary);
    free(rom);
    return result;
}
