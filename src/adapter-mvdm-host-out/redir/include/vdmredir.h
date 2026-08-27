#ifndef MVDM_REDIRECTOR_VDMREDIR_BINDING_H
#define MVDM_REDIRECTOR_VDMREDIR_BINDING_H

/* DIVERGENCE(ADAPTER-REDIR-001): retain the complete original declaration
 * carrier, but turn the historical 32-bit handle coincidence into resolution
 * through the session host-resource mapping instance. */
#include "../../../mvdm-support/inc/vdmredir.h"
#include "mvdm_redirector_handle.h"

#undef HANDLE_FROM_WORDS
#define HANDLE_FROM_WORDS(high_word, low_word) \
    mvdm_redirector_handle_from_words((WORD)(high_word), (WORD)(low_word))

#endif
