#include "app/launch_declaration.h"
#include "session/session.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static int append_text(char *destination, size_t capacity, size_t *length,
    const char *source);
static int split_target_command(app_launch_declaration *declaration,
    size_t *command_length);

void app_launch_declaration_initialize(app_launch_declaration *declaration)
{
    if (declaration == NULL) return;
    memset(declaration, 0, sizeof(*declaration));
    base_vdm_local_initialize(&declaration->base_vdm);
    base_vdm_broker_initialize(&declaration->broker);
}

static int is_softpc_option(const char *argument)
{
    return argument != NULL && (argument[0] == '-' || argument[0] == '/') &&
        argument[1] != '\0';
}

static int append_dos_argument(char *destination, size_t capacity,
    size_t *length, const char *argument)
{
    int quote;

    if (argument == NULL || argument[0] == '\0' ||
        strpbrk(argument, "\r\n\"") != NULL) return 0;
    quote = strpbrk(argument, " \t") != NULL;
    if (quote && !append_text(destination, capacity, length, "\"")) return 0;
    if (!append_text(destination, capacity, length, argument)) return 0;
    return !quote || append_text(destination, capacity, length, "\"");
}

static int set_positional_command(app_launch_declaration *declaration,
    int argc, char **argv, int start_index)
{
    int index;
    size_t length = 0u;

    if (declaration == NULL || argv == NULL || start_index < 1 ||
        start_index >= argc) return 0;
    declaration->requested_command[0] = '\0';
    if (start_index + 1 == argc) {
        size_t bytes = strlen(argv[start_index]);
        if (bytes == 0u || bytes >= sizeof(declaration->requested_command) ||
            strpbrk(argv[start_index], "\r\n") != NULL) return 0;
        memcpy(declaration->requested_command, argv[start_index], bytes + 1u);
        declaration->command_declared = 1u;
        return 1;
    }
    for (index = start_index; index < argc; ++index) {
        if (index != start_index &&
            !append_text(declaration->requested_command,
                sizeof(declaration->requested_command), &length, " ")) return 0;
        if (!append_dos_argument(declaration->requested_command,
                sizeof(declaration->requested_command), &length, argv[index]))
            return 0;
    }
    declaration->command_declared = 1u;
    return 1;
}

int app_launch_declaration_consume_options(app_launch_declaration *declaration,
    int *argc, char **argv)
{
    int read_index;
    int write_index;
    int original_argc;
    int positional_start = -1;

    if (declaration == NULL || argc == NULL || argv == NULL || *argc < 1)
        return 0;
    original_argc = *argc;
    write_index = 1;
    for (read_index = 1; read_index < original_argc; ++read_index) {
        if (strcmp(argv[read_index], "--command") == 0) {
            size_t command_bytes;
            if (declaration->command_declared != 0u ||
                positional_start >= 0 || read_index + 1 >= original_argc) return 0;
            command_bytes = strlen(argv[read_index + 1]);
            if (command_bytes == 0u || command_bytes >=
                    sizeof(declaration->requested_command) ||
                strpbrk(argv[read_index + 1], "\r\n") != NULL) return 0;
            memcpy(declaration->requested_command, argv[read_index + 1],
                command_bytes + 1u);
            declaration->command_declared = 1u;
            ++read_index;
            continue;
        }
        if (positional_start >= 0) continue;
        if (is_softpc_option(argv[read_index])) {
            argv[write_index++] = argv[read_index];
            continue;
        }
        if (declaration->command_declared != 0u) return 0;
        positional_start = read_index;
    }
    if (positional_start >= 0 &&
        !set_positional_command(declaration, original_argc, argv,
            positional_start)) return 0;
    *argc = write_index;
    return 1;
}

mvdm_image_kind app_launch_declaration_requested_image(
    const app_launch_declaration *declaration)
{
    if (declaration == NULL || declaration->command_declared == 0u)
        return MVDM_IMAGE_UNKNOWN;
    return mvdm_image_classify_command_line(declaration->requested_command);
}

int app_launch_declaration_resolve_requested_command(
    app_launch_declaration *declaration)
{
    char resolved[MAXIMUM_VDM_COMMAND_LENGTH];
    mvdm_image_kind image_kind;

    if (declaration == NULL || declaration->command_declared == 0u) return 0;
    if (!mvdm_image_resolve_command_line(declaration->requested_command,
            resolved, sizeof(resolved), &image_kind)) return 0;
    memcpy(declaration->requested_command, resolved, strlen(resolved) + 1u);
    declaration->command_resolved = 1u;
    return 1;
}

int app_launch_declaration_prepare_softpc_arguments(int argc, char **argv,
    int *softpc_argc, char ***softpc_argv)
{
    static char foreground_option[] = "-f";
    char **forwarded;
    int index;
    int has_foreground = 0;

    if (argc < 1 || argv == NULL || softpc_argc == NULL ||
        softpc_argv == NULL) return 0;
    forwarded = (char **)calloc((size_t)argc + 2u, sizeof(*forwarded));
    if (forwarded == NULL) return 0;
    for (index = 0; index < argc; ++index) {
        forwarded[index] = argv[index];
        if (is_softpc_option(argv[index]) && tolower((unsigned char)argv[index][1]) == 'f')
            has_foreground = 1;
    }
    if (!has_foreground) forwarded[argc++] = foreground_option;
    *softpc_argc = argc;
    *softpc_argv = forwarded;
    return 1;
}

void app_launch_declaration_release_softpc_arguments(char **softpc_argv)
{
    free(softpc_argv);
}

int app_launch_declaration_bind(app_launch_declaration *declaration,
    session *owner)
{
    if (declaration == NULL || declaration->bound != 0u ||
        !base_vdm_local_valid(&declaration->base_vdm)) return 0;
    if (!base_vdm_local_bind(&declaration->base_vdm, owner)) return 0;
    /* DIVERGENCE(APP-DIV-017): a standalone CLI has no NT4 CSRSS command
     * producer after its declared launch has returned. The local broker
     * therefore ends only at the next original BaseVDM request after the
     * copied DOS record was consumed. The first COMMAND remains PermCom;
     * it is never started with `/C`. */
    if (!base_vdm_local_set_terminal_on_command_exhaustion(
            &declaration->base_vdm, 1)) {
        (void)base_vdm_local_unbind(&declaration->base_vdm);
        return 0;
    }
    /* The current app has a single local broker instance.  Its stable broker
     * identity is not a guest or host pointer; a later public transport may
     * assign this value externally without changing the original MVDM caller. */
    if (!base_vdm_broker_bind(&declaration->broker, owner, 1u)) {
        (void)base_vdm_local_unbind(&declaration->base_vdm);
        return 0;
    }
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

/* `cmdGetNextCmd` copies VDMINFO.AppName to COMMAND's EXEC path and appends
 * VDMINFO.CmdLine as that target's tail. Preserve that division instead of
 * inventing `COMMAND.COM /C <target>` in the app composition layer. */
static int split_target_command(app_launch_declaration *declaration,
    size_t *command_length)
{
    const char *source;
    size_t image_length = 0u;
    size_t index = 0u;
    int quoted = 0;

    if (declaration == NULL || command_length == NULL) return 0;
    declaration->target_application[0] = '\0';
    declaration->command[0] = '\0';
    *command_length = 0u;
    if (declaration->command_declared == 0u) {
        /* A bare executable remains a bounded one-shot child of PermCom.
         * This is not a wrapper for an application supplied by the user. */
        if (!append_text(declaration->target_application,
                sizeof(declaration->target_application), &image_length,
                declaration->application) ||
            !append_text(declaration->command, sizeof(declaration->command),
                command_length, "/C")) return 0;
    } else {
        source = declaration->requested_command;
        if (source[0] == '\"') { quoted = 1; ++index; }
        while (source[index] != '\0') {
            if ((quoted && source[index] == '\"') ||
                (!quoted && (source[index] == ' ' || source[index] == '\t')))
                break;
            if (image_length + 1u >= sizeof(declaration->target_application)) return 0;
            declaration->target_application[image_length++] = source[index++];
        }
        if (quoted) {
            if (source[index] != '\"') return 0;
            ++index;
        }
        if (image_length == 0u) return 0;
        declaration->target_application[image_length] = '\0';
        while (source[index] == ' ' || source[index] == '\t') ++index;
        if (!append_text(declaration->command, sizeof(declaration->command),
                command_length, source + index)) return 0;
    }
    return append_text(declaration->command, sizeof(declaration->command),
        command_length, "\r\n");
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

    if (declaration == NULL || declaration->bound == 0u || owner == NULL ||
        !session_valid(owner) || owner->state != SESSION_STATE_ACTIVE ||
        declaration->base_vdm.available != 0u) return 0;
    root = session_mvdm_system_root(owner);
    if (root == NULL || root[0] == '\0' || root[1] != ':') return 0;
    drive_letter = (unsigned char)toupper((unsigned char)root[0]);
    if (drive_letter < 'A' || drive_letter > 'Z') return 0;
    if (!make_path(declaration->application, sizeof(declaration->application),
            root, "system32\\COMMAND.COM") ||
        !make_path(declaration->pif, sizeof(declaration->pif), root,
            "profiles\\pure-dos\\pure-dos.pif") ||
        !append_text(declaration->environment, sizeof(declaration->environment),
            &environment_length, "COMSPEC=") ||
         !append_text(declaration->environment, sizeof(declaration->environment),
             &environment_length, declaration->application)) return 0;
    if (!split_target_command(declaration, &command_length)) return 0;
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
    /* The original BaseSrv chooses a DOS record independently from a WOW
     * record before it copies VDMINFO.  Preserve that existing discriminant
     * at the sole app-owned initial declaration boundary. */
    command.command_owner = BASE_VDM_COMMAND_DOS;
    command.command = (const uint8_t *)declaration->command;
    /* Original cmdGetNextCmd treats VDMINFO.CmdLine as a command line with
     * a mandatory CR/LF tail, followed by this transport NUL.  Keep that
     * BaseSrv/BaseClient contract at the app composition boundary rather
     * than making the original COMMAND body accept a new string shape. */
    command.command_bytes = (uint16_t)(command_length + 1u);
    command.application = (const uint8_t *)declaration->target_application;
    command.application_bytes = (uint16_t)(strlen(declaration->target_application) + 1u);
    /* DIVERGENCE(APP-DIV-017): the standalone product has one selected
     * source-shaped pure-DOS profile for every initial PermCom record. It
     * leaves the target identity and its argument tail in the original
     * AppName/CmdLine carriers, instead of selecting a target-name route. */
    command.pif = (const uint8_t *)declaration->pif;
    command.pif_bytes = (uint16_t)(strlen(declaration->pif) + 1u);
    command.environment = (const uint8_t *)declaration->environment;
    command.environment_bytes = (uint32_t)environment_length;
    command.current_directory = (const uint8_t *)declaration->current_directory;
    command.current_directory_bytes = (uint16_t)(directory_length + 1u);
    if (!base_vdm_broker_publish(&declaration->broker, &command)) return 0;
    return base_vdm_broker_deliver(&declaration->broker,
        &declaration->base_vdm) == BASE_VDM_BROKER_DELIVERY_COMPLETE;
}
