#ifndef WOW_USER_REGISTRATION_STATE_H
#define WOW_USER_REGISTRATION_STATE_H
#include "wow_user_task_lifecycle.h"
#include "wow_user_registered_callbacks.h"
wow_user_task_lifecycle *WINAPI wow_user_registered_lifecycle(void);
void WINAPI wow_user_set_registered_callbacks(APFNWOWHANDLERSIN input);
#endif
