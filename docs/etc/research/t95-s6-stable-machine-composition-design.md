# T95 S6 Stable-Machine Composition Design

## Question

How can the now stable native PC reset-floor machine receive the first
source-built OpenNT guest input through CLI and adapter ownership, without
turning Bochs into NTVDM or allowing adapter/CLI DOS semantics to leak?

## Existing Facts

- `src/cli/ntdos64_run.c` already validates a BYOB profile/root, launches one
  child engine, and constructs a child-only environment containing only the
  selected immutable profile/root and drive-policy snapshot.
- The adapter already owns a fixed-width `startup_plan_v1`. It accepts one
  validated NTIO payload, writes it at physical `0x700`, restores the four
  bytes at `0x714..0x717`, and changes only real-mode `CS:IP` to `0070:0000`.
- The source-built payload facts are immutable: NTIO is 33,792 bytes, NTDOS is
  27,858 bytes and COMMAND is 50,384 bytes. The existing startup plan carries
  only NTIO; it neither loads NTDOS/COMMAND nor interprets their DOS behavior.
- r4 deliberately omits all adapter objects and keeps
  `BX_NTVDM_ENABLE_EXECUTION_PLAN=0` by default. Its accepted reset-floor
  profile has no guest input.

## Composition Boundary

```text
CLI validates BYOB identity and launches child
  -> child-only immutable profile/root + drive policy
  -> companion same-toolchain adapter observation target
  -> adapter validates/makes one pointer-free NTIO startup_plan_v1
  -> generic Bochs RAM copy + preserved bytes + CS:IP delta
  -> native Bochs CPU executes guest bytes
```

The CLI never reads guest bytes or changes CPU/memory. The adapter owns copied
payload storage and fixed-width plan validation, but does not parse NTIO,
NTDOS, COMMAND, DOS paths or BOP/DEM selectors. Bochs receives only the
generic write/range and real-mode entry delta; it remains unaware of BYOB,
host roots and services. NTDOS/COMMAND identities stay admitted inputs but are
not placed or launched until a separate original-owner loader contract exists.

## Required Sequencing

1. Preserve `ntdos64-native-container.exe` as the default adapter-free r4
   target and the accepted PC reset-floor profile unchanged.
2. Perform a read-only MSVC/x86 same-island adapter-link closure audit. It
   must classify the exact C objects, CRT convention, exported functions and
   required default-off compile definitions before any target is added.
3. If and only if that closure is stable, add a separately named
   `ntdos64-native-adapter-observation.exe` target. It enables the existing
   `BX_NTVDM_ENABLE_EXECUTION_PLAN` seam; it does not modify Bochs semantics or
   make the default engine depend on adapter code.
4. Use one synthetic one-byte `HLT` NTIO identity only for generic mechanics
   proof, then one source-built NTIO payload proof. Neither test admits NTDOS,
   COMMAND, filesystem, BOP/DEM services or a CLI product launch.
5. Only after that native proof may CLI integration validate the existing
   child-only handoff against the companion target.

## Refusals

- No command-line parser or host path is added to Bochs.
- No adapter code loads a disk image, emulates BIOS/DOS, or maps host files as
  a guest drive.
- No `NTIO -> NTDOS -> COMMAND` runtime claim follows from the NTIO plan.
- No new Bochs patch is assumed. If adapter same-island linkage exposes one,
  it needs a new exception-register entry and cannot be hidden in the profile.

## Next Gate

The next admitted work is the read-only adapter same-island closure audit,
not a guest run. It must prove whether the existing adapter object set can be
compiled and linked with the r4 MSVC/x86 tuple while default-off mechanics and
the accepted reset-floor profile remain unchanged.
