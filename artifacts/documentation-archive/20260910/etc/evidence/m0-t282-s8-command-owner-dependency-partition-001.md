# M0 T282 S8 — COMMAND owner dependency partition evidence

## Question

After the direct initialization and local command-acquisition slices, which
complete original COMMAND owner slice can be implemented next without turning
a BOP trace into a leaf patch?

## Inputs and procedure

- The exact eleven-source `dos/command` package and original dispatcher order
  in `cmddisp.c`.
- Original `cmdmisc.c`, `cmdenv.c`, `cmdconf.c`, `cmdexec.c`, `cmdredir.c`,
  `cmdkeyb.c`, `cmdpif.c`, `cmdexit.c`, `cmd.c`, and `cmddata.c` helpers.
- T231's historical service ledger, T282/S5 acquisition ledger, T282/S6
  transport evidence and T282/S7 direct composition evidence.
- The [S8 ledger](../operations/m0-t282-s8-command-owner-dependency-ledger-001.tsv)
  was checked with `Test-T282CommandOwnerDependencyPartition.ps1`.

## Observations

The verifier reports 32 unique owner records: all 17 dispatcher services,
the completed initialization pair and 13 reached helper groups. Every row has
one disposition, recovery rung and successor owner slice.

The package partitions into these dependency-ordered boundaries:

1. `cmd.c` + `cmddata.c` initialization is direct original source and closed.
2. Environment/current-directory guest queries (`54:02`, `54:04`, `54:0F`,
   environment and directory helpers) require the existing adapter-win32 and
   bounded adapter-softpc guest-memory seams, but no broker or child process.
3. `CMDINFO` registration and saved-world state require source-shaped
   adapter-softpc descriptor/register contracts.
4. Console/stream/keyboard and binary-admission groups require their own
   full local capability audits. Their WOW/Redirector leaves are not absorbed
   into a local replacement.
5. Configuration/PIF/startup requires a coherent original configuration input
   package rather than temporary boot-file synthesis.
6. Exec/COMSPEC/return stay one child-profile lifecycle group pending
   Redirector, WOW and guest parent-return contracts.
7. Dispatcher ingress stays in adapter-bop, while WOW stays in adapter-wow.

## Interpretation

The next eligible T282 behavior slice is the **environment/current-directory
guest-query group**. It is a coherent source owner boundary with only already
named adapter-win32 and adapter-softpc prerequisites. It must first audit the
existing seams for original argument, layout, error and lease compatibility;
it cannot be implemented as three isolated service bodies.

No interface remains anonymous. The partition preserves the original COMMAND
owner units and records later dependencies explicitly rather than reporting
them as generic incompleteness.
