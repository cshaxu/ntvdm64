#include <stdio.h>

#include "session.h"
#include "vdm.h"

static int callback_count;

static int32_t test_dispatch(void *context, uint32_t operation, void *request)
{
    VDMQUERYDIRINFO *query = (VDMQUERYDIRINFO *)request;
    (void)context;
    if (operation != (uint32_t)VdmQueryDir || query == NULL ||
        query->FileIndex != 17u) return (int32_t)STATUS_INVALID_PARAMETER;
    ++callback_count;
    return (int32_t)STATUS_SUCCESS;
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
    if (!session_set_control_dispatch(&instance, test_dispatch, NULL)) return 4;
    if (NtVdmControl(VdmStartExecution, &query) != STATUS_NOT_IMPLEMENTED) return 5;
    if (NtVdmControl(VdmQueryDir, &query) != STATUS_SUCCESS || callback_count != 1) return 6;
    if (!session_thread_unbind(&instance) || !session_dispose(&instance)) return 7;
    puts("PASS: VdmQueryDir adapter contract");
    return 0;
}
