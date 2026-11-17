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

#include "wow_user_client_view_layout.h"
#include "wow_input_group_bindings.h"
#include "wow_edit_lookaside_bindings.h"
#line 14
#pragma hdrstop

//#define MARKPATH
#ifdef MARKPATH
#error
#endif


#line 22
#ifdef TRACESYSPEEK // is defined in userk.h
#error
#line 56
#endif //TRACESYSPEEK

#line 58
#if DBG
#error
#line 84
#endif  // DBG
#line 85

int LangToggleKeyState = 0;

PVOID KeyStateLookasideBase;
PVOID KeyStateLookasideBounds;
ZONE_HEADER KeyStateLookasideZone;
#line 91
#if DBG
#error
#line 97
#endif // DBG
#line 98

#define CANCEL_ACTIVESTATE  0
#define CANCEL_FOCUSSTATE   1
#define CANCEL_CAPTURESTATE 2

#define KEYSTATESIZE    (CBKEYSTATE + CBKEYSTATERECENTDOWN)


/*
 * xxxGetNextSysMsg return values
 */
#define PQMSG_PLAYBACK       ((PQMSG)1)

BOOL xxxScanSysQueue(PTHREADINFO ptiCurrent, LPMSG lpMsg, PWND pwndFilter,
        UINT msgMinFilter, UINT msgMaxFilter, DWORD flags, DWORD fsReason);
BOOL xxxReadPostMessage(PTHREADINFO pti, LPMSG lpMsg, PWND pwndFilter,
        UINT msgMin, UINT msgMax, BOOL fRemoveMsg);
void CleanEventMessage(PQMSG pqmsg);
void FreeQEntry(PQMSG pqmsg);
UINT GetMouseKeyFlags(PQ pq);
#line 238
#ifdef MARKPATH
#error
#line 243
#else
#define PATHTAKEN(x)
#define DUMPPATHTAKEN()
#endif
#line 1154
#ifdef SYSMODALWINDOWS
#error
#line 1181
#endif //LATER
#line 1409
#if 0
#error
#line 1430
#endif
#line 1641
#if DBG
#error
#endif // DBG
#line 1647
#if DBG
#error
#line 1653
#endif // DBG
#line 1669
#if DBG
#error
#endif // DBG
#line 1679
#if DBG
#error
#endif // DBG
#line 1764
#if (CBKEYSTATERECENTDOWN % 4) != 0
#error "CBKEYSTATERECENTDOWN assumed to be an integral number of DWORDs"
#endif
#line 2295
#ifdef NEVER
#error
#line 2309
#endif
#line 2924
#ifdef TRACESYSPEEK
#error
#endif

#line 2928
#ifdef DEBUG
#error
#line 2949
#endif
#line 2957
#ifdef DEBUG
#error
#line 2964
#endif
#line 2971
#ifdef DEBUG
#error
#endif
#line 3188
#if DBG
#error
#line 3230
#endif  // DBG
#line 3307
#ifdef FE_IME
#error
#endif
#ifdef MARKPATH
#error
#line 3320
#else
#define PATHTAKEN(x)
#define PATHTAKEN2(x)
#define PATHTAKEN3(x)
#define DUMPPATHTAKEN()
#define DUMPSUBPATHTAKEN(p, x)
#endif
#line 4551
#ifdef FE_IME
#error
#line 4575
#endif
#line 4750
#ifdef FE_IME
#error
#line 4757
#else
#line 4759
#endif
#line 4764
#if DBG
#error
#line 4767
#endif  // DBG
#line 4991
/***************************************************************************\
* RecalcThreadAttachment
* Recalc2
* AddAttachment
* CheckAttachment
*
* Runs through all the attachinfo fields for all threads and calculates
* which threads share which queues. Puts calculated result in pqAttach
* field in each threadinfo structure. This is a difficult problem
* whose only solution in iterative. The basic algorithm is:
*
* 0. Find next unattached thread and attach a queue to it. If none, stop.
* 1. Loop through all threads: If thread X assigned to this queue or any
*    of X's attach requests assigned to this queue, assign X and all X's
*    attachments to this queue. Remember if we ever attach a 16 bit thread.
* 2. If thread X is a 16 bit thread and we've already attached another
*    16 bit thread, assign X and all X's attachments to this queue.
* 3. If any change found in 1-2, goto 1
* 4. Goto 0
*
* 12-11-92 ScottLu      Created.
* 01-Oct-1993 mikeke    Fixed to work with MWOWs
\***************************************************************************/

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
#line 5046
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
#line 5295
#if DBG
#error
#line 5299
#endif
#line 5316
#if DBG
#error
#line 5320
#endif
