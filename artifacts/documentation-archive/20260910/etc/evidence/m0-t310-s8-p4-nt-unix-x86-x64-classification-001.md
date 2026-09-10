# M0 T310 S8 P4: `nt_unix` x86/x64 Classification

The complete original `softpc.new/host/src/nt_unix.c` unit was rebuilt on x64
and x86 and read with its reached declarations.

- K&R warnings for `host_get_system_error`, `host_memset`, `host_time`,
  `host_file_is_char_dev` and `host_find_file` occur on both host targets.
  Their value classes are fixed `char`/`int`/Windows `long` or private native
  pointers; none converts a pointer, handle or guest identity through a
  fixed-width carrier.
- The two C4057 records are signed `char` versus `BYTE` diagnostics on a
  private diagnostic buffer. They are not pointer-width diagnostics.
- Existing `MVDM-HOST-DIV-032` and `...037` already retain the source-selected
  time and session-firmware contracts without a width loss.

Per the P4 rule, these visible non-x86/x64 warnings are intentionally retained;
no source cleanup or suppression was introduced.
