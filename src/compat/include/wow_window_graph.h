#ifndef COMPAT_WOW_WINDOW_GRAPH_H
#define COMPAT_WOW_WINDOW_GRAPH_H

/* Copied original-client relations: full generation-bearing native identities,
 * never guest addresses or pointers to private USER objects. */
typedef struct mvdm_softpc_wow_window_links {
    unsigned long window, next, parent, child, owner;
} mvdm_softpc_wow_window_links;

/* Counted class query bytes are copied during this synchronous call. They
 * neither register native classes nor carry native pointers into guest state. */
typedef struct mvdm_softpc_wow_window_descriptor {
    unsigned short atom;
    unsigned long class_style, window_extra, name_bytes;
    char name[256];
} mvdm_softpc_wow_window_descriptor;
/* Call-local copy of the committed view. Numeric rectangles/styles retain
 * the original x86 meaning, not a modern USER pointer/layout. The caller
 * serializes two-pass sizing/copy with guest execution and owns the arrays.
 * An empty class descriptor denotes the not-yet-observed desktop root. */
typedef struct mvdm_softpc_wow_window_observation {
    mvdm_softpc_wow_window_links links;
    mvdm_softpc_wow_window_descriptor descriptor;
    long window_rect[4], client_rect[4];
    unsigned long style, exstyle, control_id;
    unsigned long process_id, thread_id, module;
    unsigned long context_help_id; /* Native scalar, retained with this generation. */
} mvdm_softpc_wow_window_observation;

/* Caller supplies 1..65535 copied rows and a zeroed 65536-slot index.
 * Success fills slot -> row+1 and preserves LastError. Failure sets LastError;
 * the partial index is discarded with the caller's uncommitted transaction.
 * This checks structure only, not native existence or reciprocal links. */
int compat_wow_window_graph_validate(unsigned long count,
    const mvdm_softpc_wow_window_links *links, unsigned long *index,
    unsigned long desktop);

#endif
