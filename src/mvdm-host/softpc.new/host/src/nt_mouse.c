#include "windows.h"
#include "insignia.h"
#include "host_def.h"
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include "conapi.h"
#include "xt.h"
#include CpuH
#include "egacpu.h"
#include "trace.h"
#include "debug.h"
#include "gvi.h"
#include "error.h"
#include "config.h"
#include "bios.h"
#include "mouse_io.h"
#include "video.h"
#include "nt_graph.h"
#include "host_nls.h"
#include "sas.h"
#include "ica.h"
#include "idetect.h"
#include "host_rrr.h"
#include "nt_mouse.h"
#include "ntcheese.h"
#include "nt_uis.h"
#include "nt_reset.h"
#include "nt_eoi.h"
#include "nt_event.h"
#include <ntddvdeo.h>
#include "nt_fulsc.h"
/* DIVERGENCE(MVDM-HOST-DIV-318): Window-relative input uses original
 * coordinates and callback masks, not Console warping. */
#include "ntvdm-exe/softpc/mvdm_softpc_mouse_guest.h"

/* DIVERGENCE(MVDM-HOST-DIV-274): original NT USER's cursor producer publishes
 * the already bounded virtual position before dispatching guest mouse work.
 * The standalone worker retains that one shared-data update at SoftPC's
 * existing post-transform boundary; it does not introduce another mouse
 * queue, coordinate conversion or USER input owner. */
#include "ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h"


#line 35
#define TEXT_MODE          1
#define GRAPHICS_MODE      2
#define NOTHING            0xff
#define NOT_INSTALLED      0
#define INSTALLED          1

#define get_pix_height() (get_pc_pix_height() * get_host_pix_height())


GLOBAL BOOL  pointer_emulation_status = POINTER_EMULATION_OS;
GLOBAL word  VirtualX,VirtualY;
GLOBAL BOOL  bPointerOff=FALSE;
GLOBAL MOUSE_STATUS os_pointer_data;

IMPORT word         DRAW_FS_POINTER_SEGMENT, DRAW_FS_POINTER_OFFSET;
IMPORT word	    POINTER_OFF_SEGMENT, POINTER_OFF_OFFSET;
IMPORT word	    POINTER_ON_SEGMENT, POINTER_ON_OFFSET;
IMPORT word	    CP_X_S, CP_X_O, CP_Y_S, CP_Y_O;
IMPORT sys_addr     conditional_off_sysaddr;

IMPORT word         F9_SEGMENT,F9_OFFSET;
IMPORT word         savedtextoffset,savedtextsegment;

#ifdef X86GFX
#line 59
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif //X86GFX
#line 70


LOCAL BOOL           mouse_state;
LOCAL BOOL           bFullscTextBkgrndSaved = FALSE;
LOCAL BOOL           bPointerInSamePlace = FALSE;
LOCAL word           text_ptr_bkgrnd=0;  // safe place for screen background
LOCAL sys_addr       old_text_addr;
RECT                 WarpBorderRect;     // in screen coordinates
RECT                 WarpClientRect;     // in client coordinates

LOCAL POINT          pMiddle; // centre point of the current Console window
LOCAL POINT	     pLast = {0,0};
LOCAL BOOL	     bAlertMessage=TRUE;
LOCAL BOOL           b256mode=FALSE;
LOCAL int            old_x=319;    // previous pointer state (position)
LOCAL int            old_y=99;     // in virtual coordinates.
LOCAL short          m2pX=8,m2pY=16;      // Mickey to pixel ratios
LOCAL BOOL           bFunctionZeroReset = TRUE;
LOCAL BOOL           bFunctionFour = FALSE;
LOCAL IS16           newF4x,newF4y;



GLOBAL  VOID    host_os_mouse_pointer(MOUSE_CURSOR_STATUS *,MOUSE_CALL_MASK *,

                                      MOUSE_VECTOR *);

FORWARD BOOL   WarpSystemPointer(IS16 *,IS16 *);
FORWARD void   MovePointerToWindowCentre(void);
FORWARD void   host_mouse_install1(void);
FORWARD void   host_mouse_install2(void);
FORWARD void   mouse_restore_cursor(void);
FORWARD void   deinstall_host_mouse(void);
FORWARD BOOL   mouse_installed(void);
FORWARD BOOL   mouse_in_use(void);
FORWARD void   mouse_reset(void);
FORWARD void   mouse_set_position(USHORT,USHORT);
FORWARD void   mouse_cursor_display(void);
FORWARD void   mouse_cursor_undisplay(void);
FORWARD void   mouse_cursor_mode_change(void);
FORWARD BOOL   HasConsoleClientRectChanged(void);
FORWARD void   MouseDisplay();
FORWARD void   CToS(RECT *);
FORWARD void   MouseDetachMenuItem(BOOL);
FORWARD void   MouseAttachMenuItem(HANDLE);
FORWARD void   MouseReattachMenuItem(HANDLE);
FORWARD void   ResetMouseOnBlock(void);
FORWARD void   ScaleToWindowedVirtualCoordinates(IS16 *,IS16 *,MOUSE_VECTOR *);
FORWARD void   host_m2p_ratio(word *,word *,word *,word *);
/* DIVERGENCE(MVDM-HOST-DIV-058): keep the actual MOUSE_SCALAR pointer
 * contract shared with mouse_io.c; width, order and behavior are unchanged. */
FORWARD void   host_x_range(word *,word *,MOUSE_SCALAR *,MOUSE_SCALAR *);
FORWARD void   host_y_range(word *,word *,MOUSE_SCALAR *,MOUSE_SCALAR *);
#line 121
FORWARD void   EmulateCoordinates(half_word,IS16,IS16,IS16 *,IS16 *);
FORWARD void   AssembleCallMask(MOUSE_CALL_MASK *);
FORWARD void   FullscTextPtr(int, int);
FORWARD void   WindowedGraphicsScale(half_word,IS16,IS16,IS16 *,IS16 *);
FORWARD void   dummy(short *,short *,unsigned short *);
FORWARD void   LimitCoordinates(half_word,IS16 *,IS16 *);
#ifdef X86GFX
#line 128
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif //X86GFX
#line 132
FORWARD void   TextScale(IS16 *,IS16 *,IS16 *, IS16 *);

void LazyMouseInterrupt();
VOID MouseEoiHook(int IrqLine, int CallCount);
BOOLEAN bSuspendMouseInterrupts=FALSE;


GLOBAL   HOSTMOUSEFUNCS   the_mouse_funcs =
{
   mouse_restore_cursor,
   deinstall_host_mouse,
   mouse_installed,
   mouse_in_use,
   mouse_reset,
   mouse_set_position,
   dummy,
   mouse_cursor_display,
   mouse_cursor_undisplay,
   mouse_cursor_mode_change
};

BOOL   bMouseMenuItemAdded=FALSE;
HMENU  hM;

//
// look up table to convert the video mode number to a mode type indicator
//

LOCAL half_word TextOrGraphicsModeLUT[] =
   {
   TEXT_MODE,    TEXT_MODE,    TEXT_MODE,    TEXT_MODE,    GRAPHICS_MODE,
   GRAPHICS_MODE,GRAPHICS_MODE,NOTHING,      NOTHING,      NOTHING,
   NOTHING,      NOTHING,      NOTHING,      GRAPHICS_MODE,GRAPHICS_MODE,
   GRAPHICS_MODE,GRAPHICS_MODE,GRAPHICS_MODE,GRAPHICS_MODE,GRAPHICS_MODE,
   GRAPHICS_MODE,NOTHING,      NOTHING,      NOTHING,      NOTHING,
   NOTHING,      NOTHING,      NOTHING,      NOTHING,      NOTHING,
   NOTHING,      NOTHING,      TEXT_MODE,    NOTHING,      NOTHING,
   NOTHING,      NOTHING,      NOTHING,      NOTHING,      NOTHING,
   NOTHING,      NOTHING,      NOTHING,      NOTHING,      NOTHING,
   NOTHING,      NOTHING,      NOTHING,      NOTHING,      NOTHING,
   NOTHING,      NOTHING,      NOTHING,      NOTHING,      NOTHING,
   NOTHING,      NOTHING,      NOTHING,      NOTHING,      NOTHING,
   NOTHING,      NOTHING,      NOTHING,      NOTHING,      TEXT_MODE,
   TEXT_MODE,    TEXT_MODE,    TEXT_MODE,    TEXT_MODE,    TEXT_MODE,
   NOTHING,      NOTHING,      NOTHING,      NOTHING,      NOTHING,
   NOTHING,      NOTHING,      NOTHING,      NOTHING,      NOTHING,
   NOTHING,      NOTHING,      NOTHING,      NOTHING,      NOTHING,
   NOTHING,      NOTHING,      NOTHING,      NOTHING,      NOTHING,
   NOTHING,      NOTHING,      NOTHING,      NOTHING,      NOTHING,
   NOTHING,      GRAPHICS_MODE,GRAPHICS_MODE,GRAPHICS_MODE
   };


LOCAL int VirtualScrCtrLUTx[] =
   {
   319,         // mode 0
   319,         // mode 1
   319,         // mode 2
   319,         // mode 3
   319,         // mode 4
   319,         // mode 5
   319,         // mode 6
   319,         // mode 7
   319,         // mode 8
   319,         // mode 9
   319,         // mode a
   319,         // mode b
   319,         // mode c
   319,         // mode d
   319,         // mode e
   319,         // mode f
   319,         // mode 10
   319,         // mode 11
   319,         // mode 12
   159          // mode 13
   };
LOCAL int VirtualScrCtrLUTy[] =
   {
   99,         // mode 0
   99,         // mode 1
   99,         // mode 2
   99,         // mode 3
   99,         // mode 4
   99,         // mode 5
   99,         // mode 6
   99,         // mode 7
   99,         // mode 8
   99,         // mode 9
   99,         // mode a
   99,         // mode b
   99,         // mode c
   99,         // mode d
   99,         // mode e
   174,        // mode f
   174,        // mode 10
   239,        // mode 11
   239,        // mode 12
   99          // mode 13
   };

//
// A look up table to convert a console cell location to a virtual pixel
// coordinate.
//

int ConsoleTextCellToVPCellLUT[] =
   {
   0, 8, 16, 24, 32, 40, 48, 56,
   64, 72, 80, 88, 96, 104, 112, 120,
   128, 136, 144, 152, 160, 168, 176, 184,
   192, 200, 208, 216, 224, 232, 240, 248,
   256, 264, 272, 280, 288, 296, 304, 312,
   320, 328, 336, 344, 352, 360, 368, 376,
   384, 392, 400, 408, 416, 424, 432, 440,
   448, 456, 464, 472, 480, 488, 496, 504,
   512, 520, 528, 536, 544, 552, 560, 568,
   576, 584, 592, 600, 608, 616, 624, 632,
   640, 648, 656, 664, 672, 680, 688, 696
   };


//
// This structure holds the information provided by int 33h functions 7 and 8.
// If one of these functions have been called, then the appropriate flag in
// the structure is set and the handler for this will ignore the default bounds
// for the current video mode and will use the values in the structure instead.
// The flags will clear if a mode change has been detected too.
//

struct
   {
   word xmin;
   word ymin;
   word xmax;
   word ymax;
   BOOL bF7;
   BOOL bF8;
   } confine = {0,0,639,199,FALSE,FALSE};
//
// Mouse interrupt regulation mechanisms
//
BOOLEAN bMseEoiPending = FALSE;
ULONG MseIntLazyCount = 0;

//
//
// The code starts here
//
//

GLOBAL void host_mouse_install1(void)
{
mouse_state = INSTALLED;
RegisterEOIHook(9, MouseEoiHook);
mouse_install1();
}


GLOBAL void host_mouse_install2(void)
{
mouse_install2();
}


GLOBAL void mouse_restore_cursor()
{
/* If mouse not in use exit */

if(!mouse_in_use())
   return;

}

GLOBAL void deinstall_host_mouse()
{
mouse_state = NOT_INSTALLED;
}


GLOBAL BOOL mouse_installed()
{
return (mouse_state);
}

void dummy(short *pooh1,short *pooh2, unsigned short *pooh3)
{
}

GLOBAL BOOL mouse_in_use()
{
return(mouse_state == INSTALLED && in_text_mode() == FALSE);
}

GLOBAL void mouse_reset()
{
/* DIVERGENCE(MVDM-HOST-DIV-318): CCPU Window uses the original counter
 * reset contract without the MONITOR guest fast-track storage. */
if(mvdm_softpc_mouse_route_active())
   {
   confine.bF7 = confine.bF8 = FALSE;
   bFunctionZeroReset = TRUE;
   }
#ifdef X86GFX
#line 328
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif /*X86GFX */
#line 374

//
// Set the default Mickey to pixel ratios
// This is set to 8 pixels to 8 Mickeys in the horizontal direction
// and 16 pixels to 8 Mickeys in the vertical.
//

m2pX = 8;
m2pY = 16;

}


GLOBAL void mouse_set_position IFN2(USHORT, newx, USHORT, newy)
{
#ifdef X86GFX
#line 390
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif //X86GFX
#line 403


//
// Both Fullscreen and MouseHidePointer enabled windowed mode
// mouse emulations are driven by a set of emulated counters.
// X,Y values are held independently to these counters and rely
// on this function to set up absolute values for X and Y. Note:
// reset does this too.
//

/* DIVERGENCE(MVDM-HOST-DIV-318): Window relative input shares the original
 * counter-position update; it does not warp a host Console pointer. */
if((sc.ScreenState == WINDOWED && bPointerOff) || mvdm_softpc_mouse_route_active())
#line 414
   {
   /* DIVERGENCE(MVDM-HOST-DIV-318): INT33/4 supersedes an unconsumed
      Window reset; otherwise the first movement is spent consuming both. */
   if(mvdm_softpc_mouse_route_active()) bFunctionZeroReset = FALSE;
#line 415
   newF4x = (IS16)newx;
   newF4y = (IS16)newy;
   bFunctionFour = TRUE;
   return;
   }
#ifdef X86GFX
#line 421
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif //X86GFX
#line 493
}

GLOBAL void mouse_cursor_display()
{
}

GLOBAL void mouse_cursor_undisplay()
{
}

GLOBAL void mouse_cursor_mode_change()
{
}


GLOBAL	void host_mouse_conditional_off_enabled(void)
{
#ifdef X86GFX
#line 511
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif
#line 528


}
//==============================================================================
// Hook function that forms the communication transition between
// the host and the base for os pointer emulation. This function
// is called by mouse_int1() which lives in mouse_io.c
//==============================================================================


VOID host_os_mouse_pointer(MOUSE_CURSOR_STATUS *mcs,MOUSE_CALL_MASK *call_mask,
                           MOUSE_VECTOR *counter)
{
#ifdef X86GFX
#line 542
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif // X86GFX
#line 544

host_ica_lock(); // synch with the event thread

/* DIVERGENCE(MVDM-HOST-DIV-318): preserve original absolute route otherwise. */
if(!mvdm_softpc_mouse_apply(mcs,counter,&old_x,&old_y,&bPointerInSamePlace,
                          bFunctionZeroReset,&bFunctionFour,&newF4x,&newF4y))
{
#line 547
GetNextMouseEvent();

#ifdef X86GFX
#line 550
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif // X86GFX
#line 557
   {
   ScaleToWindowedVirtualCoordinates(&mcs->position.x,&mcs->position.y,counter);
   }

}
// Publish the same final virtual coordinates consumed by the guest mouse path.
(void)mvdm_softpc_wow_page_domain_publish_cursor((long)mcs->position.x,
                                                 (long)mcs->position.y);

//
// Create a condition mask for use by any call back installed
// by the application.
//

#line 566
AssembleCallMask(call_mask);

//
// Tell the base about the button state
//

mcs->button_status=os_pointer_data.button_l | os_pointer_data.button_r<<1;

#line 574
host_ica_unlock();  // synch with the event thread


//
// Has the pointer moved since the last mouse interrupt.
// This can happen if a button press occurs but no physical
// movement of the mouse body takes place.
//

if(bPointerInSamePlace)
   {
   //
   // The mouse has not moved since the last mouse
   // hardware interrupt.
   //

   *call_mask &= ~1;
   }
else
   {
#ifdef X86GFX
#line 595
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif // X86GFX
#line 597

   //
   // The mouse has moved.
   //

   *call_mask |= 1;

#ifdef X86GFX
#line 605
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif	   // X86GFX
#line 701
   }


//
// Write data to the 16 bit driver for int 33h function 3 to pick up
// without having to BOP to the 32 bit side.
// Int 33h function 3 requires this data:
//     BX = button status
//     CX = virtual pixel position in x
//     DX = virtual pixel position in y
//

#ifdef X86GFX
#line 714
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif //X86GFX
#line 719
}

//==============================================================================
// Function to munge the status register value for the InPort adapter.
// Nothing much else to say about it really.
//
//==============================================================================

void AssembleCallMask(MOUSE_CALL_MASK *call_mask)
{
static int old_l_button=0;      // previous mouse button state
static int old_r_button=0;


//
// add in the left button current status
//

if(os_pointer_data.button_l)
   {
   //
   // Left button is down.
   //

   if(!old_l_button)
      {
      //
      // The button has just been pressed
      //

      *call_mask |= (1<<1);
      }
   else
      {
      //
      // The button was down on the last hardware interrupt, so
      // release the edge detect.
      //

      *call_mask &= ~(1<<1);
      }
   }
else
   {
   //
   // Left button is up.
   //

   if(old_l_button)
      {
      //
      // The button has just been released
      //

      *call_mask |= (1<<2);
      }
   else
      {
      //
      // The button was up on the last hardware interrupt, so
      // release the edge detect.
      //

      *call_mask &= ~(1<<2);
      }
   }

//
// add in the right button current status
//

if(os_pointer_data.button_r)
   {
   //
   // Right button is down.
   //

   if(!old_r_button)
      {
      //
      // The button has just been pressed
      //

      *call_mask |= (1<<3);
      }
   else
      {
      //
      // The button was down on the last hardware interrupt, so
      // release the edge detect.
      //

      *call_mask &= ~(1<<3);
      }
   }
else
   {
   //
   // Right button is up.
   //

   if(old_r_button)
      {
      //
      // The button has just been released
      //

      *call_mask |= (1<<4);
      }
   else
      {
      //
      // The button was up on the last hardware interrupt, so
      // release the edge detect.
      //

      *call_mask &= ~(1<<4);
      }
   }

//
//
// save the current mouse button status.
//

old_l_button = os_pointer_data.button_l;
old_r_button = os_pointer_data.button_r;

}

#ifdef X86GFX
#line 850
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif // X86GFX
#line 1040

//=========================================================================
// Function to scale mouse coordinates as returned by the event loop
// mechanism to virtual coordinates as defined by the Microsoft Mouse
// Programmer's Reference. The method used for scaling depends on the style
// of mouse buffer selected, the current video mode and if (for X86) the
// video system is operating in full screen or windowed mode.
//
// Output: Virtual cartesian coordinates in the same pointers
//=========================================================================

void ScaleToWindowedVirtualCoordinates(IS16 *outx,IS16 *outy,
				       MOUSE_VECTOR *counter)
{
half_word video_mode,textorgraphics;
SAVED SHORT last_text_good_x = 0, last_text_good_y = 0;

sas_load(0x449,&video_mode);

//
// Follow different code paths if the user has the system pointer
// hidden or displayed.
//

if(!bPointerOff)
   {
   //
   // The pointer has not been hidden by the user, so use the
   // x,y values as got from the system pointer via the Windows
   // messaging system.
   //

   //
   // get the video mode to determine if we're running text or graphics
   //

   textorgraphics=TextOrGraphicsModeLUT[video_mode];

   if(textorgraphics == TEXT_MODE)
      {
      //
      // validate received data to ensure not graphics mode coords
      // received during mode switch.
      //

      if(os_pointer_data.x > 87)
	 {
	 *outx = last_text_good_x;
	 }
      else
	 {
	 *outx = ConsoleTextCellToVPCellLUT[os_pointer_data.x];
	 last_text_good_x = *outx;
	 }
      if(os_pointer_data.y > 87)
	 {
	 *outy = last_text_good_y;
	 }
      else
	 {
	 *outy = ConsoleTextCellToVPCellLUT[os_pointer_data.y];
	 last_text_good_y = *outy;
	 }
      }
   else // GRAPHICS_MODE
      {
      //
      // If the mouse is NOT in a warping mode, then x,y's as received
      // from the console are scaled if they are for a low resolution
      // video mode and these are what the application sees and the
      // system pointer image is in the correct place to simulate the
      // 16 bit pointer.
      // If the application decides to extend the x,y bounds returned
      // from the driver beyond the limits of the console being used,
      // these x,y's are not appropriate, and the mouse must be switched
      // into warp mode by the user and the code will emulate the x,y
      // generation.
      //

      WindowedGraphicsScale(video_mode,(IS16)(os_pointer_data.x),
			    (IS16)(os_pointer_data.y),outx,outy);
      }

   //
   // No warping, so set up the old_x and old_y values directly.
   // Signal that the pointer hasn't moved, if it hasn't
   //

   bPointerInSamePlace = (old_x == *outx && old_y == *outy) ? TRUE : FALSE;

   //
   // Set these statics for the next time through.
   //

   old_x = *outx;
   old_y = *outy;
   }
else
   {
   //
   // The user has set the system pointer to be hidden via the
   // console's system menu.
   // Handle by emulating the counters and generating absolute x,y
   // data from this.
   //

   IS16 move_x,move_y;

   //
   // Get positional data from the system pointer and maintain some
   // counters.
   //

   if(WarpSystemPointer(&move_x,&move_y))
      {
      //
      // The mouse moved.
      // Generate some new emulated absolute position information.
      //

      EmulateCoordinates(video_mode,move_x,move_y,outx,outy);

      //
      // Save up the current emulated position for next time through.
      //


      old_x = *outx;
      old_y = *outy;

      //
      // Send back the relative motion since last time through.
      //

      counter->x = move_x;
      counter->y = move_y;

      bPointerInSamePlace = FALSE;
      }
   else
      {
      //
      // No recorded movement of the mouse.
      //

      *outx = old_x;
      *outy = old_y;
      counter->x = counter->y = 0;
      bPointerInSamePlace = TRUE;
      }
   }
}


//==============================================================================
// Function to scale the incoming fullscreen coordinates to mouse motion (both
// absolute and relative) for fullscreen text mode.
// Note: if the application chooses to, it can reset the virtual coordinate
// bounds of the virtual screen. This function checks the bound flags and either
// selects the default values or the app imposed values.
//
// This function is also used to modify the coordinates produced directly from
// the motion counters in windowed text mode.
//==============================================================================

void TextScale(IS16 *iX,IS16 *iY,IS16 *oX, IS16 *oY)
{
half_word no_of_rows;

//
// Calculate the current system pointer location in virtual
// pixels for the application.
//

if(confine.bF7)
   {
   //
   // Limits have been imposed by the application.
   //

   if(*iX < confine.xmin)
      {
      *iX = confine.xmin;
      }
   else if(*iX > confine.xmax)
      {
      *iX = confine.xmax;
      }
   }
else // use the default virtual screen constraints.
   {
   //
   // No limits have been imposed by the application.
   // x is always 0 -> 639 virtual pixels for text mode
   //

   if(*iX < 0)
      {
      *iX = 0;
      }
   else if(*iX > 639)
      {
      *iX = 639;
      }
   }

//
// Bind the y cartesian coordinate appropriately
//

if(confine.bF8)
   {
   //
   // Limits have been imposed by the application.
   //

   if(*iY < confine.ymin)
      {
      *iY = confine.ymin;
      }
   else if(*iY > confine.ymax)
      {
      *iY = confine.ymax;
      }
   }
else
   {
   //
   // The application has not imposed constraints on the Y virtual pixel
   // motion, so confine the Y movement to the default virtual pixel size
   // of the virtual screen for the current video mode.
   //

   if(*iY < 0)
      {
      *iY = 0;
      }
   else
      {
      //
      // Get the number of text rows minus one, from the B.D.A.
      //

      sas_load(0x484,&no_of_rows);

      switch(no_of_rows)
         {
         case 24:
            {
            //
            // 25 rows, so there are 200 vertical virtual pixels
            //
            if(*iY > 199)
               {
               *iY = 199;
               }
            break;
            }
         case 42:
            {
            //
            // 43 rows, so there are 344 vertical virtual pixels
            //
            if(*iY > 343)
               {
               *iY = 343;
               }
            break;
            }
         case 49:
            {
            //
            // 50 rows, so there are 400 vertical virtual pixels
            //
            if(*iY > 399)
               {
               *iY = 399;
               }
            break;
            }
         default:
            {
            //
            // default - assume 25 rows, so there are
            // 200 vertical virtual pixels
            //
            if(*iY > 199)
               {
               *iY = 199;
               }
            break;
            }
         }
      }
   }
*oX = *iX;
*oY = *iY;
}


//==============================================================================
// Fit the raw x,y coordinates generated from the counters to the bounds of
// the virtual screen that is currently set up. This can be set up either by
// the application (in the confine structure) or as set by the mouse driver
// as default.
//==============================================================================

void LimitCoordinates(half_word vm,IS16 *iX,IS16 *iY)
{
//
// Select the appropriate conditioning code for
// the current video mode.
//
switch(vm)
   {

   //
   // Do the common text modes.
   //

   case(2):
   case(3):
   case(7):
      {
      IS16 oX,oY;

      //
      // Scale the generated coordinates for the given text mode
      // and the number of rows of text displayed on the screen.
      //

      TextScale(iX,iY, &oX, &oY);
      *iX = oX;
      *iY = oY;
      break;
      }

   //
   // The regular VGA supported graphics video modes.
   //
   // The following modes are all 640 x 200 virtual pixels
   //
   case(4):
   case(5):
   case(6):
   case(0xd):
   case(0xe):
   case(0x13):
      {
      if(confine.bF7)
         {
         //
         // Limits have been imposed by the application.
         //

         if(*iX < confine.xmin)
            {
            *iX = confine.xmin;
            }
         else if(*iX > confine.xmax)
            {
            *iX = confine.xmax;
            }
         }
      else
         {
         //
         // The application has not imposed limits, so use
         // the default virtual screen bounds as defined byt
         // the mouse driver.
         //

         if(*iX < 0)
            *iX = 0;
         else if(*iX > 639)
            *iX = 639;
         }

      if(confine.bF8)
         {
         //
         // Limits have been imposed by the application.
         //

         if(*iY < confine.ymin)
            {
            *iY = confine.ymin;
            }
         else if(*iY > confine.ymax)
            {
            *iY = confine.ymax;
            }
         }
      else
         {
         //
         // The application has not imposed limits, so use
         // the default virtual screen bounds as defined byt
         // the mouse driver.
         //
         if(*iY < 0)
            *iY = 0;
         else if(*iY > 199)
            *iY = 199;
         }
      break;
      }
   //
   // The following modes are all 640 x 350 virtual pixels
   //
   case(0xf):
   case(0x10):
      {
      if(confine.bF7)
         {
         //
         // Limits have been imposed by the application.
         //

         if(*iX < confine.xmin)
            {
            *iX = confine.xmin;
            }
         else if(*iX > confine.xmax)
            {
            *iX = confine.xmax;
            }
         }
      else
         {
         //
         // The application has not imposed limits, so use
         // the default virtual screen bounds as defined byt
         // the mouse driver.
         //

         if(*iX < 0)
            *iX = 0;
         else if(*iX > 639)
            *iX = 639;
         }

      if(confine.bF8)
         {
         //
         // Limits have been imposed by the application.
         //

         if(*iY < confine.ymin)
            {
            *iY = confine.ymin;
            }
         else if(*iY > confine.ymax)
            {
            *iY = confine.ymax;
            }
         }
      else
         {
         //
         // The application has not imposed limits, so use
         // the default virtual screen bounds as defined byt
         // the mouse driver.
         //

         if(*iY < 0)
            *iY = 0;
         else if(*iY > 349)
            *iY = 349;
         }
      break;
      }
   //
   // The following modes are all 640 x 480 virtual pixels
   //
   case(0x11):
   case(0x12):
      {
      if(confine.bF7)
         {
         //
         // Limits have been imposed by the application.
         //

         if(*iX < confine.xmin)
            {
            *iX = confine.xmin;
            }
         else if(*iX > confine.xmax)
            {
            *iX = confine.xmax;
            }
         }
      else
         {
         //
         // The application has not imposed limits, so use
         // the default virtual screen bounds as defined byt
         // the mouse driver.
         //

         if(*iX < 0)
            *iX = 0;
         else if(*iX > 639)
            *iX = 639;
         }

      if(confine.bF8)
         {
         //
         // Limits have been imposed by the application.
         //

         if(*iY < confine.ymin)
            {
            *iY = confine.ymin;
            }
         else if(*iY > confine.ymax)
            {
            *iY = confine.ymax;
            }
         }
      else
         {
         //
         // The application has not imposed limits, so use
         // the default virtual screen bounds as defined byt
         // the mouse driver.
         //

         if(*iY < 0)
            *iY = 0;
         else if(*iY > 479)
            *iY = 479;
         }
      break;
      }

   //
   // From here on down are the Video7 modes
   //

   case(0x60):
      {
      if(*iX < 0)
         *iX = 0;
      else if(*iX > 751)
         *iX = 751;

      if(*iY < 0)
         *iY = 0;
      else if(*iY > 407)
         *iY = 407;
      break;
      }
   case(0x61):
      {
      if(*iX < 0)
         *iX = 0;
      else if(*iX > 719)
         *iX = 719;

      if(*iY < 0)
         *iY = 0;
      else if(*iY > 535)
         *iY = 535;
      break;
      }
   case(0x62):
   case(0x69):
      {
      if(*iX < 0)
         *iX = 0;
      else if(*iX > 799)
         *iX = 799;

      if(*iY < 0)
         *iY = 0;
      else if(*iY > 599)
         *iY = 599;
      break;
      }
   case(0x63):
   case(0x64):
   case(0x65):
      {
      if(*iX < 0)
         *iX = 0;
      else if(*iX > 1023)
         *iX = 1023;

      if(*iY < 0)
         *iY = 0;
      else if(*iY > 767)
         *iY = 767;
      break;
      }
   case(0x66):
      {
      if(*iX < 0)
         *iX = 0;
      else if(*iX > 639)
         *iX = 639;

      if(*iY < 0)
         *iY = 0;
      else if(*iY > 399)
         *iY = 399;
      break;
      }
   case(0x67):
      {
      if(*iX < 0)
         *iX = 0;
      else if(*iX > 639)
         *iX = 639;

      if(*iY < 0)
         *iY = 0;
      else if(*iY > 479)
         *iY = 479;
      break;
      }
   case(0x68):
      {
      if(*iX < 0)
         *iX = 0;
      else if(*iX > 719)
         *iX = 719;

      if(*iY < 0)
         *iY = 0;
      else if(*iY > 539)
         *iY = 539;
      break;
      }
   default:
      {
      if(*iX < 0)
         *iX = 0;
      else if(*iX > 639)
         *iX = 639;

      if(*iY < 0)
         *iY = 0;
      else if(*iY > 199)
         *iY = 199;
      break;
      }
   }
}

//==============================================================================
// Function to scale the incoming windowed coordinates since the window size
// can be larger (for the low resolution modes) than the virtual screen size
// for that mode.
//==============================================================================

void WindowedGraphicsScale(half_word vm,IS16 iX,IS16 iY,IS16 *oX, IS16 *oY)
{
switch(vm)
   {
   //
   // The following modes are all 640 x 200 virtual pixels
   //
   case(4):
   case(5):
   case(6):
   case(0xd):
   case(0xe):
   case(0x13):
      {
      //
      // Low resolution graphics modes. The window is 640 x 400 real host
      // pixels, but the virtual screen resolution is 640 x 200. Hence
      // must divide the y value by 2 to scale appropriately.
      //

      iY >> 1;
      break;
      }
   }
//
// prepare the cartesian coordinate values to return.
//

*oX = iX;
*oY = iY;
}

//===========================================================================
// Function to turn on the mouse cursor in FULLSCREEN mode.
// Note: This function only gets called after the 16 bit code checks
// to see if the internal cursor flag is zero. The 16 bit code does
// not do the BOP if, after incrementing this counter, the above is
// true.
//===========================================================================

void host_show_pointer()
{
#ifdef X86GFX
#line 1741
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif // X86GFX
#line 1774
}

//===========================================================================
// Function to turn off the mouse cursor in FULLSCREEN mode.
// Note: This function only gets called after the 16 bit code checks
// to see if the internal cursor flag is zero. The 16 bit code does
// not do the BOP if, after decrementing this counter, the above is
// true.
//===========================================================================

void host_hide_pointer()
{
#ifdef X86GFX
#line 1787
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif // X86GFX
#line 1822
}

//=========================================================================
// Function to remove the mouse pointer item from the console system menu
// when SoftPC lets an application quit or iconise.
// If the system pointer is OFF i.e. clipped to a region in the current window,
// then relinquish it to the system.
//
// bForce allows the code which gets called when there is a fullscreen to
// windowed graphics switch on x86 to force the menu item off.
//=========================================================================

void MouseDetachMenuItem(BOOL bForce)
{
if(bMouseMenuItemAdded || bForce)
   {
   DeleteMenu(hM,IDM_POINTER,MF_BYCOMMAND);
   bMouseMenuItemAdded=FALSE;
   ClipCursor(NULL);
   }
bAlertMessage=TRUE; // blocking, so reset the int33h f11 alert mechanism
}

void MouseAttachMenuItem(HANDLE hBuff)
{
if(!bMouseMenuItemAdded)
   {
   //
   // Read in the relavent string from resource
   //

   hM = ConsoleMenuControl(hBuff,IDM_POINTER,IDM_POINTER);
   AppendMenu(hM,MF_STRING,IDM_POINTER,szHideMouseMenuStr);
   bMouseMenuItemAdded=TRUE;

   //
   // initial state -> system pointer is ON
   //

   bPointerOff=FALSE;
   }
}

//=========================================================================
// Function to determine if the active output buffer has changed if the VDM
// has done something weird like resized or gone from graphics to fullscreen
// or vice versa. If so, a new handle to the new buffer must be got, and the
// old menu item deleted and a new menu item attach so that the new buffer
// "knows" about the menu item I.D.
//=========================================================================

void MouseReattachMenuItem(HANDLE hBuff)
{
static HANDLE hOld = 0;   // The handle for the last buffer selected.

//
// If the output buffer has not changed, then don't do anything.
//

if(hOld == hBuff)
   return;

//
// First thing, remove the old menu item.
//

MouseDetachMenuItem(TRUE);

//
// Next, Add in the new menu item for the current buffer.
//

MouseAttachMenuItem(hBuff);

//
// Record the value of the latest buffer for next time.
//

hOld = hBuff;
}

void MouseHide(void)
{

ModifyMenu(hM,IDM_POINTER,MF_BYCOMMAND,IDM_POINTER,
	   (LPTSTR)szDisplayMouseMenuStr);

//
// Clip the pointer to a region inside the console window
// and move the pointer to the window centre
//

while(ShowConsoleCursor(sc.ActiveOutputBufferHandle,FALSE)>=0)
   ;
MovePointerToWindowCentre();
bPointerOff=TRUE;
}

void MouseDisplay(void)
{

ModifyMenu(hM,IDM_POINTER,MF_BYCOMMAND,IDM_POINTER,
	  (LPTSTR)szHideMouseMenuStr);

//
// Let the pointer move anywhere on the screen
//

ClipCursor(NULL);
while(ShowConsoleCursor(sc.ActiveOutputBufferHandle,TRUE)<0)
   ;
bPointerOff=FALSE;
}

void MovePointerToWindowCentre(void)
{
RECT rTemp;

//
// Get current console client rectangle, set the clipping region to match
// the client rect. Retrieve the new clipping rect from the system (is
// different from what we requested!) and save it.
//
VDMConsoleOperation(VDM_CLIENT_RECT,&WarpClientRect);
rTemp = WarpClientRect;
CToS(&rTemp);
ClipCursor(&rTemp);
GetClipCursor(&WarpBorderRect);

//
// Note : LowerRight clip point is exclusive, UpperLeft point is inclusive
//
WarpBorderRect.right--;
WarpBorderRect.bottom--;

pMiddle.x = ((WarpBorderRect.right - WarpBorderRect.left)>>1)
             +WarpBorderRect.left;
pMiddle.y = ((WarpBorderRect.bottom - WarpBorderRect.top)>>1)
             +WarpBorderRect.top;
//
// move the pointer to the centre of the client area
//

SetCursorPos((int)pMiddle.x,(int)pMiddle.y);

//
// Prevent the next counter calculation from resulting in a
// large warp.
//

pLast = pMiddle;
}

//=============================================================================
// Function to convert a rectangle structure from client coordinates to
// screen coordinates.
//=============================================================================

void CToS(RECT *r)
{
POINT pt;

//
// Sort out the top, lefthand corner of the rectangle.
//

pt.x = r->left;
pt.y = r->top;
VDMConsoleOperation(VDM_CLIENT_TO_SCREEN,(LPVOID)&pt);
r->left = pt.x;
r->top = pt.y;

//
// Now do the bottom, right hand corner.
//

pt.x = r->right;
pt.y = r->bottom;
VDMConsoleOperation(VDM_CLIENT_TO_SCREEN,(LPVOID)&pt);
r->right = pt.x;
r->bottom = pt.y;
}


//=============================================================================
//
//  Function - EmulateCoordinates.
//  Purpose  - When the mouse is hidden by the user in windowed mode, this
//	       function generate absolute x,y values from the relative motion
//	       of the system pointer between mouse hardware interrupts
//
//  Returns  - Nothing.
//
//
//
//  Author   - Andrew Watson.
//  Date     - 19-Mar-1994.
//
//=============================================================================

void EmulateCoordinates(half_word video_mode,IS16 move_x,IS16 move_y,IS16 *x,IS16 *y)
{
static IS16 lastinternalX=0,lastinternalY=0;
IS16 internalX,internalY;

//
// If the  application has reset the mouse, set the x,y position to
// the centre of the default virtual screen for the current video mode.
//


if(bFunctionZeroReset)
   {
   //
   // Calculate the centre of the default virtual screen
   // and set the current generated coordinates to it.
   //

   internalX = (VirtualX >> 1) - 1;
   internalY = (VirtualY >> 1) - 1;

   bFunctionZeroReset = FALSE;
   }
else if(bFunctionFour)
   {
   //
   // The application has set the pointer to a new location.
   // Tell the counter emulation about this.
   //

   internalX = newF4x;
   internalY = newF4y;

   //
   // Don't come in here again until the next function 4.
   //

   bFunctionFour = FALSE;
   }
else
   {
   //
   // Generate the new x,y position based on the counter change and
   // clip to whatever boundary (default or application imposed) has
   // been selected.
   //

   internalX = lastinternalX + move_x;
   internalY = lastinternalY + move_y;
   LimitCoordinates(video_mode,&internalX,&internalY);
   }

//
// Set up the current emulated position for next time through
// this function.
//

lastinternalX = internalX;
lastinternalY = internalY;

//
// set up the return x,y values.
//

*x = internalX;
*y = internalY;
}

//=============================================================================
// Function which hooks into the base mechanism for dealing with int 33h and
// catches the function (AX=0xf) when the application tries to set the default
// mickey to pixel ratio.
//=============================================================================

void host_m2p_ratio(word *a, word *b, word *CX, word *DX)
{
m2pX = *(short *)CX;
m2pY = *(short *)DX;
}


//=============================================================================
//
//  Function - WarpSystem Pointer
//  Purpose  - Allows movement vectors to be calculated from the movement of
//	       the operating system pointer. This function will not let the
//	       the system pointer move out of the client area. This, plus the
//	       warping mechanism ensures that the emulated pointer can move
//	       forever in any given direction.
//
//  Returns  - TRUE if the system pointer has moved, FALSE if not.
//
//
//
//  Author   - Andrew Watson.
//  Date     - 19-Mar-1994.
//
//=============================================================================

BOOL WarpSystemPointer(IS16 *move_x,IS16 *move_y)
{
POINT pt;

//
// Is the Console window in the same place or changed
// Update the client rect data accordingly
//

HasConsoleClientRectChanged();

//
// Get the current position of the system pointer
//

GetCursorPos(&pt);


//
// How far has the system pointer moved since the last call.
//

*move_x = (IS16)(pt.x - pLast.x);
*move_y = (IS16)(pt.y - pLast.y);

//
// Do a fast exit if no movement has been determined.
//

if(*move_x || *move_y)
   {

   //
   // The system mouse pointer has moved.
   // See if the pointer has reached the client area boundary(s)
   //

   if(pt.y <= WarpBorderRect.top || pt.y >= WarpBorderRect.bottom ||
      pt.x >= WarpBorderRect.right || pt.x <= WarpBorderRect.left)
      {
      //
      // if the boundary(s) was/were met, warp the pointer to the
      // client area centre.
      //

      SetCursorPos((int)pMiddle.x,(int)pMiddle.y);

      //
      // The current position is now the centre of the client rectangle.
      // Save this as the counter delta start point for next time.
      //

      pLast = pMiddle;
      }
   else
      {
      //
      // There wasn't a warp.
      // Update the last known position data for next time through.
      //

      pLast = pt;
      }
   //
   // The cursor has to moved as determined from the previous and current
   // system pointer positions.
   //

   return TRUE;
   }
//
// No movement, so return appropriately.
//

return FALSE;
}

//==============================================================================
// Function to detect if the Console window has moved/resized. If it has, this
// function updates the WarpBorderRect and the pMiddle structures to reflect
// this.
//
// Returns TRUE if moved/resized, FALSE if not.
//==============================================================================

BOOL HasConsoleClientRectChanged(void)
{
RECT tR;

//
// If console client rectangle has changed, reset the mouse clipping
// else nothing to do!
//
VDMConsoleOperation(VDM_CLIENT_RECT,&tR);

if (tR.top != WarpClientRect.top ||
    tR.bottom != WarpClientRect.bottom ||
    tR.right != WarpClientRect.right ||
    tR.left != WarpClientRect.left)
  {
   CToS(&tR);

#line 2223
#ifdef MONITOR
#line 2224
#error DIVERGENCE MVDM-HOST-DIV-507: MONITOR branch is not selected for this source; see mirror register.
#endif    //MONITOR
#line 2244

   //
   // Clip the pointer to the new client rectangle, and retrive
   // the new clipping borders.
   //
   ClipCursor(&tR);
   GetClipCursor(&WarpBorderRect);

   //
   // Note: LowerRight clip point is exclusive, UpperLeft point is inclusive
   //
   WarpBorderRect.right--;
   WarpBorderRect.bottom--;



   //
   // determine the middle point in the new client rectangle
   //

   pMiddle.x = ((WarpBorderRect.right - WarpBorderRect.left)>>1)
                +WarpBorderRect.left;
   pMiddle.y = ((WarpBorderRect.bottom - WarpBorderRect.top)>>1)
                +WarpBorderRect.top;
   return TRUE;
   }

return FALSE;
}

//==============================================================================
// Focus sensing routines for the pointer clipping system. Focus events come
// via the main event loop where the following two modules are called. If
// the application is using int33hf11, this is detected, and on focus gain or
// loss, the routines clip or unclip the pointer to the console window.
//==============================================================================

void MouseInFocus(void)
{
MouseAttachMenuItem(sc.ActiveOutputBufferHandle);

//
// only do when app. uses int33hf11
//

if(!bPointerOff)
   return;
MovePointerToWindowCentre();

//
// Lose system pointer image again
//

ShowConsoleCursor(sc.ActiveOutputBufferHandle, FALSE);
}

void MouseOutOfFocus(void)
{
//
// only do when app. uses int33hf11
//

if(!bPointerOff)
   {
   MouseDetachMenuItem(FALSE);
   return;
   }

//
// Clip the pointer to the whole world (but leave its mother alone)
//

ClipCursor(NULL);

//
// Re-enable system pointer image
//

ShowConsoleCursor(sc.ActiveOutputBufferHandle, TRUE);
}

/* system memu active, stop cursor clipping */
void MouseSystemMenuON (void)
{
    if (bPointerOff)
	ClipCursor(NULL);
}
/* system menu off, restore clipping */
void MouseSystemMenuOFF(void)
{
    if (bPointerOff)
	ClipCursor(&WarpBorderRect);
}
void ResetMouseOnBlock(void)
{
host_ica_lock();

os_pointer_data.x=0;
os_pointer_data.y=0;

host_ica_unlock();
}
#ifdef X86GFX
#line 2347
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif //X86GFX
#line 2399

//===========================================================================
// Function to display the text cursor image for fullscreen text mode.
// INPUT: x,y pointer virtual cartesian coordinates for text screen buffer.
// Note: the Y coordinates are received in the sequence 0, 8, 16, 24, 32, ...
// since a virtual text cell is 8 virtual pixels square.
//===========================================================================


void FullscTextPtr(int x,int y)
{
#ifdef X86GFX
#line 2411
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif // X86GFX
#line 2479
}

//==============================================================================
// Function to get the maximum and minimum possible virtual pixel locations
// in X as requested by the application through int 33h function 7.
//==============================================================================

void host_x_range(word *blah, word *blah2, MOUSE_SCALAR *CX, MOUSE_SCALAR *DX)
{
confine.bF7 = TRUE;
confine.xmin = *CX;
confine.xmax = *DX;

//
// Force a mouse interrupt to make it happen.
//

  LazyMouseInterrupt();
}

//==============================================================================
// Function to get the maximum and minimum possible virtual pixel locations
// in Y as requested by the application through int 33h function 8.
//==============================================================================

void host_y_range(word *blah, word *blah2, MOUSE_SCALAR *CX, MOUSE_SCALAR *DX)
{
confine.bF8 = TRUE;
confine.ymin = *CX;
confine.ymax = *DX;

//
// Force a mouse interrupt to make it happen.
//

   LazyMouseInterrupt();
}



/*
 *  LazyMouseInterrupt -
 *
 */
void LazyMouseInterrupt(void)
{
    host_ica_lock();
    if (!bMseEoiPending && !bSuspendMouseInterrupts) {
        if (MseIntLazyCount)
            MseIntLazyCount--;
        bMseEoiPending = TRUE;
        ica_hw_interrupt(AT_CPU_MOUSE_ADAPTER,AT_CPU_MOUSE_INT,1);
        HostIdleNoActivity();
        }
    else if (!MseIntLazyCount) {
        MseIntLazyCount++;
        }
    host_ica_unlock();
}


/* SuspendMouseInterrupts
 *
 *  Prevents Mouse Interrupts from occuring until
 *  ResumeMouseInterrupts is called
 *
 */
void SuspendMouseInterrupts(void)
{
    host_ica_lock();
    bSuspendMouseInterrupts = TRUE;
    host_ica_unlock();
}


/*
 * ResumeMouseInterrupts
 *
 */
void ResumeMouseInterrupts(void)
{
    host_ica_lock();
    bSuspendMouseInterrupts = FALSE;

    if (!bMseEoiPending &&
        (MseIntLazyCount || MoreMouseEvents()) )
      {
        if (MseIntLazyCount)
            MseIntLazyCount--;
        bMseEoiPending   = TRUE;
        host_DelayHwInterrupt(9,  // AT_CPU_MOUSE_ADAPTER,AT_CPU_MOUSE_INT
                              1,  // count
                              10000  // Delay
                              );
        HostIdleNoActivity();
        }

    host_ica_unlock();
}



/*
 *  DoMouseInterrupt, assumes we are holding the ica lock
 *
 */
void DoMouseInterrupt(void)
{

   if (bMseEoiPending || bSuspendMouseInterrupts) {
       MseIntLazyCount++;
       return;
       }

   if (MseIntLazyCount)
       MseIntLazyCount--;
   bMseEoiPending   = TRUE;
   ica_hw_interrupt(AT_CPU_MOUSE_ADAPTER,AT_CPU_MOUSE_INT,1);
   HostIdleNoActivity();
}


/*
 * MouseEoiHook, assumes we are holding the ica lock
 *
 */
VOID MouseEoiHook(int IrqLine, int CallCount)
{

    if (CallCount < 0) {         // interrupts were cancelled
        MseIntLazyCount = 0;
        FlushMouseEvents();
        bMseEoiPending = FALSE;
        return;
        }

    if (!bSuspendMouseInterrupts &&
        (MseIntLazyCount || MoreMouseEvents()))
      {
       if (MseIntLazyCount)
           MseIntLazyCount--;
       bMseEoiPending = TRUE;
       host_DelayHwInterrupt(9,  // AT_CPU_MOUSE_ADAPTER,AT_CPU_MOUSE_INT
                             1,  // count
                             10000 // Delay usecs
                             );
       HostIdleNoActivity();
       }
    else {
       bMseEoiPending = FALSE;
       }
}
