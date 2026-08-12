# Core, Mantle, and NTDOS Delta Packet

Status: M4 handoff proposal, 2026-08-08. This is a design packet for the
future `ntvdm64` core/mantle workstream. It does not modify that checkout or
declare a stable DLL ABI.

## Evidence Base

- The read-only software-core probe in
  `OWNED-MONITOR-M3-EXECUTION-BACKEND-WORK-ORDER.md` proves real execution,
  freeze, reset, entry-plan, and controlled stop through current core archives.
- The owned M0/M1/M2 fixtures prove the desired neutral failure and adapter
  behavior without importing historical runtime code.
- `R1-ORIGINAL-CHAIN-CLOSURE-LEDGER.md` and the fixed OpenNT sources prove a
  bounded historical caller/result sequence. They are behavior evidence, not
  a core implementation dependency.

## Ownership

```text
core
  generic CPU execution, checked physical memory, mapping, reset/entry plan,
  transition detection, validated state commit, typed stop/fault

mantle
  selected-profile composition, transition registrations, copied profile
  context, clock/files/media/input capability injection, session diagnostics

ntdos adapter
  version-locked trace descriptors, source-derived register interpretation,
  BYOB role validation, NTDOS image placement and later request descriptors
```

`nxvm` uses core with a PC/AT profile. `nxvdm` uses core plus mantle with its
own runtime profile. The external research adapter uses the same core/mantle
contracts but owns no NXVM/NXVDM UI or session policy. None of these layers
owns Microsoft guest binaries.

## Required Generic Core Changes

### C1. Transition Fetch Input

Extend the undefined-instruction transition input with a copied physical fetch
address. It is a physical address after the CPU's own real/protected-mode
translation, not a caller-supplied linear address. Keep `CS:EIP` as diagnostic
information but do not require an adapter to recalculate translation.

### C2. Candidate-Length Fetch

Do not require a 15-byte physical read before considering every transition.
The registry already rejects prefix overlap, so core can evaluate each
registered candidate using its exact length, or report the number of safely
fetched bytes. A valid one-byte transition at the final mapped byte must be
observable; an unavailable byte must never be treated as a matching zero.

### C3. Validated Partial Patch

Replace unconditional general-register replacement with a response that has:

- an explicit general-register write mask;
- an explicit permitted FLAGS write mask;
- core-owned consumed length equal to the registered pattern length;
- no raw IP, CS, CPU-mode, mapping, A20, IRQ, or memory mutation capability;
- validation of disposition, masks, and fault detail before a single commit.

`HANDLED_RESUME` commits the validated candidate state atomically. `UNHANDLED`,
`STOP`, and `FAULT` retain the pre-instruction state. Fault has a typed,
nonzero core detail owned by the generic transition mechanism, not by a
Microsoft profile.

### C4. Keep Core Neutral

Do not add BOP names, selector values, DOS services, date conversion,
filesystem callbacks, BIOS policy, or product paths. Current 15-byte maximum,
prefix conflict rejection, configuration freeze, entry-plan preflight, and
checked memory routing are the correct generic direction.

## Mantle Contract

Mantle is the first consumer of C1-C4. It owns a profile-local transition
descriptor registry and creates copied adapter contexts before core freeze.
Its adapter callback receives copied core input and emits only the constrained
response. It has no borrowed guest pointer, no direct machine handle, and no
ambient host access.

Mantle supplies a small typed capability surface. The first needed capability
is an immutable civil-time snapshot obtained outside the adapter. Files,
directory, media, stream, and input capabilities remain later additions,
admitted only by a source-derived live trace. The capability surface records
redacted operation/result diagnostics and never retains BYOB bytes.

## NTDOS Adapter Contract

The NTDOS adapter is an external, fixed-profile consumer of mantle. It owns:

- EN-US profile identity and pairwise BYOB validation;
- non-distributed user-selected image placement declarations;
- source-derived transition descriptors and register-result mappings;
- progression from bounded trace to request descriptor only after each
  historical caller, inputs, outputs, and capability need is recorded.

It may not create a second instruction decoder, arbitrary service switch,
PC/AT device model, or core patch protocol. The original historical
CCPU/BIOS/`MS_bop_0`/DEM chain remains its oracle where available.

## Current Fixture Disposition

`owned_monitor_m0`, `owned_monitor_m1`, and `nt4_oracle_date_adapter` are
executable specification fixtures. After C1-C3 exist in core, their permanent
logic must not be duplicated: retain them as differential/conformance tests,
move only generic semantics into core and profile composition into mantle, and
retire any redundant runtime path.

## M4 Acceptance Checklist

1. Each C1-C3 requirement maps to a core test that does not name NTDOS.
2. A mantle test consumes C1-C3 with only owned arbitrary fixture data.
3. The external NTDOS adapter first binds a clock snapshot and one recorded
   profile behavior without linking product UI/session code.
4. Default `nxvm`, `nxvdm`, and `ntdos64-run` graphs stay independent of the
   external BYOB profile and historical guest files.
5. No `ntdos64` wrapper workaround becomes the permanent owner of C1-C3.
