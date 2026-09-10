# M0 T394 S6 — selected direct-boundary extraction design

The selected S6 source change is deliberately narrow:

1. `session` receives the existing `terminate_current_session` and
   termination-origin contract, including its TLS origin and optional
   `MVDM_SESSION_TERMINATION_REPORT_PATH` behavior.  The nine current callers
   move to that owner without changing completion code or failure direction.
2. `app` captures and removes both startup-only report environment variables
   before original COMMAND environment import.  It owns the copied bounded
   values.
3. A read-only app configuration query supplies those already-captured values
   to later S7/S8 observation writers. It cannot write reports, mutate a
   session or alter a guest environment.

This preserves the existing default-off contract while removing the mixed
termination file's app/session state. No CPU, keyboard, PIC, SAS, COMMAND,
DEM, exception or main-return observation may move in S6.
