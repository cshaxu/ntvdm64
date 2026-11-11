#include "wow_class_lookup_bindings.h"
#include <stdio.h>
#include <stddef.h>

/* Original lookup only, with caller-owned lists; not native registration. */
typedef char next_is_first[(offsetof(wow_class_lookup_entry, next) == 0) ? 1 : -1];
static unsigned errors, checks;
#define CHECK(x) do { ++checks; if (!(x)) { ++errors; printf("FAIL line=%d\n", __LINE__); } } while (0)
int __cdecl main(void)
{
    wow_class_lookup_entry private_class = {0}, public_class = {0};
    wow_class_lookup_entry system_class = {0}, client_class = {0}, other = {0};
    wow_class_lookup_context context = {0};
    HANDLE module = (HANDLE)0x23451234, other_instance = (HANDLE)0x23455678;
    HANDLE outside = (HANDLE)0x45671234;
    context.client_module = (HANDLE)0x76540000;
    private_class.atomClassName = public_class.atomClassName = system_class.atomClassName = 0xc123;
    private_class.hModule = public_class.hModule = module;
    context.pclsPrivateList = &private_class;
    context.pclsPublicList = &public_class;
    context.system_classes = &system_class;
    CHECK(GetClassPtr(0, &context, module) == NULL);
    CHECK(GetClassPtr(0xc124, &context, module) == NULL);
    CHECK(GetClassPtr(0xc123, &context, module) == &context.pclsPrivateList);
    CHECK(GetClassPtr(0xc123, &context, other_instance) == &context.pclsPrivateList);
    CHECK(GetClassPtr(0xc123, &context, outside) == &context.pclsPublicList);
    private_class.flags = CSF_WOWDEFERDESTROY;
    CHECK(GetClassPtr(0xc123, &context, module) == &context.pclsPublicList);
    public_class.flags = CSF_WOWDEFERDESTROY;
    CHECK(GetClassPtr(0xc123, &context, module) == &context.system_classes);
    system_class.flags = CSF_WOWDEFERDESTROY;
    CHECK(GetClassPtr(0xc123, &context, module) == NULL);
    system_class.flags = 0;
    client_class.atomClassName = 0xc123; client_class.hModule = context.client_module;
    private_class.next = &client_class;
    CHECK(GetClassPtr(0xc123, &context, outside) == &private_class.next);
    /* Return the owning link, not a temporary pointer to the selected entry. */
    *GetClassPtr(0xc123, &context, outside) = NULL;
    CHECK(private_class.next == NULL);
    public_class.next = &client_class;
    CHECK(GetClassPtr(0xc123, &context, outside) == &public_class.next);
    other.atomClassName = 0xc123; other.hModule = outside;
    private_class.flags = 0; private_class.next = &other;
    CHECK(GetClassPtr(0xc123, &context, outside) == &private_class.next);
    CHECK(GetClassPtr(0xc123, &context, module) == &context.pclsPrivateList);
    CHECK(_InnerGetClassPtr(0xc123, &context.pclsPrivateList, NULL) == &context.pclsPrivateList);
    printf("WOW_ORIGINAL_CLASS_LOOKUP errors=%u checks=%u\n", errors, checks);
    return errors ? 3 : 0;
}
