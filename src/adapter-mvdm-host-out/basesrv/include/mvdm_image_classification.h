#ifndef MVDM_IMAGE_CLASSIFICATION_H
#define MVDM_IMAGE_CLASSIFICATION_H

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

/* A direct CLI native image is not an MVDM request.  Preserve inherited
 * console, environment and current directory and return its process exit
 * code.  Shell syntax is deliberately not interpreted here. */
int mvdm_image_launch_native(char *command_line, DWORD *exit_code_out,
    DWORD *failure_out);

#endif
