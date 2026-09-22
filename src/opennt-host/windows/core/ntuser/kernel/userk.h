/* Copyright (c) Microsoft Corporation. Original USER kernel header subset.
 * DIVERGENCE(OPENNT-HOST-049): retain pseudo-event closure and execution-owner
 * bookkeeping only; no private kernel object structures are imported. */
/* OPENNT-HOST-063: original object-definition control flags. */
#define OCF_THREADOWNED      0x0001
#define OCF_PROCESSOWNED     0x0002
#define OCF_MARKTHREAD       0x0004
#define OCF_USEQUOTA         0x0008
#ifndef WOW_USER_OBJECT_FLAGS_ONLY
#define PSEUDO_EVENT_ON ((HANDLE)0xFFFFFFFF)
_inline
VOID
EnterWowCritSect(
    PTHREADINFO pti,
    PWOWPROCESSINFO pwpi
    )
{
   if (!++pwpi->CSLockCount) {
       pwpi->CSOwningThread = pti;
       return;
       }

   RIPMSG2(RIP_ERROR,
         "MultipleWowTasks running simultaneously %x %x\n",
         pwpi->CSOwningThread,
         pwpi->CSLockCount
         );

   return;
}
#define PSEUDO_EVENT_OFF ((HANDLE)0x00000000)
#define CLOSE_PSEUDO_EVENT(phE)                                 \
    CheckCritIn();                                              \
    if (*(phE) == PSEUDO_EVENT_ON) *(phE) = PSEUDO_EVENT_OFF;   \
    else if (*(phE) != PSEUDO_EVENT_OFF) {                      \
        KeSetEvent(*(phE), EVENT_INCREMENT, FALSE);             \
        ObDereferenceObject(*(phE));                            \
        *(phE) = PSEUDO_EVENT_OFF;                              \
    }

_inline
VOID
ExitWowCritSect(
    PTHREADINFO pti,
    PWOWPROCESSINFO pwpi
    )
{
   if (pti == pwpi->CSOwningThread) {
       pwpi->CSOwningThread = NULL;
       pwpi->CSLockCount--;
       }

   return;
}
#endif
