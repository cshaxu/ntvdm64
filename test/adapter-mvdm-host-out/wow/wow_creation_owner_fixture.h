#ifndef WOW_CREATION_OWNER_FIXTURE_H
#define WOW_CREATION_OWNER_FIXTURE_H
#include "wow_window_creation_binding.h"
#include "wow_user_object_bindings.h"
/* Controlled single-thread owner for native-boundary fixtures only. No USER
 * TDB, guest scheduling or production current-thread provider is simulated. */
static wow_task_order_process fixture_creation_process;
static wow_task_order_thread fixture_creation_thread;
static wow_user_object_table fixture_creation_objects;
static wow_task_order_thread *wow_fixture_creation_owner(wow_class_lookup_context *classes)
{
    fixture_creation_process.classes=classes;
    fixture_creation_process.objects=&fixture_creation_objects;
    fixture_creation_thread.ppi=&fixture_creation_process;
    return &fixture_creation_thread;
}
#endif
