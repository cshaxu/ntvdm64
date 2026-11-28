#include "winuserp.h"
#include "non-mvdm/public/oak/inc/kbd.h"
#pragma push_macro("_USER_")
#undef _USER_
#include "non-mvdm/windows/core/ntuser/inc/user.h"
#pragma pop_macro("_USER_")
