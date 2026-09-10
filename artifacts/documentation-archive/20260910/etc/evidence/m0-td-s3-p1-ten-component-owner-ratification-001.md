# M0 Td S3 P1 — Ten-component owner-contract ratification

## Question

Does the approved source layout express one stable, minimal owner model for
the adopted Bochs machine, OpenNT sources, historical compatibility interfaces
and application composition?

## Inputs

- Owner direction: four original-code components (`bx-core`, `opennt-guest`,
  `opennt-bop`, `opennt-host`); four mechanical adaptation components
  (`bx-mantle`, `adapter-bop`, `adapter-softpc`, `adapter-win32`); and the
  project-composition components (`app`, `session`).
- Current architecture authority: `docs/design/ARCHITECTURE.md`.
- Current source-layout authority: `docs/design/CODING.md`.
- Enforceable ownership rule: `docs/rules/ARCHITECTURE.md`.

## Ratified contract

The source components have the following exclusive roles.

1. The original-code group retains recognizable upstream source ownership and
   individually registered `DIVERGENCE:` edits: adopted Bochs core in
   `bx-core`; OpenNT guest images and source in `opennt-guest`; OpenNT BOP
   providers in `opennt-bop`; and independently composable OpenNT host
   capabilities in `opennt-host`.
2. `bx-mantle` is Bochs-only machine assembly.  It may crop or compose native
   Bochs mechanisms but has no OpenNT, DOS, VDM, WOW or Win32 meaning.
3. `adapter-bop` is the selector-blind copied-frame ingress/completion
   boundary.  It does not identify a BOP selector or implement a provider.
4. `adapter-softpc` is the one same-shaped historical SoftPC/CCPU/SAS
   compatibility owner.  It preserves reached historical names, parameters,
   ABI and observable mechanical result while requesting bounded Bochs
   mechanics through `bx-mantle` and `bx-core`.  No generic `adapter-bx` is
   admitted: it would duplicate or blur this exact responsibility.
5. `adapter-win32` is the one same-shaped unavailable historical Win32/NTDLL
   compatibility owner, implemented only with applicable public modern Win32
   APIs.  It neither owns BOP meaning nor accesses Bochs.
6. `session` is a dependency-free, neutral lifecycle/resource/event owner;
   `app` is the CLI and one-session composition owner.  Neither becomes an
   alternate OpenNT service provider.

The resulting inward call direction is
`app -> {opennt-guest, session, bx-mantle, adapter-bop}`;
`adapter-bop -> opennt-bop`; and
`opennt-bop/opennt-host -> {adapter-win32, adapter-softpc, session}`, with
`adapter-softpc -> bx-mantle -> bx-core`.  Runtime callbacks are wired by
`app`; they are not reverse static imports into Bochs.

## Interpretation

The current architecture, coding and architecture-rule authorities already
state this model consistently.  This Td delivery records the owner approval
and makes the explicit rejection of a generic `adapter-bx` auditable.  It
does not move production source, alter a provider, expand a Bochs boundary or
change the active M0 T261 S8/P5 recovery work.

## Verification

- Authority cross-read performed against the three listed current authorities.
- This record is indexed by the documentation-inventory gate.
- Documentation governance and `git diff --check` are required before the
  commit is accepted.

## Follow-up

Continue M0 T261 S8 P5 with source-shaped `vrmslot.c` recovery.  Any later
owner conflict is resolved against the ratified contract before a new helper
or component is created.
