#ifndef WOW_MENU_CLIENT_BINDINGS_H
#define WOW_MENU_CLIENT_BINDINGS_H
#include "wow_user_object_bindings.h"
#include "wow_task_order_bindings.h"

/* The existing USER owner serializes these operations with guest entry.
 * Public USER still owns all menu policy. The companion supplies only the
 * missing process/task association and immutable USER16's query graph.
 * New objects require full handles from native producers, not unknown guest
 * short aliases. A failed refresh must not be followed by guest execution of
 * a stale view. */
BOOL WINAPI wow_menu_client_publish_tree(wow_task_order_process *, DWORD, HMENU);
BOOL WINAPI wow_menu_client_retire_destroyed(wow_task_order_process *);
BOOL WINAPI wow_menu_client_destroy_object(wow_task_order_process *,
    wow_cleanup_handle *);
HMENU WINAPI wow_menu_client_full_handle(wow_user_object_table *, HMENU);
struct wow_cleanup_context;
void WINAPI wow_menu_client_cleanup(struct wow_cleanup_context *, wow_cleanup_handle *);
#endif
