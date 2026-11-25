#pragma once
/* Host-only compilation of the XP CodePairs object; no kernel services. */
#define WIN32_NO_STATUS
#include <windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#include <wtypes.h>
#include "status.h"
