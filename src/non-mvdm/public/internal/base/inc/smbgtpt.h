/*++

Copyright (c) 1990  Microsoft Corporation

Module Name:

    smbgtpt.h

Abstract:

    This module defines macros for retrieving and storing SMB data.
    The macros account for the misaligned nature of the SMB protocol.
    They also translate from the little-endian SMB format into
    big-endian format, when necessary.

Author:

    Chuck Lenzmeier (chuckl) 2-Mar-90
    David Treadwell (davditr)

Revision History:

    15-Apr-1991 JohnRo
        Include <smbtypes.h>, to define SMBDBG etc.
--*/

#ifndef _SMBGTPT_
#define _SMBGTPT_

#include <smbtypes.h>
//#include <smb.h>

//
// The following macros store and retrieve USHORTS and ULONGS from
// potentially unaligned addresses, avoiding alignment faults.  They
// would best be written as inline assembly code.
//
// The macros are designed to be used for accessing SMB fields.  Such
// fields are always stored in little-endian byte order, so these macros
// do byte swapping when compiled for a big-endian machine.
//
// !!! Not yet.
//

#if !SMBDBG

#define BYTE_0_MASK 0xFF

#define BYTE_0(Value) (UCHAR)(  (Value)        & BYTE_0_MASK)
#define BYTE_1(Value) (UCHAR)( ((Value) >>  8) & BYTE_0_MASK)
#define BYTE_2(Value) (UCHAR)( ((Value) >> 16) & BYTE_0_MASK)
#define BYTE_3(Value) (UCHAR)( ((Value) >> 24) & BYTE_0_MASK)

#endif

//++
//
// USHORT
// SmbGetUshort (
//     IN PSMB_USHORT SrcAddress
//     )
//
// Routine Description:
//
//     This macro retrieves a USHORT value from the possibly misaligned
//     source address, avoiding alignment faults.
//
// Arguments:
//
//     SrcAddress - where to retrieve USHORT value from
//
// Return Value:
//
//     USHORT - the value retrieved.  The target must be aligned.
//
//--

#if !SMBDBG

#if !SMBDBG1
#if SMB_USE_UNALIGNED
#define SmbGetUshort(SrcAddress) *(PSMB_USHORT)(SrcAddress)
#else
#line 84
#error SMB bytewise alignment profile is not supported
#line 88
#endif
#line 89
#else
#line 90
#error SMB debug field-name profile is not supported
#line 94
#endif
#line 95

#else
#line 97
#error SMB debug get/put profile is not supported
#line 103
#endif
#line 104

//++
//
// USHORT
// SmbGetAlignedUshort (
//     IN PUSHORT SrcAddress
//     )
//
// Routine Description:
//
//     This macro retrieves a USHORT value from the source address,
//     correcting for the endian characteristics of the server if
//     necessary.
//
// Arguments:
//
//     SrcAddress - where to retrieve USHORT value from; must be aligned.
//
// Return Value:
//
//     USHORT - the value retrieved.  The target must be aligned.
//
//--

#if !SMBDBG

#if !SMBDBG1
#define SmbGetAlignedUshort(SrcAddress) *(SrcAddress)
#else
#line 133
#error SMB debug field-name profile is not supported
#line 134
#endif
#line 135

#else
#line 137
#error SMB debug get/put profile is not supported
#line 143
#endif
#line 144

//++
//
// VOID
// SmbPutUshort (
//     OUT PSMB_USHORT DestAddress,
//     IN USHORT Value
//     )
//
// Routine Description:
//
//     This macro stores a USHORT value at the possibly misaligned
//     destination address, avoiding alignment faults.
//
// Arguments:
//
//     DestAddress - where to store USHORT value.  Address may be
//         misaligned.
//
//     Value - USHORT to store.  Value must be a constant or an aligned
//         field.
//
// Return Value:
//
//     None.
//
//--

#if !SMBDBG

#if !SMBDBG1
#if SMB_USE_UNALIGNED
#define SmbPutUshort(SrcAddress, Value) \
                            *(PSMB_USHORT)(SrcAddress) = (Value)
#else
#line 179
#error SMB bytewise alignment profile is not supported
#line 183
#endif
#line 184
#else
#line 185
#error SMB debug field-name profile is not supported
#line 189
#endif
#line 190

#else
#line 192
#error SMB debug get/put profile is not supported
#line 199
#endif
#line 200

//++
//
// VOID
// SmbPutAlignedUshort (
//     OUT PUSHORT DestAddres,
//     IN USHORT Value
//     )
//
// Routine Description:
//
//     This macro stores a USHORT value from the source address,
//     correcting for the endian characteristics of the server if
//     necessary.
//
// Arguments:
//
//     DestAddress - where to store USHORT value.  Address may not be
//         misaligned.
//
//     Value - USHORT to store.  Value must be a constant or an aligned
//         field.
//
// Return Value:
//
//     None.
//
//--

#if !SMBDBG

#if !SMBDBG1
#define SmbPutAlignedUshort(DestAddress,Value) *(DestAddress) = (Value)
#else
#line 234
#error SMB debug field-name profile is not supported
#line 235
#endif
#line 236

#else
#line 238
#error SMB debug get/put profile is not supported
#line 245
#endif
#line 246

//++
//
// VOID
// SmbMoveUshort (
//     OUT PSMB_USHORT DestAddress
//     IN PSMB_USHORT SrcAddress
//     )
//
// Routine Description:
//
//     This macro moves a USHORT value from the possibly misaligned
//     source address to the possibly misaligned destination address,
//     avoiding alignment faults.
//
// Arguments:
//
//     DestAddress - where to store USHORT value
//
//     SrcAddress - where to retrieve USHORT value from
//
// Return Value:
//
//     None.
//
//--

#if !SMBDBG

#if !SMBDBG1
#if SMB_USE_UNALIGNED
#define SmbMoveUshort(DestAddress, SrcAddress) \
        *(PSMB_USHORT)(DestAddress) = *(PSMB_USHORT)(SrcAddress)
#else
#line 280
#error SMB bytewise alignment profile is not supported
#line 284
#endif
#line 285
#else
#line 286
#error SMB debug field-name profile is not supported
#line 290
#endif
#line 291

#else
#line 293
#error SMB debug get/put profile is not supported
#line 300
#endif
#line 301

//++
//
// ULONG
// SmbGetUlong (
//     IN PSMB_ULONG SrcAddress
//     )
//
// Routine Description:
//
//     This macro retrieves a ULONG value from the possibly misaligned
//     source address, avoiding alignment faults.
//
// Arguments:
//
//     SrcAddress - where to retrieve ULONG value from
//
// Return Value:
//
//     ULONG - the value retrieved.  The target must be aligned.
//
//--

#if !SMBDBG

#if !SMBDBG1
#if SMB_USE_UNALIGNED
#define SmbGetUlong(SrcAddress) *(PSMB_ULONG)(SrcAddress)
#else
#line 330
#error SMB bytewise alignment profile is not supported
#line 336
#endif
#line 337
#else
#line 338
#error SMB debug field-name profile is not supported
#line 344
#endif
#line 345

#else
#line 347
#error SMB debug get/put profile is not supported
#line 353
#endif
#line 354

//++
//
// USHORT
// SmbGetAlignedUlong (
//     IN PULONG SrcAddress
//     )
//
// Routine Description:
//
//     This macro retrieves a ULONG value from the source address,
//     correcting for the endian characteristics of the server if
//     necessary.
//
// Arguments:
//
//     SrcAddress - where to retrieve ULONG value from; must be aligned.
//
// Return Value:
//
//     ULONG - the value retrieved.  The target must be aligned.
//
//--

#if !SMBDBG

#if !SMBDBG1
#define SmbGetAlignedUlong(SrcAddress) *(SrcAddress)
#else
#line 383
#error SMB debug field-name profile is not supported
#line 384
#endif
#line 385

#else
#line 387
#error SMB debug get/put profile is not supported
#line 393
#endif
#line 394

//++
//
// VOID
// SmbPutUlong (
//     OUT PSMB_ULONG DestAddress,
//     IN ULONG Value
//     )
//
// Routine Description:
//
//     This macro stores a ULONG value at the possibly misaligned
//     destination address, avoiding alignment faults.
//
// Arguments:
//
//     DestAddress - where to store ULONG value
//
//     Value - ULONG to store.  Value must be a constant or an aligned
//         field.
//
// Return Value:
//
//     None.
//
//--

#if !SMBDBG

#if !SMBDBG1
#if SMB_USE_UNALIGNED
#define SmbPutUlong(SrcAddress, Value) *(PSMB_ULONG)(SrcAddress) = Value
#else
#line 427
#error SMB bytewise alignment profile is not supported
#line 433
#endif
#line 434
#else
#line 435
#error SMB debug field-name profile is not supported
#line 441
#endif
#line 442

#else
#line 444
#error SMB debug get/put profile is not supported
#line 451
#endif
#line 452

//++
//
// VOID
// SmbPutAlignedUlong (
//     OUT PULONG DestAddres,
//     IN ULONG Value
//     )
//
// Routine Description:
//
//     This macro stores a ULONG value from the source address,
//     correcting for the endian characteristics of the server if
//     necessary.
//
// Arguments:
//
//     DestAddress - where to store ULONG value.  Address may not be
//         misaligned.
//
//     Value - ULONG to store.  Value must be a constant or an aligned
//         field.
//
// Return Value:
//
//     None.
//
//--

#if !SMBDBG

#if !SMBDBG1
#define SmbPutAlignedUlong(DestAddress,Value) *(DestAddress) = (Value)
#else
#line 486
#error SMB debug field-name profile is not supported
#line 487
#endif
#line 488

#else
#line 490
#error SMB debug get/put profile is not supported
#line 497
#endif
#line 498

//++
//
// VOID
// SmbMoveUlong (
//     OUT PSMB_ULONG DestAddress,
//     IN PSMB_ULONG SrcAddress
//     )
//
// Routine Description:
//
//     This macro moves a ULONG value from the possibly misaligned
//     source address to the possible misaligned destination address,
//     avoiding alignment faults.
//
// Arguments:
//
//     DestAddress - where to store ULONG value
//
//     SrcAddress - where to retrieve ULONG value from
//
// Return Value:
//
//     None.
//
//--

#if !SMBDBG

#if !SMBDBG1
#if SMB_USE_UNALIGNED
#define SmbMoveUlong(DestAddress,SrcAddress) \
        *(PSMB_ULONG)(DestAddress) = *(PSMB_ULONG)(SrcAddress)
#else
#line 532
#error SMB bytewise alignment profile is not supported
#line 538
#endif
#line 539
#else
#line 540
#error SMB debug field-name profile is not supported
#line 546
#endif
#line 547

#else
#line 549
#error SMB debug get/put profile is not supported
#line 556
#endif
#line 557

//++
//
// VOID
// SmbPutDate (
//     OUT PSMB_DATE DestAddress,
//     IN SMB_DATE Value
//     )
//
// Routine Description:
//
//     This macro stores an SMB_DATE value at the possibly misaligned
//     destination address, avoiding alignment faults.  This macro
//     is different from SmbPutUshort in order to be able to handle
//     funny bitfield / big-endian interactions.
//
// Arguments:
//
//     DestAddress - where to store SMB_DATE value
//
//     Value - SMB_DATE to store.  Value must be a constant or an
//         aligned field.
//
// Return Value:
//
//     None.
//
//--

#if !SMBDBG

#if SMB_USE_UNALIGNED
#define SmbPutDate(DestAddress,Value) (DestAddress)->Ushort = (Value).Ushort
#else
#line 591
#error SMB bytewise alignment profile is not supported
#line 595
#endif
#line 596

#else
#line 598
#error SMB debug get/put profile is not supported
#line 605
#endif
#line 606

//++
//
// VOID
// SmbMoveDate (
//     OUT PSMB_DATE DestAddress,
//     IN PSMB_DATE SrcAddress
//     )
//
// Routine Description:
//
//     This macro copies an SMB_DATE value from the possibly misaligned
//     source address, avoiding alignment faults.  This macro is
//     different from SmbGetUshort in order to be able to handle funny
//     bitfield / big-endian interactions.
//
//     Note that there is no SmbGetDate because of the way SMB_DATE is
//     defined.  It is a union containing a USHORT and a bitfield
//     struct.  The caller of an SmbGetDate macro would have to
//     explicitly use one part of the union.
//
// Arguments:
//
//     DestAddress - where to store SMB_DATE value.  MUST BE ALIGNED!
//
//     SrcAddress - where to retrieve SMB_DATE value from
//
// Return Value:
//
//     None.
//
//--

#if !SMBDBG

#if SMB_USE_UNALIGNED
#define SmbMoveDate(DestAddress,SrcAddress)     \
            (DestAddress)->Ushort = (SrcAddress)->Ushort
#else
#line 645
#error SMB bytewise alignment profile is not supported
#line 649
#endif
#line 650

#else
#line 652
#error SMB debug get/put profile is not supported
#line 659
#endif
#line 660

//++
//
// VOID
// SmbZeroDate (
//     IN PSMB_DATE Date
//     )
//
// Routine Description:
//
//     This macro zeroes a possibly misaligned SMB_DATE field.
//
// Arguments:
//
//     Date - Pointer to SMB_DATE field to zero.
//
// Return Value:
//
//     None.
//
//--

#if !SMBDBG

#if SMB_USE_UNALIGNED
#define SmbZeroDate(Date) (Date)->Ushort = 0
#else
#line 687
#error SMB bytewise alignment profile is not supported
#line 691
#endif
#line 692

#else
#line 694
#error SMB debug get/put profile is not supported
#line 700
#endif
#line 701

//++
//
// BOOLEAN
// SmbIsDateZero (
//     IN PSMB_DATE Date
//     )
//
// Routine Description:
//
//     This macro returns TRUE if the supplied SMB_DATE value is zero.
//
// Arguments:
//
//     Date - Pointer to SMB_DATE value to check.  MUST BE ALIGNED!
//
// Return Value:
//
//     BOOLEAN - TRUE if Date is zero, else FALSE.
//
//--

#if !SMBDBG

#define SmbIsDateZero(Date) ( (Date)->Ushort == 0 )

#else
#line 728
#error SMB debug get/put profile is not supported
#line 734
#endif
#line 735

//++
//
// VOID
// SmbPutTime (
//     OUT PSMB_TIME DestAddress,
//     IN SMB_TIME Value
//     )
//
// Routine Description:
//
//     This macro stores an SMB_TIME value at the possibly misaligned
//     destination address, avoiding alignment faults.  This macro
//     is different from SmbPutUshort in order to be able to handle
//     funny bitfield / big-endian interactions.
//
// Arguments:
//
//     DestAddress - where to store SMB_TIME value
//
//     Value - SMB_TIME to store.  Value must be a constant or an
//         aligned field.
//
// Return Value:
//
//     None.
//
//--

#if !SMBDBG

#if SMB_USE_UNALIGNED
#define SmbPutTime(DestAddress,Value) (DestAddress)->Ushort = (Value).Ushort
#else
#line 769
#error SMB bytewise alignment profile is not supported
#line 773
#endif
#line 774

#else
#line 776
#error SMB debug get/put profile is not supported
#line 783
#endif
#line 784

//++
//
// VOID
// SmbMoveTime (
//     OUT PSMB_TIME DestAddress,
//     IN PSMB_TIME SrcAddress
//     )
//
// Routine Description:
//
//     This macro copies an SMB_TIME value from the possibly
//     misaligned source address, avoiding alignment faults.  This macro
//     is different from SmbGetUshort in order to be able to handle
//     funny bitfield / big-endian interactions.
//
//     Note that there is no SmbGetTime because of the way SMB_TIME is
//     defined.  It is a union containing a USHORT and a bitfield
//     struct.  The caller of an SmbGetTime macro would have to
//     explicitly use one part of the union.
//
// Arguments:
//
//     DestAddress - where to store SMB_TIME value.  MUST BE ALIGNED!
//
//     SrcAddress - where to retrieve SMB_TIME value from
//
// Return Value:
//
//     None.
//
//--

#if !SMBDBG

#if SMB_USE_UNALIGNED
#define SmbMoveTime(DestAddress,SrcAddress) \
                (DestAddress)->Ushort = (SrcAddress)->Ushort
#else
#line 823
#error SMB bytewise alignment profile is not supported
#line 827
#endif
#line 828

#else
#line 830
#error SMB debug get/put profile is not supported
#line 837
#endif
#line 838

//++
//
// VOID
// SmbZeroTime (
//     IN PSMB_TIME Time
//     )
//
// Routine Description:
//
//     This macro zeroes a possibly misaligned SMB_TIME field.
//
// Arguments:
//
//     Time - Pointer to SMB_TIME field to zero.
//
// Return Value:
//
//     None.
//
//--

#if !SMBDBG

#if SMB_USE_UNALIGNED
#define SmbZeroTime(Time) (Time)->Ushort = 0
#else
#line 865
#error SMB bytewise alignment profile is not supported
#line 869
#endif
#line 870

#else
#line 872
#error SMB debug get/put profile is not supported
#line 878
#endif
#line 879

//++
//
// BOOLEAN
// SmbIsTimeZero (
//     IN PSMB_TIME Time
//     )
//
// Routine Description:
//
//     This macro returns TRUE if the supplied SMB_TIME value is zero.
//
// Arguments:
//
//     Time - Pointer to SMB_TIME value to check.  Must be aligned and
//         in native format!
//
// Return Value:
//
//     BOOLEAN - TRUE if Time is zero, else FALSE.
//
//--

#if !SMBDBG

#define SmbIsTimeZero(Time) ( (Time)->Ushort == 0 )

#else
#line 907
#error SMB debug get/put profile is not supported
#line 913
#endif
#line 914

#endif // def _SMBGTPT_
