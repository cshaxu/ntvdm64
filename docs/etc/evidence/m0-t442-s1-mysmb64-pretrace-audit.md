# T442 S1 mysmb64 pre-trace audit

## Question

Can the proposed native CUI Console/window repair be designed as an exact,
authenticated path before observing `mysmb64.exe`'s actual Console/window API
calls?

## Inputs

- `src/nthook32-dll/create_process.cpp`: the current injected hook detours only
  `CreateProcessA/W` and injects the next target before resuming it.
- `src/nthook32-dll/context.cpp`: the copied bootstrap contains the frontend
  and execution attachments, not an NTCON pipe or a display API.
- `src/ntvwm-exe/main.c`: native presentation has a serialized owner thread
  and existing worker I/O lifecycle.
- `src/ntcon-exe/frontend_session.c`: NTCON already owns the local
  Console/Window projection selector through `frontend_session_display()`.
- `src/ntsrv-exe/opennt/source/frontend_registry.c`: NTSRV owns native route
  authorization and worker I/O transition state.

At this preliminary stage, the audit searched the permitted likely O: package/
source locations for `mysmb64.exe` and did not find it. The owner subsequently
identified the deployed `mysmb64.exe`; the resulting source-backed target sequence
and stop condition are recorded by the
[successor transition evidence](m0-t442-s1-mysmb64-console-identity-transition.md).

## Procedure

1. Trace static target bootstrap from NTVWM target creation into nthook.
2. Inspect every current hook detour and its communication resources.
3. Trace native-worker presentation ownership, service route ownership and
   NTCON display selection.
4. Compare the candidate's required state transition with existing Console-I/O
   operations.

## Observations

- A target hook cannot legitimately call NTCON: it has neither its pipe nor a
  frontend-local authority.
- NTVWM is the existing serialized worker-side issuer of frontend I/O; NTCON
  is the existing local display selector.
- NTSRV is the only component that can validate a target's execution attachment
  against the active direct request and frontend route.
- `CONSOLE_IO_SET_DISPLAY_MODE` is a different Win32 Console API concept and
  cannot stand for projection selection.
- The current hook does not detour `GetConsoleWindow`, `ShowWindow`,
  `ShowWindowAsync`, `SetForegroundWindow`, `SetWindowPos`, `FreeConsole`,
  `AllocConsole`, or `AttachConsole`.

## Interpretation and confidence

The ownership design is high confidence for a target that retains its carrier
Console identity: one validated intent through NTSRV, one event to NTVWM, and
one typed existing-channel request to NTCON is the smallest route consistent
with current ownership. The successor evidence proves that `mysmb64` does not
meet that condition, so this record is not an implementation authorization for
that target.

## Follow-up

The target location and source sequence are now known. The stop condition was
reached: obtain re-admission for an explicit Console-identity design before
performing any product implementation.
