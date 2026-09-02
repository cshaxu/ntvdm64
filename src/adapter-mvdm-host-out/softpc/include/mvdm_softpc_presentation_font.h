#ifndef MVDM_SOFTPC_PRESENTATION_FONT_H
#define MVDM_SOFTPC_PRESENTATION_FONT_H

#include <stdint.h>

#include "session/session.h"

#define MVDM_SOFTPC_PRESENTATION_FONT_BYTES (256u * 16u)

/* Copy the source-selected SoftPC 8x16 font through the original SAS read
 * contract. The destination is app-owned; no source pointer crosses this ABI. */
int mvdm_softpc_presentation_font_snapshot(session *owner, uint8_t *bytes,
    uint32_t capacity);

#endif
