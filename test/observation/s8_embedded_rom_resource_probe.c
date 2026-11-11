/* Verifies the exact S8 firmware resources without starting a VDM. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

static BOOL CALLBACK print_name(HMODULE module, LPCSTR type, LPSTR name,
                                LONG_PTR value)
{
    (void)module;
    (void)type;
    (void)value;
    if (IS_INTRESOURCE(name)) printf("  id=%u\n", (unsigned int)(ULONG_PTR)name);
    else printf("  name=%s\n", name);
    return TRUE;
}

static BOOL CALLBACK print_type(HMODULE module, LPSTR type, LONG_PTR value)
{
    (void)value;
    if (IS_INTRESOURCE(type)) printf("type=id-%u\n", (unsigned int)(ULONG_PTR)type);
    else printf("type=name-%s\n", type);
    EnumResourceNamesA(module, type, print_name, 0);
    return TRUE;
}

static int require_resource(HMODULE image, const char *name, DWORD expected)
{
    HRSRC resource = FindResourceA(image, name, RT_RCDATA);
    HRSRC literal_type = FindResourceA(image, name, "RCDATA");
    HGLOBAL loaded;

    if (resource == NULL) {
        printf("%s=missing\n", name);
        printf("%s=literal-type-%s\n", name, literal_type ? "present" : "missing");
        return 1;
    }
    if (SizeofResource(image, resource) != expected) {
        printf("%s=size-%lu\n", name,
            (unsigned long)SizeofResource(image, resource));
        return 1;
    }
    loaded = LoadResource(image, resource);
    if (loaded == NULL || LockResource(loaded) == NULL) {
        printf("%s=unreadable\n", name);
        return 1;
    }
    printf("%s=ok\n", name);
    return 0;
}

static int reject_resource(HMODULE image, const char *name)
{
    if (FindResourceA(image, name, RT_RCDATA) != NULL) {
        printf("%s=unexpected\n", name);
        return 1;
    }
    printf("%s=absent\n", name);
    return 0;
}

int main(int argc, char **argv)
{
    HMODULE image;
    int failed;

    if (argc != 2) return 64;
    image = LoadLibraryExA(argv[1], NULL, LOAD_LIBRARY_AS_DATAFILE);
    if (image == NULL) return 65;
    EnumResourceTypesA(image, print_type, 0);
    failed = require_resource(image, "SOFTPC_BIOS1", 28420u) ||
        require_resource(image, "SOFTPC_BIOS4", 8191u) ||
        require_resource(image, "SOFTPC_V7VGA", 18832u) ||
        reject_resource(image, "bios2.rom") ||
        reject_resource(image, "profile.spc") ||
        reject_resource(image, "cmos.ram");
    FreeLibrary(image);
    printf("s8-embedded-rom-resources=%s\n", failed ? "fail" : "pass");
    return failed ? 1 : 0;
}
