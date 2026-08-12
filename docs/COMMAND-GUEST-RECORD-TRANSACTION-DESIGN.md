# Command Guest-Record Transaction Design

Status: M33 design admission, 2026-08-08.  This is a source-derived contract
for a future neutral execution backend and adapter.  It does not implement a
DOS service, change `ntvdm64`, or authorize a locally invented command
dispatcher.

## Question

The original command host code stores guest addresses on `SVC_CMDSETINFO` and
later updates several separate guest objects while servicing command requests.
The historical implementation obtains raw pointers with `GetVDMAddr`:

- `cmdmisc.c:591-601` stores a packed `SCSINFO` at `DS:DX`, a DOS-binary byte
  at `DS:BX`, and an FD-access word at `DS:CX`;
- `cmdmisc.c:356-362` and `cmdmisc.c:505-511` set `SCS_ToSync` and the
  DOS-binary byte while composing the next command;
- `cmd.h:98-114` fixes the packed `PARAMBLOCK`/`SCSINFO` layout.

That raw-pointer route is not an admissible x64 adapter ABI.  The replacement
must preserve the original selector and record ownership while ensuring that
an invalid second target cannot leave the first target changed.

## Non-Goals

- No selector-specific API is added to core.
- No raw host pointer, `GetVDMAddr` equivalent, provider callback, or mutable
  guest-memory map is exposed to a command adapter.
- This does not define BOP dispatch, DOS SVC semantics, protected-mode
  translation, A20 policy, ROM mapping, standard-handle transport, or native
  process execution.
- This does not make command code a replacement for historical `CmdDispatch`.

## Neutral Backend Contract

At a paused transition boundary, mantle/backend offers an opaque
`guest_record_transaction` with these operations:

1. `read(address, bytes)` copies a requested guest span into caller-owned
   staging after checked address translation and complete-range validation.
2. `stage_write(address, immutable_bytes)` records a proposed guest patch.
3. `validate()` resolves every staged destination through the same mapping
   route as ordinary execution.  Every non-empty destination must be writable
   `ORDINARY_RAM`; a ROM, device/provider mapping, hole, overflow, failed
   translation, or stale transition epoch rejects the entire transaction.
4. `commit()` applies all validated patches atomically, or applies none.
5. `abort()` discards all staged copies and patches.

The transaction has a backend-issued boundary epoch.  Resume, reset, stop,
fault, machine destruction, and a different trap/event invalidate it.  A
commit after invalidation fails without memory or register mutation.

`size == 0` is valid only for a read-less/no-patch internal item; it has no
mapping effect.  All command record payloads in this cut are non-empty.
Addition of address and length is checked before translation and cannot wrap.

### Overlap Rule

Before commit, patches are normalized in increasing translated physical range.
Disjoint patches are retained.  Exact overlapping bytes are allowed only if
their proposed byte values agree; differing values reject the transaction.
This permits an adapter to describe a record plus a separately named byte
inside it without relying on write order.  The normalized, non-conflicting
plan is committed as one operation.

### Register and Host-State Ordering

The adapter keeps register outputs and session-state changes in separate
staging.  It commits guest memory first, then applies its finite register
disposition and session-state update.  A preflight or commit failure applies
neither.  No host file operation, process creation, or stream action may run
between memory validation and commit.

Core owns checked translation, mapping classification, and atomic ordinary-RAM
publication.  Mantle/backend owns the paused epoch and opaque address values.
The command adapter owns NT4 packed-record interpretation and profile bytes.

## Service Plans

| Historical selector | Required transaction behavior | Failure behavior |
| --- | --- | --- |
| `CMDSETINFO` | Read the complete packed `SCSINFO` record at `DS:DX`; independently preflight writable spans for `SCS_ToSync`, the byte at `DS:BX`, and the word at `DS:CX`; store opaque address bindings plus current session epoch only after all validation succeeds. | Retain prior bindings; no partial replacement. |
| `GETCONFIGSYS`, `GETAUTOEXECBAT`, `CMDCOMSPEC` | Stage one bounded NUL-terminated profile path to the historical guest buffer.  The path is profile data, not a host-discovered path. | No guest mutation and an original-compatible service failure disposition. |
| `GETINITENVIRONMENT` | First calculate required paragraph capacity.  On insufficient capacity, return the required size through the defined register result and stage no memory patch.  Otherwise stage the full double-NUL environment block as one patch. | Capacity retry leaves guest memory unchanged. |
| `CMDGETNEXTCMD` | Read the packed `CMDINFO` first into staging; validate every embedded writable output span and the previously bound SCS/DOS-binary locations; construct all command-tail, parameter, path, record-field, SCS-sync, and DOS-binary patches before validation. | No changed record field, sync byte, DOS-binary byte, or command-consumption state. |

`CMDINITCONSOLE`, `GETKBDLAYOUT`, completion, and terminal acknowledgement do
not require a guest record mutation in the first cut.  `CMDGETCURDIR` uses the
same contract when it writes a bounded contained-directory result.

## Record Rules

The M33 adapter treats source layouts as byte layouts, never as host C
structures.  In particular, `SCSINFO` is packed and contains two character
arrays followed by `PARAMBLOCK` and `SCS_ToSync`; an implementation must use
fixed field offsets and little-endian field encoding derived from the selected
NT4 source profile.  It must not cast a copied byte buffer to a host pointer
or assume host alignment/word width.

The persistent results of `CMDSETINFO` are opaque guest-address bindings, not
borrowed guest pointers.  Each later use repeats its complete-range checked
preflight in the newly opened transaction.  A reset or invalidated session
epoch clears all bindings.

## Required Tests Before Implementation Admission

1. Valid disjoint `CMDGETNEXTCMD` record, SCS, and DOS-binary writes commit
   together.
2. A second invalid span leaves the first valid span, registers, and command
   consumption state unchanged.
3. Conflicting aliases reject; agreeing aliases result in the expected bytes.
4. `CMDSETINFO` rejects a record, byte, or word that crosses a hole, provider,
   ROM, or address overflow and retains old bindings.
5. Environment capacity retry writes no bytes, while sufficient capacity emits
   exactly one double-NUL terminated environment image.
6. Resume/reset/fault between staging and commit invalidates the transaction.
7. A forced publication failure proves rollback for every staged span.
8. Diagnostics name only service class, span size, route class, and failure
   category; they contain no guest file bytes or host root paths.

## Current Gap And Next Use

`ntvdm64` currently exposes checked physical `memory_query`, `memory_read`,
and `memory_write` with an `ORDINARY_RAM`/`PROVIDER` route distinction, but it
does not yet expose an external copied guest-address translation boundary or a
multi-span atomic commit.  This is an evidence-backed neutral core/mantle
delta, not a reason to add an NT4 or command-specific entry point.

The next task is to audit the original command object/build boundary and map
the first profile record to the preserved `CmdDispatch` ownership.  Runtime
integration remains blocked until a selected backend can satisfy this contract.
