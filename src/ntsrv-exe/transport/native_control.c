#include "native_control.h"

DWORD broker_native_reply_status(const native_request_reply *reply,BOOL launch)
{
    if(!reply)return ERROR_INVALID_PARAMETER;
    if(reply->version!=NATIVE_REQUEST_VERSION || reply->reserved ||
        reply->target>(uint64_t)(ULONG_PTR)-1 || reply->receipt>(uint64_t)(ULONG_PTR)-1 ||
        (reply->error && (reply->target || reply->receipt || reply->request)) ||
        (!launch && (reply->target || reply->receipt || reply->request)) ||
        (launch && !reply->error && (!reply->target || !reply->receipt || !reply->request)))
        return ERROR_INVALID_DATA;
    return reply->error;
}

DWORD broker_native_completion_status(const native_request_completion *reply)
{
    if(!reply)return ERROR_INVALID_PARAMETER;
    return reply->version==NATIVE_REQUEST_VERSION &&
        !(reply->flags & ~NATIVE_COMPLETION_CONSOLE_EMPTY) ? reply->error : ERROR_INVALID_DATA;
}
