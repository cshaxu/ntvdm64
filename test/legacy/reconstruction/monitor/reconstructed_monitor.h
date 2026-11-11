#ifndef RECONSTRUCTED_MONITOR_H
#define RECONSTRUCTED_MONITOR_H

#include <stddef.h>
#include <stdint.h>

typedef enum reconstructed_monitor_run_result {
    RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED = 0,
    RECONSTRUCTED_MONITOR_BOP_EXIT,
    RECONSTRUCTED_MONITOR_BRIDGE_REFUSED,
    RECONSTRUCTED_MONITOR_UNSUPPORTED_BOP,
    RECONSTRUCTED_MONITOR_UNKNOWN_OPCODE,
    RECONSTRUCTED_MONITOR_MEMORY_FAULT,
    RECONSTRUCTED_MONITOR_INVALID_ARGUMENT,
    RECONSTRUCTED_MONITOR_NO_STOP_EVENT
} reconstructed_monitor_run_result;

typedef enum reconstructed_monitor_bridge_disposition {
    RECONSTRUCTED_MONITOR_BRIDGE_DISPOSITION_NOT_ATTEMPTED = 0,
    RECONSTRUCTED_MONITOR_BRIDGE_DISPOSITION_COMPLETED,
    RECONSTRUCTED_MONITOR_BRIDGE_DISPOSITION_REFUSED
} reconstructed_monitor_bridge_disposition;

typedef struct reconstructed_monitor_state {
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
} reconstructed_monitor_state;

enum {
    RECONSTRUCTED_MONITOR_FLAG_CF = 0x0001u,
    RECONSTRUCTED_MONITOR_FLAG_PF = 0x0004u,
    RECONSTRUCTED_MONITOR_FLAG_AF = 0x0010u,
    RECONSTRUCTED_MONITOR_FLAG_ZF = 0x0040u,
    RECONSTRUCTED_MONITOR_FLAG_SF = 0x0080u,
    RECONSTRUCTED_MONITOR_FLAG_IF = 0x0200u,
    RECONSTRUCTED_MONITOR_FLAG_DF = 0x0400u
};

typedef struct reconstructed_monitor_bridge_request {
    uint8_t selector;
    uint8_t next_byte;
    uint16_t reserved;
    uint32_t physical_fetch;
    uint32_t physical_next;
    reconstructed_monitor_state state;
} reconstructed_monitor_bridge_request;

enum {
    RECONSTRUCTED_MONITOR_BRIDGE_WRITE_AX = 1u << 0,
    RECONSTRUCTED_MONITOR_BRIDGE_WRITE_CX = 1u << 1,
    RECONSTRUCTED_MONITOR_BRIDGE_WRITE_DX = 1u << 2,
    RECONSTRUCTED_MONITOR_BRIDGE_WRITE_CS = 1u << 3,
    RECONSTRUCTED_MONITOR_BRIDGE_WRITE_IP = 1u << 4,
    /* The bridge may report only CF, never an arbitrary FLAGS image. */
    RECONSTRUCTED_MONITOR_BRIDGE_WRITE_CF = 1u << 5,
    RECONSTRUCTED_MONITOR_BRIDGE_WRITE_ALL = RECONSTRUCTED_MONITOR_BRIDGE_WRITE_AX |
        RECONSTRUCTED_MONITOR_BRIDGE_WRITE_CX | RECONSTRUCTED_MONITOR_BRIDGE_WRITE_DX |
        RECONSTRUCTED_MONITOR_BRIDGE_WRITE_CS | RECONSTRUCTED_MONITOR_BRIDGE_WRITE_IP |
        RECONSTRUCTED_MONITOR_BRIDGE_WRITE_CF
};

typedef struct reconstructed_monitor_bridge_response {
    uint32_t write_mask;
    uint16_t ax;
    uint16_t cx;
    uint16_t dx;
    uint16_t cs;
    uint16_t ip;
    uint8_t carry;
} reconstructed_monitor_bridge_response;

typedef int (*reconstructed_monitor_bridge_transport)(
    void *context,
    const reconstructed_monitor_bridge_request *request,
    reconstructed_monitor_bridge_response *response);

typedef struct reconstructed_monitor_stop_event {
    reconstructed_monitor_run_result reason;
    reconstructed_monitor_bridge_disposition bridge_disposition;
    uint16_t cs;
    uint16_t ip;
    uint32_t physical_fetch;
    uint8_t physical_fetch_valid;
    uint8_t opcode_bytes;
    uint8_t opcode_window[4];
} reconstructed_monitor_stop_event;

typedef struct reconstructed_monitor_monitor {
    uint8_t *ram;
    size_t ram_bytes;
    reconstructed_monitor_state state;
    reconstructed_monitor_bridge_transport bridge_transport;
    void *bridge_context;
    reconstructed_monitor_stop_event last_stop;
    uint8_t last_stop_valid;
} reconstructed_monitor_monitor;

reconstructed_monitor_run_result reconstructed_monitor_monitor_initialize(
    reconstructed_monitor_monitor *monitor,
    uint8_t *ram,
    size_t ram_bytes,
    reconstructed_monitor_bridge_transport bridge_transport,
    void *bridge_context);
reconstructed_monitor_run_result reconstructed_monitor_monitor_run(
    reconstructed_monitor_monitor *monitor,
    size_t instruction_budget);
reconstructed_monitor_run_result reconstructed_monitor_monitor_get_state(
    const reconstructed_monitor_monitor *monitor,
    reconstructed_monitor_state *out_state);
reconstructed_monitor_run_result reconstructed_monitor_monitor_set_state(
    reconstructed_monitor_monitor *monitor,
    const reconstructed_monitor_state *state);
reconstructed_monitor_run_result reconstructed_monitor_monitor_get_last_stop(
    const reconstructed_monitor_monitor *monitor,
    reconstructed_monitor_stop_event *out_event);

#endif
