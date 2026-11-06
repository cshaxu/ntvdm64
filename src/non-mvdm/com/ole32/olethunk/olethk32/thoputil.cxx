#line 1
//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1992 - 1994.
//
//  File:       thoputil.cxx
//
//  Contents:   Utility routines for thunking
//
//  History:    01-Mar-94       DrewB   Created
//
//----------------------------------------------------------------------------

/* DIVERGENCE(OPENNT-HOST-297): selected descriptor conversion closure. */
#include "wow_ole_descriptor_private.h"

#line 504
#define SEL_TI          0x0004
#define SEL_RPL         0x0003
#define SEL_INDEX       0xfff8

#define IS_LDT_SELECTOR(sel) (((sel) & SEL_TI) == SEL_TI)

// LDT bit constants
#define LTYPE_APP       0x0010
#define LTYPE_CODE      0x0008
#define LTYPE_CREAD     0x0002
#define LTYPE_DDOWN     0x0004
#define LTYPE_DWRITE    0x0002

// Pointer access types, or'able
// Defined to be the same as thop in/out so that no translation
// is necessary for checks on thop memory access
#define PACC_READ       THOP_IN
#define PACC_WRITE      THOP_OUT
#define PACC_CODE       1           // Special for CODE PTRs

// Information about a VDM pointer
typedef struct _VPTRDESC
{
    BYTE *pbFlat;
    DWORD dwLengthLeft;
} VPTRDESC;


#line 539
#define S_GDTENTRY      MAKE_SCODE(SEVERITY_SUCCESS, FACILITY_NULL, 1)
#define S_SYSLDTENTRY   MAKE_SCODE(SEVERITY_SUCCESS, FACILITY_NULL, 2)

#line 565

SCODE GetPtr16Description(VPVOID vp,
                          WORD grfAccess,
                          DWORD dwSize,
                          VPTRDESC *pvpd)
{
    VDMLDT_ENTRY *vle;
#if defined(_CHICAGO_)
#error Excluded unselected original OLE profile
#line 574
#endif
    WORD wSel;
    WORD wOff;
    DWORD dwLength;

    thkAssert(vp != 0);
    thkAssert(dwSize > 0);
    thkAssert(grfAccess != 0);

    wSel = (WORD)(vp >> 16);
    wOff = (WORD)(vp & 0xffff);

    pvpd->dwLengthLeft = 0xffff-wOff+1;     // Default length remaining

    if (!IS_LDT_SELECTOR(wSel))
    {
        // According to the WOW developers, the only GDT selector
        // is for the BIOS data area so we should never see one

        thkDebugOut((DEB_ERROR, "GDT selector: 0x%04X\n", wSel));

        // Handle it just in case
        pvpd->pbFlat = (BYTE *)WOWFIXVDMPTR(vp, dwSize);

        return S_GDTENTRY;
    }

#if defined(_CHICAGO_)
#error Excluded unselected original OLE profile
#line 607
#else
    vle = (VDMLDT_ENTRY *)((BYTE *)(ExpLdt)+(wSel & SEL_INDEX));
#endif

    if ((vle->HighWord.Bits.Type & LTYPE_APP) == 0)
    {
        // According to the WOW developers, they don't use
        // system segments so we should never see one

        thkDebugOut((DEB_ERROR, "System descriptor: 0x%04X\n", wSel));

        // Handle it just in case
        pvpd->pbFlat = (BYTE *)WOWFIXVDMPTR(vp, dwSize);

        return S_SYSLDTENTRY;
    }

    // Do as much up-front validation as possible
    // Since the segment may not be present, we are restricted to
    // only checking the access permissions

    if (vle->HighWord.Bits.Type & LTYPE_CODE)
    {
        // Validate access for code segments
        // Code segments are never writable

        if (((grfAccess & PACC_READ) &&
             (vle->HighWord.Bits.Type & LTYPE_CREAD) == 0) ||
            (grfAccess & PACC_WRITE))
        {
            return E_INVALIDARG;
        }
    }
    else
    {
        // Validate access for data segments
        // Data segments are always readable never executable

        if (((grfAccess & PACC_WRITE) &&
             (vle->HighWord.Bits.Type & LTYPE_DWRITE) == 0) ||
             (grfAccess & PACC_CODE))
        {
            return E_INVALIDARG;
        }
    }

    // Bring in segment if it's not present
    if (!vle->HighWord.Bits.Pres)
    {
        // We've validated access permissions and segments must
        // always be at least one byte long so it's safe to
        // touch the first byte to bring it in

        // On Win95, this will call GlobalFix on the pointer
        // to ensure that it stays in memory
        /* DIVERGENCE(OPENNT-HOST-297): same original kernel segment fault. */
        wow_ole_touch_pointer(vp);
#line 663

#if defined(_CHICAGO_)
#error Excluded unselected original OLE profile
#line 667
#endif

        thkAssert(vle->HighWord.Bits.Pres);
    }
#ifdef _CHICAGO_
#error Excluded unselected original OLE profile
#line 679
#endif


    dwLength = ((DWORD)vle->LimitLow |
                ((DWORD)vle->HighWord.Bits.LimitHi << 16))+1;
    if (vle->HighWord.Bits.Granularity)
    {
        // 4K granularity
        dwLength <<= 12;
    }

    if ((vle->HighWord.Bits.Type & LTYPE_CODE) ||
        (vle->HighWord.Bits.Type & LTYPE_DDOWN) == 0)
    {
        // Validate length for code and normal data segments

        if (wOff+dwSize > dwLength)
        {
            WOWRELVDMPTR(vp);
            return E_INVALIDARG;
        }

        pvpd->dwLengthLeft = dwLength-wOff;
    }
    else
    {
        // Expand-down segment

        if (wOff < dwLength)
        {
            WOWRELVDMPTR(vp);
            return E_INVALIDARG;
        }

        // Check for wraparound

        if (vle->HighWord.Bits.Granularity)
        {
            // BUGBUG - Compiler - This should be +1, but
            // the compiler generates a warning about an overflow
            // in constant arithmetic
            pvpd->dwLengthLeft = 0xffffffff-wOff;
        }

        if (dwSize > pvpd->dwLengthLeft)
        {
            WOWRELVDMPTR(vp);
            return E_INVALIDARG;
        }
    }

    /* DIVERGENCE(OPENNT-HOST-297): use the existing synchronous WOW
     * pointer boundary, not a reconstructed flat-memory base. */
    pvpd->pbFlat = (BYTE *)WOWFIXVDMPTR(vp, dwSize);
#line 745

#if DBG == 1
#error Excluded unselected original OLE profile
#line 753
#endif

    return S_OK;
}
#line 774

VOID  *
GetReadPtr16(
    THUNKINFO   *pti,
    VPVOID      vp,
    DWORD       dwSize )
{
    VPTRDESC vpd;
    SCODE sc;

    sc = GetPtr16Description(vp, PACC_READ, dwSize, &vpd);
    if (FAILED(sc))
    {
        pti->scResult = sc;
        return NULL;
    }
    else
    {
        return vpd.pbFlat;
    }
}
#line 928

CHAR  *
GetStringPtr16(
    THUNKINFO   *pti,
    VPSTR       vp,
    UINT        cchMax,
    PUINT       lpSize )
{
    VPTRDESC vpd;
    SCODE sc;

    // Check the first byte to ensure read access to the segment
    sc = GetPtr16Description(vp, PACC_READ, 1, &vpd);
    if (FAILED(sc))
    {
        pti->scResult = sc;
        return NULL;
    }
    else
    {
        UINT cchLen;
        BYTE *pb;
        BOOL fMbLead;

        pb = vpd.pbFlat;

        if (pb == NULL)
        {
            goto Exit;
        }


        // Restrict zero-termination search to cchMax characters
        // or valid remaining memory
        // Since we specified one in GetPtr16Description, dwLengthLeft
        // is one off here
        cchMax = min(cchMax, vpd.dwLengthLeft+1);

        cchLen = 0;
        fMbLead = FALSE;
        while (cchMax > 0)
        {
            cchLen++;

            if (*pb == 0 && !fMbLead)
            {
                break;
            }
            else
            {
                fMbLead = (BOOL)g_abLeadTable[*pb++];
                cchMax--;
            }
        }

        if (cchMax > 0)
        {
            *lpSize = cchLen;
            return (LPSTR)vpd.pbFlat;
        }

Exit:
        {
            pti->scResult = E_INVALIDARG;
            WOWRELVDMPTR(vp);
            return NULL;
        }
    }
}
#line 2603

SCODE ConvertObjDesc1632(THUNKINFO *pti,
                         HMEM16 hg16,
                         HGLOBAL *phg32)
{
    SCODE sc;
    VPVOID vp16;
    HGLOBAL hg32;
    DWORD dwSize;
    OBJECTDESCRIPTOR UNALIGNED *pod16;
    OBJECTDESCRIPTOR *pod32;
    char *pszFutn, *pszSoc;
    UINT cchFutn, cchSoc;
    UINT cbOffset;

    sc = S_OK;

    vp16 = WOWGlobalLock16(hg16);
    if ( vp16 == 0 )
    {
        return E_INVALIDARG;
    }

    pszFutn = NULL;
    pszSoc = NULL;

    pod16 = (OBJECTDESCRIPTOR UNALIGNED *)
        GetReadPtr16(pti, vp16, sizeof(OBJECTDESCRIPTOR));
    if (pod16 == NULL)
    {
        sc = pti->scResult;
        goto EH_Unlock;
    }

    dwSize = sizeof(OBJECTDESCRIPTOR);

    if (pod16->dwFullUserTypeName > 0)
    {
        pszFutn = (char *)GetStringPtr16(pti, vp16+pod16->dwFullUserTypeName,
                                         CCHMAXSTRING, &cchFutn);
        if (pszFutn == NULL)
        {
            sc = pti->scResult;
            goto EH_Unlock;
        }

        dwSize += cchFutn*sizeof(WCHAR);
    }

    if (pod16->dwSrcOfCopy > 0)
    {
        pszSoc = (char *)GetStringPtr16(pti, vp16+pod16->dwSrcOfCopy,
                                        CCHMAXSTRING, &cchSoc);
        if (pszSoc == NULL)
        {
            sc = pti->scResult;
            goto EH_Unlock;
        }

        dwSize += cchSoc*sizeof(WCHAR);
    }

    hg32 = GlobalAlloc(GMEM_MOVEABLE, dwSize);
    if ( hg32 == 0 )
    {
        sc = E_OUTOFMEMORY;
        goto EH_Unlock;
    }

    pod32 = (OBJECTDESCRIPTOR *)GlobalLock(hg32);
    memcpy(pod32, pod16, sizeof(OBJECTDESCRIPTOR));
    pod32->cbSize = dwSize;

    cbOffset = sizeof(OBJECTDESCRIPTOR);

    if (pod16->dwFullUserTypeName > 0)
    {
        if (MultiByteToWideChar(AreFileApisANSI() ? CP_ACP : CP_OEMCP,
                                0, pszFutn, cchFutn,
                                (WCHAR *)((BYTE *)pod32+cbOffset),
                                cchFutn) == 0)
        {
            sc = E_UNEXPECTED;
            goto EH_Free;
        }

        pod32->dwFullUserTypeName = cbOffset;
        cbOffset += cchFutn*sizeof(WCHAR);
    }

    if (pod16->dwSrcOfCopy > 0)
    {
        if (MultiByteToWideChar(AreFileApisANSI() ? CP_ACP : CP_OEMCP,
                                0, pszSoc, cchSoc,
                                (WCHAR *)((BYTE *)pod32+cbOffset),
                                cchSoc)  == 0)
        {
            sc = E_UNEXPECTED;
            goto EH_Free;
        }

        pod32->dwSrcOfCopy = cbOffset;
        cbOffset += cchFutn*sizeof(WCHAR);
    }

#if DBG == 1
#error Excluded unselected original OLE profile
#line 2730
#endif

    GlobalUnlock(hg32);

    *phg32 = hg32;

 EH_Unlock:
    if (pszFutn != NULL)
    {
        WOWRELVDMPTR(vp16+pod16->dwFullUserTypeName);
    }
    if (pszSoc != NULL)
    {
        WOWRELVDMPTR(vp16+pod16->dwSrcOfCopy);
    }
    if (pod16 != NULL)
    {
        WOWRELVDMPTR(vp16);
    }

    WOWGlobalUnlock16(hg16);

    return sc;

 EH_Free:
    GlobalUnlock(hg32);
    GlobalFree(hg32);
    goto EH_Unlock;
}
#line 2777

SCODE ConvertObjDesc3216(THUNKINFO *pti,
                         HGLOBAL hg32,
                         HMEM16 *phg16)
{
    SCODE sc;
    VPVOID vp16;
    HMEM16 hg16;
    DWORD dwSize;
    OBJECTDESCRIPTOR UNALIGNED *pod16;
    OBJECTDESCRIPTOR *pod32;
    WCHAR *pwcsFutn, *pwcsSoc;
    UINT cchFutn, cchSoc;
    UINT cbOffset;

    sc = S_OK;

    pod32 = (OBJECTDESCRIPTOR *)GlobalLock(hg32);
    if ( pod32 == 0 )
    {
        return E_INVALIDARG;
    }

    if (IsBadReadPtr(pod32, sizeof(OBJECTDESCRIPTOR)))
    {
        sc = E_INVALIDARG;
        goto EH_Unlock;
    }

    dwSize = sizeof(OBJECTDESCRIPTOR);

    pwcsFutn = NULL;
    if (pod32->dwFullUserTypeName > 0)
    {
        pwcsFutn = (WCHAR *)((BYTE *)pod32+pod32->dwFullUserTypeName);
        if (IsBadStringPtrW(pwcsFutn, CCHMAXSTRING))
        {
            sc = E_INVALIDARG;
            goto EH_Unlock;
        }

        cchFutn = lstrlenW(pwcsFutn)+1;
        dwSize += cchFutn*2;
    }

    pwcsSoc = NULL;
    if (pod32->dwSrcOfCopy > 0)
    {
        pwcsSoc = (WCHAR *)((BYTE *)pod32+pod32->dwSrcOfCopy);
        if (IsBadStringPtrW(pwcsSoc, CCHMAXSTRING))
        {
            sc = E_INVALIDARG;
            goto EH_Unlock;
        }

        cchSoc = lstrlenW(pwcsSoc)+1;
        dwSize += cchSoc*2;
    }

    vp16 = WOWGlobalAllocLock16(GMEM_MOVEABLE, dwSize, &hg16);
    if ( vp16 == 0 )
    {
        sc = E_OUTOFMEMORY;
        goto EH_Unlock;
    }

    pod16 = FIXVDMPTR(vp16, OBJECTDESCRIPTOR);
    memcpy(pod16, pod32, sizeof(OBJECTDESCRIPTOR));
    pod16->cbSize = dwSize;

    cbOffset = sizeof(OBJECTDESCRIPTOR);

    if (pod32->dwFullUserTypeName > 0)
    {
        if (WideCharToMultiByte(AreFileApisANSI() ? CP_ACP : CP_OEMCP,
                                0, pwcsFutn, cchFutn,
                                (char *)pod16+cbOffset, 2 * cchFutn,
                                NULL, NULL) == 0)
        {
            sc = E_UNEXPECTED;
            goto EH_Free;
        }

        pod16->dwFullUserTypeName = cbOffset;
        cbOffset += cchFutn*2;
    }

    if (pod32->dwSrcOfCopy > 0)
    {
        if (WideCharToMultiByte(AreFileApisANSI() ? CP_ACP : CP_OEMCP,
                                0, pwcsSoc, cchSoc,
                                (char *)pod16+cbOffset, 2 * cchSoc,
                                NULL, NULL) == 0)
        {
            sc = E_UNEXPECTED;
            goto EH_Free;
        }

        pod16->dwSrcOfCopy = cbOffset;
        cbOffset += cchFutn*2;
    }

#if DBG == 1
#error Excluded unselected original OLE profile
#line 2901
#endif

    RELVDMPTR(vp16);

    WOWGlobalUnlock16(hg16);

    *phg16 = hg16;

 EH_Unlock:
    GlobalUnlock(hg32);

    return sc;

 EH_Free:
    WOWGlobalUnlockFree16(vp16);
    goto EH_Unlock;
}

#line 4403
STDAPI_(HGLOBAL) ConvertObjDescriptor( HANDLE hMem, UINT flag )
{

   /* DIVERGENCE(OPENNT-HOST-297): C constant expressions; values already
    * come from the selected WOW clipboard declarations. */
#line 4408

   THUNKINFO ti;
   HGLOBAL hMem32;
   HMEM16  hMem16;

    switch ( flag )
    {
    case CFOLE_UNICODE_TO_ANSI:
        if( FAILED( ConvertObjDesc3216( &ti, (HGLOBAL) hMem, &hMem16 )))
        {
            return (HGLOBAL) NULL;
        }
        else
        {
            return (HGLOBAL)  hMem16;
        }
        break;

    case CFOLE_ANSI_TO_UNICODE:
        if( FAILED( ConvertObjDesc1632( &ti, (HMEM16) hMem, &hMem32 )))
        {
            return (HGLOBAL) NULL;
        }
        else
        {
            return (HGLOBAL) hMem32;
        }
        break;

    default:
        thkAssert(!"ConvertObjDescriptor, Invalid flag");
        break;
    }
    return (HGLOBAL) NULL;
}
