# M0 T458 S3 — original accelerator conversion and retirement

**Status:** closed original-provider boundary; no production or guest change.

`waccel.c` is the sole accelerator authority: it converts the packed Win16
five-byte records, assigns aliases to `CURRENTPTD()->htask16`, and destroys the
native table during that task's retirement.  `wumsg.c::WU32TranslateAccelerator`
only resolves the original alias and delegates delivery to public USER32;
T447 remains the exclusive MENU-01 graph/parser/mutation/owner-draw owner.

The extracted-current-source fixture in `build/t458-s3/accelerator-r6` passes:

* `WOW_ACCEL_CREATE32 errors=0 entries=2 partial_tail=1` proves the original
  16-bit record conversion and its intentional truncation of a partial tail.
  The Win16 final-record 0x80 sentinel delimits source data; USER32's public
  `HACCEL` deliberately stores only public virtual-key flags.
* `WOW_ACCEL_ALIAS_RETIRE errors=0 flags=2 destroys=2` proves two task-local
  original aliases, current-task-only retirement, cursor-icon free marking,
  and exactly-one native table destruction per alias.

S2's formal x86 graph remains current and selects the same unchanged original
provider.  No accelerator cache, menu graph, parser, or replacement dispatch
was introduced.  S4 owns the finite consumer/application observation; a
consumer failure must be traced to T447 MENU-01 or the original dispatch path,
not repaired by a parallel accelerator owner.
