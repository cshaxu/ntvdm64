#ifndef WOW_MESSAGE_LIFETIME_H
#define WOW_MESSAGE_LIFETIME_H
/* ADAPTER-WOW-095: call-local progress, never a second allocation registry. */
#define WOW_COPY_HEADER 1u
#define WOW_COPY_DATA 2u
#define WOW_COPY_DATA_NODE 4u
#define WOW_COPY_HEADER_NODE 8u
/* ADAPTER-WOW-098: progress of the original input-only text converters. */
#define WOW_TEXT_ACTIVE 1u
#define WOW_TEXT_MAPPED 2u
#define WOW_TEXT_STACK 4u
struct _WM32MSGPARAMEX;
struct _MSGPARAMEX;
/* ADAPTER-WOW-101: completed acquisitions only, in existing call scratch. */
DWORD WINAPI wow_message_heap32(struct _WM32MSGPARAMEX *, DWORD);
void WINAPI wow_message_free16(DWORD *key, BOOL class_atom);
void WINAPI wow_message_release32(struct _WM32MSGPARAMEX *);
struct _TD;
typedef struct wow_message_stack {
    struct _TD *task;
    DWORD cursor, init;
    WORD task16;
    BOOL completed;
} wow_message_stack;
void WINAPI wow_message_stack_enter(wow_message_stack *);
void WINAPI wow_message_stack_leave(wow_message_stack *, BOOL abnormal);
void WINAPI wow_message_unwind32(struct _WM32MSGPARAMEX *);
void WINAPI wow_message_unwind16(struct _MSGPARAMEX *);
#endif
