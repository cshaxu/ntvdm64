/* Actual VrRemoteApi plus production RTL; NetAPI data and unavailable RAP
 * transport are controlled. No remote server or guest execution is claimed. */
#include <nt.h>
#include <windows.h>
#include <lm.h>
#include <rxp.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
NET_API_STATUS VrRemoteApi(DWORD, LPBYTE, LPSTR, LPSTR, LPSTR, BOOL);
static LPCWSTR name_value;
static DWORD api_error, freed, reached;
static WCHAR observed[64];
NET_API_STATUS NET_API_FUNCTION NetWkstaGetInfo(LMSTR server, DWORD level, LPBYTE *out)
{
    static WKSTA_INFO_100 info;
    if (server || level != 100) return ERROR_INVALID_PARAMETER;
    if (api_error) return api_error;
    memset(&info, 0, sizeof(info));
    info.wki100_computername = (LPWSTR)name_value;
    *out = (LPBYTE)&info;
    return 0;
}
NET_API_STATUS NET_API_FUNCTION NetApiBufferFree(LPVOID p)
{ if (p) ++freed; return 0; }
LPVOID _inlinePointerFromWords(WORD seg, WORD off)
{ return seg || off ? (LPVOID)1 : NULL; }
NET_API_STATUS RxpTransactSmb(LPTSTR name, LPTSTR transport, LPVOID sp, DWORD spl,
    LPVOID sd, DWORD sdl, LPVOID rp, DWORD rpl, LPVOID rd, LPDWORD rdl, BOOL np)
{
    ++reached; wcsncpy(observed, (LPCWSTR)name, 63);
    if (rpl >= 4) memset(rp, 0, 4);
    return ERROR_CALL_NOT_IMPLEMENTED;
}
int main(void)
{
    static LPCWSTR names[] = {L"AB", L"\x00e9", L"123456789012345", L"AB"};
    unsigned i, failures = 0;
    printf("ACP=%u OEM=%u CNLEN=%u\n", GetACP(), GetOEMCP(), CNLEN);
    for (i = 0; i < 4; ++i) {
        DWORD zero = 0, result; WCHAR expected[64]; CHAR ansi[64]; int ok;
        name_value = names[i]; api_error = i == 3 ? ERROR_ACCESS_DENIED : 0;
        freed = reached = 0; memset(observed, 0, sizeof(observed));
        wcscpy(expected, L"\\\\");
        if (!WideCharToMultiByte(CP_ACP, 0, name_value, -1,
                                ansi, sizeof(ansi), NULL, NULL) ||
            !MultiByteToWideChar(CP_ACP, 0, ansi, -1, expected + 2, 62))
            return 2;
        result = VrRemoteApi(0, (LPBYTE)&zero, "", "", "", FALSE);
        ok = i < 2 ? result == ERROR_CALL_NOT_IMPLEMENTED && reached == 1 && freed == 1 && !wcscmp(observed, expected) :
             i == 2 ? result == ERROR_INVALID_DATA && reached == 0 && freed == 1 :
                      result == ERROR_ACCESS_DENIED && reached == 0 && freed == 0;
        printf("case=%u result=%lu reached=%lu freed=%lu name_match=%d ok=%d\n", i, result, reached, freed, !wcscmp(observed, expected), ok);
        if (!ok) ++failures;
    }
    printf("cases=4 failures=%u; long-name rejection is retained boundary, not fixed\n", failures);
    return failures ? 1 : 0;
}
