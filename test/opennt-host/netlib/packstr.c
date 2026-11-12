/* Link the selected original packstr.obj; no copied packing implementation. */
#include <windows.h>
#include <stdio.h>
#include <string.h>
BOOL NetpCopyStringToBuffer(LPTSTR, DWORD, LPBYTE, LPTSTR *, LPTSTR *);
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
int main(void)
{
    char buffer[8], *end, *value;
    memset(buffer, '!', sizeof(buffer));
    end = buffer + 7;
    CHECK(NetpCopyStringToBuffer("abc", 3, (LPBYTE)(buffer + 3), &end, &value));
    CHECK(end == buffer + 3 && value == end && !strcmp(value, "abc"));
    CHECK(buffer[2] == '!' && buffer[7] == '!');
    end = buffer + 7;
    CHECK(!NetpCopyStringToBuffer("abcd", 4, (LPBYTE)(buffer + 3), &end, &value));
    CHECK(end == buffer + 7 && value == NULL && !strcmp(buffer + 3, "abc"));
    CHECK(NetpCopyStringToBuffer(NULL, 0, (LPBYTE)(buffer + 3), &end, &value));
    CHECK(end == buffer + 6 && value == end && *value == 0 && buffer[7] == '!');
    end = buffer + 3;
    CHECK(!NetpCopyStringToBuffer(NULL, 0, (LPBYTE)(buffer + 3), &end, &value));
    CHECK(end == buffer + 3 && value == NULL);
    puts("PASS original packstr: exact fit, overflow/no-write, NULL terminator, no-room");
    return 0;
}
