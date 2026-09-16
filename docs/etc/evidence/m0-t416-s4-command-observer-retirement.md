# T416 S4 — COMMAND observer retirement

## Question

Can the default-off COMMAND tracing introduced in mirror dispatch and command
retrieval be removed without changing original command delivery, redirection,
broker requests or guest-memory binding?

## Result

The first retired cohort has no remaining source, test or tool references:
record_command_call, record_command_vdm_result, record_command_guest_return,
record_command_exit_policy, record_command_vdm_record and
record_command_environment_return_code. Their mirror call sites,
implementations and declarations are removed.

cmddisp.c now differs from the original only by the existing
restore_child_report_paths boundary after SVC_GETINITENVIRONMENT. That call
remains because non-COMMAND host-only observation selectors still use the same
child-environment isolation route; it is not part of the removed command
recorder cohort.

## Verification

- A full source/test/tool symbol sweep produced zero hits for all six retired
  recorder names.
- The pinned OpenNT comparison confirms the COMMAND dispatch table and call
  order are original apart from the retained child-selector restoration seam.
- git diff --check and documentation governance pass.

No build claim is made at this intermediate delivery. The remaining
COMMAND-environment/stub observer bodies and continuation-path storage are the
next coherent deletion group and require their own post-removal x86 gate.
