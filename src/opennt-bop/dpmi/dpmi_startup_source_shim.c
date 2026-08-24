#include "dpmi_startup_source_shim.h"

static uint32_t last_illegal_index;

void runtime_dpmi_startup_source_deferred(void)
{
  /* The caller has reached a declared later owner package.  No guest CPU
   * state is changed here, so an accidental local table call cannot succeed. */
}

void runtime_dpmi_startup_source_note_illegal(uint32_t index)
{
  last_illegal_index = index;
}

int runtime_dpmi_startup_source_take_dispatch(ULONG *index)
{
  uint32_t staged_index;
  if (index == 0 || !runtime_dpmi_startup_session_runtime_take_dispatch(&staged_index))
    return 0;
  *index = (ULONG)staged_index;
  return 1;
}

void runtime_dpmi_startup_source_advance_ip(uint32_t bytes)
{
  runtime_dpmi_startup_session_runtime_advance_ip(bytes);
}

void runtime_dpmi_startup_source_initialize_dosx(void)
{
  runtime_dpmi_startup_session_runtime_initialize_dosx();
}

void runtime_dpmi_startup_source_initialize_app(void)
{
  runtime_dpmi_startup_session_runtime_initialize_app();
}

void runtime_dpmi_startup_source_publish_selector_table(void)
{
  runtime_dpmi_startup_session_runtime_publish_selector_table();
}
