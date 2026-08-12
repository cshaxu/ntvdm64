# ntdos64 TODO

## Architecture-boundary verification target

Before declaring a first-profile composition complete, verify this directional
architecture target rather than measuring success by how much project-authored
emulation exists:

1. **Minimum Bochs:** retain only the generic CPU, RAM/ROM, exception and
   evidence-proven native PC mechanics required by the selected guest path.
   Bochs must not own OpenNT BOP, DOS, DEM, CLI, or host-capability semantics.
2. **Minimum bx-ntvdm-adapter:** retain only fixed-width bridge records,
   generic `#UD`/BOP ingress, provider routing, and checked guest-copy/result
   transport. It must not become a DOS kernel/filesystem, BIOS, SoftPC, or
   replacement virtual machine.
3. **Maximum original OpenNT guest:** prefer the largest source-built,
   historically owned NTIO/NTDOS/COMMAND/DOS/WOW16 component set that can be
   demonstrated with its original toolchain island. Do not replace guest
   behavior merely because a smaller adapter fixture is easier to run.
4. **Maximum original OpenNT host service:** prefer original dispatcher and
   provider composition, including calling order, layouts and failure paths.
   Use a contained CLI capability backend or source-derived rehost only after
   recording the precise CCPU/SAS/NT-host composition blocker.
5. **Maximum explicit host capability:** make each admitted capability
   profile-owned, finite, reproducible and revocable. Do not infer permission
   to expose host drives, paths, handles, clocks, GUI, or NT-intrusive APIs.

For each newly admitted component or capability, record which of these five
axes moved, what original OpenNT/Bochs owner evidence justifies it, and why a
smaller boundary or a more original provider was not sufficient.

## Current capability boundary

The next real-NTIO stop is `SVC_DEMFINDFIRSTFCB` (`50:0A`).  Do not implement
it as an isolated BOP response.  It is the first member of the host filesystem
and search capability, which must be specified as one coherent adapter-owned
contract before code is admitted.

Required source-closure work:

1. Recover the exact FCB `SRCHBUF` input/output layout, returned error and
   carry/register contract from OpenNT DOS and DEM sources.
2. Trace `demFindFirstFCB`, `demFindNextFCB`, path-style find-first/find-next,
   and the first callers that consume their retained search state.
3. Define CLI-owned `--include-drives`/`--exclude-drives` admission, snapshot
   the resulting host logical-drive inventory at session start, and never let
   a guest path or BOP choose additional drives.
4. Define deterministic 8.3/wildcard, attribute, volume-label, ordering,
   error and handle/iteration behavior, with explicit unsupported cases.
5. Prove the required bounded guest reads/writes and their atomicity through
   existing generic adapter/Bochs transport, before proposing any new external
   intrusion.

## Correctness follow-up

The source-derived `DemGetDPBList` producer has runtime proof for non-empty
qualified drive inventories.  Its empty-qualified-list return must be modeled
as an adapter result without inventing an empty multi-write transaction; this
is an adapter-only contract correction and is not a reason for a Bochs change.

## Explicitly deferred

- DOS filesystem algorithms, COMMAND semantics, WOW behavior and a general
  host filesystem API.
- New Bochs devices, firmware changes, memory exceptions, or service-specific
  branches.
- Claims of a booted/runnable DOS session based on fixture evidence.
