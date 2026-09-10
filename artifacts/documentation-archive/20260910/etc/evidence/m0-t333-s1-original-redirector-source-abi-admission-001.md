# M0 T333 S1 — Original Redirector source and ABI admission

## Question

What is the current, source-accurate admission boundary for the complete
original OpenNT Redirector provider package before any `57:xx` behavior is
changed?

## Inputs

- `O:\repos.external\OpenNT\base\mvdm\vdmredir`.
- `src/mvdm-host/vdmredir` and its original `sources` manifest.
- `src/mvdm-host/dos/command/cmdredir.c`, `src/mvdm-host/dos/dem/demfile.c`,
  and `src/mvdm-host/dos/dem/demhndl.c`.
- `src/adapter-mvdm-host-out/redir`, its SoftPC facade, session mappings and
  `tools/build/New-T310OriginalSoftpcNinja.ps1`.

## Procedure

1. Enumerated and compared all source-manifest provider paths.
2. Read the original 32-row `VrDispatchTable` and classified every row.
3. Located direct DEM/COMMAND consumers and all current external bindings.
4. Scanned the formal product graph and separately marked T290/T324 scripts
   whose paths or warning policies are historic rather than current.

## Observations

- All 16 provider translation units are present. Twelve are byte-identical;
  four have only registered boundary divergences: `vrmslot.c` (DIV-170),
  `vrnmpipe.c` (DIV-167..169), `vrnetapi.c` (DIV-171), and `vrremote.c`
  (DIV-172).
- The original 32-row dispatcher and its original provider ownership remain
  intact. No adapter owns a `57:xx` selector or Redirector policy.
- The three current adapter forms are bounded guest copying, opaque resource
  identity and mailslot identity. Each delegates to the existing session-owned
  mapping-manager instances through the SoftPC facade; none is a family mapper.
- The current CPU40 formal product graph selects no `vdmredir` body. Old
  T290/T324 fixture builders therefore cannot demonstrate current package
  selection, and their obsolete overlay inputs/warning suppressions are not an
  admissible recipe.

## Interpretation

S1 is complete. The original mirror is the provider owner, and the existing
Redir/SoftPC/session forms are the only admitted modern boundaries. S2 must
classify immediate external calls and exact original failure paths before it
selects a source cohort. It must not create a local Redirector broker or let a
trace choose an individual service.

## Follow-up

The companion ledger gives each dispatch cohort, caller and external form a
current disposition. It makes no runtime-complete claim for named pipes,
mailslots, asynchronous completion, NetAPI/RAP, NetBIOS, DLC, VDD window
setup or protected NetBIOS.
