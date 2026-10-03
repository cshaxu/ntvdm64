#include "common/rpc/local_binding.h"
#include <stdio.h>

static unsigned checks,failures,step,string_live,binding_live,failing_step;
static RPC_STATUS injected;
#define CHECK(x) do {++checks;if(!(x)){++failures;printf("FAIL %u %s\n",__LINE__,#x);}} while(0)
static RPC_STATUS compose(RPC_WSTR uuid,RPC_WSTR protocol,RPC_WSTR address,
    RPC_WSTR endpoint,RPC_WSTR options,RPC_WSTR *text)
{
    CHECK(!uuid && !address && !options);
    CHECK(!wcscmp((WCHAR *)protocol,L"ncalrpc") && !wcscmp((WCHAR *)endpoint,L"test-endpoint"));
    CHECK(++step==1);
    if(failing_step==step)return injected;
    *text=(RPC_WSTR)L"test-string";++string_live;return RPC_S_OK;
}
static RPC_STATUS from_string(RPC_WSTR text,RPC_BINDING_HANDLE *binding)
{
    CHECK(string_live==1 && !wcscmp((WCHAR *)text,L"test-string"));
    CHECK(++step==2);
    if(failing_step==step)return injected;
    *binding=(RPC_BINDING_HANDLE)(ULONG_PTR)123;++binding_live;return RPC_S_OK;
}
static RPC_STATUS authenticate(RPC_BINDING_HANDLE binding,RPC_WSTR principal,
    ULONG level,ULONG service,RPC_AUTH_IDENTITY_HANDLE identity,ULONG authorization)
{
    CHECK(binding_live==1 && binding==(RPC_BINDING_HANDLE)(ULONG_PTR)123);
    CHECK(!principal && !identity && level==RPC_C_AUTHN_LEVEL_PKT_PRIVACY &&
        service==RPC_C_AUTHN_WINNT && authorization==RPC_C_AUTHZ_NONE);
    CHECK(++step==3);
    return failing_step==step ? injected : RPC_S_OK;
}
static RPC_STATUS free_binding(RPC_BINDING_HANDLE *binding)
{
    CHECK(binding_live==1 && *binding==(RPC_BINDING_HANDLE)(ULONG_PTR)123);
    --binding_live;*binding=NULL;return RPC_S_OK;
}
static RPC_STATUS free_string(RPC_WSTR *text)
{
    CHECK(string_live==1 && *text);--string_live;*text=NULL;return RPC_S_OK;
}
#define RpcStringBindingComposeW compose
#define RpcBindingFromStringBindingW from_string
#define RpcBindingSetAuthInfoW authenticate
#define RpcBindingFree free_binding
#define RpcStringFreeW free_string
#include "common/rpc/local_binding.c"

int main(void)
{
    RPC_BINDING_HANDLE binding=(RPC_BINDING_HANDLE)(ULONG_PTR)99;
    unsigned i;
    CHECK(common_rpc_bind_local(L"test-endpoint",NULL)==RPC_S_INVALID_ARG);
    CHECK(common_rpc_bind_local(NULL,&binding)==RPC_S_INVALID_ARG && !binding);
    CHECK(common_rpc_bind_local(L"",&binding)==RPC_S_INVALID_ARG && !binding);
    CHECK(!step && !binding_live && !string_live);
    for(i=1;i<=3;++i) {
        failing_step=i;injected=RPC_S_ACCESS_DENIED+i;step=0;
        binding=(RPC_BINDING_HANDLE)(ULONG_PTR)99;
        CHECK(common_rpc_bind_local(L"test-endpoint",&binding)==injected);
        CHECK(!binding && !binding_live && !string_live && step==i);
    }
    failing_step=0;step=0;
    CHECK(common_rpc_bind_local(L"test-endpoint",&binding)==RPC_S_OK);
    CHECK(binding_live==1 && !string_live && step==3);
    CHECK(free_binding(&binding)==RPC_S_OK && !binding && !binding_live);
    printf("COMMON-RPC-BINDING checks=%u failures=%u resources=%u\n",checks,failures,string_live+binding_live);
    return failures ? 1 : 0;
}
