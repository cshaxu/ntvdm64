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
