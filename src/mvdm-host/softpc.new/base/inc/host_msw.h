/*[
 *	Product:	SoftPC-AT Revision 3.0
 *
 *	Name:		host_msw.h
 *
 *	Derived From:	Alpha MS-Windows Driver by Ross Beresford
 *
 *	Author:		Rob Tizzard
 *
 *	Created On:	1st November 1990
 *
 *	Sccs ID:	@(#)host_msw.h	1.43 07/08/94
 *
 *	Purpose:	All host dependent definitions for SoftPC MicroSoft
 *		  	Windows 3.0 driver.
 *
 *	(c)Copyright Insignia Solutions Ltd., 1990. All rights reserved.
 *
]*/

#if defined(MSWDVR) && defined(XWINDOW)
#error DIVERGENCE MVDM-HOST-DIV-606: unsupported MSWDVR_XWINDOW profile
#endif /* MSWDVR_DEBUG */
#line 457

/* 
 * -----------------------------------------------------------------------------
 * Host driver routines 
 * -----------------------------------------------------------------------------
 */

#if !(defined(MSWDVR_DEBUG) && defined(MSWDVR_MAIN))

/* For some reason, all of these function names are remapped with #defines.
 * The MSWDVR_DEBUG mechanism further remaps them to point to the
 * various debug routines instead, but this only happens in ms_windows.h
 *
 * To allow for fussy pre-processors, add a #define MSWDVR_MAIN which is
 * used in ms_windows.c to disable this particular translation.
 * 
 * The ideal solution would be to do away with this whole level of misdirection.
 */

#define	HostBitblt		BltBitblt
#define	HostColorInfo		ColColorInfo
#define HostControl		WinControl
#define HostDeviceBitmapBits	DibDeviceBitmapBits
#define	HostDisable		WinDisable
#define	HostEnable		WinEnable
#define HostEventEnd            PtrEventEnd
#define	HostExtTextOut		TxtExtTextOut
#define	HostFastBorder		BltFastBorder
#define HostSetDIBitsToDevice	DibSetDIBitsToDevice
#define	HostRealizeObject	ObjRealizeObject
#define	HostStrblt		TxtStrblt
#define	HostOutput		OutOutput
#define	HostPixel		WinPixel
#define	HostScanlr		WinScanlr
#define	HostSetCursor		PtrSetCursor
#define	HostSaveScreenBitmap	SavSaveScreenBitmap
#define	HostGetCharWidth	TxtGetCharWidth
#define HostSetPalette		ColSetPalette
#define HostGetPalette		ColGetPalette
#define HostSetPalTrans		ColSetPalTrans
#define HostGetPalTrans		ColGetPalTrans
#define HostUpdateColors	ColUpdateColors
#define HostPtrEnable           PtrEnable
#define HostPtrDisable          PtrDisable
#ifdef SWIN_DEVBMP
#define HostBitmapBits      ObjBitmapBits
#define HostSelectBitmap    ObjSelectBitmap
#endif /* SWIN_DEVBMP */


#endif /* !(defined(MSWDVR_DEBUG) && defined(MSWDVR_MAIN)) */

/* The following don't get a debug wrapper - don't know why... */

#define	HostLogo		LgoLogo
#define HostFillGDIInfo		WinFillGDIInfo
#define HostFillPDEVInfo	WinFillPDEVInfo
#define	HostMoveCursor		PtrMoveCursor
#define	HostCheckCursor		PtrCheckCursor	
#define HostStretchBlt		BltStretchBlt

/*
 * -----------------------------------------------------------------------------
 * Debug Entry Points
 * -----------------------------------------------------------------------------
 */

/* Functions */

#ifdef MSWDVR_DEBUG
#error DIVERGENCE MVDM-HOST-DIV-605: unsupported MSWDVR_DEBUG profile
#else
#line 614

#define DPrintMonoBitmap(bitmap,bytes_per_line,width,height)
#define DReportColEnquire(p1,p2)
#define DReportPixelEnquire(p1,p2)
#define	DPrintImageDetails(p1)
#define	DPrintBitmapDetails(p1) 
#define DPrintMessage(p1)
#define DPrintColourmap(p1,p2)
#define DPrintSrcDstRect(p1,p2,p3,p4,p5,p6)
#define DPrintGDIInfo
#define DPrintBitmap(p1,p2,p3)
#define DPrintPBrush(p1,p2)
#define DPrintPpen(p1)
#define DPrintDrawMode(p1,p2)
#define DPrintDevice(p1)
#define DPrintlpPoints(p1,p2)
#define DPrintClipRect(p1)
#define DPrintInteger(p1) 
#define DPrintTransTable(p1,p2)
#define DGXopToString (p1)
#define DPrintLineStyle (p1)
#define DPrintBrushStyle (p1)
#define	DPrintStyle (p1)

#define DBitblt(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11)
#define DColorInfo(p1,p2,p3)
#define DControl(p1,p2,p3,p4)
#define DDisable(p1)
#define DDeviceBitmapBits(p1,p2,p3,p4,p5,p6,p7,p8)
#define DEnable(p1,p2,p3,p4,p5)
#define DExtTextOut(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12)
#define DFastBorder(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11)
#define DGetPaletteEntries(p1,p2,p3)
#define DGetPaletteTranslate(p1,p2)
#define DOutput(p1,p2,p3,p4,p5,p6,p7,p8)
#define DPixel(p1,p2,p3,p4,p5)
#define DRealizeObject(p1,p2,p3,p4,p5)
#define DSaveScreenBitmap(p1,p2,p3,p4,p5)
#define DScanlr(p1,p2,p3,p4,p5)
#define DSetDIBitsToDevice(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10)
#define DSetPaletteEntries(p1,p2,p3)
#define DSetPaletteTranslate(p1,p2)
#define DStrblt(p1,p2,p3,p4,p5,p6,p7,p8,p9)
#define DUpdateColors(p1,p2,p3,p4,p5)

#endif 
#line 660
