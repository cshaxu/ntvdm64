# WOW cross-language declaration roles

The selected original C headers `wowcmpat.h` and `wowusr.h` remain partial.
The [paired review](shard-1/wow-cross-language-constants-review.md) records
matching original ASM/C numeric definitions and normalized compiler
dependencies, but does not prove structure packing, live guest image identity,
table-slot semantics or API and callback lifetime.

The corresponding guest ASM include files remain pending: a lack of host C
dependencies does not establish their guest selection or irrelevance. The 16
differences between constant names and table labels require contextual review;
aliases and non-dispatch uses mean those names alone do not prove a defect.
No guest or product image was modified or run.
