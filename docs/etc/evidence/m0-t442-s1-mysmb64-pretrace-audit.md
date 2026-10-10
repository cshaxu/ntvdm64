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

The audit also searched the permitted likely O: package/source locations for
`mysmb64.exe`. No path was returned. This is not an assertion that the entire
volume has no copy; it is a target-location gap.

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

The ownership design is high confidence: one validated intent through NTSRV,
one event to NTVWM, and one typed existing-channel request to NTCON is the
smallest route consistent with current ownership. The exact detour target is
unknown by design: inferring it from the user's Tab gesture would be
unsound. A dynamic trace is required before implementation.

## Follow-up

Obtain the exact `mysmb64.exe` path or a reproducible package location, then
capture its direct-CMD and `run16` Console/window API sequences. If it calls
`FreeConsole`, `AllocConsole`, or `AttachConsole`, stop this visibility-only
task for re-admission.
