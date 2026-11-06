#ifndef WOW_OLE_DESCRIPTOR_PRIVATE_H
#define WOW_OLE_DESCRIPTOR_PRIVATE_H
/* OPENNT-HOST-297: finite C declaration facade; original thop.hxx private
 * records and SDK oleidl.h OBJECTDESCRIPTOR retain their complete fields. */
#include "precomp.h"
#include <string.h>
typedef LONG SCODE;
typedef LDT_ENTRY VDMLDT_ENTRY;
typedef struct CThkMgr CThkMgr;
typedef struct IUnknown IUnknown;
typedef DWORD IIDIDX;
typedef unsigned char THOP;
#define THOP_IN 0x40
#define THOP_OUT 0x80
#define CCHMAXSTRING 0x1000
typedef DWORD (__stdcall * VTBLFN)(DWORD dw);

typedef struct tagSTACK16INFO
{
    BYTE UNALIGNED  *pbStart;
    BYTE UNALIGNED  *pbCurrent;
    int             iDir;
} STACK16INFO;

typedef struct tagSTACK32INFO
{
    BYTE            *pbStart;
    BYTE            *pbCurrent;
} STACK32INFO;

typedef struct tagTHUNKINFO
{
    STACK16INFO     s16;
    STACK32INFO     s32;
    THOP CONST      *pThop;
    VTBLFN          pvfn;
    SCODE           scResult;
    BOOL            fResultThunked;
    DWORD           dwCallerProxy;
    CThkMgr        *pThkMgr;
    IIDIDX          iidx;
    DWORD           dwMethod;
    IUnknown       *this32;
} THUNKINFO;
typedef struct  tagOBJECTDESCRIPTOR
    {
    ULONG cbSize;
    CLSID clsid;
    DWORD dwDrawAspect;
    SIZEL sizel;
    POINTL pointl;
    DWORD dwStatus;
    DWORD dwFullUserTypeName;
    DWORD dwSrcOfCopy;
    }	OBJECTDESCRIPTOR;
extern DECLSPEC_IMPORT ULONG *ExpLdt;
extern BYTE g_abLeadTable[256];
VPVOID WINAPI WOWGlobalLock16(HMEM16);
BOOL WINAPI WOWGlobalUnlock16(HMEM16);
VPVOID WINAPI WOWGlobalAllocLock16(WORD,DWORD,HMEM16 *);
HMEM16 WINAPI WOWGlobalUnlockFree16(VPVOID);
void WINAPI wow_ole_touch_pointer(VPVOID);
#define WOWFIXVDMPTR(vp,size) WOWGetVDMPointer((vp),(size),TRUE)
#define WOWRELVDMPTR(vp) ((void)(vp))
#define FIXVDMPTR(vp,type) ((type *)WOWFIXVDMPTR((vp),sizeof(type)))
#define RELVDMPTR(vp) WOWRELVDMPTR(vp)
#define thkAssert(value) ((void)0)
#define thkDebugOut(args) ((void)0)
C_ASSERT(sizeof(THUNKINFO)==56);
C_ASSERT(FIELD_OFFSET(THUNKINFO,scResult)==28);
C_ASSERT(sizeof(OBJECTDESCRIPTOR)==52);
HGLOBAL WINAPI ConvertObjDescriptor(HANDLE,UINT);
HGLOBAL WINAPI wow_ole_convert_descriptor(HANDLE,UINT);
#endif

/* Original thop.hxx declaration notice:
 * Microsoft Windows. Copyright (C) Microsoft Corporation, 1992 - 1994.
 * OBJECTDESCRIPTOR fields originate in the pinned generated SDK oleidl.h.
 * Provenance and exact upstream hashes: OPENNT-HOST-297 evidence. */
