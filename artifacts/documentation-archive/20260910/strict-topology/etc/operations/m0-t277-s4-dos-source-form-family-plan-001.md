# M0 T277 S4 — DOS DEM/COMMAND source-form family plan

## Scope

S4 reviews exactly the 73 `dos` caller rows in the committed T277/S2
source-form worklist. The source roots are the original `dos/dem` and
`dos/command` host provider forms. This is one owner-package audit: it does
not make a trace-selected BOP implementation list.

## Required family outcomes

- `TerminateVDM`: original product termination callers retain their call
  shape, but resolve to a session-owned controlled-stop successor; no caller
  may terminate the surrounding app process.
- `nt_block_event_thread`, `nt_resume_event_thread`, and
  `nt_std_handle_notification`: retain the original COMMAND lifecycle call
  ordering; session/broker disposition is based on the original event form,
  not an invented generic thread API.
- `cmdPipeFile*` and `cmdPushExitInConsoleBuffer`: distinguish local child
  lifecycle from Redirector or WOW completion paths; name the successor for
  every unavailable product-shell branch.
- `host_direct_access_error`, reset/flush and `host_simulate`: preserve the
  original top-level/machine boundary. S3 remains the sole evidence for
  bounded machine execution; S4 must not duplicate or widen it.
- DEM address, file, device and search callers: identify the original
  `adapter-softpc`, `adapter-win32`, `adapter-bop`, session or later
  Redirector owner; guest addresses use only the session mapping manager.
- CRT `malloc` and `getenv`: retain ordinary runtime binding forms and their
  original caller-side failure paths.

## Exclusions

No original DOS/DEM/COMMAND source body is edited, built or linked. No
adapter ABI, BOP route, CLI policy, Bochs behavior, host drive action, child
process, broker, Redirector or guest execution is enabled. The result is an
auditable source-form and successor map only.

## Evidence and closure

The generated S4 ledger must cover every one of the 73 stable S2 work item
IDs once. Each row records the original caller and provider form, a unique
owner, retained interface shape, mapping rule when applicable, source-derived
failure disposition and an implementation-disabled state. Any source branch
requiring private NT4 transport, Redirector, WOW, VDD or debugger support is
assigned to its named future owner package rather than described as direct.
