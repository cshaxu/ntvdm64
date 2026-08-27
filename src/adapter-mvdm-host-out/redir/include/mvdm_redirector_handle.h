#ifndef MVDM_REDIRECTOR_HANDLE_H
#define MVDM_REDIRECTOR_HANDLE_H

#include <windows.h>

HANDLE mvdm_redirector_handle_from_words(WORD high_word, WORD low_word);

#endif
