/* S43 boundary evidence: BaseCheckVDM carries DOS command paths as OEM bytes.
 * Verify the original COMMAND-side uppercase operation under the current CRT
 * locale before attributing a non-ASCII launch failure to another component. */
#include <stdio.h>
#include <string.h>

static int equal_bytes(const unsigned char *left, const unsigned char *right,
    size_t count)
{
    return !memcmp(left, right, count);
}

int main(void)
{
    /* U+00E9 is byte 0x82 in CP437, which is the active OEM code page. */
    char path[] = "O:\\WINNT\\TESTS\\D43E\x82\\MEM.EXE";
    const unsigned char expected[] =
        "O:\\WINNT\\TESTS\\D43E\x82\\MEM.EXE";

    if (_strupr_s(path, sizeof(path)) != 0 || !equal_bytes((const unsigned char *)path,
            expected, sizeof(expected))) {
        printf("S43_OEM_COMMAND_UPPERCASE_MUTATED\n");
        return 1;
    }

    printf("S43_OEM_COMMAND_UPPERCASE_PRESERVED\n");
    return 0;
}
