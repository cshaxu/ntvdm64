#ifndef COMPAT_WOW_WINDOW_GRAPH_H
#define COMPAT_WOW_WINDOW_GRAPH_H

/* Copied original-client relations: full generation-bearing native identities,
 * never guest addresses or pointers to private USER objects. */
typedef struct mvdm_softpc_wow_window_links {
    unsigned long window, next, parent, child, owner;
} mvdm_softpc_wow_window_links;

/* Caller supplies 1..65535 copied rows and a zeroed 65536-slot index.
 * Success fills slot -> row+1 and preserves LastError. Failure sets LastError;
 * the partial index is discarded with the caller's uncommitted transaction.
 * This checks structure only, not native existence or reciprocal links. */
int compat_wow_window_graph_validate(unsigned long count,
    const mvdm_softpc_wow_window_links *links, unsigned long *index,
    unsigned long desktop);

#endif
