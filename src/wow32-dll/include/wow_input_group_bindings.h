#ifndef WOW_INPUT_GROUP_BINDINGS_H
#define WOW_INPUT_GROUP_BINDINGS_H
#include "wow_task_order_bindings.h"

/* ADAPTER-WOW-043: finite original desktop membership and attachment views.
 * The USER owner supplies serialized live membership and queue release;
 * no native connection policy or independent task registry is defined here. */
typedef struct wow_input_attachment {
    struct wow_input_attachment *paiNext;
    wow_task_order_thread *pti1, *pti2;
} wow_input_attachment;
typedef struct wow_input_desktop {
    LIST_ENTRY PtiList;
    wow_input_attachment *attachments;
    void (WINAPI *destroy_queue)(wow_task_order_queue *, wow_task_order_thread *);
    wow_task_order_queue *(WINAPI *allocate_queue)(struct wow_input_desktop *);
    struct wow_cleanup_desktop_info *pDeskInfo;
} wow_input_desktop;
void WINAPI AddAttachment(wow_task_order_thread *, wow_task_order_queue *, LPBOOL);
void WINAPI Recalc2(wow_task_order_queue *, wow_task_order_thread *);
void WINAPI RecalcThreadAttachment(wow_task_order_thread *);
typedef wow_task_order_queue *PQ;
typedef wow_task_order_thread THREADINFO, *PTHREADINFO;
typedef wow_input_attachment *PATTACHINFO;
#define PtiCurrent() ptiCurrent
#define gpai (ptiCurrent->rpdesk->attachments)
#define DestroyQueue(queue, thread) (thread)->rpdesk->destroy_queue(queue,thread)
#define TIF_16BIT WOW_TASK_TIF_16BIT
/* This original owner requests only AllocQueue(NULL,NULL). */
#define AllocQueue(thread, source) ptiCurrent->rpdesk->allocate_queue(ptiCurrent->rpdesk)
#endif
