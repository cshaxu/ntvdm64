#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Test preparation only: preserve the selected package except this boot key. */
int main(int argc, char **argv)
{
    FILE *file;
    long size;
    char *text, *line, *end, *tail;
    const char *value;
    unsigned boots = 0;
    char actual[32];
    char temporary[MAX_PATH];
    int result = 1;
    if (argc != 3) return 2;
    if (!strcmp(argv[2], "Empty")) value = "";
    else if (!strcmp(argv[2], "WfwNet")) value = "wfwnet.drv";
    else return 2;
    file = fopen(argv[1], "rb");
    if (!file) return 1;
    if (fseek(file, 0, SEEK_END) || (size = ftell(file)) < 0 ||
        fseek(file, 0, SEEK_SET)) { fclose(file); return 1; }
    text = malloc((size_t)size + 1);
    if (!text) { fclose(file); return 1; }
    if (fread(text, 1, (size_t)size, file) != (size_t)size) {
        free(text); fclose(file); return 1;
    }
    fclose(file);
    text[size] = 0;
    if (strlen(text) != (size_t)size) { free(text); return 2; }
    for (line = text; *line; line = *end ? end + 1 : end) {
        end = strchr(line, '\n');
        if (!end) end = line + strlen(line);
        if (end - line < 6 || _strnicmp(line, "[boot]", 6)) continue;
        tail = line + 6;
        while (tail < end && (*tail == ' ' || *tail == '\t' || *tail == '\r')) ++tail;
        if (tail == end) ++boots;
    }
    free(text);
    if (boots != 1) return 2;
    /* SYSTEM.INI has host registry mappings. Edit a private sibling name,
       then replace the owned file; never pass SYSTEM.INI to profile APIs. */
    if (strlen(argv[1]) + sizeof(".ntvdm-network") > sizeof(temporary)) return 2;
    strcpy(temporary, argv[1]);
    strcat(temporary, ".ntvdm-network");
    if (!CopyFileA(argv[1], temporary, TRUE)) return 1;
    if (!WritePrivateProfileStringA("boot", "network.drv", value, temporary)) goto done;
    /* Flush the API cache before the filesystem rename. This operation's
       return value is not the success contract of a key write. */
    WritePrivateProfileStringA(NULL, NULL, NULL, temporary);
    GetPrivateProfileStringA("boot", "network.drv", "missing", actual,
                            sizeof(actual), temporary);
    if (strcmp(actual, value)) goto done;
    if (MoveFileExA(temporary, argv[1], MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) result = 0;
done:
    if (result) DeleteFileA(temporary);
    return result;
}
