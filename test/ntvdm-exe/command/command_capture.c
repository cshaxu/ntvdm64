#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "ntvdm-exe/command/include/mvdm_command_native_child.h"
#include "ntvdm-exe/softpc/include/mvdm_guest_location.h"
#include "ntvdm-exe/session/session.h"

static BYTE memory[65536];
static unsigned checks,leases;
static int fail_acquire,fail_release;
static session owner;
static session_teardown_fn teardown;
static void *teardown_context;
#define CHECK(x) do {++checks;if(!(x)){fprintf(stderr,"line %u: %s\n",__LINE__,#x);return 1;}} while(0)
session *__cdecl session_thread_current(void){return &owner;}
int session_valid(const session *s){return s==&owner;}
int session_register_teardown(session *s,session_teardown_fn f,void *c)
{(void)s;teardown=f;teardown_context=c;return 1;}
int mvdm_guest_location_set_real_mode(mvdm_guest_location *p,uint16_t s,uint16_t o)
{p->segment=s;p->offset=o;p->valid=1;return 1;}
int mvdm_guest_location_acquire(const mvdm_guest_location *p,uint32_t n,uint32_t access,mvdm_guest_location_lease *out)
{
    size_t address=(size_t)p->segment*16+p->offset;
    (void)access;
    if(fail_acquire || address>sizeof(memory) || n>sizeof(memory)-address)return 0;
    ++leases;out->bytes=memory+address;out->lease=NULL;return 1;
}
int mvdm_guest_location_release(mvdm_guest_location_lease *p,int commit)
{(void)commit;--leases;p->bytes=NULL;return !fail_release;}

int main(void)
{
    mvdm_command_capture_input input={0};
    mvdm_command_capture *a=NULL,*b=NULL,*failed=NULL;
    mvdm_command_capture_view av,bv;
    ULONG handles[3]={10,20,30};
    const char *ordinary_command,*ordinary_environment;
    owner.state=SESSION_STATE_ACTIVE;owner.epoch=1;
    memcpy(memory+100,"ordinary\r",9);memcpy(memory+400,"A=ordinary\0",12);
    memcpy(memory+800,handles,sizeof(handles));
    CHECK(mvdm_command_native_child_capture_guest(0,100,0,400,0,800));
    input.command_offset=100;input.environment_offset=400;input.standard_offset=800;
    input.default_drive=2;input.service=MVDM_COMMAND_SERVICE_EXECUTE;
    memcpy(memory+100,"first\r",6);memcpy(memory+400,"A=first\0",9);
    CHECK(mvdm_command_capture_create(&input,&a));CHECK(!memory[105]);CHECK(!leases);
    input.host_command="second";input.default_drive=3;input.service=MVDM_COMMAND_SERVICE_INTERACTIVE;
    memcpy(memory+400,"A=other\0",9);handles[1]=99;memcpy(memory+800,handles,sizeof(handles));
    CHECK(mvdm_command_capture_create(&input,&b));CHECK(!leases);
    memset(memory,0xcc,sizeof(memory));
    CHECK(mvdm_command_capture_read(a,&av));CHECK(mvdm_command_capture_read(b,&bv));
    CHECK(!strcmp(av.command,"first") && !strcmp(bv.command,"second"));
    CHECK(!strcmp(av.environment,"A=first") && !strcmp(bv.environment,"A=other"));
    CHECK(av.environment_bytes==9 && bv.environment_bytes==9);
    CHECK(av.standard_handles[1]==20 && bv.standard_handles[1]==99);
    CHECK(av.default_drive==2 && bv.default_drive==3);
    CHECK(av.service==MVDM_COMMAND_SERVICE_EXECUTE && bv.service==MVDM_COMMAND_SERVICE_INTERACTIVE);
    CHECK(mvdm_command_native_child_activate("unused","unused",&ordinary_command,&ordinary_environment));
    CHECK(!strcmp(ordinary_command,"ordinary") && !strcmp(ordinary_environment,"A=ordinary"));
    mvdm_command_capture_destroy(b);
    CHECK(!strcmp(av.command,"first"));
    input.host_command=NULL;input.command_offset=65535;
    CHECK(!mvdm_command_capture_create(&input,&failed));CHECK(!failed && !leases);
    input.command_offset=100;memcpy(memory+100,"x\r",2);
    input.environment_offset=65535;
    CHECK(!mvdm_command_capture_create(&input,&failed));CHECK(!failed && !leases);
    input.environment_offset=400;memcpy(memory+400,"\0",2);
    input.standard_offset=65530;
    CHECK(!mvdm_command_capture_create(&input,&failed));CHECK(!failed && !leases);
    input.standard_offset=800;fail_acquire=1;
    CHECK(!mvdm_command_capture_create(&input,&failed));CHECK(!failed && !leases);
    fail_acquire=0;fail_release=1;
    CHECK(!mvdm_command_capture_create(&input,&failed));CHECK(!failed && !leases);
    fail_release=0;input.default_drive=26;
    CHECK(!mvdm_command_capture_create(&input,&failed));CHECK(!failed);
    mvdm_command_capture_destroy(a);mvdm_command_native_child_finish();
    CHECK(!mvdm_command_native_child_command());
    teardown(teardown_context);CHECK(!owner.mvdm_command_native_child);
    printf("PASS %u independent command capture assertions\n",checks);return 0;
}
