# T416 S4 — COMMAND observer retirement

## Question

Can the default-off COMMAND tracing introduced in mirror dispatch and command
retrieval be removed without changing original command delivery, redirection,
broker requests or guest-memory binding?

## Result

The complete COMMAND-only default-off observer family is retired. The first
cohort removed record_command_call, record_command_vdm_result,
record_command_guest_return, record_command_exit_policy,
record_command_vdm_record and record_command_environment_return_code. The
second removes the now-unreachable command-environment and resident-stub-table
recorders, their private continuation report path, declarations and all
tool/test acceptance of MVDM_COMMAND_CONTINUATION_REPORT_PATH.

cmddisp.c now differs from the original only by the existing
restore_child_report_paths boundary after SVC_GETINITENVIRONMENT. That call
remains because non-COMMAND host-only observation selectors still use the same
child-environment isolation route; it is not part of the removed command
recorder cohort.

## Verification

- A full source/test/tool symbol sweep produces zero hits for the eight
  retired recorder symbols, the private continuation storage and its
  environment selector.
- The pinned OpenNT comparison confirms the COMMAND dispatch table and call
  order are original apart from the retained child-selector restoration seam.
- The formal x86 CCPU40 graph recompiles cmddisp.c, cmdmisc.c and
  mvdm_softpc_termination.c successfully. The latter object reduces from
  49,358 to 44,412 bytes.
- git diff --check and documentation governance pass.

The retained mvdm_softpc_restore_child_report_paths seam serves the other
host-only observation selectors; it is not part of the retired COMMAND
family. No product behavior or original COMMAND control path was changed.
