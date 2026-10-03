#include "local_binding.h"

RPC_STATUS common_rpc_bind_local(const WCHAR *endpoint,RPC_BINDING_HANDLE *binding)
{
    RPC_WSTR text=NULL;
    RPC_BINDING_HANDLE local=NULL;
    RPC_STATUS status;
    if(!binding)return RPC_S_INVALID_ARG;
    *binding=NULL;
    if(!endpoint || !*endpoint)return RPC_S_INVALID_ARG;
    status=RpcStringBindingComposeW(NULL,(RPC_WSTR)L"ncalrpc",NULL,
        (RPC_WSTR)endpoint,NULL,&text);
    if(status!=RPC_S_OK)goto done;
    status=RpcBindingFromStringBindingW(text,&local);
    if(status!=RPC_S_OK)goto done;
    status=RpcBindingSetAuthInfoW(local,NULL,RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
        RPC_C_AUTHN_WINNT,NULL,RPC_C_AUTHZ_NONE);
    if(status==RPC_S_OK){*binding=local;local=NULL;}
done:
    if(local)RpcBindingFree(&local);
    if(text)RpcStringFreeW(&text);
    return status;
}
