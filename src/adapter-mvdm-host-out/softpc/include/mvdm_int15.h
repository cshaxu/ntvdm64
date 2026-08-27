#ifndef MVDM_INT15_H
#define MVDM_INT15_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Source-shaped firmware callout from softpc.new/base/keymouse/keybd_io.c. */
void UpdateKbdInt15(uint16_t segment, uint16_t offset);

/* Composition-only lifecycle hooks corresponding to keyboard_init's initial
 * IVT capture. They own no keyboard, PIC, IRQ or CPU-execution semantics. */
void mvdm_int15_capture_vector(void);
void mvdm_int15_reset_vector(void);
int mvdm_int15_matches_current_vector(void);

#ifdef __cplusplus
}
#endif

#endif
