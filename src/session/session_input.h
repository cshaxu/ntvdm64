#ifndef SESSION_INPUT_H
#define SESSION_INPUT_H

#include <stdint.h>

#define SESSION_INPUT_MAGIC UINT32_C(0x53494e50)
#define SESSION_INPUT_VERSION UINT32_C(1)
#define SESSION_INPUT_TARGET_BYTES 261u
#define SESSION_INPUT_ARGUMENT_BYTES 128u

/* A neutral, copied input/publish record.  It intentionally contains no
 * guest pointer, native HANDLE, BOP, DOS, VDM, Win32 or machine object. */
typedef struct session_input {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t ready;
    uint32_t delivered;
    uint32_t repeat_pending;
    uint16_t location;
    uint16_t text_code_page;
    char target[SESSION_INPUT_TARGET_BYTES];
    char arguments[SESSION_INPUT_ARGUMENT_BYTES];
    uint32_t environment_bytes;
    char *environment;
    uint32_t transformed_environment_bytes;
    char *transformed_environment;
    uint32_t published_directories_bytes;
    char *published_directories;
} session_input;

void session_input_initialize(session_input *input);
void session_input_dispose(session_input *input);
int session_input_valid(const session_input *input);
int session_input_set_startup(session_input *input, const char *target,
    const char *arguments, uint16_t location, uint16_t text_code_page);
int session_input_set_environment(session_input *input, const char *environment,
    uint32_t bytes);
int session_input_set_transformed_environment(session_input *input,
    const char *environment, uint32_t bytes);
int session_input_set_published_directories(session_input *input,
    const char *directories, uint32_t bytes);

#endif
