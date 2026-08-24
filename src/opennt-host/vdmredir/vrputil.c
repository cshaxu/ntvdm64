/*
 * Directly retained OpenNT VDMREDIR source body.
 *
 * Original source: refs/opennt/base/mvdm/vdmredir/vrputil.c.
 */

#include <ctype.h>
#include <windows.h>
#include <lmcons.h>
#include <lmerr.h>

#include "vrputil.h" /* DIVERGENCE(HOST-DIV-018): private NT4
                               * product headers are replaced only by the
                               * public Win32/LAN Manager declarations used
                               * by this source body. */

WORD
VrpMapLastError(
    VOID
    )
{
    DWORD LastError;

    LastError = GetLastError();
    return VrpMapDosError(LastError);
}

WORD
VrpMapDosError(
    IN DWORD ErrorCode
    )
{
    switch (ErrorCode) {
    case NERR_UseNotFound:
        ErrorCode = ERROR_PATH_NOT_FOUND;
        break;
    }
    return (WORD)ErrorCode;
}

WORD
VrpTranslateDosNetPath(
    IN OUT LPSTR *InputString,
    OUT LPSTR *OutputString
    )
{
    char ch;
    char lastCh = 0;
    int state = 0;
    int slashesToGo = 2;
    int charsToGo = 0;
    int maxLen = LM20_PATHLEN;

    /* DIVERGENCE(HOST-DIV-019): parenthesize the original assignment test so
     * current warning-as-error builds retain its exact scan/termination
     * ordering. */
    while ((ch = *((*InputString)++)) != 0) {
        --maxLen;
        if (maxLen < 0) break;
        if (ch == '/') ch = '\\';
        else ch = (char)toupper((unsigned char)ch);
        if (ch == '\\') {
            --slashesToGo;
            if (slashesToGo < 0) break;
        } else {
            if (lastCh == '\\') {
                if (slashesToGo) break;
                if (state == 0) {
                    state = 1;
                    charsToGo = LM20_CNLEN;
                    slashesToGo = 1;
                } else if (state == 1) {
                    state = 2;
                    charsToGo = LM20_NNLEN;
                    slashesToGo = 0;
                }
            }
            --charsToGo;
            if (charsToGo < 0) break;
        }
        *((*OutputString)++) = ch;
        lastCh = ch;
    }
    *((*OutputString)++) = 0;
    return ch ? (WORD)ERROR_INVALID_PARAMETER : 0;
}
