#include "byob_profile.h"

#include "byob_identity.h"

#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define BYOB_PROFILE_MAX_BYTES 65536u
#define BYOB_PROFILE_MAX_COMPONENTS 16u

typedef struct json_cursor {
    const char *data;
    size_t length;
    size_t position;
    unsigned int depth;
} json_cursor;

typedef struct byob_component {
    char role[24];
    char file_name[64];
    char sha256[65];
    uint64_t bytes;
    int required;
} byob_component;

typedef struct byob_machine_observation {
    uint32_t id;
    uint64_t physical_address;
    uint64_t byte_count;
} byob_machine_observation;

typedef struct byob_machine_observation_trigger {
    uint32_t observation_id;
    uint32_t exception_vector;
    uint8_t instruction_bytes[2];
    uint32_t resume_bytes;
} byob_machine_observation_trigger;

typedef struct byob_machine_startup_snapshot_trigger {
    uint32_t observation_count;
    uint32_t observation_ids[BYOB_PROFILE_MAX_MACHINE_OBSERVATIONS];
    uint32_t exception_vector;
    uint8_t instruction_bytes[2];
    uint32_t resume_bytes;
} byob_machine_startup_snapshot_trigger;

typedef struct byob_machine_startup_handoff {
    uint32_t exception_vector;
    uint8_t instruction_bytes[2];
    uint16_t cs;
    uint16_t ip;
} byob_machine_startup_handoff;

typedef struct byob_host_drive_inventory {
    uint8_t types[BYOB_PROFILE_DRIVE_SLOT_COUNT];
} byob_host_drive_inventory;

typedef struct byob_guest_artifact_placement {
    char path[64];
    uint32_t drive_index;
} byob_guest_artifact_placement;

typedef struct byob_guest_dos_metadata {
    uint32_t attributes;
    uint16_t dos_time;
    uint16_t dos_date;
} byob_guest_dos_metadata;

typedef struct byob_declared_target {
    char role[32];
    byob_guest_artifact_placement placement;
} byob_declared_target;

typedef struct byob_profile_document {
    char schema[32];
    char profile[40];
    char architecture[8];
    char locale[16];
    char compatibility_group[128];
    unsigned int features;
    byob_component components[BYOB_PROFILE_MAX_COMPONENTS];
    size_t component_count;
    byob_machine_observation
        machine_observations[BYOB_PROFILE_MAX_MACHINE_OBSERVATIONS];
    size_t machine_observation_count;
    int has_machine_observation_trigger;
    byob_machine_observation_trigger machine_observation_trigger;
    int has_machine_startup_snapshot_trigger;
    byob_machine_startup_snapshot_trigger machine_startup_snapshot_trigger;
    int has_machine_startup_snapshot_evidence_file;
    char machine_startup_snapshot_evidence_file_name[64];
    int has_machine_startup_plan;
    int machine_startup_plan_enabled;
    int has_machine_startup_entry;
    char machine_startup_entry[32];
    int has_machine_external_initial_state;
    char machine_external_initial_state_evidence_sha256[65];
    int has_machine_startup_handoff;
    byob_machine_startup_handoff machine_startup_handoff;
    int has_host_drive_inventory;
    byob_host_drive_inventory host_drive_inventory;
    int has_command_placement;
    byob_guest_artifact_placement command_placement;
    int has_target_placement;
    byob_guest_artifact_placement target_placement;
    int has_guest_boot_files;
    byob_guest_artifact_placement config_file;
    byob_guest_artifact_placement autoexec_file;
    int has_guest_search_metadata;
    byob_guest_dos_metadata command_metadata;
    byob_guest_dos_metadata target_metadata;
    byob_guest_dos_metadata terminal_quit_metadata;
    byob_guest_dos_metadata config_metadata;
    byob_guest_dos_metadata autoexec_metadata;
    int has_guest_display_state;
    char guest_display_state[24];
    int has_declared_targets;
    byob_declared_target declared_targets[BYOB_PROFILE_MAX_DECLARED_TARGETS];
    size_t declared_target_count;
} byob_profile_document;

enum {
    FEATURE_XMS = 1u << 0,
    FEATURE_DPMI = 1u << 1,
    FEATURE_STARTUP_PROFILE = 1u << 2,
    FEATURE_NLS = 1u << 3,
    FEATURE_REDIRECTOR = 1u << 4,
    FEATURE_UTILITIES = 1u << 5
};

const char *byob_profile_result_name(byob_profile_result result)
{
    static const char *const names[] = {
        "accepted", "profile_format_invalid", "profile_target_mismatch",
        "role_missing_or_duplicate", "feature_declaration_invalid",
        "component_name_unsafe", "component_open_rejected",
        "component_identity_mismatch", "compatibility_group_mismatch"
    };
    return result >= BYOB_PROFILE_ACCEPTED &&
        result <= BYOB_PROFILE_COMPATIBILITY_GROUP_MISMATCH ?
        names[result] : "profile_format_invalid";
}

static void json_space(json_cursor *cursor)
{
    while (cursor->position < cursor->length &&
        (cursor->data[cursor->position] == ' ' || cursor->data[cursor->position] == '\n' ||
         cursor->data[cursor->position] == '\r' || cursor->data[cursor->position] == '\t')) {
        ++cursor->position;
    }
}

static int json_take(json_cursor *cursor, char value)
{
    json_space(cursor);
    if (cursor->position >= cursor->length || cursor->data[cursor->position] != value) return 0;
    ++cursor->position;
    return 1;
}

static int json_literal(json_cursor *cursor, const char *literal)
{
    size_t size = strlen(literal);
    json_space(cursor);
    if (size > cursor->length - cursor->position ||
        memcmp(cursor->data + cursor->position, literal, size) != 0) return 0;
    cursor->position += size;
    return 1;
}

static int json_skip_string(json_cursor *cursor)
{
    if (!json_take(cursor, '"')) return 0;
    while (cursor->position < cursor->length) {
        unsigned char ch = (unsigned char)cursor->data[cursor->position++];
        if (ch == '"') return 1;
        if (ch < 0x20u) return 0;
        if (ch == '\\') {
            unsigned int index;
            if (cursor->position >= cursor->length) return 0;
            ch = (unsigned char)cursor->data[cursor->position++];
            if (strchr("\"\\/bfnrt", (int)ch) != NULL) continue;
            if (ch != 'u' || cursor->length - cursor->position < 4u) return 0;
            for (index = 0u; index < 4u; ++index) {
                ch = (unsigned char)cursor->data[cursor->position++];
                if (!((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') ||
                    (ch >= 'A' && ch <= 'F'))) return 0;
            }
        }
    }
    return 0;
}

static int json_string_ascii(json_cursor *cursor, char *out, size_t capacity)
{
    size_t used = 0u;
    if (out == NULL || capacity == 0u || !json_take(cursor, '"')) return 0;
    while (cursor->position < cursor->length) {
        unsigned char ch = (unsigned char)cursor->data[cursor->position++];
        if (ch == '"') {
            out[used] = '\0';
            return 1;
        }
        if (ch < 0x20u || ch > 0x7eu || ch == '\\' || used + 1u >= capacity) return 0;
        out[used++] = (char)ch;
    }
    return 0;
}

static int json_u64(json_cursor *cursor, uint64_t *out_value)
{
    uint64_t value = 0u;
    int digits = 0;
    json_space(cursor);
    while (cursor->position < cursor->length) {
        unsigned char ch = (unsigned char)cursor->data[cursor->position];
        if (ch < '0' || ch > '9') break;
        if (value > (UINT64_MAX - (uint64_t)(ch - '0')) / 10u) return 0;
        value = value * 10u + (uint64_t)(ch - '0');
        ++cursor->position;
        digits = 1;
    }
    if (!digits) return 0;
    *out_value = value;
    return 1;
}

static int json_boolean(json_cursor *cursor, int *out_value)
{
    if (json_literal(cursor, "true")) { *out_value = 1; return 1; }
    if (json_literal(cursor, "false")) { *out_value = 0; return 1; }
    return 0;
}

static int json_nullable_string(json_cursor *cursor)
{
    return json_literal(cursor, "null") || json_skip_string(cursor);
}

static int json_skip_value(json_cursor *cursor)
{
    char ch;
    json_space(cursor);
    if (cursor->position >= cursor->length || cursor->depth++ > 16u) return 0;
    ch = cursor->data[cursor->position];
    if (ch == '"') {
        --cursor->depth;
        return json_skip_string(cursor);
    }
    if (ch == '{' || ch == '[') {
        char close = ch == '{' ? '}' : ']';
        int first = 1;
        ++cursor->position;
        json_space(cursor);
        while (cursor->position < cursor->length && cursor->data[cursor->position] != close) {
            if (!first && !json_take(cursor, ',')) { --cursor->depth; return 0; }
            if (ch == '{' && (!json_skip_string(cursor) || !json_take(cursor, ':'))) {
                --cursor->depth; return 0;
            }
            if (!json_skip_value(cursor)) { --cursor->depth; return 0; }
            first = 0;
            json_space(cursor);
        }
        if (!json_take(cursor, close)) { --cursor->depth; return 0; }
        --cursor->depth;
        return 1;
    }
    --cursor->depth;
    if (json_literal(cursor, "null") || json_literal(cursor, "true") || json_literal(cursor, "false")) return 1;
    if (ch == '-' || (ch >= '0' && ch <= '9')) {
        ++cursor->position;
        while (cursor->position < cursor->length && strchr("0123456789+-.eE", cursor->data[cursor->position]) != NULL) ++cursor->position;
        return 1;
    }
    return 0;
}

static unsigned int feature_bit(const char *name)
{
    if (strcmp(name, "xms") == 0) return FEATURE_XMS;
    if (strcmp(name, "dpmi") == 0) return FEATURE_DPMI;
    if (strcmp(name, "startup-profile") == 0) return FEATURE_STARTUP_PROFILE;
    if (strcmp(name, "nls") == 0) return FEATURE_NLS;
    if (strcmp(name, "redirector") == 0) return FEATURE_REDIRECTOR;
    if (strcmp(name, "utilities") == 0) return FEATURE_UTILITIES;
    return 0u;
}

static int json_features(json_cursor *cursor, unsigned int *out_features)
{
    unsigned int features = 0u;
    int first = 1;
    if (!json_take(cursor, '[')) return 0;
    json_space(cursor);
    while (cursor->position < cursor->length && cursor->data[cursor->position] != ']') {
        char name[32];
        unsigned int bit;
        if (!first && !json_take(cursor, ',')) return 0;
        if (!json_string_ascii(cursor, name, sizeof(name)) || (bit = feature_bit(name)) == 0u ||
            (features & bit) != 0u) return 0;
        features |= bit;
        first = 0;
        json_space(cursor);
    }
    if (!json_take(cursor, ']')) return 0;
    *out_features = features;
    return 1;
}

static int json_component(json_cursor *cursor, byob_component *component)
{
    unsigned int seen = 0u;
    int first = 1;
    memset(component, 0, sizeof(*component));
    if (!json_take(cursor, '{')) return 0;
    json_space(cursor);
    while (cursor->position < cursor->length && cursor->data[cursor->position] != '}') {
        char key[64];
        unsigned int bit;
        if (!first && !json_take(cursor, ',')) return 0;
        if (!json_string_ascii(cursor, key, sizeof(key)) || !json_take(cursor, ':')) return 0;
        if (strcmp(key, "role") == 0) bit = 1u;
        else if (strcmp(key, "file_name") == 0) bit = 2u;
        else if (strcmp(key, "required") == 0) bit = 4u;
        else if (strcmp(key, "bytes") == 0) bit = 8u;
        else if (strcmp(key, "sha256") == 0) bit = 16u;
        else if (strcmp(key, "version") == 0) bit = 32u;
        else return 0;
        if ((seen & bit) != 0u) return 0;
        seen |= bit;
        if (bit == 1u && !json_string_ascii(cursor, component->role, sizeof(component->role))) return 0;
        if (bit == 2u && !json_string_ascii(cursor, component->file_name, sizeof(component->file_name))) return 0;
        if (bit == 4u && !json_boolean(cursor, &component->required)) return 0;
        if (bit == 8u && !json_u64(cursor, &component->bytes)) return 0;
        if (bit == 16u && !json_string_ascii(cursor, component->sha256, sizeof(component->sha256))) return 0;
        if (bit == 32u && !json_nullable_string(cursor)) return 0;
        first = 0;
        json_space(cursor);
    }
    return json_take(cursor, '}') && (seen & 31u) == 31u;
}

static int json_components(json_cursor *cursor, byob_profile_document *document)
{
    int first = 1;
    if (!json_take(cursor, '[')) return 0;
    json_space(cursor);
    while (cursor->position < cursor->length && cursor->data[cursor->position] != ']') {
        if (!first && !json_take(cursor, ',')) return 0;
        if (document->component_count >= BYOB_PROFILE_MAX_COMPONENTS ||
            !json_component(cursor, &document->components[document->component_count++])) return 0;
        first = 0;
        json_space(cursor);
    }
    return json_take(cursor, ']');
}

static int json_machine_observation(json_cursor *cursor,
    byob_machine_observation *observation)
{
    unsigned int seen = 0u;
    int first = 1;
    memset(observation, 0, sizeof(*observation));
    if (!json_take(cursor, '{')) return 0;
    json_space(cursor);
    while (cursor->position < cursor->length && cursor->data[cursor->position] != '}') {
        char key[24];
        unsigned int bit;
        uint64_t value;
        if (!first && !json_take(cursor, ',')) return 0;
        if (!json_string_ascii(cursor, key, sizeof(key)) || !json_take(cursor, ':')) return 0;
        if (strcmp(key, "id") == 0) bit = 1u;
        else if (strcmp(key, "physical_address") == 0) bit = 2u;
        else if (strcmp(key, "byte_count") == 0) bit = 4u;
        else return 0;
        if ((seen & bit) != 0u || !json_u64(cursor, &value)) return 0;
        if (bit == 1u) {
            if (value == 0u || value > UINT32_MAX) return 0;
            observation->id = (uint32_t)value;
        } else if (bit == 2u) {
            observation->physical_address = value;
        } else {
            observation->byte_count = value;
        }
        seen |= bit;
        first = 0;
        json_space(cursor);
    }
    return json_take(cursor, '}') && seen == 7u;
}

static int json_machine_observations(json_cursor *cursor,
    byob_profile_document *document)
{
    int first = 1;
    if (!json_take(cursor, '[')) return 0;
    json_space(cursor);
    while (cursor->position < cursor->length && cursor->data[cursor->position] != ']') {
        if (!first && !json_take(cursor, ',')) return 0;
        if (document->machine_observation_count >= BYOB_PROFILE_MAX_MACHINE_OBSERVATIONS ||
            !json_machine_observation(cursor,
                &document->machine_observations[document->machine_observation_count++])) return 0;
        first = 0;
        json_space(cursor);
    }
    return json_take(cursor, ']');
}

static int json_machine_observation_trigger(json_cursor *cursor,
    byob_machine_observation_trigger *trigger)
{
    unsigned int seen = 0u;
    int first = 1;
    memset(trigger, 0, sizeof(*trigger));
    if (!json_take(cursor, '{')) return 0;
    json_space(cursor);
    while (cursor->position < cursor->length && cursor->data[cursor->position] != '}') {
        char key[24], bytes[8];
        unsigned int bit;
        uint64_t value;
        if (!first && !json_take(cursor, ',')) return 0;
        if (!json_string_ascii(cursor, key, sizeof(key)) || !json_take(cursor, ':')) return 0;
        if (strcmp(key, "observation_id") == 0) bit = 1u;
        else if (strcmp(key, "exception_vector") == 0) bit = 2u;
        else if (strcmp(key, "instruction_bytes") == 0) bit = 4u;
        else if (strcmp(key, "resume_bytes") == 0) bit = 8u;
        else return 0;
        if ((seen & bit) != 0u) return 0;
        if (bit == 4u) {
            if (!json_string_ascii(cursor, bytes, sizeof(bytes)) ||
                strcmp(bytes, "0f0b") != 0) return 0;
            trigger->instruction_bytes[0] = 0x0fu;
            trigger->instruction_bytes[1] = 0x0bu;
        } else {
            if (!json_u64(cursor, &value) || value == 0u || value > UINT32_MAX) return 0;
            if (bit == 1u) trigger->observation_id = (uint32_t)value;
            if (bit == 2u) trigger->exception_vector = (uint32_t)value;
            if (bit == 8u) trigger->resume_bytes = (uint32_t)value;
        }
        seen |= bit;
        first = 0;
        json_space(cursor);
    }
    return json_take(cursor, '}') && seen == 15u;
}

static int json_snapshot_observation_ids(json_cursor *cursor,
    byob_machine_startup_snapshot_trigger *trigger)
{
    int first = 1;
    if (!json_take(cursor, '[')) return 0;
    json_space(cursor);
    while (cursor->position < cursor->length && cursor->data[cursor->position] != ']') {
        uint64_t value;
        uint32_t index;
        if (!first && !json_take(cursor, ',')) return 0;
        if (trigger->observation_count >= BYOB_PROFILE_MAX_MACHINE_OBSERVATIONS ||
            !json_u64(cursor, &value) || value == 0u || value > UINT32_MAX)
            return 0;
        for (index = 0u; index < trigger->observation_count; ++index) {
            if (trigger->observation_ids[index] == (uint32_t)value) return 0;
        }
        trigger->observation_ids[trigger->observation_count++] = (uint32_t)value;
        first = 0;
        json_space(cursor);
    }
    return trigger->observation_count != 0u && json_take(cursor, ']');
}

static int json_machine_startup_snapshot_trigger(json_cursor *cursor,
    byob_machine_startup_snapshot_trigger *trigger)
{
    unsigned int seen = 0u;
    int first = 1;
    memset(trigger, 0, sizeof(*trigger));
    if (!json_take(cursor, '{')) return 0;
    json_space(cursor);
    while (cursor->position < cursor->length && cursor->data[cursor->position] != '}') {
        char key[32], bytes[8];
        unsigned int bit;
        uint64_t value;
        if (!first && !json_take(cursor, ',')) return 0;
        if (!json_string_ascii(cursor, key, sizeof(key)) || !json_take(cursor, ':')) return 0;
        if (strcmp(key, "observation_ids") == 0) bit = 1u;
        else if (strcmp(key, "exception_vector") == 0) bit = 2u;
        else if (strcmp(key, "instruction_bytes") == 0) bit = 4u;
        else if (strcmp(key, "resume_bytes") == 0) bit = 8u;
        else return 0;
        if ((seen & bit) != 0u) return 0;
        if (bit == 1u) {
            if (!json_snapshot_observation_ids(cursor, trigger)) return 0;
        } else if (bit == 4u) {
            if (!json_string_ascii(cursor, bytes, sizeof(bytes)) ||
                strcmp(bytes, "0f0b") != 0) return 0;
            trigger->instruction_bytes[0] = 0x0fu;
            trigger->instruction_bytes[1] = 0x0bu;
        } else {
            if (!json_u64(cursor, &value) || value == 0u || value > UINT32_MAX) return 0;
            if (bit == 2u) trigger->exception_vector = (uint32_t)value;
            if (bit == 8u) trigger->resume_bytes = (uint32_t)value;
        }
        seen |= bit;
        first = 0;
        json_space(cursor);
    }
    return json_take(cursor, '}') && seen == 15u;
}

static int json_machine_startup_handoff(json_cursor *cursor,
    byob_machine_startup_handoff *handoff)
{
    unsigned int seen = 0u;
    int first = 1;
    memset(handoff, 0, sizeof(*handoff));
    if (!json_take(cursor, '{')) return 0;
    json_space(cursor);
    while (cursor->position < cursor->length && cursor->data[cursor->position] != '}') {
        char key[24], bytes[8];
        unsigned int bit;
        uint64_t value;
        if (!first && !json_take(cursor, ',')) return 0;
        if (!json_string_ascii(cursor, key, sizeof(key)) || !json_take(cursor, ':')) return 0;
        if (strcmp(key, "exception_vector") == 0) bit = 1u;
        else if (strcmp(key, "instruction_bytes") == 0) bit = 2u;
        else if (strcmp(key, "cs") == 0) bit = 4u;
        else if (strcmp(key, "ip") == 0) bit = 8u;
        else return 0;
        if ((seen & bit) != 0u) return 0;
        if (bit == 2u) {
            if (!json_string_ascii(cursor, bytes, sizeof(bytes)) ||
                strcmp(bytes, "0f0b") != 0) return 0;
            handoff->instruction_bytes[0] = 0x0fu;
            handoff->instruction_bytes[1] = 0x0bu;
        } else {
            if (!json_u64(cursor, &value) || value > UINT32_MAX) return 0;
            if (bit == 1u) handoff->exception_vector = (uint32_t)value;
            else if (bit == 4u) {
                if (value > UINT16_MAX) return 0;
                handoff->cs = (uint16_t)value;
            } else {
                if (value > UINT16_MAX) return 0;
                handoff->ip = (uint16_t)value;
            }
        }
        seen |= bit;
        first = 0;
        json_space(cursor);
    }
    return json_take(cursor, '}') && seen == 15u;
}

static int json_host_drive_inventory(json_cursor *cursor,
    byob_host_drive_inventory *inventory)
{
    unsigned int seen = 0u;
    int first = 1;
    memset(inventory, 0, sizeof(*inventory));
    if (!json_take(cursor, '{')) return 0;
    json_space(cursor);
    while (cursor->position < cursor->length && cursor->data[cursor->position] != '}') {
        char key[16];
        uint32_t index = 0u;
        if (!first && !json_take(cursor, ',')) return 0;
        if (!json_string_ascii(cursor, key, sizeof(key)) || !json_take(cursor, ':') ||
            strcmp(key, "types") != 0 || seen != 0u || !json_take(cursor, '[')) return 0;
        json_space(cursor);
        while (cursor->position < cursor->length && cursor->data[cursor->position] != ']') {
            uint64_t value;
            if (index != 0u && !json_take(cursor, ',')) return 0;
            if (index >= BYOB_PROFILE_DRIVE_SLOT_COUNT || !json_u64(cursor, &value) ||
                value > 6u) return 0;
            inventory->types[index++] = (uint8_t)value;
            json_space(cursor);
        }
        if (index != BYOB_PROFILE_DRIVE_SLOT_COUNT || !json_take(cursor, ']')) return 0;
        seen = 1u;
        first = 0;
        json_space(cursor);
    }
    return json_take(cursor, '}') && seen == 1u;
}

static int json_command_placement(json_cursor *cursor,
    byob_guest_artifact_placement *placement)
{
    unsigned int seen = 0u;
    int first = 1;
    memset(placement, 0, sizeof(*placement));
    if (!json_take(cursor, '{')) return 0;
    json_space(cursor);
    while (cursor->position < cursor->length && cursor->data[cursor->position] != '}') {
        char key[24];
        uint64_t value;
        unsigned int bit;
        if (!first && !json_take(cursor, ',')) return 0;
        if (!json_string_ascii(cursor, key, sizeof(key)) || !json_take(cursor, ':')) return 0;
        if (strcmp(key, "path") == 0) bit = 1u;
        else if (strcmp(key, "drive_index") == 0) bit = 2u;
        else return 0;
        if ((seen & bit) != 0u) return 0;
        if (bit == 1u) {
            size_t used = 0u;
            int terminated = 0;
            if (!json_take(cursor, '"')) return 0;
            while (cursor->position < cursor->length) {
                unsigned char ch = (unsigned char)cursor->data[cursor->position++];
                if (ch == '"') { placement->path[used] = '\0'; terminated = 1; break; }
                if (ch == '\\') {
                    if (cursor->position >= cursor->length ||
                        cursor->data[cursor->position++] != '\\') return 0;
                    ch = '\\';
                }
                if (ch < 0x20u || ch > 0x7eu || used + 1u >= sizeof(placement->path)) return 0;
                placement->path[used++] = (char)ch;
            }
            if (!terminated) return 0;
        } else {
            if (!json_u64(cursor, &value) || value >= BYOB_PROFILE_DRIVE_SLOT_COUNT) return 0;
            placement->drive_index = (uint32_t)value;
        }
        seen |= bit;
        first = 0;
        json_space(cursor);
    }
    return json_take(cursor, '}') && seen == 3u;
}

static int json_declared_target(json_cursor *cursor, byob_declared_target *target)
{
    unsigned int seen = 0u;
    int first = 1;
    memset(target, 0, sizeof(*target));
    if (!json_take(cursor, '{')) return 0;
    json_space(cursor);
    while (cursor->position < cursor->length && cursor->data[cursor->position] != '}') {
        char key[24];
        unsigned int bit;
        if (!first && !json_take(cursor, ',')) return 0;
        if (!json_string_ascii(cursor, key, sizeof(key)) || !json_take(cursor, ':')) return 0;
        if (strcmp(key, "role") == 0) bit = 1u;
        else if (strcmp(key, "placement") == 0) bit = 2u;
        else return 0;
        if ((seen & bit) != 0u) return 0;
        if (bit == 1u && !json_string_ascii(cursor, target->role, sizeof(target->role))) return 0;
        if (bit == 2u && !json_command_placement(cursor, &target->placement)) return 0;
        seen |= bit;
        first = 0;
        json_space(cursor);
    }
    return json_take(cursor, '}') && seen == 3u;
}

static int json_declared_targets(json_cursor *cursor, byob_profile_document *document)
{
    int first = 1;
    if (!json_take(cursor, '[')) return 0;
    json_space(cursor);
    while (cursor->position < cursor->length && cursor->data[cursor->position] != ']') {
        if (!first && !json_take(cursor, ',')) return 0;
        if (document->declared_target_count >= BYOB_PROFILE_MAX_DECLARED_TARGETS ||
            !json_declared_target(cursor,
                &document->declared_targets[document->declared_target_count++])) return 0;
        first = 0;
        json_space(cursor);
    }
    return json_take(cursor, ']') && document->declared_target_count != 0u &&
        document->declared_target_count <= BYOB_PROFILE_MAX_DECLARED_TARGETS;
}

static int json_boot_file(json_cursor *cursor, byob_guest_artifact_placement *placement,
    char *materialization, size_t materialization_capacity)
{
    unsigned int seen = 0u;
    char key[32];
    if (!json_take(cursor, '{')) return 0;
    json_space(cursor);
    while (!json_take(cursor, '}')) {
        if (seen != 0u && !json_take(cursor, ',')) return 0;
        if (!json_string_ascii(cursor, key, sizeof(key)) || !json_take(cursor, ':')) return 0;
        if (strcmp(key, "path") == 0) {
            size_t used = 0u;
            int terminated = 0;
            if ((seen & 1u) != 0u || !json_take(cursor, '"')) return 0;
            while (cursor->position < cursor->length) {
                unsigned char ch = (unsigned char)cursor->data[cursor->position++];
                if (ch == '"') { placement->path[used] = '\0'; terminated = 1; break; }
                if (ch == '\\') {
                    if (cursor->position >= cursor->length ||
                        cursor->data[cursor->position++] != '\\') return 0;
                    ch = '\\';
                }
                if (ch < 0x20u || ch > 0x7eu || used + 1u >= sizeof(placement->path)) return 0;
                placement->path[used++] = (char)ch;
            }
            if (!terminated) return 0;
            seen |= 1u;
        } else if (strcmp(key, "materialization") == 0) {
            if ((seen & 2u) != 0u || !json_string_ascii(cursor, materialization,
                materialization_capacity)) return 0;
            seen |= 2u;
        } else return 0;
        json_space(cursor);
    }
    return seen == 3u;
}

static int json_guest_boot_files(json_cursor *cursor, byob_profile_document *document)
{
    unsigned int seen = 0u;
    char key[32];
    char config_materialization[32], autoexec_materialization[32];
    if (!json_take(cursor, '{')) return 0;
    json_space(cursor);
    while (!json_take(cursor, '}')) {
        if (seen != 0u && !json_take(cursor, ',')) return 0;
        if (!json_string_ascii(cursor, key, sizeof(key)) || !json_take(cursor, ':')) return 0;
        if (strcmp(key, "config") == 0) {
            if ((seen & 1u) != 0u || !json_boot_file(cursor, &document->config_file,
                config_materialization, sizeof(config_materialization))) return 0;
            if (strcmp(config_materialization, "minimal-comment-v1") != 0) return 0;
            seen |= 1u;
        } else if (strcmp(key, "autoexec") == 0) {
            if ((seen & 2u) != 0u || !json_boot_file(cursor, &document->autoexec_file,
                autoexec_materialization, sizeof(autoexec_materialization))) return 0;
            if (strcmp(autoexec_materialization, "empty-v1") != 0) return 0;
            seen |= 2u;
        } else return 0;
        json_space(cursor);
    }
    return seen == 3u;
}

static int json_guest_dos_metadata(json_cursor *cursor,
    byob_guest_dos_metadata *metadata)
{
    unsigned int seen = 0u;
    int first = 1;
    memset(metadata, 0, sizeof(*metadata));
    if (!json_take(cursor, '{')) return 0;
    json_space(cursor);
    while (cursor->position < cursor->length && cursor->data[cursor->position] != '}') {
        char key[24];
        uint64_t value;
        unsigned int bit;
        if (!first && !json_take(cursor, ',')) return 0;
        if (!json_string_ascii(cursor, key, sizeof(key)) || !json_take(cursor, ':')) return 0;
        if (strcmp(key, "attributes") == 0) bit = 1u;
        else if (strcmp(key, "dos_time") == 0) bit = 2u;
        else if (strcmp(key, "dos_date") == 0) bit = 4u;
        else return 0;
        if ((seen & bit) != 0u || !json_u64(cursor, &value)) return 0;
        if (bit == 1u) {
            if (value > 0xffu) return 0;
            metadata->attributes = (uint32_t)value;
        } else if (bit == 2u) {
            if (value > 0xffffu) return 0;
            metadata->dos_time = (uint16_t)value;
        } else {
            if (value > 0xffffu) return 0;
            metadata->dos_date = (uint16_t)value;
        }
        seen |= bit;
        first = 0;
        json_space(cursor);
    }
    return json_take(cursor, '}') && seen == 7u;
}

static int json_guest_search_metadata(json_cursor *cursor,
    byob_profile_document *document)
{
    unsigned int seen = 0u;
    int first = 1;
    if (!json_take(cursor, '{')) return 0;
    json_space(cursor);
    while (cursor->position < cursor->length && cursor->data[cursor->position] != '}') {
        char key[24];
        unsigned int bit;
        byob_guest_dos_metadata *metadata;
        if (!first && !json_take(cursor, ',')) return 0;
        if (!json_string_ascii(cursor, key, sizeof(key)) || !json_take(cursor, ':')) return 0;
        if (strcmp(key, "command") == 0) {
            bit = 1u; metadata = &document->command_metadata;
        } else if (strcmp(key, "target") == 0) {
            bit = 2u; metadata = &document->target_metadata;
        } else if (strcmp(key, "terminal-quit") == 0) {
            bit = 16u; metadata = &document->terminal_quit_metadata;
        } else if (strcmp(key, "config") == 0) {
            bit = 4u; metadata = &document->config_metadata;
        } else if (strcmp(key, "autoexec") == 0) {
            bit = 8u; metadata = &document->autoexec_metadata;
        } else return 0;
        if ((seen & bit) != 0u || !json_guest_dos_metadata(cursor, metadata)) return 0;
        seen |= bit;
        first = 0;
        json_space(cursor);
    }
    return json_take(cursor, '}') && (seen == 15u || seen == 31u);
}

static int json_document(const char *data, size_t length, byob_profile_document *document)
{
    json_cursor cursor = { data, length, 0u, 0u };
    unsigned int seen = 0u;
    int first = 1;
    memset(document, 0, sizeof(*document));
    if (!json_take(&cursor, '{')) return 0;
    json_space(&cursor);
    while (cursor.position < cursor.length && cursor.data[cursor.position] != '}') {
        char key[64];
        unsigned int bit;
        if (!first && !json_take(&cursor, ',')) return 0;
        if (!json_string_ascii(&cursor, key, sizeof(key)) || !json_take(&cursor, ':')) return 0;
        if (strcmp(key, "schema") == 0) bit = 1u;
        else if (strcmp(key, "profile") == 0) bit = 2u;
        else if (strcmp(key, "architecture") == 0) bit = 4u;
        else if (strcmp(key, "locale") == 0) bit = 8u;
        else if (strcmp(key, "compatibility_group") == 0) bit = 16u;
        else if (strcmp(key, "components") == 0) bit = 32u;
        else if (strcmp(key, "features") == 0) bit = 64u;
        else if (strcmp(key, "owner_note") == 0) bit = 128u;
        else if (strcmp(key, "machine_observations") == 0) bit = 256u;
        else if (strcmp(key, "machine_observation_trigger") == 0) bit = 512u;
        else if (strcmp(key, "machine_startup_snapshot_trigger") == 0) bit = 1024u;
        else if (strcmp(key, "machine_startup_snapshot_evidence_file") == 0) bit = 2048u;
        else if (strcmp(key, "machine_startup_plan") == 0) bit = 4096u;
        else if (strcmp(key, "machine_startup_entry") == 0) bit = 8192u;
        else if (strcmp(key, "machine_external_initial_state_evidence_sha256") == 0) bit = 4194304u;
        else if (strcmp(key, "machine_startup_handoff") == 0) bit = 131072u;
        else if (strcmp(key, "host_drive_inventory") == 0) bit = 16384u;
        else if (strcmp(key, "guest_command_placement") == 0) bit = 32768u;
        else if (strcmp(key, "guest_boot_files") == 0) bit = 65536u;
        else if (strcmp(key, "guest_target_placement") == 0) bit = 262144u;
        else if (strcmp(key, "guest_search_metadata") == 0) bit = 524288u;
        else if (strcmp(key, "guest_declared_targets") == 0) bit = 1048576u;
        else if (strcmp(key, "guest_display_state") == 0) bit = 2097152u;
        else return 0;
        if ((seen & bit) != 0u) return 0;
        seen |= bit;
        if (bit == 1u && !json_string_ascii(&cursor, document->schema, sizeof(document->schema))) return 0;
        if (bit == 2u && !json_string_ascii(&cursor, document->profile, sizeof(document->profile))) return 0;
        if (bit == 4u && !json_string_ascii(&cursor, document->architecture, sizeof(document->architecture))) return 0;
        if (bit == 8u && !json_string_ascii(&cursor, document->locale, sizeof(document->locale))) return 0;
        if (bit == 16u && !json_string_ascii(&cursor, document->compatibility_group, sizeof(document->compatibility_group))) return 0;
        if (bit == 32u && !json_components(&cursor, document)) return 0;
        if (bit == 64u && !json_features(&cursor, &document->features)) return 0;
        if (bit == 128u && !json_nullable_string(&cursor)) return 0;
        if (bit == 256u && !json_machine_observations(&cursor, document)) return 0;
        if (bit == 512u && !json_machine_observation_trigger(&cursor,
            &document->machine_observation_trigger)) return 0;
        if (bit == 512u) document->has_machine_observation_trigger = 1;
        if (bit == 1024u && !json_machine_startup_snapshot_trigger(&cursor,
            &document->machine_startup_snapshot_trigger)) return 0;
        if (bit == 1024u) document->has_machine_startup_snapshot_trigger = 1;
        if (bit == 2048u && !json_string_ascii(&cursor,
            document->machine_startup_snapshot_evidence_file_name,
            sizeof(document->machine_startup_snapshot_evidence_file_name))) return 0;
        if (bit == 2048u) document->has_machine_startup_snapshot_evidence_file = 1;
        if (bit == 4096u && !json_boolean(&cursor,
            &document->machine_startup_plan_enabled)) return 0;
        if (bit == 4096u) document->has_machine_startup_plan = 1;
        if (bit == 8192u && !json_string_ascii(&cursor,
            document->machine_startup_entry,
            sizeof(document->machine_startup_entry))) return 0;
        if (bit == 8192u) document->has_machine_startup_entry = 1;
        if (bit == 4194304u && !json_string_ascii(&cursor,
            document->machine_external_initial_state_evidence_sha256,
            sizeof(document->machine_external_initial_state_evidence_sha256))) return 0;
        if (bit == 4194304u) document->has_machine_external_initial_state = 1;
        if (bit == 131072u && !json_machine_startup_handoff(&cursor,
            &document->machine_startup_handoff)) return 0;
        if (bit == 131072u) document->has_machine_startup_handoff = 1;
        if (bit == 16384u && !json_host_drive_inventory(&cursor,
            &document->host_drive_inventory)) return 0;
        if (bit == 16384u) document->has_host_drive_inventory = 1;
        if (bit == 32768u && !json_command_placement(&cursor,
            &document->command_placement)) return 0;
        if (bit == 32768u) document->has_command_placement = 1;
        if (bit == 65536u && !json_guest_boot_files(&cursor, document)) return 0;
        if (bit == 65536u) document->has_guest_boot_files = 1;
        if (bit == 262144u && !json_command_placement(&cursor,
            &document->target_placement)) return 0;
        if (bit == 262144u) document->has_target_placement = 1;
        if (bit == 524288u && !json_guest_search_metadata(&cursor, document)) return 0;
        if (bit == 524288u) document->has_guest_search_metadata = 1;
        if (bit == 1048576u && !json_declared_targets(&cursor, document)) return 0;
        if (bit == 1048576u) document->has_declared_targets = 1;
        if (bit == 2097152u && !json_string_ascii(&cursor,
            document->guest_display_state,
            sizeof(document->guest_display_state))) return 0;
        if (bit == 2097152u) document->has_guest_display_state = 1;
        first = 0;
        json_space(&cursor);
    }
    if (!json_take(&cursor, '}')) return 0;
    json_space(&cursor);
    return cursor.position == cursor.length && (seen & 127u) == 127u;
}

static int ascii_to_wide(const char *source, wchar_t *destination, size_t capacity)
{
    size_t index;
    if (source == NULL || destination == NULL) return 0;
    for (index = 0u; source[index] != '\0'; ++index) {
        if ((unsigned char)source[index] > 0x7fu || index + 1u >= capacity) return 0;
        destination[index] = (wchar_t)(unsigned char)source[index];
    }
    destination[index] = L'\0';
    return 1;
}

static int sha256_lowercase(const char *value)
{
    size_t index;
    if (strlen(value) != 64u) return 0;
    for (index = 0u; index < 64u; ++index) {
        if (!((value[index] >= '0' && value[index] <= '9') ||
            (value[index] >= 'a' && value[index] <= 'f'))) return 0;
    }
    return 1;
}

static unsigned int role_feature(const char *role)
{
    if (strcmp(role, "himem") == 0) return FEATURE_XMS;
    if (strcmp(role, "dosx") == 0) return FEATURE_DPMI;
    if (strcmp(role, "pif-default") == 0) return FEATURE_STARTUP_PROFILE;
    if (strcmp(role, "country") == 0 || strcmp(role, "keyb") == 0) return FEATURE_NLS;
    if (strcmp(role, "redir") == 0) return FEATURE_REDIRECTOR;
    if (strcmp(role, "edit") == 0 || strcmp(role, "debug") == 0 || strcmp(role, "edlin") == 0) return FEATURE_UTILITIES;
    return 0u;
}

static const char *canonical_file_name(const char *role)
{
    if (strcmp(role, "ntio") == 0) return "NTIO.SYS";
    if (strcmp(role, "ntdos") == 0) return "NTDOS.SYS";
    if (strcmp(role, "command") == 0) return "COMMAND.COM";
    if (strcmp(role, "terminal-quit") == 0) return "QUIT.COM";
    if (strcmp(role, "himem") == 0) return "HIMEM.SYS";
    if (strcmp(role, "dosx") == 0) return "DOSX.EXE";
    if (strcmp(role, "pif-default") == 0) return "_default.pif";
    if (strcmp(role, "country") == 0) return "COUNTRY.SYS";
    if (strcmp(role, "keyb") == 0) return "KEYB.COM";
    if (strcmp(role, "redir") == 0) return "REDIR.EXE";
    if (strcmp(role, "edit") == 0) return "EDIT.COM";
    if (strcmp(role, "debug") == 0) return "DEBUG.EXE";
    if (strcmp(role, "edlin") == 0) return "EDLIN.EXE";
    return NULL;
}

static void descriptor_copy(byob_component_descriptor *destination,
    const byob_component *component)
{
    if (destination == NULL || component == NULL) return;
    (void)ascii_to_wide(component->file_name, destination->file_name,
        sizeof(destination->file_name) / sizeof(destination->file_name[0]));
    (void)ascii_to_wide(component->sha256, destination->sha256,
        sizeof(destination->sha256) / sizeof(destination->sha256[0]));
    destination->bytes = component->bytes;
}

static byob_profile_result validate_document(const byob_profile_document *document,
    const wchar_t *root, byob_profile_selection *selection)
{
    size_t index;
    unsigned int required_roles = 0u;
    int is_v1 = strcmp(document->schema, "ntdos64-byob-profile-v1") == 0 &&
        strcmp(document->profile, "nt4-en-us-command-smoke-v1") == 0;
    int is_v2 = strcmp(document->schema, "ntdos64-byob-profile-v2") == 0 &&
        strcmp(document->profile, "nt4-en-us-command-smoke-v2") == 0;
    int is_v3 = strcmp(document->schema, "ntdos64-byob-profile-v3") == 0 &&
        strcmp(document->profile, "nt4-en-us-command-smoke-v3") == 0;
    int is_v4 = strcmp(document->schema, "ntdos64-byob-profile-v4") == 0 &&
        strcmp(document->profile, "nt4-en-us-command-smoke-v4") == 0;
    int is_v5 = strcmp(document->schema, "ntdos64-byob-profile-v5") == 0 &&
        strcmp(document->profile, "nt4-en-us-command-smoke-v5") == 0;
    int is_v6 = strcmp(document->schema, "ntdos64-byob-profile-v6") == 0 &&
        strcmp(document->profile, "nt4-en-us-command-normal-return-v6") == 0;
    int is_v7 = strcmp(document->schema, "ntdos64-byob-profile-v7") == 0 &&
        strcmp(document->profile, "nt4-en-us-cli-stream-v7") == 0;
    int is_v8 = strcmp(document->schema, "ntdos64-byob-profile-v8") == 0 &&
        strcmp(document->profile, "nt4-en-us-cli-stream-v8") == 0;
    const byob_component *target_component = NULL, *terminal_quit_component = NULL;
    if ((!is_v1 && !is_v2 && !is_v3 && !is_v4 && !is_v5 && !is_v6 && !is_v7 && !is_v8) ||
        strcmp(document->architecture, "x86") != 0 || strcmp(document->locale, "en-US") != 0) {
        return BYOB_PROFILE_TARGET_MISMATCH;
    }
    if ((is_v2 || is_v3 || is_v4 || is_v5 || is_v6 || is_v7 || is_v8) && (!document->has_command_placement || !document->has_guest_boot_files ||
        strcmp(document->command_placement.path, "\\COMMAND.COM") != 0 ||
        strcmp(document->config_file.path, "\\CONFIG.SYS") != 0 ||
        strcmp(document->autoexec_file.path, "\\AUTOEXEC.BAT") != 0))
        return BYOB_PROFILE_FORMAT_INVALID;
    if ((is_v3 || is_v4) && !document->has_target_placement)
        return BYOB_PROFILE_FORMAT_INVALID;
    if (is_v5 && (!document->has_declared_targets || !document->has_guest_search_metadata ||
        document->declared_target_count != 2u ||
        strcmp(document->declared_targets[0].role, "target") != 0 ||
        strcmp(document->declared_targets[1].role, "terminal-quit") != 0))
        return BYOB_PROFILE_FORMAT_INVALID;
    if ((is_v6 || is_v7 || is_v8) && (!document->has_declared_targets || !document->has_guest_search_metadata ||
        document->declared_target_count != 1u ||
        strcmp(document->declared_targets[0].role, "target") != 0))
        return BYOB_PROFILE_FORMAT_INVALID;
    if ((is_v4 || is_v5 || is_v6 || is_v7 || is_v8) && !document->has_guest_search_metadata)
        return BYOB_PROFILE_FORMAT_INVALID;
    if ((is_v7 || is_v8) && (!document->has_guest_display_state ||
        strcmp(document->guest_display_state, "stream-io-v1") != 0))
        return BYOB_PROFILE_FORMAT_INVALID;
    if (is_v8 && (!document->has_machine_external_initial_state ||
        !sha256_lowercase(document->machine_external_initial_state_evidence_sha256)))
        return BYOB_PROFILE_FORMAT_INVALID;
    if (document->compatibility_group[0] == '\0') return BYOB_PROFILE_COMPATIBILITY_GROUP_MISMATCH;
    for (index = 0u; index < document->machine_observation_count; ++index) {
        const byob_machine_observation *observation =
            &document->machine_observations[index];
        size_t prior;
        if (observation->id == 0u || observation->byte_count == 0u ||
            observation->physical_address > UINT64_C(0x100000) ||
            observation->byte_count > UINT64_C(0x100000) - observation->physical_address)
            return BYOB_PROFILE_FORMAT_INVALID;
        for (prior = 0u; prior < index; ++prior) {
            if (document->machine_observations[prior].id == observation->id)
                return BYOB_PROFILE_FORMAT_INVALID;
        }
        if (selection != NULL) {
            selection->machine_observations[index].id = observation->id;
            selection->machine_observations[index].physical_address = observation->physical_address;
            selection->machine_observations[index].byte_count = observation->byte_count;
        }
    }
    if (selection != NULL)
        selection->machine_observation_count = (uint32_t)document->machine_observation_count;
    if (document->has_machine_observation_trigger) {
        int found = 0;
        if (document->machine_observation_trigger.exception_vector != 6u ||
            document->machine_observation_trigger.instruction_bytes[0] != 0x0fu ||
            document->machine_observation_trigger.instruction_bytes[1] != 0x0bu ||
            document->machine_observation_trigger.resume_bytes != 2u)
            return BYOB_PROFILE_FORMAT_INVALID;
        for (index = 0u; index < document->machine_observation_count; ++index) {
            if (document->machine_observations[index].id ==
                document->machine_observation_trigger.observation_id) {
                found = 1;
                break;
            }
        }
        if (!found) return BYOB_PROFILE_FORMAT_INVALID;
        if (selection != NULL) {
            selection->has_machine_observation_trigger = 1u;
            selection->machine_observation_trigger.observation_id =
                document->machine_observation_trigger.observation_id;
            selection->machine_observation_trigger.exception_vector =
                document->machine_observation_trigger.exception_vector;
            selection->machine_observation_trigger.instruction_bytes[0] = 0x0fu;
            selection->machine_observation_trigger.instruction_bytes[1] = 0x0bu;
            selection->machine_observation_trigger.resume_bytes =
                document->machine_observation_trigger.resume_bytes;
        }
    }
    if (document->has_machine_observation_trigger &&
        document->has_machine_startup_snapshot_trigger)
        return BYOB_PROFILE_FORMAT_INVALID;
    if (document->has_machine_startup_snapshot_trigger) {
        const byob_machine_startup_snapshot_trigger *trigger =
            &document->machine_startup_snapshot_trigger;
        uint32_t snapshot_index;
        if (trigger->exception_vector != 6u ||
            trigger->instruction_bytes[0] != 0x0fu ||
            trigger->instruction_bytes[1] != 0x0bu || trigger->resume_bytes != 2u)
            return BYOB_PROFILE_FORMAT_INVALID;
        for (snapshot_index = 0u; snapshot_index < trigger->observation_count;
            ++snapshot_index) {
            int found = 0;
            for (index = 0u; index < document->machine_observation_count; ++index) {
                if (document->machine_observations[index].id ==
                    trigger->observation_ids[snapshot_index]) { found = 1; break; }
            }
            if (!found) return BYOB_PROFILE_FORMAT_INVALID;
        }
        if (selection != NULL) {
            selection->has_machine_startup_snapshot_trigger = 1u;
            selection->machine_startup_snapshot_trigger.observation_count =
                trigger->observation_count;
            memcpy(selection->machine_startup_snapshot_trigger.observation_ids,
                trigger->observation_ids, sizeof(trigger->observation_ids));
            selection->machine_startup_snapshot_trigger.exception_vector = 6u;
            selection->machine_startup_snapshot_trigger.instruction_bytes[0] = 0x0fu;
            selection->machine_startup_snapshot_trigger.instruction_bytes[1] = 0x0bu;
            selection->machine_startup_snapshot_trigger.resume_bytes = 2u;
        }
    }
    if (document->has_machine_startup_snapshot_evidence_file) {
        wchar_t evidence_name[64];
        if (!document->has_machine_startup_snapshot_trigger ||
            document->machine_startup_snapshot_evidence_file_name[0] == '\0' ||
            !ascii_to_wide(document->machine_startup_snapshot_evidence_file_name,
                evidence_name, sizeof(evidence_name) / sizeof(evidence_name[0])) ||
            !byob_component_name_safe(evidence_name))
            return BYOB_PROFILE_FORMAT_INVALID;
        if (selection != NULL) {
            (void)ascii_to_wide(document->machine_startup_snapshot_evidence_file_name,
                selection->machine_startup_snapshot_evidence_file_name,
                sizeof(selection->machine_startup_snapshot_evidence_file_name) /
                sizeof(selection->machine_startup_snapshot_evidence_file_name[0]));
        }
    }
    if (selection != NULL && document->has_machine_startup_plan &&
        document->machine_startup_plan_enabled)
        selection->machine_startup_plan_enabled = 1u;
    if (document->has_machine_startup_entry) {
        if (!document->has_machine_startup_plan ||
            !document->machine_startup_plan_enabled ||
            strcmp(document->machine_startup_entry, "ntio-v0") != 0)
            return BYOB_PROFILE_FORMAT_INVALID;
        if (selection != NULL) selection->machine_startup_entry_ntio_v0 = 1u;
    }
    if (selection != NULL && document->has_machine_external_initial_state) {
        if (!ascii_to_wide(document->machine_external_initial_state_evidence_sha256,
                selection->machine_external_initial_state_evidence_sha256, 65u))
            return BYOB_PROFILE_FORMAT_INVALID;
        selection->has_machine_external_initial_state = 1u;
    }
    if (document->has_machine_startup_plan &&
        document->machine_startup_plan_enabled &&
        !document->has_machine_startup_entry)
        return BYOB_PROFILE_FORMAT_INVALID;
    if (document->has_machine_startup_handoff) {
        const byob_machine_startup_handoff *handoff =
            &document->machine_startup_handoff;
        if (!document->has_machine_startup_plan ||
            !document->machine_startup_plan_enabled ||
            !document->has_machine_startup_entry ||
            strcmp(document->machine_startup_entry, "ntio-v0") != 0 ||
            handoff->exception_vector != 6u ||
            handoff->instruction_bytes[0] != 0x0fu ||
            handoff->instruction_bytes[1] != 0x0bu ||
            handoff->cs != 0xca80u || handoff->ip != 3u)
            return BYOB_PROFILE_FORMAT_INVALID;
        if (selection != NULL) {
            selection->has_machine_startup_handoff = 1u;
            selection->machine_startup_handoff.exception_vector = 6u;
            selection->machine_startup_handoff.instruction_bytes[0] = 0x0fu;
            selection->machine_startup_handoff.instruction_bytes[1] = 0x0bu;
            selection->machine_startup_handoff.cs = 0xca80u;
            selection->machine_startup_handoff.ip = 3u;
        }
    }
    if (selection != NULL && document->has_host_drive_inventory) {
        selection->has_host_drive_inventory = 1u;
        memcpy(selection->host_drive_inventory.types,
            document->host_drive_inventory.types,
            sizeof(selection->host_drive_inventory.types));
    }
    if (selection != NULL && (is_v2 || is_v3 || is_v4 || is_v5 || is_v6 || is_v7 || is_v8)) {
        if (!ascii_to_wide(document->command_placement.path,
                selection->command_placement.path,
                sizeof(selection->command_placement.path) /
                sizeof(selection->command_placement.path[0])))
            return BYOB_PROFILE_FORMAT_INVALID;
        selection->command_placement.drive_index =
            document->command_placement.drive_index;
        selection->has_command_placement = 1u;
        if (!ascii_to_wide(document->config_file.path, selection->config_file.path,
                sizeof(selection->config_file.path) / sizeof(selection->config_file.path[0])) ||
            !ascii_to_wide(document->autoexec_file.path, selection->autoexec_file.path,
                sizeof(selection->autoexec_file.path) / sizeof(selection->autoexec_file.path[0])))
            return BYOB_PROFILE_FORMAT_INVALID;
        selection->config_file.materialization = BYOB_GUEST_BOOT_FILE_MINIMAL_COMMENT_V1;
        selection->autoexec_file.materialization = BYOB_GUEST_BOOT_FILE_EMPTY_V1;
        selection->has_guest_boot_files = 1u;
    }
    for (index = 0u; index < document->component_count; ++index) {
        const byob_component *component = &document->components[index];
        const char *canonical = canonical_file_name(component->role);
        wchar_t name[64], hash[65];
        size_t prior;
        unsigned int feature;
        if (strcmp(component->role, "target") == 0 &&
            (is_v3 || is_v4 || is_v5 || is_v6 || is_v7 || is_v8)) {
            /* A target is an explicitly placed, identity-checked guest artifact,
             * not a synthetic TARGET.COM/TARGET.EXE smoke-image name. */
            canonical = component->file_name;
            target_component = component;
        }
        if (strcmp(component->role, "terminal-quit") == 0 && is_v5) {
            canonical = component->file_name;
            terminal_quit_component = component;
        }
        if (canonical == NULL) return component->required ?
            BYOB_PROFILE_ROLE_MISSING_OR_DUPLICATE : BYOB_PROFILE_FEATURE_DECLARATION_INVALID;
        feature = role_feature(component->role);
        if ((feature == 0u && !component->required) ||
            (feature != 0u && (document->features & feature) == 0u)) return BYOB_PROFILE_FEATURE_DECLARATION_INVALID;
        if (component->bytes == 0u || !sha256_lowercase(component->sha256)) {
            return BYOB_PROFILE_FORMAT_INVALID;
        }
        if (strcmp(component->file_name, canonical) != 0 ||
            !ascii_to_wide(component->file_name, name, sizeof(name) / sizeof(name[0])) ||
            !ascii_to_wide(component->sha256, hash, sizeof(hash) / sizeof(hash[0])) || !byob_component_name_safe(name)) {
            return BYOB_PROFILE_COMPONENT_NAME_UNSAFE;
        }
        for (prior = 0u; prior < index; ++prior) {
            if (strcmp(component->role, document->components[prior].role) == 0 ||
                _stricmp(component->file_name, document->components[prior].file_name) == 0) {
                return BYOB_PROFILE_ROLE_MISSING_OR_DUPLICATE;
            }
        }
        if (strcmp(component->role, "ntio") == 0 && component->required) required_roles |= 1u;
        if (strcmp(component->role, "ntdos") == 0 && component->required) required_roles |= 2u;
        if (strcmp(component->role, "command") == 0 && component->required) required_roles |= 4u;
        if (strcmp(component->role, "target") == 0 && component->required) required_roles |= 8u;
        if (strcmp(component->role, "terminal-quit") == 0 && component->required) required_roles |= 16u;
    }
    if (required_roles != (is_v5 ? 31u : (is_v8 || is_v7 || is_v6 || is_v3 || is_v4 ? 15u : 7u))) return BYOB_PROFILE_ROLE_MISSING_OR_DUPLICATE;
    if ((is_v3 || is_v4) && (target_component == NULL ||
        document->target_placement.path[0] != '\\' ||
        strcmp(document->target_placement.path + 1, target_component->file_name) != 0 ||
        document->target_placement.drive_index != document->command_placement.drive_index))
        return BYOB_PROFILE_FORMAT_INVALID;
    if (is_v5 && (target_component == NULL || terminal_quit_component == NULL ||
        strcmp(terminal_quit_component->file_name, "QUIT.COM") != 0 ||
        terminal_quit_component->bytes != 3u ||
        strcmp(terminal_quit_component->sha256,
            "06a37dff559df7325de8b003f4df53c188f733e0ca312aad961c34dae48d7b83") != 0 ||
        document->declared_targets[0].placement.path[0] != '\\' ||
        strcmp(document->declared_targets[0].placement.path + 1, target_component->file_name) != 0 ||
        strcmp(document->declared_targets[1].placement.path, "\\QUIT.COM") != 0 ||
        document->declared_targets[0].placement.drive_index != document->command_placement.drive_index ||
        document->declared_targets[1].placement.drive_index != document->command_placement.drive_index))
        return BYOB_PROFILE_FORMAT_INVALID;
    if ((is_v6 || is_v7 || is_v8) && (target_component == NULL ||
        document->declared_targets[0].placement.path[0] != '\\' ||
        strcmp(document->declared_targets[0].placement.path + 1, target_component->file_name) != 0 ||
        document->declared_targets[0].placement.drive_index != document->command_placement.drive_index))
        return BYOB_PROFILE_FORMAT_INVALID;
    for (index = 0u; index < document->component_count; ++index) {
        const byob_component *component = &document->components[index];
        wchar_t name[64], hash[65];
        byob_identity_result identity;
        (void)ascii_to_wide(component->file_name, name, sizeof(name) / sizeof(name[0]));
        (void)ascii_to_wide(component->sha256, hash, sizeof(hash) / sizeof(hash[0]));
        identity = byob_verify_component(root, name, component->bytes, hash);
        if (identity == BYOB_IDENTITY_NAME_UNSAFE) return BYOB_PROFILE_COMPONENT_NAME_UNSAFE;
        if (identity == BYOB_IDENTITY_OPEN_REJECTED) return BYOB_PROFILE_COMPONENT_OPEN_REJECTED;
        if (identity != BYOB_IDENTITY_OK) return BYOB_PROFILE_COMPONENT_IDENTITY_MISMATCH;
        if (selection != NULL) {
            if (strcmp(component->role, "ntio") == 0) descriptor_copy(&selection->ntio, component);
            if (strcmp(component->role, "ntdos") == 0) descriptor_copy(&selection->ntdos, component);
            if (strcmp(component->role, "command") == 0) descriptor_copy(&selection->command, component);
            if (strcmp(component->role, "target") == 0) descriptor_copy(&selection->target, component);
            if (strcmp(component->role, "terminal-quit") == 0) descriptor_copy(&selection->terminal_quit, component);
        }
    }
    if (selection != NULL && (is_v3 || is_v4)) {
        if (!ascii_to_wide(document->target_placement.path, selection->target_placement.path,
                sizeof(selection->target_placement.path) / sizeof(selection->target_placement.path[0])))
            return BYOB_PROFILE_FORMAT_INVALID;
        selection->target_placement.drive_index = document->target_placement.drive_index;
        selection->has_target_placement = 1u;
    }
    if (selection != NULL && (is_v4 || is_v5 || is_v6 || is_v7 || is_v8)) {
        selection->has_guest_search_metadata = 1u;
        selection->command_metadata.attributes = document->command_metadata.attributes;
        selection->command_metadata.dos_time = document->command_metadata.dos_time;
        selection->command_metadata.dos_date = document->command_metadata.dos_date;
        selection->target_metadata.attributes = document->target_metadata.attributes;
        selection->target_metadata.dos_time = document->target_metadata.dos_time;
        selection->target_metadata.dos_date = document->target_metadata.dos_date;
        selection->config_metadata.attributes = document->config_metadata.attributes;
        selection->config_metadata.dos_time = document->config_metadata.dos_time;
        selection->config_metadata.dos_date = document->config_metadata.dos_date;
        selection->autoexec_metadata.attributes = document->autoexec_metadata.attributes;
        selection->autoexec_metadata.dos_time = document->autoexec_metadata.dos_time;
        selection->autoexec_metadata.dos_date = document->autoexec_metadata.dos_date;
    }
    if (selection != NULL && (is_v5 || is_v6 || is_v7 || is_v8)) {
        uint32_t slot;
        selection->has_guest_search_metadata = 1u;
        selection->terminal_quit_metadata.attributes = document->terminal_quit_metadata.attributes;
        selection->terminal_quit_metadata.dos_time = document->terminal_quit_metadata.dos_time;
        selection->terminal_quit_metadata.dos_date = document->terminal_quit_metadata.dos_date;
        selection->declared_target_count = is_v5 ? 2u : 1u;
        for (slot = 0u; slot < selection->declared_target_count; ++slot) {
            const byob_component_descriptor *component = slot == 0u ? &selection->target : &selection->terminal_quit;
            selection->declared_targets[slot].component = *component;
            selection->declared_targets[slot].terminal = slot == 1u ? 1u : 0u;
            if (!ascii_to_wide(document->declared_targets[slot].placement.path,
                    selection->declared_targets[slot].placement.path,
                    BYOB_PROFILE_GUEST_PATH_MAX_CHARS)) return BYOB_PROFILE_FORMAT_INVALID;
            selection->declared_targets[slot].placement.drive_index =
                document->declared_targets[slot].placement.drive_index;
            selection->declared_targets[slot].metadata = slot == 0u ? selection->target_metadata :
                selection->terminal_quit_metadata;
        }
        /* The v5 sequence supersedes guest_target_placement, but the runner's
         * fixed DOS-engine gate still consumes this compatibility projection.
         * It is exactly slot zero, not a second target declaration. */
        selection->target_placement = selection->declared_targets[0].placement;
        selection->has_target_placement = 1u;
    }
    if (selection != NULL && (is_v7 || is_v8))
        selection->guest_display_state = BYOB_GUEST_DISPLAY_STATE_STREAM_IO_V1;
    return BYOB_PROFILE_ACCEPTED;
}

byob_profile_result byob_profile_validate_file_select(const wchar_t *manifest_path,
    const wchar_t *byob_root, byob_profile_selection *out_selection)
{
    HANDLE file = INVALID_HANDLE_VALUE;
    BY_HANDLE_FILE_INFORMATION info;
    LARGE_INTEGER size;
    DWORD read = 0u;
    char *bytes = NULL;
    byob_profile_document document;
    byob_profile_result result = BYOB_PROFILE_FORMAT_INVALID;

    if (out_selection != NULL) memset(out_selection, 0, sizeof(*out_selection));
    if (manifest_path == NULL || byob_root == NULL || *manifest_path == L'\0' || *byob_root == L'\0') return result;
    file = CreateFileW(manifest_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
        FILE_FLAG_OPEN_REPARSE_POINT, NULL);
    if (file == INVALID_HANDLE_VALUE || !GetFileInformationByHandle(file, &info) ||
        (info.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) != 0u ||
        !GetFileSizeEx(file, &size) || size.QuadPart <= 0 || size.QuadPart > BYOB_PROFILE_MAX_BYTES) goto done;
    bytes = (char *)HeapAlloc(GetProcessHeap(), 0, (SIZE_T)size.QuadPart + 1u);
    if (bytes == NULL || !ReadFile(file, bytes, (DWORD)size.QuadPart, &read, NULL) || read != (DWORD)size.QuadPart) goto done;
    bytes[read] = '\0';
    if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, bytes, (int)read, NULL, 0) == 0) goto done;
    if (!json_document(bytes, read, &document)) goto done;
    result = validate_document(&document, byob_root, out_selection);
    if (result != BYOB_PROFILE_ACCEPTED && out_selection != NULL) {
        memset(out_selection, 0, sizeof(*out_selection));
    }
done:
    if (bytes != NULL) HeapFree(GetProcessHeap(), 0, bytes);
    if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
    return result;
}

byob_profile_result byob_profile_validate_file(const wchar_t *manifest_path,
    const wchar_t *byob_root)
{
    return byob_profile_validate_file_select(manifest_path, byob_root, NULL);
}
