#ifndef WOW_PAGE_COST_DATA_H
#define WOW_PAGE_COST_DATA_H

/* Native diagnostic ABI only. No original USER/guest pointer is represented.
 * Durations are inclusive: nested kinds must not be summed as disjoint time. */
enum wow_page_cost_kind {
    PAGE_COST_CLASS, PAGE_COST_RETIRED, PAGE_COST_COPY, PAGE_COST_GRAPH,
    PAGE_COST_CLASS_SORT, PAGE_COST_VIEW_SORT, PAGE_COST_COUNT
};
typedef struct wow_page_cost_row {
    ULONGLONG calls, ticks, items, largest_items, longest_ticks;
} wow_page_cost_row;
typedef struct wow_page_cost_data {
    DWORD magic, version;
    volatile LONG sequence;
    DWORD reserved;
    ULONGLONG frequency;
    wow_page_cost_row rows[PAGE_COST_COUNT];
} wow_page_cost_data;
#define WOW_PAGE_COST_MAGIC 0x447c0571u

#endif
