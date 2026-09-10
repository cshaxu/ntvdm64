# Lane P Initial Environment Dossier

Status: M42 design dossier, 2026-08-08. This is Lane P evidence under M40. It does not implement the historical service or a replacement selector.

## Header

| Field | Dossier |
| --- | --- |
| Historical source/function | `base/mvdm/dos/command/cmdenv.c:179-335`, `cmdGetInitEnvironment` |
| Dispatcher/entry | original command dispatcher; caller supplies `ES:0000` and paragraph capacity in `BX` |
| Lane | P only |
| Historical policy replaced | ambient environment enumeration/mutation, PROMPT injection, ANSI-to-OEM conversion, WINDir filtering, heap buffers, and raw `GetVDMAddr` destination |
| Required backend | copied `ES:0000` translation, ordinary-RAM transaction, atomic register/memory disposition |

## Immutable Profile Grammar

The profile supplies one pre-encoded OEM environment suffix before guest mutation. It contains zero or more nonempty `NAME=VALUE` byte strings, each NUL terminated, followed by one final NUL. It must contain no embedded NUL, no host path inferred at runtime, and no implicit environment variable expansion. Its order and byte case are profile facts.

The service constructs the final guest image as:

```text
"COMSPEC=" + selected guest COMSPEC bytes + NUL
+ profile environment suffix
```

The selected COMSPEC bytes are the already validated LP-02 value. The suffix must not contain another `COMSPEC=` entry. The final image is therefore double-NUL terminated even when the suffix has no variable entry. Total byte count and required paragraphs are calculated with checked arithmetic before any guest read or write:

```text
required_paragraphs = ceil(total_bytes / 16)
```

## Entry And Result Contract

1. If the historical first-call state is false, the service sets `BX=0`, makes no guest-memory change, and does not query profile environment data. This preserves the source's explicitly observed branch.
2. On first call, the adapter captures the `ES:0000` destination and `BX << 4` capacity from copied state.
3. If capacity is smaller than the complete image, it stages no memory write and updates only `BX` to `required_paragraphs`. The original code does not explicitly set carry on this path; a future implementation preserves all flags unless an independently sourced caller contract proves otherwise.
4. If capacity is sufficient, it creates one ordinary-RAM transaction for the exact complete image, commits it once, and updates `BX` to `required_paragraphs`.
5. No temporary host buffer, process environment, registry, current directory, system directory, or host COMSPEC is read or changed during service execution.

## Failure Contract

Profile grammar error, COMSPEC absence, multiplication/size overflow, unavailable transaction capability, invalid translation, non-ordinary destination, stale epoch, cancellation, or commit failure produces the Lane P declared fault/stop result. It performs no guest-memory or register mutation. A capacity retry is not an error and must leave guest memory unchanged.

## Trace And Fixtures

The trace labels the event `lane_p` and records only image byte count, required paragraphs, disposition category, and redacted profile identity.

- zero-entry suffix produces `COMSPEC=...` followed by a double NUL;
- multi-entry suffix preserves exactly declared OEM byte order;
- insufficient capacity updates only BX and commits no transaction;
- sufficient capacity writes one complete image and the expected BX;
- invalid/mapped/stale destination preserves bytes, BX, and flags;
- non-first-call branch writes no bytes and returns BX zero;
- no fixture reads the host environment or reports historical-host restoration.

## Deferred

Environment merging after program exit, batch reentry, separate WOW behavior, variable expansion, and host environment synchronization remain unavailable. They do not fall back to ambient host behavior.
