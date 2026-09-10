# Historical First-Cut Overlay Granularity Audit

Status: M38 source-path audit, 2026-08-08. This record names exact historical
first-cut call sites. It admits no overlay or replacement implementation.

## Result First

The preserved command table can remain intact, but a noninteractive contained
first-command profile cannot be restored merely by supplying a few absent Win32
APIs. Several required function bodies directly encode historical Base/CSR,
PIF, temporary-file, ambient-environment, console-thread, or raw-pointer
policy. A future change must classify each as either a narrow host-operation
seam or an explicitly approved adapter service-body overlay. Calling the latter
a generic shim would conceal a change in historical service behavior.

## First-Cut Call Map

| Historical function | Reached obsolete operation | Narrowest possible future boundary | Failure route | Status |
| --- | --- | --- | --- | --- |
| `cmdSetInfo` (`cmdmisc.c:591-601`) | three `GetVDMAddr` pointer borrows | M33 copied binding transaction over checked guest addresses | no historical error branch; invalid binding must fault/stop at transition rather than silently return success | requires adapter overlay/bridge |
| `cmdComSpec` (`cmdmisc.c:635-648`) | `GetVDMAddr`, mutable process command metadata | copied NUL path read and session metadata | invalid/mismatched input must reject before metadata mutation | requires adapter bridge |
| `cmdGetInitEnvironment` (`cmdenv.c:179-335`) | ambient `Get/SetEnvironmentVariable`, `GetEnvironmentStrings`, mutable conversion buffer, `GetVDMAddr` | immutable pre-encoded profile environment plus M33 capacity transaction | capacity retry is historical; malformed target must not mutate guest bytes | requires explicit service-body overlay; not an API shim |
| `cmdGetConfigSys` / `cmdGetAutoexecBat` (`cmdconf.c:60-120`) | `ExpandConfigFiles`, PIF configuration, temporary host files, conversion and `GetVDMAddr` | profile-declared virtual configuration result plus checked bounded path transaction | original code terminates on error | requires explicit service-body overlay; not an API shim |
| `cmdGetNextCmd` first-call path (`cmdmisc.c:25-364`) | `GetNextVDMCommand`, raw `CMDINFO` pointers, `cmdCheckForPIF`, unconditional `cmdCheckStandardHandles`, printer initialization | immutable one-shot command provider plus M33 record transaction and later stream policy | provider/record failure follows existing termination route; no fabricated command | requires an adapter overlay around multiple internal operations |
| `cmdGetCurrentDir` (`cmdmisc.c:530-577`) | `GetEnvironmentVariableOem`, drive-type queries, `GetVDMAddr` | contained namespace current-directory lookup plus bounded result transaction | original carry/AX directory errors remain the required result | requires adapter bridge or explicit body overlay; never ambient drive enumeration |
| `cmdInitConsole` (`cmdmisc.c:729-735`) | `nt_init_event_thread` | terminal-capability acknowledgement | no guest data write | narrow lifecycle seam candidate |
| `cmdGetKbdLayout` (`cmdkeyb.c:56-225`) | console layout, registry, system directory, host file existence, `GetVDMAddr` | fixed no-install profile response (`DX=0`) | no install is a normal historical branch result | requires explicit service-body overlay; no narrow host shim exists |
| `cmdReturnExitCode` (`cmdexec.c:602-645`) | event-thread control, raw redirection info, `GetNextVDMCommand` shell-out/reentry | one-shot completion result and bounded runner stop | clear carry ends completion; no shell-out | requires explicit service-body overlay |
| `cmdExitVDM` (`cmdexit.c:27-31`) | `TerminateVDM` process termination | typed session stop request | runner reports stop/fault/cancel | narrow lifecycle seam candidate |

## Why `cmdGetNextCmd` Is Not A Single Shim

The first-call branch does reach a genuine historical command-source boundary,
`GetNextVDMCommand`. A profile-owned provider can in principle replace that
source with one immutable command record. However, the same function also:

- borrows guest `CMDINFO`, command-line and environment pointers;
- conditionally changes environment/current-directory state;
- invokes PIF parsing even for a normal command path; and
- unconditionally constructs and stores historical standard-handle/redirection
  data at the end of successful assembly.

Therefore replacing only `GetNextVDMCommand` still leaves forbidden pointer,
PIF and raw-handle behavior. M33 and a stream policy are necessary but not
sufficient; a future adapter must explicitly admit an internal operation-level
overlay while preserving `cmdGetNextCmd` as the dispatcher-owned function.

## Required Rules For Any Future Overlay

1. The table entry, original function signature, selector consumption, and
   original failure/termination owner remain unchanged.
2. It replaces only a named obsolete operation or an explicitly named,
   irreducibly product-bound body region from this table.
3. It uses copied transaction/capability inputs and no host RAM pointer,
   process-global environment, system-directory search, registry, temporary
   file, PIF discovery, raw host handle, or ambient drive state.
4. It is default-disabled until a selected backend satisfies M36.
5. Each overlay has an original caller, success/failure fixture, and runtime
   link audit proving absence from the generic runner/default targets.

## Decision

M38 rejects the idea that a modern compatibility shim alone can restore this
first-cut command profile. The historical dispatcher and function boundaries
remain valuable and must be preserved, but profile-specific service-body
overlays are a separate, high-risk admission category. M39 should decide
whether such overlays are compatible with the research project's historical
fidelity rule, before any code is written.
