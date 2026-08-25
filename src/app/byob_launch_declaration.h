#ifndef BYOB_LAUNCH_DECLARATION_H
#define BYOB_LAUNCH_DECLARATION_H

#include <stdint.h>
#include <wchar.h>

/* COMMAND.COM's 128-byte UCOMBUF must also contain TARGET, a separator and
 * CR/LF. The source-shaped contained-command maximum is therefore 118. */
#define BYOB_LAUNCH_DECLARATION_TAIL_BYTES 118u

typedef enum byob_launch_target_kind {
    BYOB_LAUNCH_TARGET_KIND_COM = 1,
    BYOB_LAUNCH_TARGET_KIND_EXE = 2,
    BYOB_LAUNCH_TARGET_KIND_BAT = 3,
    BYOB_LAUNCH_TARGET_KIND_PIF = 4
} byob_launch_target_kind;

/* Copied launch metadata.  It intentionally has no host path, handle, guest
 * address, guest pointer, Bochs object or adapter service selector. */
typedef struct byob_launch_declaration {
    uint32_t version;
    uint32_t target_kind;
    uint32_t tail_bytes;
    uint8_t tail[BYOB_LAUNCH_DECLARATION_TAIL_BYTES];
} byob_launch_declaration;

int byob_launch_declaration_from_arguments(
    byob_launch_declaration *out_value, const wchar_t *target_file_name,
    int argument_count, wchar_t *const *arguments);
int byob_launch_declaration_to_environment(
    const byob_launch_declaration *value, wchar_t *kind, uint32_t kind_capacity,
    wchar_t *tail, uint32_t tail_capacity);
int byob_launch_declaration_from_environment(
    byob_launch_declaration *out_value, const wchar_t *kind, const wchar_t *tail);

#endif
