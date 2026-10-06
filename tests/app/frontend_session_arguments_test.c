#include "ntcon-exe/session_arguments.h"
#include <stdio.h>
#define CHECK(x) do{++checks;if(!(x)){++failures;printf("FAIL line %d\n",__LINE__);}}while(0)
int main(void)
{
    unsigned checks=0,failures=0,i;
    UINT_PTR resource=42;
    uint64_t window=42;
    const WCHAR *bad[]={NULL,L"",L"0",L"0x",L"-1",L"+1",L" 1",L"1 ",L"1g",L"10000000000000000"};
    for(i=0;i<sizeof(bad)/sizeof(bad[0]);++i) {
        CHECK(!frontend_session_resource(bad[i],&resource));CHECK(resource==42);
        CHECK(!frontend_session_window(bad[i],&window));CHECK(window==42);
    }
    CHECK(!frontend_session_resource(L"1",NULL));
    CHECK(!frontend_session_window(L"1",NULL));
    CHECK(frontend_session_resource(L"0xAbCd",&resource));CHECK(resource==0xabcd);
    CHECK(frontend_session_window(L"FFFFFFFFFFFFFFFF",&window));CHECK(window==UINT64_MAX);
#ifdef _WIN64
    CHECK(frontend_session_resource(L"123456789abcdef0",&resource));
    CHECK(resource==(UINT_PTR)UINT64_C(0x123456789abcdef0));
#else
    CHECK(!frontend_session_resource(L"100000000",&resource));
    CHECK(resource==0xabcd);
#endif
    printf("session arguments: %u checks, %u failures, pointer bits=%u\n",checks,failures,(unsigned)(8*sizeof(resource)));
    return failures?1:0;
}
