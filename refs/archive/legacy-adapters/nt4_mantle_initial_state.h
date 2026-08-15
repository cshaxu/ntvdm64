#ifndef NT4_MANTLE_INITIAL_STATE_H
#define NT4_MANTLE_INITIAL_STATE_H

#include <stdint.h>

typedef enum nt4_mantle_evidence_class {
    NT4_MANTLE_EVIDENCE_UNPROVEN = 0,
    NT4_MANTLE_EVIDENCE_SOURCE_LOCKED,
    NT4_MANTLE_EVIDENCE_TRACE_LOCKED
} nt4_mantle_evidence_class;

typedef struct nt4_mantle_evidence {
    nt4_mantle_evidence_class evidence_class;
    uint64_t record_id;
} nt4_mantle_evidence;

typedef struct nt4_mantle_ivt_value {
    uint16_t offset;
    uint16_t segment;
    nt4_mantle_evidence evidence;
} nt4_mantle_ivt_value;

typedef struct nt4_mantle_bda_byte {
    uint8_t value;
    nt4_mantle_evidence evidence;
} nt4_mantle_bda_byte;

typedef struct nt4_mantle_initial_state {
    nt4_mantle_ivt_value incoming_int10;
    nt4_mantle_ivt_value incoming_int15;
    nt4_mantle_ivt_value incoming_int19;
    nt4_mantle_ivt_value incoming_int1b;
    nt4_mantle_bda_byte keyboard_flags_0096;
} nt4_mantle_initial_state;

typedef enum nt4_mantle_initial_state_result {
    NT4_MANTLE_INITIAL_STATE_OK = 0,
    NT4_MANTLE_INITIAL_STATE_INVALID_ARGUMENT,
    NT4_MANTLE_INITIAL_STATE_INT10_UNPROVEN,
    NT4_MANTLE_INITIAL_STATE_INT15_UNPROVEN,
    NT4_MANTLE_INITIAL_STATE_INT19_UNPROVEN,
    NT4_MANTLE_INITIAL_STATE_INT1B_UNPROVEN,
    NT4_MANTLE_INITIAL_STATE_BDA_0096_UNPROVEN
} nt4_mantle_initial_state_result;

/* On success, out_accepted receives a copied profile record. On failure it is
 * unchanged. This validates evidence only; it performs no guest-memory write. */
nt4_mantle_initial_state_result nt4_mantle_initial_state_validate(
    const nt4_mantle_initial_state *candidate,
    nt4_mantle_initial_state *out_accepted);

#endif
