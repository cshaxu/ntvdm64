#ifndef WOW_BUTTON_RESOURCE_FIXTURE_PROVIDER_H
#define WOW_BUTTON_RESOURCE_FIXTURE_PROVIDER_H
#include "wow_button_resources.h"
#define WOW_ORIGINAL_BUTTON_RESOURCES
#include "compat/include/opennt_user_multipass.h"
#undef WOW_ORIGINAL_BUTTON_RESOURCES
const wow_button_resource_provider *WINAPI wow_original_button_resources(void);
#endif
