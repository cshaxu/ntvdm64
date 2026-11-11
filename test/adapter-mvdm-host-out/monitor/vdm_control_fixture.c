#include <stdio.h>

#include "session.h"
#include "vdm_control.h"

static int callback_count;

static NTSTATUS test_query_dir(void *context, PVDMQUERYDIRINFO query)
{
    (void)context;
    if (query == NULL || query->FileIndex != 17u) return STATUS_INVALID_PARAMETER;
    ++callback_count;
    return STATUS_SUCCESS;
}

int main(void)
{
    session instance;
    VDMQUERYDIRINFO query = { 0 };

    query.FileIndex = 17u;
    if (NtVdmControl(VdmQueryDir, &query) != STATUS_NOT_IMPLEMENTED) return 1;
    session_initialize(&instance, 1u);
    if (!session_activate(&instance) || !session_thread_bind(&instance)) return 2;
    if (NtVdmControl(VdmQueryDir, &query) != STATUS_NOT_IMPLEMENTED) return 3;
    if (!adapter_vdm_monitor_bind_query_dir(&instance, test_query_dir, NULL)) return 4;
    if (NtVdmControl(VdmStartExecution, &query) != STATUS_NOT_IMPLEMENTED) return 5;
    if (NtVdmControl(VdmQueryDir, &query) != STATUS_SUCCESS || callback_count != 1) return 6;
    if (!adapter_vdm_monitor_unbind_query_dir(&instance) ||
        !session_thread_unbind(&instance) || !session_dispose(&instance)) return 7;
    puts("PASS: VdmQueryDir adapter contract");
    return 0;
}
