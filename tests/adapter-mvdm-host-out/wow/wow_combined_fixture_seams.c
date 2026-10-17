#include "common/rpc/local_binding.h"

/* Combined CCPU/WOW fixtures retain the production BaseClient functions.
 * Their one external service boundary is forbidden: an unexpected attempt
 * to connect must fail the process, never contact the user's BaseSrv.
 * This definition prevents extraction of local_binding.obj from common-rpc,
 * not selection of a second BaseClient implementation. */
RPC_STATUS common_rpc_bind_local(const WCHAR *endpoint,
    RPC_BINDING_HANDLE *binding)
{
    (void)endpoint;
    (void)binding;
    ExitProcess(ERROR_CALL_NOT_IMPLEMENTED);
    return RPC_S_CANNOT_SUPPORT;
}

/* Same absent-configuration input as the CPU-only host fixture. */
char *__cdecl mvdm_host_getenv(const char *name)
{
    (void)name;
    return NULL;
}
