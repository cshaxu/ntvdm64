/* Copyright (c) Microsoft Corporation. Original USER kernel header subset.
 * DIVERGENCE(OPENNT-HOST-049): retain pseudo-event closure and execution-owner
 * bookkeeping only; no private kernel object structures are imported. */
#line 16
#ifndef _USERK_
#define _USERK_
/* OPENNT-HOST-063: original object-definition control flags. */
#define OCF_THREADOWNED      0x0001
#define OCF_PROCESSOWNED     0x0002
#define OCF_MARKTHREAD       0x0004
#define OCF_USEQUOTA         0x0008
#ifndef WOW_USER_OBJECT_FLAGS_ONLY
#define PSEUDO_EVENT_ON ((HANDLE)0xFFFFFFFF)
#line 2119
#define PSEUDO_EVENT_OFF ((HANDLE)0x00000000)
#line 2138
#define CLOSE_PSEUDO_EVENT(phE)                                 \
    CheckCritIn();                                              \
    if (*(phE) == PSEUDO_EVENT_ON) *(phE) = PSEUDO_EVENT_OFF;   \
    else if (*(phE) != PSEUDO_EVENT_OFF) {                      \
        KeSetEvent(*(phE), EVENT_INCREMENT, FALSE);             \
        ObDereferenceObject(*(phE));                            \
        *(phE) = PSEUDO_EVENT_OFF;                              \
    }

#line 3935
#ifndef MSDWP
#line 3989
_inline
VOID
EnterWowCritSect(
    PTHREADINFO pti,
    PWOWPROCESSINFO pwpi
    )
{
   if (!++pwpi->CSLockCount) {
       /* DIVERGENCE(OPENNT-HOST-124): transfer the projected client view. */
       if (pti->host) pti->host->client_context(pwpi->CSOwningThread, pti);
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
#line 4010
_inline
VOID
ExitWowCritSect(
    PTHREADINFO pti,
    PWOWPROCESSINFO pwpi
    )
{
   if (pti == pwpi->CSOwningThread) {
       /* DIVERGENCE(OPENNT-HOST-124): suspend this thread's client view. */
       if (pti->host) pti->host->client_context(pti, NULL);
       pwpi->CSOwningThread = NULL;
       pwpi->CSLockCount--;
       }

   return;
}
#line 4104
#ifdef MEMPHIS_MENUS
#line 4105
#error Excluded unsupported original USER kernel condition
#else
#line 4113
#endif
#line 4121
#line 4121
#ifdef MEMPHIS_MENU_WATERMARKS
#line 4122
#error Excluded unsupported original USER kernel condition
#endif
#line 4124
#line 4788
#ifdef FE_IME
#line 4789
#error Excluded unsupported original USER kernel condition
#endif
#line 4793
#endif
#line 4865
#endif
#line 22
#ifndef _WINBASE_
#line 23
#line 54
#if(WINVER >= 0x0400)
#line 55
#endif
#line 57
#endif
#line 59
#line 61
#if DBG
#line 62
#error Excluded unsupported original USER kernel condition
#endif
#line 64
#line 65
#ifdef RIP_COMPONENT
#line 66
#endif
#line 68
#line 81
#ifdef TRACESYSPEEK
#line 82
#error Excluded unsupported original USER kernel condition
#else
#line 85
#endif
#line 88
#line 113
#ifdef DEBUG
#line 114
#error Excluded unsupported original USER kernel condition
#else
#line 117
#endif
#line 120
#line 253
#ifdef MEMPHIS_MENU_WATERMARKS
#line 254
#error Excluded unsupported original USER kernel condition
#endif
#line 267
#line 790
#ifdef DEBUG
#line 791
#error Excluded unsupported original USER kernel condition
#else
#line 793
#endif
#line 795
#line 799
#if DBG
#line 800
#error Excluded unsupported original USER kernel condition
#else
#line 843
#endif
#line 853
#line 893
#ifdef DEBUG
#line 894
#error Excluded unsupported original USER kernel condition
#else
#line 896
#endif
#line 910
#line 911
#ifdef DEBUG
#line 912
#error Excluded unsupported original USER kernel condition
#else
#line 918
#endif
#line 930
#line 931
#ifdef DEBUG
#line 932
#error Excluded unsupported original USER kernel condition
#else
#line 939
#endif
#line 947
#line 948
#ifdef DEBUG
#line 949
#error Excluded unsupported original USER kernel condition
#else
#line 955
#endif
#line 965
#line 966
#ifdef DEBUG
#line 967
#error Excluded unsupported original USER kernel condition
#else
#line 970
#endif
#line 973
#line 979
#ifdef DEBUG
#line 980
#error Excluded unsupported original USER kernel condition
#else
#line 983
#endif
#line 985
#line 1048
#ifdef DEBUG
#line 1049
#error Excluded unsupported original USER kernel condition
#else
#line 1056
#endif
#line 1062
#line 1084
#ifdef DEBUG
#line 1085
#error Excluded unsupported original USER kernel condition
#else
#line 1096
#endif
#line 1102
#line 1219
#ifdef POOL_TAGGING
#line 1220
#else
#line 1246
#endif
#line 1270
#line 1271
#ifdef DEBUG
#line 1272
#error Excluded unsupported original USER kernel condition
#else
#line 1279
#endif
#line 1281
#line 1535
#ifdef DEBUG
#line 1536
#error Excluded unsupported original USER kernel condition
#endif
#line 1539
#line 1555
#ifdef DEBUG
#line 1556
#error Excluded unsupported original USER kernel condition
#endif
#line 1560
#line 1622
#ifdef FE_IME
#line 1623
#error Excluded unsupported original USER kernel condition
#endif
#line 1634
#line 1659
#ifdef FE_IME
#line 1660
#error Excluded unsupported original USER kernel condition
#endif
#line 1662
#line 1798
#ifdef DEBUG
#line 1799
#error Excluded unsupported original USER kernel condition
#endif
#line 1801
#line 1875
#ifdef MEMPHIS_MENUS
#line 1876
#error Excluded unsupported original USER kernel condition
#endif
#line 1879
#line 1977
#ifdef MEMPHIS_MENU_ANIMATION
#line 1978
#error Excluded unsupported original USER kernel condition
#endif
#line 1980
#line 2343
#ifdef FE_IME
#line 2344
#error Excluded unsupported original USER kernel condition
#endif
#line 2348
#line 2595
#if DBG
#line 2596
#error Excluded unsupported original USER kernel condition
#endif
#line 2599
#line 2738
#ifdef KANJI
#line 2739
#error Excluded unsupported original USER kernel condition
#endif
#line 2743
#line 2907
#ifdef FE_SB // SfnINWPARAMDBCSCHAR()
#line 2908
#error Excluded unsupported original USER kernel condition
#endif
#line 2910
#line 2911
#ifdef FE_SB // SfnGETDBCSTEXTLENGTHS
#line 2912
#error Excluded unsupported original USER kernel condition
#endif
#line 2914
#line 3022
#ifdef LATER
#line 3023
#error Excluded unsupported original USER kernel condition
#endif
#line 3027
#line 3547
#ifdef MEMPHIS_MENU_ANIMATION
#line 3548
#error Excluded unsupported original USER kernel condition
#endif
#line 3554
#line 3568
#ifdef DEBUG
#line 3569
#error Excluded unsupported original USER kernel condition
#else
#line 3571
#endif
#line 3573
#line 3712
#ifndef MSDWP
#line 3713
#line 3761
#ifdef DISABLE
#line 3762
#error Excluded unsupported original USER kernel condition
#endif
#line 3765
#endif
#line 3774
#line 3781
#ifndef MSDWP
#line 3782
#line 3794
#ifdef LATER    // Hopefully we won't need these
#line 3795
#error Excluded unsupported original USER kernel condition
#endif
#line 3803
#endif
#line 3899
#line 5023
#ifdef FE_SB // PMAP_WINLOGON
#line 5024
#error Excluded unsupported original USER kernel condition
#else
#line 5027
#endif
#line 5029
#line 5105
#ifdef FE_IME
#line 5106
#error Excluded unsupported original USER kernel condition
#endif
#line 5216
#line 5217
#endif
