#undef ANSI     // until file tidied or better still, lost

#include <windows.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "host_def.h"
#include "insignia.h"
#include "xt.h"
#include "debug.h"
#include "sas.h"
#include "config.h"
#include "chkmallc.h"

#ifdef X86GFX
#line 17
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif
#line 20

#ifdef MONITOR
#line 22
#error DIVERGENCE MVDM-HOST-DIV-507: MONITOR branch is not selected for this source; see mirror register.
#endif
#line 39


/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: log1p */

#ifndef MONITOR
GLOBAL double log1p(x)
double x;
{
    return log(1+x);
}

#endif /* !MONITOR */


/*:::::::::::::::::::::::::::::::::::::::::::::::::::::: SAS wrapping stubs */

#ifdef MONITOR
#line 56
#error DIVERGENCE MVDM-HOST-DIV-507: MONITOR branch is not selected for this source; see mirror register.
#endif	//MONITOR
#line 242

/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: ASSERT CODE */


void _assert(void *exp, void *file, unsigned line)
{
    char linestr[100];

    OutputDebugString("ASSERT FAILED - ");
    OutputDebugString(exp);
    OutputDebugString("  ");
    OutputDebugString(file);

    sprintf(linestr," (%d)\n",line);
    OutputDebugString(linestr);
}

/*:::::::::::::::::::::::::::: Unix specific string functions ::::::::::::::*/

char *index(char *string, int c)
{
    return(strchr(string, c));
}

char *rindex(char *string, int c)
{
    return(strrchr(string, c));
}

/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

host_mouse_in_use()
{
return(FALSE);
}

void redirector() {}
void reset_delta_data_structures() {}
void set_hfx_severity() {}

host_check_for_lock()
{
    assert0(NO,"host_check_for_lock stubbed\n");
    return(0);
}

host_place_lock(int dummy1)
{
    assert0(NO,"host_place_lock stubbed\n");
    return(0);
}

host_clear_lock(int fd)
{
    assert0(NO,"host_clear_lock stubbed\n");
    return(0);
}


int host_com_send_delay_done(int dummy1, int dummy2)
{
        return(0);
}
#ifndef A2CPU
delta_cpu_test_frag()
{
        assert0(NO,"delta_cpu_test_frag stubbed\n");
        return(0);
}

examine_delta_data_structs(int o, int i)
{
        assert0(NO,"examine_delta_data_structs stubbed\n");
        return(0);
}
code_gen_files_init()
{
        assert0(NO,"code_gen_files_init stubbed\n");
        return(0);
}
decode_files_init()
{
        assert0(NO,"decode_files_init stubbed\n");
        return(0);
}

#ifdef ALPHA
#line 329
#error Unsupported excluded original configuration
#endif
#line 341

#ifdef CPU_40_STYLE

GLOBAL BOOL sas_manage_xms IFN3(VOID *,start_addr, ULONG, cb, INT, a_or_f)
{
    printf("sas_manage_xms called(%lx,%lx,%lx)\n",start_addr,cb,a_or_f);
    return (TRUE);
}

#undef sas_loadw
GLOBAL void sas_loadw IFN2(sys_addr, addr, word *, val)
{
    *val = sas_w_at(addr);
}

GLOBAL void host_sigio_event IPT0()
{
}

#endif /* CPU_40_STYLE */

#ifndef GENERIC_NPX
#ifndef CPU_40_STYLE
#line 364
#error DIVERGENCE MVDM-HOST-DIV-513: non-CPU40 branch is not selected; see mirror register.
#endif /* CPU_40_STYLE */
#line 385
#endif /* GENERIC_NPX */
#endif

int     rate_min;
int     rate_max;
int     compiling;
int     rate_delta;
int     show_stuff;
int     stat_rate;
int     compile_off[99];
int     rate_norm;
int     cut_off_level;
int     max_host_fragment_size;
int     delta_err_message[100];
int     last_destination_address;


host_flip_real_floppy_ind()
{
        assert0(NO,"host_flip_real_floppy_ind stubbed\n");
        return(0);
}

#ifndef A2CPU
int     haddr_of_src_string;
int     INTEL_STATUS;
int     R_SI;
int     R_DI;
int     R_BP;
int     R_OPA;
int     R_DEF_SS;
int     R_OPB;
int     R_SP;
int     R_DEF_DS;
int     R_AX;
int     R_BX;
int     R_CX;
int     R_DX;
int     R_IP;
int     R_ACT_CS;
int     R_ACT_SS;
int     R_OPR;
int     R_ACT_DS;
int     R_ACT_ES;
#endif
int     compile_yoda_in;
int     m_s_w;
int     trap_delay_count = 0, temp_trap_flag = 0;
int     cpui;
int     sbp;

host_EOA_hook()
{
        assert0(NO,"host_EOA_hook stubbed\n");
        return(0);
}

getsdosunit()
{
        assert0(NO,"dispatch_q_event stubbed\n");
        return(1);
}

#line 448
#ifdef NOT_IN_USE
#line 449
#error Unsupported excluded original configuration
#endif
#line 460

int host_timer_2_frig_factor = 20;

host_check_read_only_drive()
{
        assert0(NO,"host_check_read_only_drive stubbed\n");
        return(0);
}

host_lock_drive_and_make_writable()
{
        assert0(NO,"host_lock_drive_and_make_writable stubbed\n");
        return(1);
}

host_floppy_init()
{
        assert0(NO,"host_floppy_init stubbed\n");
        return(1);
}

SHORT validate_hfx_drive()
{
        assert0(NO,"host_lpt_valid\n");
        return C_CONFIG_OP_OK;
}

SHORT host_keymap_valid()
{
        assert0(NO,"host_keymap_valid\n");
        return C_CONFIG_OP_OK;
}

VOID host_keymap_change()
{
        assert0(NO,"host_keymap_change\n");
}

char *host_strerror(int errno)
{
    assert1(NO,"Error : host_strerror (%d)\n",errno);
    return("****** again\n");
}

int link(void)
{
return -1;
}

LOCAL ULONG   dummy()
{
        return(0);
}

GLOBAL  ULONG    (*clear_v7ptr)() = dummy;
GLOBAL  ULONG    (*paint_v7ptr)() = dummy;

GLOBAL ULONG host_speed IFN1(ULONG, temp)
{
return 10000L;
}

#ifndef MONITOR

// allows getIntelRegistersPointer to be exported bt ntvdm.def
getIntelRegistersPointer()
{
    assert0(NO,"getIntelRegistersPointer stubbed\n");
    return(0);
}

#line 531
#if  0
#line 532
#error DIVERGENCE MVDM-HOST-DIV-496: original literal #if 0 excludes this legacy branch; see mirror register.
#endif
#line 543

#line 546
int FlatAddress[1];
int Ldt[1];
int VdmTib;
#line 549
int VdmTibStruct;

void DispatchInterrupts()
{
}

#else //MONITOR
#line 556
#error DIVERGENCE MVDM-HOST-DIV-508: MONITOR alternative is not selected for this source; see mirror register.
#endif  //MONITOR
#line 569


#ifndef MONITOR

//
// davehart 9-Dec-92 HACKHACK
// Build fix -- we export cpu_createthread from ntvdm.exe,
// even though this function from v86\monitor\i386 doesn't
// exist on MIPS.
//
// If we really need to export the function, we may want to
// take the ntoskrnl.src -> obj\i386\ntoskrnl.def approach.
//

VOID
cpu_createthread(
    HANDLE Thread
    )
/*++

Routine Description:

    This routine adds a thread to the list of threads that could be executing
    in application mode.

Arguments:

    Thread -- Supplies a thread handle

Return Value:

    None.

--*/
{
}

#endif // ndef MONITOR

#ifndef MONITOR
ULONG CurrentMonitorTeb;
#endif

void host_note_queue_added() 
{
}
