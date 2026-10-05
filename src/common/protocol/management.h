/* Management projection only. These categories are not worker kinds or
 * execution states. Selectors contain copied identities, never handles. */
#ifndef COMMON_PROTOCOL_MANAGEMENT_H
#define COMMON_PROTOCOL_MANAGEMENT_H
#define MANAGEMENT_KIND_DOS 0u
#define MANAGEMENT_KIND_WIN16 1u
#define MANAGEMENT_KIND_WIN32 2u
#define MANAGEMENT_KIND_WIN64 3u
#define MANAGEMENT_FRONTEND 1u
#define MANAGEMENT_WORKER 2u
#define MANAGEMENT_WOW_TASK 3u
#define MANAGEMENT_GUI_TARGET 4u
#define MANAGEMENT_UNKNOWN 0u
#define MANAGEMENT_IDLE 1u
#define MANAGEMENT_BUSY 2u
#define MANAGEMENT_MISSING 3u
#define MANAGEMENT_CLOSING 4u
#define MANAGEMENT_CAN_CLOSE 1u
#endif
