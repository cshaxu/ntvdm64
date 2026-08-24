/* DBG's historical source uses the old SEH spellings after the OpenNT native
 * NT preamble has already established exception declarations. */
#ifndef APP_OPENNT_DBG_SEH_SPELLING_H
#define APP_OPENNT_DBG_SEH_SPELLING_H

#define try __try
#define finally __finally
#define except __except

#endif
