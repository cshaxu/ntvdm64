#include "app/launch_declaration.h"
#include "session/session.h"

#include <ctype.h>
#include <string.h>

void app_launch_declaration_initialize(app_launch_declaration *declaration)
{
    if (declaration == NULL) return;
    memset(declaration, 0, sizeof(*declaration));
    base_vdm_local_initialize(&declaration->base_vdm);
}

int app_launch_declaration_consume_options(app_launch_declaration *declaration,
    int *argc, char **argv)
{
    int read_index;
    int write_index;

    if (declaration == NULL || argc == NULL || argv == NULL || *argc < 1)
        return 0;
    write_index = 1;
    for (read_index = 1; read_index < *argc; ++read_index) {
        if (strcmp(argv[read_index], "--ordinary-child") == 0) {
            if (declaration->ordinary_child != 0u) return 0;
            declaration->ordinary_child = 1u;
            continue;
        }
        argv[write_index++] = argv[read_index];
    }
    *argc = write_index;
    return 1;
}

int app_launch_declaration_bind(app_launch_declaration *declaration,
    session *owner)
{
    if (declaration == NULL || declaration->bound != 0u ||
        !base_vdm_local_valid(&declaration->base_vdm)) return 0;
    if (!base_vdm_local_bind(&declaration->base_vdm, owner)) return 0;
    declaration->bound = 1u;
    return 1;
}

static int append_text(char *destination, size_t capacity, size_t *length,
    const char *source)
{
    size_t source_length;

    if (destination == NULL || length == NULL || source == NULL) return 0;
    source_length = strlen(source);
    if (*length > capacity || source_length >= capacity - *length) return 0;
    memcpy(destination + *length, source, source_length);
    *length += source_length;
    destination[*length] = '\0';
    return 1;
}

static int make_path(char *destination, size_t capacity, const char *root,
    const char *tail)
{
    size_t length = 0u;

    if (!append_text(destination, capacity, &length, root)) return 0;
    if (length != 0u && destination[length - 1u] != '\\' &&
        destination[length - 1u] != '/') {
        if (!append_text(destination, capacity, &length, "\\")) return 0;
    }
    return append_text(destination, capacity, &length, tail);
}

int app_launch_declaration_publish(app_launch_declaration *declaration,
    const session *owner)
{
    base_vdm_command command;
    const char *root;
    size_t command_length = 0u;
    size_t environment_length = 0u;
    size_t directory_length;
    unsigned char drive_letter;

    if (declaration == NULL || declaration->bound == 0u ||
        declaration->ordinary_child == 0u || owner == NULL ||
        !session_valid(owner) || owner->state != SESSION_STATE_ACTIVE ||
        declaration->base_vdm.available != 0u) return 0;
    root = session_dos_media_root(owner);
    if (root == NULL || root[0] == '\0' || root[1] != ':') return 0;
    drive_letter = (unsigned char)toupper((unsigned char)root[0]);
    if (drive_letter < 'A' || drive_letter > 'Z') return 0;
    if (!make_path(declaration->application, sizeof(declaration->application),
            root, "COMMAND.COM") ||
        !append_text(declaration->command, sizeof(declaration->command),
            &command_length, declaration->application) ||
        !append_text(declaration->command, sizeof(declaration->command),
            &command_length, " /C VER") ||
        !append_text(declaration->environment, sizeof(declaration->environment),
            &environment_length, "COMSPEC=") ||
        !append_text(declaration->environment, sizeof(declaration->environment),
            &environment_length, declaration->application)) return 0;
    if (environment_length + 1u >= sizeof(declaration->environment)) return 0;
    declaration->environment[environment_length++] = '\0';
    declaration->environment[environment_length] = '\0';
    if (!append_text(declaration->environment, sizeof(declaration->environment),
            &environment_length, "PATH=") ||
        !append_text(declaration->environment, sizeof(declaration->environment),
            &environment_length, root)) return 0;
    if (environment_length + 2u > sizeof(declaration->environment)) return 0;
    declaration->environment[environment_length++] = '\0';
    declaration->environment[environment_length++] = '\0';
    directory_length = strlen(root);
    if (directory_length + 1u > sizeof(declaration->current_directory)) return 0;
    memcpy(declaration->current_directory, root, directory_length + 1u);

    memset(&command, 0, sizeof(command));
    command.struct_bytes = (uint32_t)sizeof(command);
    command.task = 1u;
    command.code_page = 437u;
    command.current_drive = (uint16_t)(drive_letter - 'A');
    command.command = (const uint8_t *)declaration->command;
    command.command_bytes = (uint16_t)(command_length + 1u);
    command.application = (const uint8_t *)declaration->application;
    command.application_bytes = (uint16_t)(strlen(declaration->application) + 1u);
    command.environment = (const uint8_t *)declaration->environment;
    command.environment_bytes = (uint32_t)environment_length;
    command.current_directory = (const uint8_t *)declaration->current_directory;
    command.current_directory_bytes = (uint16_t)(directory_length + 1u);
    return base_vdm_local_publish(&declaration->base_vdm, &command);
}
