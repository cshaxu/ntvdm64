/* Actual selected gfi.obj + gfi_mpty.obj; no disk or host path is opened.
 * Only configuration/host-media boundaries are recording seams. */
#include <stdio.h>
#include <string.h>
#include "insignia.h"
#include "xt.h"
#include "bios.h"
#include "config.h"
#include "gfi.h"
#ifndef MVDM_GFI_EXPECT_TABLE_BYTES
#define MVDM_GFI_EXPECT_TABLE_BYTES 28
#endif
typedef char gfi_table_abi[(sizeof(GFI_FUNCTION_ENTRY)==MVDM_GFI_EXPECT_TABLE_BYTES && sizeof(void *)==4 && sizeof(SHORT)==2) ? 1 : -1];
static unsigned checks, failures, callbacks, runtime_calls, activations;
static UTINY runtime_id, active_id;
static SHORT runtime_value;
static BOOL active_value;
#define CHECK(x) do { ++checks; if(!(x)){++failures; fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x);} } while(0)
void host_runtime_set(UTINY what, SHORT value)
{ ++runtime_calls; runtime_id=what; runtime_value=value; }
void config_activate(UTINY id, BOOL active)
{ ++activations; active_id=id; active_value=active; }
SHORT host_gfi_rdiskette_valid(UTINY id, ConfigValues *v, CHAR *e)
{ (void)id;(void)v;(void)e;++callbacks;return 17; }
SHORT host_gfi_rdiskette_active(UTINY id, BOOL a, CHAR *e)
{ (void)id;(void)a;(void)e;++callbacks;return 19; }
void host_gfi_rdiskette_change(UTINY id, BOOL a)
{ (void)id;(void)a;++callbacks; }
extern SHORT gfi_floppy_valid(UTINY,ConfigValues *,NameTable *,CHAR *);
extern VOID gfi_floppy_change(UTINY,BOOL);
extern SHORT gfi_floppy_active(UTINY,BOOL,CHAR *);
extern VOID gfi_attach_adapter(UTINY,BOOL);
static SHORT recorded(FDC_CMD_BLOCK *c,FDC_RESULT_BLOCK *r)
{ (void)c;(void)r;++callbacks;return 23; }
int main(void)
{
    FDC_CMD_BLOCK command[MAX_COMMAND_LEN];
    FDC_RESULT_BLOCK result[MAX_RESULT_LEN+2];
    unsigned drive, op, before;
    gfi_init();
    CHECK(runtime_calls==1 && runtime_id==C_FLOPPY_SERVER && runtime_value==GFI_REAL_DISKETTE_SERVER);
    for(drive=0;drive<MAX_DISKETTES;++drive){
        CHECK(gfi_function_table[drive].command_fn!=0 && gfi_function_table[drive].reset_fn!=0);
        CHECK(gfi_drive_on((UTINY)drive)==SUCCESS);
        CHECK(gfi_drive_off((UTINY)drive)==SUCCESS);
        CHECK(gfi_high((UTINY)drive,0)==FAILURE);
        CHECK(gfi_drive_type((UTINY)drive)==GFI_DRIVE_TYPE_NULL);
        CHECK(gfi_change((UTINY)drive)==TRUE);
        memset(result,0xa5,sizeof(result));
        CHECK(gfi_reset(result+1,(UTINY)drive)==SUCCESS);
        CHECK(result[0]==0xa5 && result[MAX_RESULT_LEN+1]==0xa5);
        CHECK(result[1]==0xc0 && result[2]==0);
        for(op=0;op<32;++op){
            SHORT rc;
            memset(command,0,sizeof(command)); memset(result,0xa5,sizeof(result));
            put_type_cmd(command,op); put_type_drive(command,drive);
            rc=gfi_fdc_command(command,result+1);
            /* SPECIFY fans out and returns SUCCESS; empty RECALIBRATE
             * returns a real abnormal-seek result, other commands fail. */
            CHECK(rc==((op==3 || op==7)?SUCCESS:FAILURE));
            CHECK(result[0]==0xa5 && result[MAX_RESULT_LEN+1]==0xa5);
            if(op==7){CHECK(result[1]==(0x70|drive) && result[2]==0 && result[3]==0);}
            else {CHECK(result[1]==0 && result[2]==0 && result[3]==0);}
        }
    }
    /* Dispatch reaches the selected slot; SPECIFY visits all four. */
    for(drive=0;drive<MAX_DISKETTES;++drive)gfi_function_table[drive].command_fn=recorded;
    memset(command,0,sizeof(command)); put_type_cmd(command,6);put_type_drive(command,2);
    before=callbacks;CHECK(gfi_fdc_command(command,result)==23 && callbacks==before+1);
    put_type_cmd(command,3);before=callbacks;
    CHECK(gfi_fdc_command(command,result)==SUCCESS && callbacks==before+MAX_DISKETTES);
    CHECK(get_type_drive(command)==MAX_DISKETTES-1);
    gfi_init();CHECK(runtime_calls==2);
    for(drive=0;drive<MAX_DISKETTES;++drive)CHECK(gfi_function_table[drive].command_fn!=recorded);
    before=callbacks;
    CHECK(gfi_floppy_valid(C_FLOPPY_A_DEVICE,0,0,0)==17);
    CHECK(gfi_floppy_active(C_FLOPPY_A_DEVICE,TRUE,0)==19);
    gfi_floppy_change(C_FLOPPY_A_DEVICE,TRUE);CHECK(callbacks==before+3);
    gfi_attach_adapter(1,TRUE);
    CHECK(activations==1 && active_id==C_FLOPPY_A_DEVICE+1 && active_value==TRUE);
    printf("GFI_OBJECT_CONTRACT checks=%u failures=%u callbacks=%u\n",checks,failures,callbacks);
    return failures?1:0;
}
