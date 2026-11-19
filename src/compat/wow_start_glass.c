/* ADAPTER-WIN32-039: preserve the existing no-presentation binding.
 * OpenNT 5c5b979ec08c17d3ca2eb70e8aad62d26515d01c,
 * windows/core/ntuser/client/client.c:2998-3002 forwards this void request
 * to private NtUserCallOneParam(SFI__SHOWSTARTGLASS). This adapter does not
 * implement that CSRSS presentation action. Each product retains its ABI. */
#include <windows.h>

void ShowStartGlass(DWORD timeout)
{
    (void)timeout;
}
