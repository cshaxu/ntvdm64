#ifndef NTDOS64_RECONSTRUCTED_MONITOR_S4_H
#define NTDOS64_RECONSTRUCTED_MONITOR_S4_H

#include <stddef.h>
#include <stdint.h>

typedef enum ntdos64_s4_run_result {
    NTDOS64_S4_BUDGET_EXHAUSTED = 0,
    NTDOS64_S4_BOP_EXIT,
    NTDOS64_S4_BRIDGE_REFUSED,
    NTDOS64_S4_UNSUPPORTED_BOP,
    NTDOS64_S4_UNKNOWN_OPCODE,
    NTDOS64_S4_MEMORY_FAULT,
    NTDOS64_S4_INVALID_ARGUMENT,
    NTDOS64_S4_NO_STOP_EVENT
} ntdos64_s4_run_result;

typedef enum ntdos64_s4_bridge_disposition {
    NTDOS64_S4_BRIDGE_DISPOSITION_NOT_ATTEMPTED = 0,
    NTDOS64_S4_BRIDGE_DISPOSITION_COMPLETED,
    NTDOS64_S4_BRIDGE_DISPOSITION_REFUSED
} ntdos64_s4_bridge_disposition;

typedef struct ntdos64_s4_state {
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;
    uint32_t esp;
    uint16_t cs;
    uint16_t ds;
    uint16_t es;
    uint16_t ss;
    uint16_t ip;
    uint16_t flags;
} ntdos64_s4_state;

enum {
    NTDOS64_S4_FLAG_CF = 0x0001u,
    NTDOS64_S4_FLAG_PF = 0x0004u,
    NTDOS64_S4_FLAG_AF = 0x0010u,
    NTDOS64_S4_FLAG_ZF = 0x0040u,
    NTDOS64_S4_FLAG_SF = 0x0080u,
    NTDOS64_S4_FLAG_IF = 0x0200u,
    NTDOS64_S4_FLAG_DF = 0x0400u
};

typedef struct ntdos64_s4_bridge_request {
    uint8_t selector;
    uint8_t next_byte;
    uint16_t reserved;
    uint32_t physical_fetch;
    uint32_t physical_next;
    ntdos64_s4_state state;
} ntdos64_s4_bridge_request;

enum {
    NTDOS64_S4_BRIDGE_WRITE_AX = 1u << 0,
    NTDOS64_S4_BRIDGE_WRITE_CX = 1u << 1,
    NTDOS64_S4_BRIDGE_WRITE_DX = 1u << 2,
    NTDOS64_S4_BRIDGE_WRITE_CS = 1u << 3,
    NTDOS64_S4_BRIDGE_WRITE_IP = 1u << 4,
    /* The bridge may report only CF, never an arbitrary FLAGS image. */
    NTDOS64_S4_BRIDGE_WRITE_CF = 1u << 5,
    NTDOS64_S4_BRIDGE_WRITE_ALL = NTDOS64_S4_BRIDGE_WRITE_AX |
        NTDOS64_S4_BRIDGE_WRITE_CX | NTDOS64_S4_BRIDGE_WRITE_DX |
        NTDOS64_S4_BRIDGE_WRITE_CS | NTDOS64_S4_BRIDGE_WRITE_IP |
        NTDOS64_S4_BRIDGE_WRITE_CF
};

typedef struct ntdos64_s4_bridge_response {
    uint32_t write_mask;
    uint16_t ax;
    uint16_t cx;
    uint16_t dx;
    uint16_t cs;
    uint16_t ip;
    uint8_t carry;
} ntdos64_s4_bridge_response;

typedef int (*ntdos64_s4_bridge_transport)(
    void *context,
    const ntdos64_s4_bridge_request *request,
    ntdos64_s4_bridge_response *response);

typedef struct ntdos64_s4_stop_event {
    ntdos64_s4_run_result reason;
    ntdos64_s4_bridge_disposition bridge_disposition;
    uint16_t cs;
    uint16_t ip;
    uint32_t physical_fetch;
    uint8_t physical_fetch_valid;
    uint8_t opcode_bytes;
    uint8_t opcode_window[4];
} ntdos64_s4_stop_event;

typedef struct ntdos64_s4_monitor {
    uint8_t *ram;
    size_t ram_bytes;
    ntdos64_s4_state state;
    ntdos64_s4_bridge_transport bridge_transport;
    void *bridge_context;
    ntdos64_s4_stop_event last_stop;
    uint8_t last_stop_valid;
} ntdos64_s4_monitor;

ntdos64_s4_run_result ntdos64_s4_monitor_initialize(
    ntdos64_s4_monitor *monitor,
    uint8_t *ram,
    size_t ram_bytes,
    ntdos64_s4_bridge_transport bridge_transport,
    void *bridge_context);
ntdos64_s4_run_result ntdos64_s4_monitor_run(
    ntdos64_s4_monitor *monitor,
    size_t instruction_budget);
ntdos64_s4_run_result ntdos64_s4_monitor_get_state(
    const ntdos64_s4_monitor *monitor,
    ntdos64_s4_state *out_state);
ntdos64_s4_run_result ntdos64_s4_monitor_set_state(
    ntdos64_s4_monitor *monitor,
    const ntdos64_s4_state *state);
ntdos64_s4_run_result ntdos64_s4_monitor_get_last_stop(
    const ntdos64_s4_monitor *monitor,
    ntdos64_s4_stop_event *out_event);

#endif
