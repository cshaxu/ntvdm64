# T181 S1 finite declared target sequence contract 001

Date: 2026-08-12  
Packet: M0 T181 S1  
Disposition: source-derived design complete; no implementation, build, or runtime observation.

## Purpose and source boundary

OpenNT COMMAND requests a command through `C4 C4 54 01`; T180 established
that its historical `cmdGetNextCmd` provider delegates command selection to
the NT host broker `GetNextVDMCommand`.  That broker is unavailable in this
non-invasive CLI composition.  The replacement is not a new BOP convention:
it is a finite, immutable source-derived provider for the already observed
COMMAND request ABI.

The first product profile must contain exactly two declared slots:

| Slot | Guest identity | Command tail | Role |
| --- | --- | --- | --- |
| 0 | The CLI-selected, profile-hash-verified `TARGET.COM` or `TARGET.EXE` | The existing copied CLI tail | User target. |
| 1 | Source-built `QUIT.COM`, three bytes, SHA-256 `06A37DFF559DF7325DE8B003F4DF53C188F733E0CA312AAD961C34DAE48D7B83` | Empty | Terminal guest target. |

Both files are profile components and are materialized at fixed declared DOS
paths on the same declared drive.  Their bytes are loaded by identity from the
BYOB root during installation; no slot holds a host pathname, handle,
environment-derived command, or guest pointer.

The fixed two-slot initial profile is deliberately not an invitation to add a
new ad-hoc target later.  The provider model is an ordered finite list so that
a future profile can expand only by a new declared schema, validation and
test package.  The current implementation's single `target`, single launch
declaration, `files[3]`, and `delivered` boolean are recognized replacement
surfaces, not extension points.

## State and transaction contract

The future sequence owner has immutable `slots[slot_count]`, with
`slot_count == 2` for the first product profile, and mutable state:

```text
next_slot: 0 .. slot_count
pending_slot: none | 0 .. slot_count-1
terminal_slot_committed: false | true
```

`54:01` keeps the current exact boundary: real mode, CPU exception vector 6,
four copied bytes `C4 C4 54 01`, and a validated `CMDINFO` gather range from
DS:DX.  There is no selector-wide fallback.

1. **Prepare.** Accept only when `pending_slot == none` and
   `next_slot < slot_count`; gather the existing fixed-size `CMDINFO` range
   and record `pending_slot = next_slot` in the pending host-session action.
   A request after exhaustion, a re-entrant pending request, malformed copied
   state, or any different instruction window is declined without writes or
   CPU change.
2. **Complete.** Build the existing bounded multi-write transaction from that
   pending slot only: declared DOS executable path, `TARGET`-shaped command
   buffer/name field as required by the retained COMMAND ABI, copied tail,
   drive count, SCS/DOS-binary flags, resume RIP `+4`, and CF clear.  The
   command display token is a presentation field; executable identity comes
   solely from the selected slot's fixed DOS path.
3. **Commit.** Advance `next_slot` exactly once only after the complete
   transaction has been accepted by the session write queue.  Clear
   `pending_slot`.  If this was slot 1, set `terminal_slot_committed`.
   A failed/abandoned gather or rejected write never consumes a slot.
4. **Stop.** The terminal file is the existing source-built `QUIT.COM`, whose
   guest bytes are `C4 C4 FE`.  Existing controlled-stop handling owns that
   generic BOP and stops the CPU loop.  The sequence provider neither
   implements `FE` nor attempts to authenticate its guest origin; it merely
   makes `QUIT.COM` the final declared slot.  This is not `54:11` and carries
   no DOS exit-status claim.

This preserves the T180 observation: a *second* `54:01` is normal and
delivers the terminal slot.  A third `54:01` is exhausted and must decline.
It never falls through to a manufactured successful no-next response.

## Ownership and required negative cases

| Concern | Owner | Required disposition |
| --- | --- | --- |
| `54:01` decode, copied gather/write/resume mechanics | adapter host-service plane | Reuse the existing bounded transaction form; no Bochs semantics. |
| Slot list, component identities, DOS paths and tails | profile/CLI declaration | Immutable, exact, validated before install. |
| DOS-visible files and metadata | boot namespace provider | Materialize only declared slot files; no host directory lookup. |
| COMMAND continuation and guest transfer | guest COMMAND/DOS | Unchanged original guest behavior. |
| `C4 C4 FE` execution | existing controlled-stop boundary | Generic terminal stop only. |
| CPU, #UD, RAM and vector execution | Bochs/native machine composition | Unchanged. |

The implementation admission must prove at least: absent terminal component;
wrong terminal hash/size/path; slot order reversal; duplicate component/path;
nonempty terminal tail; duplicate completion; re-entrant request; malformed
`54:01`; request after exhaustion; and any attempt to obtain an undeclared
file.  It must also prove that a normal repeated `54:01` consumes slot 1,
not a synthetic no-next result.

## Deliberate exclusions

No part of this contract imports `cmdGetNextCmd`, `GetNextVDMCommand`, Win32
event/console behavior, environment merging, current-directory synchronization,
redirection, host process creation, ambient file search, or `54:11` result
transport.  No code, build target, Bochs modification, OpenNT modification or
runtime observation is authorized by S1.
