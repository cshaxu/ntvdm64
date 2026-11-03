/*
 * ==========================================================================
 *	Name:		nt_fulsc.h
 *	Author:		Jerry Sexton
 *	Derived From:
 *	Created On:	5th February 1992
 *	Purpose:	This header file contains definitions etc. for
 *			full-screen graphics modules.
 *
 *	(c)Copyright Insignia Solutions Ltd., 1992. All rights reserved.
 * ==========================================================================
 */

/*
 * ==========================================================================
 * Macros
 * ==========================================================================
 */
#define ErrorExit() DisplayErrorTerm(EHS_FUNC_FAILED,GetLastError(),__FILE__,__LINE__);


#define CURRENT_OFFSET(r)	((DWORD) (r) - (DWORD) videoState)
#define GET_OFFSET(offset)	((BYTE *) videoState + videoState->offset)

#define MID_VAL(x, y)	(x + (y - x) / 2)

#define NUM_SEQ_REGS	5
#define NUM_CRTC_REGS	25
#define NUM_GC_REGS	9
#define NUM_AC_REGS	21
#define NUM_DAC_REGS	256

#define STATELENGTH		sizeof(VIDEO_HARDWARE_STATE_HEADER)
#define LATCHLENGTH		4
#define RGBLENGTH		3
#define PLANELENGTH		0x10000

#define BASICSEQUENCEROFFSET	STATELENGTH
#define BASICCRTCONTOFFSET	BASICSEQUENCEROFFSET + NUM_SEQ_REGS
#define BASICGRAPHCONTOFFSET	BASICCRTCONTOFFSET + NUM_CRTC_REGS
#define BASICATTRIBCONTOFFSET	BASICGRAPHCONTOFFSET + NUM_GC_REGS
#define BASICDACOFFSET		BASICATTRIBCONTOFFSET + NUM_AC_REGS
#define BASICLATCHESOFFSET	BASICDACOFFSET + NUM_DAC_REGS * RGBLENGTH
#define PLANE1OFFSET		BASICLATCHESOFFSET + LATCHLENGTH
#define PLANE2OFFSET		PLANE1OFFSET + PLANELENGTH
#define PLANE3OFFSET		PLANE2OFFSET + PLANELENGTH
#define PLANE4OFFSET		PLANE3OFFSET + PLANELENGTH

#define VIDEO_PAGE_SIZE	(80 * 25 * 2)	// Cols * rows * (char + attrib)

#define BIT_PLANE_SIZE	65536

#define VGA_WIDTH	80

#define VGA_HEIGHT_0	22
#define VGA_HEIGHT_1	25
#define VGA_HEIGHT_2	28
#define VGA_HEIGHT_3	43
#define VGA_HEIGHT_4	50

#define MAX_TITLE_LEN	256

#define MAX_CONSOLE_HEIGHT	50
#define MAX_CONSOLE_WIDTH	80
#define MAX_CONSOLE_SIZE	(MAX_CONSOLE_HEIGHT * MAX_CONSOLE_WIDTH)

#define DEF_FONT_WIDTH  8
#define DEF_FONT_HEIGHT 8
                         
#define GET	FALSE
#define SET	TRUE

#define DISPLAY_TYPE	0x40

#define AC_MODE_CONTROL_REG	16

#ifndef PROD
#error DIVERGENCE MVDM-HOST-DIV-504: non-PROD header branch is not selected; see mirror register.
#else /* !PROD */
#define FullScreenTrace0(s)
#define FullScreenTrace1(s ,p0)
#define FullScreenTrace2(s, p0, p1)
#define FullScreenTrace3(s, p0, p1, p2)
#define FullScreenTrace4(s, p0, p1, p2, p3)
#define FullScreenTrace5(s, p0, p1, p2, p3, p4)
#define FullScreenTrace6(s, p0, p1, p2, p3, p4, p5)
#define FullScreenTrace7(s, p0, p1, p2, p3, p4, p5, p6)
#define FullScreenTrace8(s, p0, p1, p2, p3, p4, p5, p6, p7)
#endif /* !PROD */

/*
 * ==========================================================================
 * Typedefs
 * ==========================================================================
 */

/* Structure for saving video block name in. */
typedef struct
{
    WCHAR   *Name;
    ULONG   NameLen;
} WCHAR_STRING;

/* Valid hardware state table entry. */
typedef struct
{
    USHORT	LinesOnScreen;
    COORD	Resolution;
    COORD	FontSize;
} HARDWARE_STATE;

/*
 * ==========================================================================
 * Global Data
 * ==========================================================================
 */
IMPORT HANDLE MainThread;
IMPORT DWORD stateLength;
#line 137
#ifdef X86GFX
#line 138
#error DIVERGENCE MVDM-HOST-DIV-512: X86GFX branch is not selected for these consumers; see mirror register.
#endif
#line 142
IMPORT PVOID textState; //Tim Nov 92.
IMPORT WCHAR_STRING videoSection;
IMPORT WCHAR_STRING textSection;
IMPORT BOOL NoTicks;
IMPORT HANDLE StartTToG;
IMPORT HANDLE EndTToG;
IMPORT BOOL BiosModeChange;
#line 149
#ifndef PROD
#line 150
#error DIVERGENCE MVDM-HOST-DIV-504: non-PROD header branch is not selected; see mirror register.
#endif /* PROD */
#line 152

extern DWORD savedScreenState;
extern BOOL  ConsoleInitialised;
extern BOOL  ConsoleNoUpdates;
#line 156
#ifdef X86GFX
#line 157
#error DIVERGENCE MVDM-HOST-DIV-512: X86GFX branch is not selected for these consumers; see mirror register.
#endif /* X86GFX */
#line 160
extern BOOL blocked_in_gfx_mode;


/*
 * ==========================================================================
 * Imported Functions
 * ==========================================================================
 */
IMPORT VOID nt_init_event_thread(VOID);
IMPORT VOID ConsoleInit(VOID);
IMPORT VOID GfxReset(VOID);
IMPORT VOID ResetConsoleState(VOID);
IMPORT VOID InitTToG(VOID);
IMPORT VOID TextToGraphics(VOID);
IMPORT VOID CheckForFullscreenSwitch(VOID);
IMPORT UTINY getNtScreenState(VOID);
IMPORT BOOL hostModeChange(VOID);
IMPORT VOID DoFullScreenResume(VOID);
IMPORT VOID GfxCloseDown(VOID);
IMPORT VOID FreezeWinTitle(VOID);
IMPORT VOID UnFreezeWinTitle(VOID);

IMPORT VOID TextSectionName(WCHAR **, ULONG *);
IMPORT VOID VideoSectionName(WCHAR **, ULONG *);
IMPORT PVOID *CreateVideoSection(ULONG);
IMPORT PVOID *CreateTextSection(ULONG);
IMPORT VOID CommitSection(PVOID *, ULONG *);
IMPORT VOID CloseSection(PVOID);
IMPORT VOID LoseRegenMemory(VOID);
IMPORT VOID RegainRegenMemory(VOID);

IMPORT VOID DoHandShake(VOID);
IMPORT HANDLE GetDetectEvent(VOID);

IMPORT VOID ResetConsoleState IPT0();
IMPORT int  getModeType(VOID);

#line 197
#ifdef X86GFX
#line 198
#error DIVERGENCE MVDM-HOST-DIV-512: X86GFX branch is not selected for these consumers; see mirror register.
#endif
#line 246


#line 248
#ifdef X86GFX
#line 249
#error DIVERGENCE MVDM-HOST-DIV-584: hardware X86GFX declarations are not selected
#endif
