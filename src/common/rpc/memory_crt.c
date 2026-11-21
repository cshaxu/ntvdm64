/* MIDL transient buffers owned by the CRT of the containing product image. */
#include <stdlib.h>
#include <rpc.h>

void *__RPC_USER MIDL_user_allocate(size_t bytes) { return malloc(bytes); }
void __RPC_USER MIDL_user_free(void *value) { free(value); }
