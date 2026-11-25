#ifndef WOW_MESSAGE_LIFETIME_H
#define WOW_MESSAGE_LIFETIME_H
#include "compat/include/wow_message_progress.h"
struct _WM32MSGPARAMEX;
struct _MSGPARAMEX;
/* ADAPTER-WOW-101: completed acquisitions only, in existing call scratch. */
DWORD WINAPI wow_message_heap32(struct _WM32MSGPARAMEX *, DWORD);
void WINAPI wow_message_free16(DWORD *key, BOOL class_atom);
void WINAPI wow_message_release32(struct _WM32MSGPARAMEX *);
struct _TD;
typedef struct wow_message_stack {
    struct _TD *task;
    DWORD cursor;
    INT allocations;
    WORD task16;
    BOOL completed;
} wow_message_stack;
void WINAPI wow_message_stack_enter(wow_message_stack *);
void WINAPI wow_message_stack_leave(wow_message_stack *, BOOL abnormal);
void WINAPI wow_message_unwind32(struct _WM32MSGPARAMEX *);
void WINAPI wow_message_unwind16(struct _MSGPARAMEX *);
static __inline void wow_message_finish32(struct _WM32MSGPARAMEX *message, wow_message_stack *stack)
{
    __try {
        wow_message_unwind32(message);
    } __finally {
        wow_message_stack_leave(stack, AbnormalTermination());
    }
}
#endif
