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
void WINAPI wow_message_unwind32(struct _WM32MSGPARAMEX *);
void WINAPI wow_message_unwind16(struct _MSGPARAMEX *);
#endif
