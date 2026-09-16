#ifndef MVDM_IMAGE_CLASSIFICATION_H
#define MVDM_IMAGE_CLASSIFICATION_H

#include <stddef.h>
#include <windows.h>

/* The historical process-creation boundary distinguishes only images which
 * require an MVDM session from native Windows images.  The enum is host-side
 * adapter state: no value crosses into VDMINFO or guest memory. */
typedef enum mvdm_image_kind {
    MVDM_IMAGE_UNKNOWN = 0,
    MVDM_IMAGE_DOS = 1,
    MVDM_IMAGE_WIN16 = 2,
    MVDM_IMAGE_NATIVE = 3
} mvdm_image_kind;

/* Static recognition at the direct CLI/BaseVDM boundary. It recognizes DOS
 * COM/MZ, NE and all PE variants; the Windows loader remains authoritative
 * for native support. Guest COMMAND EXEC retains its original classifier. */
mvdm_image_kind mvdm_image_classify_path(const char *path);

/* Classify the executable token of one ordinary CLI command line.  This is
 * not a COMMAND parser: shell operators are rejected and remain unavailable
 * at the direct product-entry boundary. */
mvdm_image_kind mvdm_image_classify_command_line(const char *command_line);

/* Resolve one ordinary executable token using DOS command-name extension
 * order (.COM, .EXE, .BAT) and the current directory/PATH search supplied by
 * SearchPath.  The result is a copied, quoted resolved image plus the original
 * copied argument tail.  A zero result means that no executable image was
 * resolved; it is deliberately not a shell-parser failure. */
int mvdm_image_resolve_command_line(const char *command_line,
    char *resolved_command_line, size_t resolved_command_line_bytes,
    mvdm_image_kind *image_kind_out);

/* A direct CLI image which is neither DOS nor Win16 is not an MVDM request.
 * Let public Windows process creation provide the authoritative native or
 * invalid-image result. Preserve inherited console, environment and current
 * directory; shell syntax is deliberately not interpreted here. */
int mvdm_image_launch_native(char *command_line, DWORD *exit_code_out,
    DWORD *failure_out);

/* An unresolvable direct product token can still be a host command built-in
 * such as VER. Preserve that host-shell contract through COMSPEC /c rather
 * than rejecting it before the shell receives the original text. */
int mvdm_image_launch_shell_command(const char *command_line,
    DWORD *exit_code_out, DWORD *failure_out);

#endif
