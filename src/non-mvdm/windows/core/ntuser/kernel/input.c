/****************************** Module Header ******************************\
* Module Name: input.c
*
* Copyright (c) 1985-91, Microsoft Corporation
*
* This module contains the core functions of the input sub-system
*
* History:
* 10-18-90 DavidPe      Created.
* 02-14-91 mikeke       Added Revalidation code
\***************************************************************************/

/* DIVERGENCE(OPENNT-HOST-057): original grouping bodies; explicit current
 * thread replaces implicit USER context. Native queue lifetime is required. */
#include "wow_input_group_bindings.h"

#line 238
#ifdef MARKPATH
#line 239
#error Excluded unsupported original MARKPATH get-message instrumentation
#else
#line 244
#define PATHTAKEN(x)
#define DUMPPATHTAKEN()
#endif
#line 247
#line 1409
#if 0
#line 1410
#error Excluded original disabled input branch
#endif
#line 1431
#line 2295
#ifdef NEVER
#line 2296
#error Excluded unsupported original NEVER input branch
#endif
#line 2310
#line 3310
#ifdef MARKPATH
#line 3311
#error Excluded unsupported original MARKPATH system-queue instrumentation
#else
#line 3321
#define PATHTAKEN(x)
#define PATHTAKEN2(x)
#define PATHTAKEN3(x)
#define DUMPPATHTAKEN()
#define DUMPSUBPATHTAKEN(p, x)
#endif
#line 3327
#line 5015
void AddAttachment(
    PTHREADINFO pti,
    PQ pqAttach,
    LPBOOL pfChanged)
{
    if (pti->pqAttach != pqAttach) {
        /*
         * LATER
         * !!! This is totally screwed up,  The only reason that this thing
         * could be non null is because two threads are going through
         * attachthreadintput() at the same time.  No one can predict
         * what kind of problems are going to be caused by that.
         * We leave the critical section in one place where we send
         * WM_CANCELMODE below.  We should figure out how to remove
         * the sendmessage.
         *
         * If there already is a queue there, as there may be, destroy it.
         * Note that DestroyQueue() will only get rid of the queue if the
         * thread reference count goes to 0.
         */
        PQ pqDestroy = pti->pqAttach;
        pti->pqAttach = pqAttach;
        if (pqDestroy != NULL)
            DestroyQueue(pqDestroy, pti);
        pqAttach->cThreads++;
        *pfChanged = TRUE;
    }
}

void Recalc2(
    PQ pqAttach,
    PTHREADINFO ptiCurrent)
{
    PATTACHINFO pai;
    PTHREADINFO pti;
    BOOL fChanged;
    PLIST_ENTRY pHead, pEntry;

    /*
     * Keep adding attachments until everything that should be attached to this
     * queue is attached
     */
    do {
        fChanged = FALSE;

        /*
         * If a thread is attached to this Q attach all of it's attachments
         * and MWOW buddies if they aren't already attached.
         */
        pHead = &PtiCurrent()->rpdesk->PtiList;
        for (pEntry = pHead->Flink; pEntry != pHead; pEntry = pEntry->Flink) {
            pti = CONTAINING_RECORD(pEntry, THREADINFO, PtiLink);

            if (pti->pqAttach == pqAttach) {
                /*
                 * check each of the attachments to see if this thread is attached
                 * to any other threads
                 */
                for (pai = gpai; pai != NULL; pai = pai->paiNext) {
                    /*
                     * if they weren't attached already, attach them
                     */
                    if (pai->pti1 == pti || pai->pti2 == pti) {
                        AddAttachment(
                                (pai->pti1 == pti) ? pai->pti2 : pai->pti1,
                                pqAttach,
                                &fChanged);
                    }
                }

                /*
                 * If this is a 16bit thread attach to all other threads in
                 * it's MWOW
                 */
                if (pti->TIF_flags & TIF_16BIT) {
                    PTHREADINFO ptiAttach;
                    PLIST_ENTRY pHeadAttach, pEntryAttach;

                    pHeadAttach = &pti->rpdesk->PtiList;
                    for (pEntryAttach = pHeadAttach->Flink;
                            pEntryAttach != pHeadAttach;
                            pEntryAttach = pEntryAttach->Flink) {
                        ptiAttach = CONTAINING_RECORD(pEntryAttach, THREADINFO, PtiLink);

                        if (ptiAttach->TIF_flags & TIF_16BIT &&
                            ptiAttach->ppi == pti->ppi) {
                            AddAttachment(ptiAttach, pqAttach, &fChanged);
                        }
                    }
                }
            }
        }
    } while (fChanged);
}

/* DIVERGENCE(OPENNT-HOST-058): original queue selection; explicit thread
 * context is passed through to Recalc2. */
void RecalcThreadAttachment(PTHREADINFO ptiCurrent)
{
    PTHREADINFO pti;
    PLIST_ENTRY pHead, pEntry;

    /*
     * For all threads, start an attach queue if a thread hasn't been
     * attached yet.
     */
    pHead = &PtiCurrent()->rpdesk->PtiList;
    for (pEntry = pHead->Flink; pEntry != pHead; pEntry = pEntry->Flink) {
        pti = CONTAINING_RECORD(pEntry, THREADINFO, PtiLink);

        /*
         * If the thread does not have a queue yet, don't
         * attach.
         */
        if (pti->pqAttach == NULL) {

            /*
             * Allocate a new queue for this thread if more than
             * one thread references it.
             */
            if (pti->pq->cThreads > 1) {
                pti->pqAttach = AllocQueue(NULL, NULL);

                if (pti->pqAttach == NULL) {
                    break;
                }

                pti->pqAttach->cThreads++;
            } else {
                pti->pqAttach = pti->pq;
            }

            /*
             * Attach every thread that is directly or indirectly attached
             * to this thread.
             */
            Recalc2(pti->pqAttach, ptiCurrent);
        }
    }
}


#line 58
#if DBG
#line 59
#error Excluded unsupported original DBG branch
#endif
#line 85

#line 91
#if DBG
#line 92
#error Excluded unsupported original DBG branch
#endif
#line 98

#line 3188
#if DBG
#line 3189
#error Excluded unsupported original DBG branch
#endif
#line 3231

#line 17
#ifdef MARKPATH
#line 18
#error Excluded unsupported original MARKPATH branch
#endif
#line 20

#line 22
#ifdef TRACESYSPEEK
#line 23
#error Excluded unsupported original TRACESYSPEEK branch
#endif
#line 57

#line 1154
#ifdef SYSMODALWINDOWS
#line 1155
#error Excluded unsupported original SYSMODALWINDOWS branch
#endif
#line 1182

#line 1764
#if (CBKEYSTATERECENTDOWN % 4) != 0
#line 1765
#error "CBKEYSTATERECENTDOWN assumed to be an integral number of DWORDs"
#endif
#line 1767

#line 1641
#if DBG
#line 1642
#error Excluded unsupported original DBG branch
#endif // DBG
#line 1644

#line 1647
#if DBG
#line 1648
#error Excluded unsupported original DBG branch
#endif // DBG
#line 1654

#line 1669
#if DBG
#line 1670
#error Excluded unsupported original DBG branch
#endif // DBG
#line 1672

#line 1679
#if DBG
#line 1680
#error Excluded unsupported original DBG branch
#endif // DBG
#line 1682

#line 2924
#ifdef TRACESYSPEEK
#line 2925
#error Excluded unsupported original TRACESYSPEEK branch
#endif
#line 2927

#line 4764
#if DBG
#line 4765
#error Excluded unsupported original DBG branch
#endif  // DBG
#line 4768

#line 5295
#if DBG
#line 5296
#error Excluded unsupported original DBG branch
#endif
#line 5300

#line 5316
#if DBG
#line 5317
#error Excluded unsupported original DBG branch
#endif
#line 5321


#line 2928
#ifdef DEBUG
#line 2929
#error Unsupported DEBUG original branch
#endif
#line 2950

#line 2957
#ifdef DEBUG
#line 2958
#error Unsupported DEBUG original branch
#endif
#line 2965

#line 2971
#ifdef DEBUG
#line 2972
#error Unsupported DEBUG original branch
#endif
#line 2974

#line 3307
#ifdef FE_IME
#line 3308
#error Unsupported FE_IME original branch
#endif
#line 3310

#line 4551
#ifdef FE_IME
#line 4552
#error Unsupported FE_IME original branch
#endif
#line 4576

#line 4750
#ifdef FE_IME
#line 4751
#error Unsupported FE_IME original branch
#else
#line 4758
#endif
#line 4760
