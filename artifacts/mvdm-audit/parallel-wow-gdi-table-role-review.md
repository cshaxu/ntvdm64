# Original WOW GDI core and thunk table owners

The [GDI core](shard-0/wow-gdi-core-review.md) and [combined thunk table](shard-1/wow-table-boundary-review.md)
match both original source editions and are classified partial. Their initial
published-image bindings cover 119 GDI table slots and ten module boundaries,
respectively. These counts do not establish guest execution, full API
behavior or a percentage of restored functionality. Guest initialization,
GDI resources, callbacks, errors and task retirement remain open.

No product or guest image changed in this source-audit checkpoint.
