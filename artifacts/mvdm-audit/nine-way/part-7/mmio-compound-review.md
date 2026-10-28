# Part 7 RIFF compound-file source and ordinary MMIO selection

The owned `wow16/mmsystem/mmiocf.c` and `mmiocf.h` describe RIFF compound-file CTOC/CGRP functions. For example `mmioCFOpen` at lines 45–49 is an original TODO stub returning NULL, despite its preceding description. That observation applies to this source body; it is not evidence that the selected product returns NULL for ordinary RIFF MMIO or that this stub is callable in the delivered DLL.

Original `mmsystem/makefile:12-18,190-191,235-237` selects `mmio.obj` and `mmioriff.obj` and does not list `mmiocf.obj`. `mmsystem.def:211-225` exports ordinary MMIOOPEN, MMIOCLOSE and MMIODESCEND; `mmio.c` and `mmioriff.c` supply their source-level implementations. This is a concrete reason to distinguish ordinary RIFF from compound-file functionality rather than describe the full multimedia provider as a stub. The original recipe alone does not prove exact delivered NE provenance, all configurations, callable ordinals, file handles, failure rollback, close/refcount or runtime behavior.

Suggested classification for owned mmiocf source/header: retained original, historical incomplete compound-file implementation, not selected by the shown recipe; keep separate pending capability/consumer review, do not mark ordinary MMIO complete or compound-file absence a proven product regression. No guest execution or modification.
