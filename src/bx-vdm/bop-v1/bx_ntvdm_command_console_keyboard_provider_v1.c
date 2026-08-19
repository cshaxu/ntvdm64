#include "bx_ntvdm_command_console_keyboard_provider_v1.h"
#include <string.h>

int bx_ntvdm_command_console_keyboard_provider_v1_initialize(
    bx_ntvdm_command_console_keyboard_provider_v1 *provider)
{
    if (!provider) return 0;
    memset(provider, 0, sizeof(*provider));
    provider->magic=BX_NTVDM_COMMAND_CONSOLE_KEYBOARD_PROVIDER_V1_MAGIC;
    provider->abi_version=BX_NTVDM_COMMAND_CONSOLE_KEYBOARD_PROVIDER_V1_VERSION;
    provider->struct_bytes=(uint32_t)sizeof(*provider);
    bx_ntvdm_command_console_capability_v1_initialize(&provider->console, 0u);
    provider->initialized=1u;
    return bx_ntvdm_command_console_keyboard_provider_v1_valid(provider);
}

int bx_ntvdm_command_console_keyboard_provider_v1_valid(
    const bx_ntvdm_command_console_keyboard_provider_v1 *provider)
{
    return provider &&
        provider->magic==BX_NTVDM_COMMAND_CONSOLE_KEYBOARD_PROVIDER_V1_MAGIC &&
        provider->abi_version==BX_NTVDM_COMMAND_CONSOLE_KEYBOARD_PROVIDER_V1_VERSION &&
        provider->struct_bytes==sizeof(*provider) && provider->initialized==1u &&
        bx_ntvdm_command_console_capability_v1_valid(&provider->console);
}

int bx_ntvdm_command_console_keyboard_provider_v1_owns_service(uint8_t service)
{
    return service==9u || service==14u;
}

int bx_ntvdm_command_console_keyboard_provider_v1_dispatch(
    bx_ntvdm_command_console_keyboard_provider_v1 *provider,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    if (!bx_ntvdm_command_console_keyboard_provider_v1_valid(provider) ||
        !event || !cpu || !window || !result ||
        !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu) ||
        !bx_ntvdm_instruction_window_v1_valid(window) || event->vector!=6u ||
        cpu->execution_mode!=BX_NTVDM_CPU_EXECUTION_REAL || window->valid_bytes<4u ||
        window->bytes[0]!=0xc4u || window->bytes[1]!=0xc4u ||
        window->bytes[2]!=0x54u || !bx_ntvdm_command_console_keyboard_provider_v1_owns_service(window->bytes[3]))
        return 0;
    if (window->bytes[3]==9u)
        return bx_ntvdm_command_console_capability_v1_dispatch(&provider->console,
            event, cpu, window, result);
    /* cmdGetKbdLayout's NoInstallkb16 path sets DX=0 then calls
       cmdInitConsole.  The bounded CLI profile intentionally has neither
       KB16 assets nor a VDD/event-thread installation path. */
    return bx_ntvdm_cmd_keyboard_layout_v1_dispatch(event, cpu, window, result) &&
        bx_ntvdm_command_console_capability_v1_record_initialized(&provider->console);
}
