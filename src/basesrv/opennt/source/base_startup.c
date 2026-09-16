/* Same-shaped finite capture binding. Field selection is original
 * base/win32/client/vdm.c BaseCheckVDM, "Copy startupinfo" block.
 * No original launch policy is replaced; CSR native-pointer transport is not
 * available across standalone processes. */
#include <base_startup.h>
#include <string.h>
typedef char startup_wire_size[(sizeof(broker_vdm_startup)==40)?1:-1];

void OpenNtBaseEncodeStartup(const STARTUPINFOA *source, broker_vdm_startup *out)
{
    memset(out,0,sizeof(*out));
    if (!source) return;
    out->present=1;
    out->x=source->dwX; out->y=source->dwY;
    out->x_size=source->dwXSize; out->y_size=source->dwYSize;
    out->x_chars=source->dwXCountChars; out->y_chars=source->dwYCountChars;
    out->fill=source->dwFillAttribute; out->flags=source->dwFlags;
    out->show=source->wShowWindow;
}

BOOL OpenNtBaseDecodeStartup(const broker_vdm_startup *source, STARTUPINFOA *out)
{
    if (!source || !out || source->present>1 || source->show>0xffffu) return FALSE;
    if (!source->present && (source->x || source->y || source->x_size || source->y_size ||
        source->x_chars || source->y_chars || source->fill || source->flags || source->show)) return FALSE;
    memset(out,0,sizeof(*out));
    if (!source->present) return TRUE;
    out->cb=sizeof(*out);
    out->dwX=source->x; out->dwY=source->y;
    out->dwXSize=source->x_size; out->dwYSize=source->y_size;
    out->dwXCountChars=source->x_chars; out->dwYCountChars=source->y_chars;
    out->dwFillAttribute=source->fill; out->dwFlags=source->flags;
    out->wShowWindow=(WORD)source->show;
    return TRUE;
}
