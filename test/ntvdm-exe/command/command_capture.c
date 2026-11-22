#include "compat/include/mvdm_cpu_access.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ntvdm-exe/command/include/command_launch.h"
#include "compat/include/guest_location.h"
#include "compat/include/mvdm_vm_capture.h"
#include "compat/include/mvdm_vm_text.h"
#include "ntvdm-exe/session/session.h"
#include "../../common/guest_memory_fixture.h"

const mvdm_cpu_access *mvdm_cpu_access_current(void)
{
    static mvdm_cpu_access provider;
    provider.acquire_memory = mvdm_softpc_guest_memory_acquire;
    provider.release_memory = mvdm_softpc_guest_memory_release;
    return &provider;
}

static BYTE memory[65536];
static BYTE vm_memory[65536];
static guest_memory_lease lease;
static unsigned checks,leases;
static int fail_acquire,fail_release;
static session owner;
#define CHECK(x) do {++checks;if(!(x)){fprintf(stderr,"line %u: %s\n",__LINE__,#x);return 1;}} while(0)
session *__cdecl session_thread_current(void){return &owner;}
int session_valid(const session *s){return s==&owner;}
int session_guest_memory_acquire(session *s,uint32_t address,uint32_t n,
    uint32_t access,guest_memory_lease **out,uint8_t **bytes)
{
    BYTE *source=memory;
    (void)s;(void)access;
    if(address>=0x80000000u){address-=0x80000000u;source=vm_memory;}
    if(fail_acquire || address>sizeof(memory) || n>sizeof(memory)-address)return 0;
    ++leases;*bytes=source+address;*out=&lease;return 1;
}
int session_guest_memory_release(session *s,guest_memory_lease *p,int commit)
{(void)s;(void)p;(void)commit;--leases;return !fail_release;}

static int read_memory(void *context, uint32_t address, uint8_t *bytes, uint32_t count)
{
    uint8_t *source = context;
    if (address >= 0x80000000u) {
        address -= 0x80000000u;
        source = vm_memory;
    }
    if (fail_acquire || address > sizeof(memory) || count > sizeof(memory) - address)
        return 0;
    memcpy(bytes, source + address, count);
    return 1;
}

static int write_memory(void *context, uint32_t address, const uint8_t *bytes, uint32_t count)
{
    uint8_t *destination = context;
    if (address >= 0x80000000u) {
        address -= 0x80000000u;
        destination = vm_memory;
    }
    if (fail_release || address > sizeof(memory) || count > sizeof(memory) - address)
        return 0;
    memcpy(destination + address, bytes, count);
    return 1;
}

static int no_active_leases(void)
{
    unsigned i;
    if (leases) return 0;
    for (i = 0; i < GUEST_MEMORY_LEASE_MAXIMUM; ++i)
        if (owner.guest_memory_lease.leases[i].active) return 0;
    return 1;
}

static unsigned directory_clears;
static uint16_t directory_segment, directory_offset;
void mvdm_command_guest_state_clear_doswow(void)
{
    ++directory_clears;
}
int mvdm_command_guest_state_register_doswow(uint16_t segment, uint16_t offset)
{
    directory_segment = segment;
    directory_offset = offset;
    return 1;
}
#include "compat/include/command_native_child.h"
#include "compat/include/command_guest_directory.h"

BOOL cmdXformEnvironment(char *source, struct _STRING *output)
{
    (void)source; (void)output;
    abort(); /* Capture must not convert the environment. */
}

int dem_command_directory_instance(struct guest_memory_lease_context *lease_context,
    uint32_t epoch, uint16_t segment, uint16_t offset, uint32_t *linear, uint32_t *bytes)
{
    (void)lease_context; (void)epoch; (void)segment; (void)offset; (void)linear; (void)bytes;
    abort(); /* Capture never resolves the DOS-owned directory instance. */
}

int main(void)
{
    mvdm_command_capture_input input={0};
    mvdm_command_capture *a=NULL,*b=NULL,*failed=NULL;
    mvdm_command_capture_view av,bv;
    uint32_t frame[18]={0};
    char comspec[MAX_PATH];
    DWORD comspec_length;
    ULONG handles[3]={10,20,30};
    const char *ordinary_command,*ordinary_environment;
    CHECK(!compat_command_install(NULL) && GetLastError()==ERROR_INVALID_PARAMETER);
    CHECK(ntvdm_command_initialize() && ntvdm_command_initialize());
    CHECK(compat_command_current_services()->transform_environment == cmdXformEnvironment);
    CHECK(compat_command_current_services()->acquire_vm == mvdm_guest_location_acquire_vm);
    CHECK(compat_command_current_services()->release_location == mvdm_guest_location_release);
    CHECK(compat_command_current_services()->directory_instance == dem_command_directory_instance);
    {
        compat_command_services incomplete = *compat_command_current_services();
        incomplete.acquire_vm = NULL;
        CHECK(!compat_command_install(&incomplete) && GetLastError() == ERROR_INVALID_PARAMETER);
        incomplete = *compat_command_current_services();
        incomplete.release_location = NULL;
        CHECK(!compat_command_install(&incomplete) && GetLastError() == ERROR_INVALID_PARAMETER);
        incomplete = *compat_command_current_services();
        incomplete.status_to_dos_error = NULL;
        CHECK(!compat_command_install(&incomplete) && GetLastError() == ERROR_INVALID_PARAMETER);
    }
    mvdm_command_guest_state_clear_doswow();
    CHECK(directory_clears == 1);
    CHECK(mvdm_command_guest_state_register_doswow(0x1234, 0x5678) &&
        directory_segment == 0x1234 && directory_offset == 0x5678);
    owner.host_context.state=SESSION_STATE_ACTIVE;owner.host_context.epoch=1;
    owner.host_context.guest_memory = &owner.guest_memory_lease;
    CHECK(fixture_lease_memory_begin(&owner.guest_memory_lease, memory, read_memory, write_memory));
    CHECK(compat_host_context_bind(&owner.host_context));
    memcpy(memory+100,"ordinary\r",9);memcpy(memory+400,"A=ordinary\0",12);
    memcpy(memory+800,handles,sizeof(handles));
    CHECK(mvdm_command_native_child_capture_guest(0,100,0,400,0,800));
    {
        ULONG captured_handles[3];
        const char *tail=NULL;
        CHECK(mvdm_command_native_child_std_handles(captured_handles));
        CHECK(!memcmp(captured_handles,handles,sizeof(handles)));
        CHECK(mvdm_command_native_child_select_command_service(MVDM_COMMAND_SERVICE_EXECUTE));
        CHECK(mvdm_command_native_child_command_service(&tail)==MVDM_COMMAND_SERVICE_EXECUTE);
        CHECK(tail && !strcmp(tail,"ordinary"));
        CHECK(mvdm_command_native_child_select_command_service(MVDM_COMMAND_SERVICE_INTERACTIVE));
        CHECK(mvdm_command_native_child_command_service(&tail)==MVDM_COMMAND_SERVICE_INTERACTIVE);
    }
    input.command_offset=100;input.environment_offset=400;input.standard_offset=800;
    input.default_drive=2;input.service=MVDM_COMMAND_SERVICE_EXECUTE;
    memcpy(memory+100,"first\r",6);memcpy(memory+400,"A=first\0",9);
    CHECK(mvdm_command_capture_create(&input,&a));CHECK(!memory[105]);CHECK(no_active_leases());
    input.host_command="second";input.default_drive=3;input.service=MVDM_COMMAND_SERVICE_INTERACTIVE;
    memcpy(memory+400,"A=other\0",9);handles[1]=99;memcpy(memory+800,handles,sizeof(handles));
    CHECK(mvdm_command_capture_create(&input,&b));CHECK(no_active_leases());
    memset(memory,0xcc,sizeof(memory));
    CHECK(mvdm_command_capture_read(a,&av));CHECK(mvdm_command_capture_read(b,&bv));
    CHECK(!strcmp(av.command,"first") && !strcmp(bv.command,"second"));
    CHECK(!strcmp(av.environment,"A=first") && !strcmp(bv.environment,"A=other"));
    CHECK(av.environment_bytes==9 && bv.environment_bytes==9);
    CHECK(av.standard_handles[1]==20 && bv.standard_handles[1]==99);
    CHECK(av.default_drive==2 && bv.default_drive==3);
    CHECK(av.service==MVDM_COMMAND_SERVICE_EXECUTE && bv.service==MVDM_COMMAND_SERVICE_INTERACTIVE);
    ordinary_command=mvdm_command_native_child_command();
    ordinary_environment=mvdm_command_native_child_environment();
    CHECK(ordinary_command && ordinary_environment);
    CHECK(!strcmp(ordinary_command,"ordinary") && !strcmp(ordinary_environment,"A=ordinary"));
    mvdm_command_capture_destroy(b);
    CHECK(!strcmp(av.command,"first"));
    input.host_command=NULL;input.vm_linear_base=0x80000000u;
    memcpy(vm_memory+100,"alias\r",6);memcpy(vm_memory+400,"VM=alias\0",10);
    memcpy(vm_memory+800,handles,sizeof(handles));
    CHECK(mvdm_command_capture_create(&input,&b));CHECK(no_active_leases());
    CHECK(!vm_memory[105] && memory[105]==0xcc);
    CHECK(mvdm_command_capture_read(b,&bv));
    CHECK(!strcmp(bv.command,"alias") && !strcmp(bv.environment,"VM=alias"));
    CHECK(bv.standard_handles[1]==99);
    mvdm_command_capture_destroy(b);
    input.vm_linear_base=UINT32_MAX-50;
    CHECK(!mvdm_command_capture_create(&input,&failed));CHECK(!failed && no_active_leases());
    input.vm_linear_base=0;
    input.host_command=NULL;input.command_offset=65535;
    CHECK(!mvdm_command_capture_create(&input,&failed));CHECK(!failed && no_active_leases());
    input.command_offset=100;memcpy(memory+100,"x\r",2);
    input.environment_offset=65535;
    CHECK(!mvdm_command_capture_create(&input,&failed));CHECK(!failed && no_active_leases());
    memory[101]='\r';
    input.environment_offset=400;memcpy(memory+400,"\0",2);
    input.standard_offset=65530;
    CHECK(!mvdm_command_capture_create(&input,&failed));CHECK(!failed && no_active_leases());
    input.standard_offset=800;fail_acquire=1;
    CHECK(!mvdm_command_capture_create(&input,&failed));CHECK(!failed && no_active_leases());
    fail_acquire=0;fail_release=1;
    CHECK(!mvdm_command_capture_create(&input,&failed));CHECK(!failed && no_active_leases());
    fail_release=0;input.default_drive=26;
    CHECK(!mvdm_command_capture_create(&input,&failed));CHECK(!failed);
    /* Actual VMM layout and original 54:08 AH / 54:0A routing. Deliberately
     * keep CS and DS distinct: command input belongs to DS:SI, not code. */
    frame[1]=100;frame[2]=800;frame[7]=2;frame[10]=0x1234;
    memset(memory+1400,0,64);memcpy(memory+1400,"VM\\A",5);
    memcpy(vm_memory+100,"vmcmd\r",6);memcpy(vm_memory+400,"VM=frame\0",10);
    memcpy(vm_memory+800,handles,sizeof(handles));frame[14]=25;
    memcpy(memory+1200,frame,sizeof(frame));
    vm_memory[105] = '\r'; /* New request: prior capture consumed the CR. */
    CHECK(!mvdm_vm_command_capture(8,1200,0x80000000u,1400,&b));
    CHECK(mvdm_command_capture_read(b,&bv));
    CHECK(!strcmp(bv.command,"vmcmd") && !strcmp(bv.environment,"VM=frame"));
    CHECK(bv.service==MVDM_COMMAND_SERVICE_NONE && bv.default_drive==2);
    CHECK(!strcmp(bv.directory,"C:\\VM\\A"));
    mvdm_command_capture_destroy(b);
    /* The saved-VM caller must preserve syntax failure, not relabel it as
     * an invalid address. The live caller observes the same capture error. */
    memset(vm_memory + 100, 'x', 128);
    CHECK(mvdm_vm_command_capture(8,1200,0x80000000u,1400,&failed)==ERROR_BAD_FORMAT);
    CHECK(!failed && no_active_leases());
    memcpy(vm_memory + 100, "vmcmd\r", 6);
    frame[7]=0x102;memcpy(memory+1200,frame,sizeof(frame));
    vm_memory[105] = '\r'; /* New request: prior capture consumed the CR. */
    CHECK(!mvdm_vm_command_capture(8,1200,0x80000000u,1400,&b));
    CHECK(mvdm_command_capture_read(b,&bv));
    CHECK(bv.service==MVDM_COMMAND_SERVICE_EXECUTE && !strcmp(bv.command,"vmcmd"));
    mvdm_command_capture_destroy(b);
    comspec_length=GetEnvironmentVariableA("COMSPEC",comspec,sizeof(comspec));
    CHECK(comspec_length && comspec_length<sizeof(comspec));
    {
        struct { char command[MAX_PATH]; char guard; } prepared;
        char tail[MAX_PATH];
        const char *shell="C:\\host\\shell.exe";
        size_t fits=MAX_PATH-strlen(shell)-5;
        CHECK(SetEnvironmentVariableA("COMSPEC",shell));
        prepared.guard=0x5a;
        CHECK(!mvdm_command_prepare_comspec(prepared.command,NULL));
        CHECK(!strcmp(prepared.command,shell));
        CHECK(!mvdm_command_prepare_comspec(prepared.command,"echo x"));
        CHECK(!strcmp(prepared.command,"C:\\host\\shell.exe /c echo x"));
        memset(tail,'x',sizeof(tail));tail[fits]=0;
        CHECK(!mvdm_command_prepare_comspec(prepared.command,tail));
        CHECK(strlen(prepared.command)==MAX_PATH-1 && prepared.guard==0x5a);
        tail[fits]='x';tail[fits+1]=0;
        CHECK(mvdm_command_prepare_comspec(prepared.command,tail)==ERROR_BAD_ENVIRONMENT);
        CHECK(prepared.guard==0x5a);
        CHECK(SetEnvironmentVariableA("COMSPEC",comspec));
    }
    vm_memory[105] = '\r'; /* New request: prior capture consumed the CR. */
    CHECK(!mvdm_vm_command_capture(10,1200,0x80000000u,1400,&b));
    CHECK(mvdm_command_capture_read(b,&bv));
    CHECK(bv.service==MVDM_COMMAND_SERVICE_INTERACTIVE && !strcmp(bv.command,comspec));
    mvdm_command_capture_destroy(b);
    CHECK(SetEnvironmentVariableA("COMSPEC",NULL));
    vm_memory[105] = '\r'; /* New request: prior capture consumed the CR. */
    CHECK(mvdm_vm_command_capture(10,1200,0x80000000u,1400,&failed)==ERROR_BAD_ENVIRONMENT);
    CHECK(!failed && no_active_leases());
    CHECK(SetEnvironmentVariableA("COMSPEC",comspec));
    CHECK(SetEnvironmentVariableA("COMSPEC",NULL));
    vm_memory[105] = '\r'; /* New request: prior capture consumed the CR. */
    CHECK(mvdm_vm_command_capture(8,1200,0x80000000u,1400,&failed)==ERROR_BAD_ENVIRONMENT);
    CHECK(!failed && no_active_leases());
    frame[7]=2;memcpy(memory+1200,frame,sizeof(frame));
    vm_memory[105] = '\r'; /* New request: prior capture consumed the CR. */
    CHECK(!mvdm_vm_command_capture(8,1200,0x80000000u,1400,&b));
    mvdm_command_capture_destroy(b);
    CHECK(SetEnvironmentVariableA("COMSPEC",comspec));
    vm_memory[105] = '\r'; /* New request: prior capture consumed the CR. */
    CHECK(mvdm_vm_command_capture(9,1200,0x80000000u,1400,&failed)==ERROR_INVALID_FUNCTION);
    CHECK(!failed && no_active_leases());
    vm_memory[105] = '\r'; /* New request: prior capture consumed the CR. */
    CHECK(mvdm_vm_command_capture(8,65530,0x80000000u,1400,&failed)==ERROR_INVALID_ADDRESS);
    CHECK(!failed && no_active_leases());
    vm_memory[105] = '\r'; /* New request: prior capture consumed the CR. */
    CHECK(mvdm_vm_command_capture(8,1200,0x80000000u,65530,&failed)==ERROR_INVALID_ADDRESS);
    CHECK(!failed && no_active_leases());
    memset(memory+1400,'X',64);
    vm_memory[105] = '\r'; /* New request: prior capture consumed the CR. */
    CHECK(mvdm_vm_command_capture(8,1200,0x80000000u,1400,&failed)==ERROR_BAD_PATHNAME);
    CHECK(!failed && no_active_leases());
    memory[1400]=0;
    vm_memory[105] = '\r'; /* New request: prior capture consumed the CR. */
    CHECK(!mvdm_vm_command_capture(8,1200,0x80000000u,1400,&b));
    CHECK(mvdm_command_capture_read(b,&bv));
    CHECK(!strcmp(bv.directory,"C:\\"));
    memory[1400]='X';
    CHECK(!strcmp(bv.directory,"C:\\"));
    mvdm_command_capture_destroy(b);

    {
        struct { ntvdm_vm_text_header h; BYTE cells[4]; } screen = {
            {2, 1, 0, 0, 0x0607, 3, 0, 437, 16}, {'A', 7, 'B', 7}};
        uint32_t descriptor[5] = {1200, 0x80000000u, 1400, 2000, sizeof(screen)};
        ntvdm_vm_text *text = NULL;
        console_io_request read = {0}; console_io_reply reply = {0};
        console_io_cell cell;
        unsigned scenario;
        memory[1400] = 0;
        memcpy(memory + 1600, descriptor, sizeof(descriptor));
        memcpy(memory + 2000, &screen, sizeof(screen));
        {
            uint32_t service, client[18];
            memcpy(client, memory + 1200, sizeof(client));
            client[9] = 1; client[10] = 32;
            memcpy(memory + 1200, client, sizeof(client));
            vm_memory[512] = 8; memory[512] = 9;
            CHECK(!mvdm_vm_command_service(1600, 20, &service) && service == 8 && no_active_leases());
            client[9] = client[10] = 0;
            memcpy(memory + 1200, client, sizeof(client));
            vm_memory[65535] = 10;
            CHECK(!mvdm_vm_command_service(1600, 20, &service) && service == 10 && no_active_leases());
            vm_memory[65535] = 9;
            CHECK(mvdm_vm_command_service(1600, 20, &service) == ERROR_INVALID_FUNCTION && no_active_leases());
            CHECK(mvdm_vm_command_service(1600, 19, &service) == ERROR_INVALID_DATA && no_active_leases());
            fail_acquire = 1;
            CHECK(mvdm_vm_command_service(1600, 20, &service) == ERROR_INVALID_ADDRESS && no_active_leases());
            fail_acquire = 0;
        }
        vm_memory[105] = '\r';
        /* Snapshot must borrow the active inner context, never an F session. */
        CHECK(compat_host_context_unbind(&owner.host_context));
        CHECK(mvdm_vm_command_snapshot(8, 1600, sizeof(descriptor), &b, &text) == ERROR_INVALID_ADDRESS);
        CHECK(!b && !text && no_active_leases());
        CHECK(compat_host_context_bind(&owner.host_context));
        fail_acquire = 1;
        CHECK(mvdm_vm_command_snapshot(8, 1600, sizeof(descriptor), &b, &text) == ERROR_INVALID_ADDRESS);
        CHECK(!b && !text && no_active_leases());
        fail_acquire = 0;
        CHECK(!mvdm_vm_command_snapshot(8, 1600, sizeof(descriptor), &b, &text));
        CHECK(b && text && no_active_leases());
        memset(memory + 2000, 0xff, sizeof(screen));
        read.operation = CONSOLE_IO_READ_CELLS_W;
        read.state.width = read.state.height = 1; reply.data = (BYTE *)&cell;
        CHECK(!ntvdm_vm_text_dispatch(text, &read, &reply));
        CHECK(cell.character == 'A' && cell.attribute == 7);
        mvdm_command_capture_destroy(b); ntvdm_vm_text_destroy(text);
        for (scenario = 0; scenario < 6; ++scenario) {
            DWORD error;
            memcpy(memory + 2000, &screen, sizeof(screen));
            descriptor[0] = 1200; descriptor[2] = 1400;
            descriptor[3] = 2000; descriptor[4] = sizeof(screen);
            if (scenario == 0) descriptor[4] = 1;
            if (scenario == 1) descriptor[4]++;
            if (scenario == 2) descriptor[3] = 65530;
            if (scenario == 3) descriptor[0] = 65530;
            if (scenario == 4) descriptor[2] = 65530;
            memcpy(memory + 1600, descriptor, sizeof(descriptor));
            vm_memory[105] = '\r'; /* New request: prior capture consumed the CR. */
            error = mvdm_vm_command_snapshot(8, 1600,
                scenario == 5 ? 19 : sizeof(descriptor), &b, &text);
            CHECK(error == (DWORD)(scenario >= 2 && scenario <= 4 ?
                ERROR_INVALID_ADDRESS : ERROR_INVALID_DATA));
            CHECK(!b && !text && no_active_leases());
        }
        CHECK(mvdm_vm_command_snapshot(9, 1600, 20, &b, &text) == ERROR_INVALID_FUNCTION);
        CHECK(!b && !text && no_active_leases());
        CHECK(mvdm_vm_command_snapshot(8, 65530, 20, &b, &text) == ERROR_INVALID_ADDRESS);
        CHECK(!b && !text && no_active_leases());
    }
    mvdm_command_capture_destroy(a);mvdm_command_native_child_abort();

    {
        static const unsigned endings[] = {0, 123, 124, 126};
        static const unsigned lengths[] = {134, MAX_PATH - 1, MAX_PATH};
        unsigned i, length;
        char command[MAX_PATH + 1];
        mvdm_command_capture_input boundary = {0};
        boundary.command_offset = 100;
        boundary.environment_offset = 400;
        boundary.standard_offset = 800;
        memset(memory + 400, 0, 2);
        memcpy(memory + 800, handles, sizeof(handles));
        for (i = 0; i < sizeof(endings)/sizeof(endings[0]); ++i) {
            memset(memory + 100, 'x', 128);
            memory[100 + endings[i]] = '\r';
            if (endings[i] < 124) {
                CHECK(mvdm_command_capture_create(&boundary, &b));
                CHECK(!memory[100 + endings[i]] && no_active_leases());
                mvdm_command_capture_destroy(b);
            } else {
                CHECK(!mvdm_command_capture_create(&boundary, &failed));
                CHECK(GetLastError() == ERROR_BAD_FORMAT && !failed && no_active_leases());
                CHECK(memory[100 + endings[i]] == '\r');
            }
        }
        memset(memory + 100, 0, 128);
        CHECK(!mvdm_command_capture_create(&boundary, &failed));
        CHECK(GetLastError() == ERROR_BAD_FORMAT && !failed && no_active_leases());
        memory[110] = '\r'; /* Original scans past embedded NUL to find CR. */
        CHECK(mvdm_command_capture_create(&boundary, &b));
        CHECK(!memory[110] && no_active_leases());
        mvdm_command_capture_destroy(b);
        fail_acquire = 1;
        CHECK(!mvdm_command_capture_create(&boundary, &failed));
        CHECK(GetLastError() == ERROR_INVALID_ADDRESS && !failed && no_active_leases());
        fail_acquire = 0;
        boundary.host_command = command;
        for (i = 0; i < sizeof(lengths)/sizeof(lengths[0]); ++i) {
            length = lengths[i];
            memset(command, 'x', length); command[length] = 0;
            if (length < MAX_PATH) {
                CHECK(mvdm_command_capture_create(&boundary, &b));
                CHECK(mvdm_command_capture_read(b, &bv) && strlen(bv.command) == length);
                mvdm_command_capture_destroy(b);
                CHECK(mvdm_command_native_child_capture_host_command(command,0,400,0,800));
                CHECK(mvdm_command_native_child_replace_command(command));
                CHECK(strlen(mvdm_command_native_child_command()) == length);
                mvdm_command_native_child_finish();
            } else {
                CHECK(!mvdm_command_capture_create(&boundary, &failed) && !failed);
                CHECK(!mvdm_command_native_child_capture_host_command(command,0,400,0,800));
                CHECK(mvdm_command_native_child_capture_host_command("seed",0,400,0,800));
                CHECK(!mvdm_command_native_child_replace_command(command));
                CHECK(!strcmp(mvdm_command_native_child_command(), "seed"));
                mvdm_command_native_child_finish();
            }
        }
    }

    CHECK(!mvdm_command_native_child_command());
    guest_memory_lease_end(&owner.guest_memory_lease);
    CHECK(compat_host_context_unbind(&owner.host_context));
    compat_host_context_dispose(&owner.host_context);CHECK(!owner.host_context.mvdm_command_native_child);
    printf("PASS %u independent command capture assertions\n",checks);return 0;
}
