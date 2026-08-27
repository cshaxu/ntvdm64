#ifndef MVDM_REDIRECTOR_MAILSLOT_H
#define MVDM_REDIRECTOR_MAILSLOT_H

#include <windows.h>

/*
 * The original mailslot provider exposes an arbitrary DOS WORD handle.  This
 * facade keeps that representation while resolving it only through the
 * session's existing host-resource mapping instance.  A native pointer or
 * HANDLE is never encoded in the WORD value.
 */
int mvdm_redirector_mailslot_publish(void *record, WORD *handle_out);
void *mvdm_redirector_mailslot_resolve(WORD handle);
int mvdm_redirector_mailslot_release(WORD handle);

#endif
