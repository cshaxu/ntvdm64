#ifndef MVDM_PROTECTED_FRAME_TRANSACTION_H
#define MVDM_PROTECTED_FRAME_TRANSACTION_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Same-shaped SoftPC/monitor register access sometimes changes CS, SS, EIP
 * and ESP as one historical operation.  The transaction keeps that operation
 * copied until adapter-bochs accepts every selector and scalar together. */
int mvdm_protected_frame_transaction_begin(void);
int mvdm_protected_frame_transaction_commit(void);
void mvdm_protected_frame_transaction_abort(void);
int mvdm_protected_frame_transaction_active(void);

uint32_t mvdm_protected_frame_transaction_eip(void);
uint32_t mvdm_protected_frame_transaction_esp(void);
uint16_t mvdm_protected_frame_transaction_cs(void);
uint16_t mvdm_protected_frame_transaction_ss(void);
uint32_t mvdm_protected_frame_transaction_eflags(void);
void mvdm_protected_frame_transaction_set_eip(uint32_t value);
void mvdm_protected_frame_transaction_set_esp(uint32_t value);
void mvdm_protected_frame_transaction_set_cs(uint16_t value);
void mvdm_protected_frame_transaction_set_ss(uint16_t value);
void mvdm_protected_frame_transaction_set_eflags(uint32_t value);

#ifdef __cplusplus
}
#endif

#endif
