/*
 * Same-shaped declaration bridge for historical SoftPC host error tables.
 *
 * DIVERGENCE: `softpc.new/host/inc/error.h` documents ERROR_STRUCT and the
 * EH and EV enumeration values but does not declare them. The exact compatible
 * definitions exist in `softpc.new/base/inc/error.h`.  Original host bodies
 * such as nt_ertbl.c require those declarations while preserving their source
 * spelling and table layout.  This adapter header is force-included only by
 * the modern host-control build island; it does not edit or replace either
 * original header.
 */
#ifndef ADAPTER_SOFTPC_ERROR_ABI_H
#define ADAPTER_SOFTPC_ERROR_ABI_H

typedef struct {
    char header;
    char varient;
} ERROR_STRUCT;

typedef enum {
    EH_WARNING = 0,
    EH_ERROR,
    EH_CONFIG,
    EH_INSTALL,
    EH_LAST
} base_error_headers;

typedef enum {
    EV_SIMPLE = 0,
    EV_EXTRA_CHAR,
    EV_BAD_FILE,
    EV_SYS_BAD_VALUE,
    EV_BAD_VALUE,
    EV_BAD_INPUT,
    EV_LAST
} base_error_varients;

#endif
