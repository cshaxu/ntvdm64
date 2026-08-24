/*
 * This preamble deliberately does not include UCRT headers. Some CCPU units
 * retain their historical declarations for host aliases such as getenv, so
 * each original compilation unit must establish its own CRT-header order.
 */
#ifndef STAGE_CRT_PREAMBLE_H
#define STAGE_CRT_PREAMBLE_H

/*
 * The modern user-mode SDK supplies LIST_ENTRY but not the legacy NT list
 * helper macros used unchanged by qevnt.c. These preserve that historical
 * doubly-linked-list operation without adding a guest or host policy.
 */
#ifndef IsListEmpty
#define IsListEmpty(ListHead) ((ListHead)->Flink == (ListHead))
#endif

#ifndef InsertHeadList
#define InsertHeadList(ListHead, Entry)                                      \
    do {                                                                      \
        (Entry)->Flink = (ListHead)->Flink;                                   \
        (Entry)->Blink = (ListHead);                                          \
        (ListHead)->Flink->Blink = (Entry);                                  \
        (ListHead)->Flink = (Entry);                                         \
    } while (0)
#endif

#ifndef RemoveEntryList
#define RemoveEntryList(Entry)                                                \
    do {                                                                      \
        (Entry)->Blink->Flink = (Entry)->Flink;                              \
        (Entry)->Flink->Blink = (Entry)->Blink;                              \
    } while (0)
#endif

#endif
