#ifndef WOW_INPUT_GROUP_LIFETIME_H
#define WOW_INPUT_GROUP_LIFETIME_H
#include "wow_input_group_bindings.h"

struct wow_input_native_link;
typedef struct wow_input_group_owner {
    wow_input_desktop *desktop;
    struct wow_input_native_link *links;
} wow_input_group_owner;
typedef struct wow_input_group_plan wow_input_group_plan;

/* Caller holds the existing USER runtime lock. There is no second thread
 * registry, native queue or transport. Plans are call-local; links own only
 * this binding's successful AttachThreadInput increments. */
void WINAPI wow_input_group_initialize(wow_input_group_owner *, wow_input_desktop *);
wow_input_group_plan *WINAPI wow_input_group_prepare(wow_input_group_owner *);
void WINAPI wow_input_group_commit(wow_input_group_owner *, wow_input_group_plan *,
    wow_task_order_thread *);
void WINAPI wow_input_group_release_plan(wow_input_group_plan *);
void WINAPI wow_input_group_retire(wow_input_group_owner *, wow_task_order_thread *);
#endif
