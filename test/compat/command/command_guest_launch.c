#include "compat/include/mvdm_command_launch.h"
#include "compat/include/mvdm_command_text.h"
#include "compat/include/command_services.h"
#include "compat/include/mvdm_command_native_child.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned checks, transforms, prepares;
static BOOL fail_transform, bad_transform;
static DWORD prepare_error;
static char expected_command[32], expected_directory[32];
static HANDLE expected_streams[3];
#define CHECK(x) do { ++checks; if (!(x)) { fprintf(stderr,"line %u: %s\n",__LINE__,#x); exit(1); } } while (0)

/* This unit checks the composition boundary, not a replacement environment
 * algorithm. Original cmdXformEnvironment is linked by the product. RTL OEM
 * conversions below are real; only environment/owned-launch delegates vary. */
BOOL cmdXformEnvironment(PCHAR source, PANSI_STRING output)
{
    static const WCHAR block[] = L"A=1\0COMSPEC=host\0";
    UNICODE_STRING unicode;
    ++transforms;
    CHECK(!memcmp(source,"A=1\0",5));
    if (fail_transform) return FALSE;
    unicode.Buffer = (PWSTR)block;
    unicode.Length = sizeof(block);
    unicode.MaximumLength = sizeof(block);
    CHECK(NT_SUCCESS(RtlUnicodeStringToAnsiString(output,&unicode,TRUE)));
    if (bad_transform) memset(output->Buffer,'X',output->Length);
    return TRUE;
}

const compat_command_services *compat_command_current_services(void)
{
    static compat_command_services services;
    services.transform_environment = cmdXformEnvironment;
    return &services;
}

DWORD ntvdm_command_launch_prepare(unsigned int service, const char *command,
    const char *environment, size_t bytes, const char *directory,
    const HANDLE streams[3], ntvdm_command_launch **output)
{
    ++prepares;
    CHECK(service == MVDM_COMMAND_SERVICE_EXECUTE);
    CHECK(!strcmp(command,expected_command));
    CHECK(!strcmp(directory,expected_directory));
    CHECK(bytes == sizeof("A=1\0COMSPEC=host\0"));
    CHECK(!memcmp(environment,"A=1\0COMSPEC=host\0",bytes));
    CHECK(!memcmp(streams,expected_streams,sizeof(expected_streams)));
    if (!prepare_error) *output = (ntvdm_command_launch *)(ULONG_PTR)1;
    return prepare_error;
}

int main(void)
{
    const HANDLE defaults[3] = {(HANDLE)11,(HANDLE)12,(HANDLE)13};
    mvdm_command_capture_view view = {0};
    ntvdm_command_launch *result = NULL;
    char command[] = "echo \x82", directory[] = "C:\\\x82";
    unsigned before;
    ANSI_STRING prepared_command = {0}, prepared_environment = {0};
    /* This composition fixture links the native converter, not the product's
     * original OpenNT status table. Exercise that binding explicitly. */
    CHECK(RtlNtStatusToDosError((NTSTATUS)0xc0000022L)==ERROR_ACCESS_DENIED);
    CHECK(OemToCharA(command,expected_command));
    CHECK(OemToCharA(directory,expected_directory));
    before=transforms;
    CHECK(!ntvdm_command_prepare_text(command,NULL,&prepared_command,&prepared_environment));
    CHECK(!strcmp(prepared_command.Buffer,expected_command));
    CHECK(!prepared_environment.Buffer && transforms==before);
    CHECK(!strcmp(command,"echo \x82"));
    RtlFreeAnsiString(&prepared_command);
    fail_transform=TRUE;
    CHECK(ntvdm_command_prepare_text(command,"A=1\0",&prepared_command,
        &prepared_environment)==ERROR_NOT_ENOUGH_MEMORY);
    CHECK(!prepared_command.Buffer && !prepared_environment.Buffer);
    CHECK(!prepared_command.Length && !prepared_environment.Length);
    fail_transform=FALSE;
    view.command=command;view.directory=directory;view.environment="A=1\0";
    view.environment_bytes=5;view.service=MVDM_COMMAND_SERVICE_EXECUTE;
    view.standard_handles[0]=UINT32_MAX;
    view.standard_handles[1]=22;view.standard_handles[2]=UINT32_MAX;
    expected_streams[0]=defaults[0];expected_streams[1]=(HANDLE)22;expected_streams[2]=defaults[2];
    CHECK(!ntvdm_command_launch_prepare_guest(&view,defaults,&result));
    CHECK(result==(ntvdm_command_launch *)(ULONG_PTR)1);
    CHECK(!strcmp(command,"echo \x82") && !strcmp(directory,"C:\\\x82"));
    prepare_error=ERROR_INVALID_HANDLE;
    CHECK(ntvdm_command_launch_prepare_guest(&view,defaults,&result)==prepare_error);
    CHECK(!result);
    prepare_error=0;before=prepares;fail_transform=TRUE;
    CHECK(ntvdm_command_launch_prepare_guest(&view,defaults,&result)==ERROR_NOT_ENOUGH_MEMORY);
    CHECK(!result && prepares==before);
    fail_transform=FALSE;bad_transform=TRUE;
    CHECK(ntvdm_command_launch_prepare_guest(&view,defaults,&result)==ERROR_INVALID_DATA);
    CHECK(!result && prepares==before);
    bad_transform=FALSE;before=transforms;view.environment_bytes=4;
    CHECK(ntvdm_command_launch_prepare_guest(&view,defaults,&result)==ERROR_INVALID_DATA);
    CHECK(!result && transforms==before);
    CHECK(ntvdm_command_launch_prepare_guest(NULL,defaults,&result)==ERROR_INVALID_PARAMETER);
    CHECK(!result);
    printf("PASS %u guest launch composition assertions\n",checks);
    return 0;
}
